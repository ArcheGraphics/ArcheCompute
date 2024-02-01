//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "device.h"
#include "stream.h"
#include "common/logging.h"
#include "metal.h"
#include <dlfcn.h>
#include <filesystem>

namespace vox {
inline std::string get_colocated_mtllib_path(const std::string &lib_name) {
    Dl_info info;
    std::string mtllib_path;
    std::string lib_ext = lib_name + ".metallib";

    int success = dladdr((void *)get_colocated_mtllib_path, &info);
    if (success) {
        auto mtllib = std::filesystem::path(info.dli_fname).remove_filename() / lib_ext;
        mtllib_path = mtllib.c_str();
    }

    return mtllib_path;
}

namespace {

constexpr const char *default_mtllib_path = METAL_PATH;

auto load_device() {
    auto devices = MTL::CopyAllDevices();
    auto device = static_cast<MTL::Device *>(devices->object(0));
    if (!device) {
        throw std::runtime_error("Failed to load device");
    }
    return device;
}

std::pair<MTL::Library *, NS::Error *> load_library_from_path(
    MTL::Device *device,
    const char *path) {
    auto library = NS::String::string(path, NS::UTF8StringEncoding);
    NS::Error *error;
    auto lib = device->newLibrary(library, &error);

    return std::make_pair(lib, error);
}

MTL::Library *load_library(
    MTL::Device *device,
    const std::string &lib_name = "mlx",
    const char *lib_path = default_mtllib_path) {
    // Firstly, search for the metallib in the same path as this binary
    std::string first_path = get_colocated_mtllib_path(lib_name);
    if (!first_path.empty()) {
        auto [lib, error] = load_library_from_path(device, first_path.c_str());
        if (lib) {
            return lib;
        }
    }

    // Couldn't find it so let's load it from default_mtllib_path
    {
        auto [lib, error] = load_library_from_path(device, lib_path);
        if (!lib) {
            ERROR("{}", error->localizedDescription()->utf8String());
        }
        return lib;
    }
}

}// namespace

Device::Device()
    : _device{load_device()} {
    library_map_ = {{"metal_kernel", load_library(_device)}};
}

Device::~Device() {
    _device->release();
}

MTL::Device *Device::handle() {
    return _device;
}

std::string Device::name() {
    return _device->name()->utf8String();
}

Stream &Device::stream(uint32_t index) {
    if (auto it = stream_map_.find(index); it != stream_map_.end()) {
        return *it->second;
    } else {
        auto new_stream = new Stream(index);
        stream_map_.insert({index, new_stream});
        return *new_stream;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Device::register_library(const std::string &lib_name,
                              const std::string &lib_path) {
    if (auto it = library_map_.find(lib_name); it == library_map_.end()) {
        auto new_lib = load_library(_device, lib_name, lib_path.c_str());
        library_map_.insert({lib_name, new_lib});
    }
}

void Device::register_library(const std::string &lib_name,
                              const std::function<std::string(const std::string &)> &lib_path_func) {
    if (auto it = library_map_.find(lib_name); it == library_map_.end()) {
        std::string new_lib_path = lib_path_func(lib_name);
        auto new_lib = load_library(_device, lib_name, new_lib_path.c_str());
        library_map_.insert({lib_name, new_lib});
    }
}

MTL::Library *Device::get_library_cache_(const std::string &lib_name) {
    // Search for cached metal lib
    MTL::Library *mtl_lib;
    if (auto it = library_map_.find(lib_name); it != library_map_.end()) {
        mtl_lib = it->second;
    } else {// Look for metallib alongside library
        register_library(lib_name, get_colocated_mtllib_path);
        mtl_lib = library_map_[lib_name];
    }

    return mtl_lib;
}

MTL::Function *Device::get_function_(const std::string &name,
                                     MTL::Library *mtl_lib) {
    // Pull kernel from library
    auto ns_name = NS::String::string(name.c_str(), NS::ASCIIStringEncoding);
    auto mtl_function = mtl_lib->newFunction(ns_name);

    return mtl_function;
}

MTL::Function *Device::get_function_(const std::string &name,
                                     const std::string &specialized_name,
                                     const MTLFCList &func_consts,
                                     MTL::Library *mtl_lib) {
    if (func_consts.empty() && (specialized_name == name)) {
        return get_function_(name, mtl_lib);
    }

    // Prepare function constants
    auto mtl_func_consts = MTL::FunctionConstantValues::alloc()->init();

    for (auto [value, type, index] : func_consts) {
        mtl_func_consts->setConstantValue(value, type, index);
    }

    // Prepare function desc
    auto desc = MTL::FunctionDescriptor::functionDescriptor();
    desc->setName(NS::String::string(name.c_str(), NS::ASCIIStringEncoding));
    desc->setSpecializedName(
        NS::String::string(specialized_name.c_str(), NS::ASCIIStringEncoding));
    desc->setConstantValues(mtl_func_consts);

    // Pull kernel from library
    NS::Error *error = nullptr;
    auto mtl_function = mtl_lib->newFunction(desc, &error);

    // Throw error if unable to build metal function
    if (!mtl_function) {
        ERROR("[metal::Device] Unable to load function {}", name);
        ERROR("{}", error->localizedDescription()->utf8String());
    }

    mtl_func_consts->release();
    desc->release();

    return mtl_function;
}

MTL::Function *Device::get_function(const std::string &base_name,
                                    MTL::Library *mtl_lib,
                                    const std::string &specialized_name /* = "" */,
                                    const MTLFCList &func_consts /* = {} */) {
    return get_function_(base_name, specialized_name, func_consts, mtl_lib);
}

MTL::Function *Device::get_function(const std::string &base_name,
                                    const std::string &lib_name /* = "mlx" */,
                                    const std::string &specialized_name /*  = "" */,
                                    const MTLFCList &func_consts /* = {} */) {
    // Search for cached metal lib
    MTL::Library *mtl_lib = get_library_cache_(lib_name);

    return get_function(base_name, mtl_lib, specialized_name, func_consts);
}

MTL::LinkedFunctions *Device::get_linked_functions_(const std::vector<MTL::Function *> &funcs) {
    if (funcs.empty()) {
        return nullptr;
    }

    auto lfuncs = MTL::LinkedFunctions::linkedFunctions();

    std::vector<NS::Object *> objs(funcs.size());
    for (int i = 0; i < funcs.size(); i++) {
        objs[i] = funcs[i];
    }

    NS::Array *funcs_arr = NS::Array::array(objs.data(), funcs.size());

    lfuncs->setPrivateFunctions(funcs_arr);

    return lfuncs;
}

MTL::ComputePipelineState *Device::get_kernel_(const std::string &name,
                                               const MTL::Function *mtl_function) {
    // Compile kernel to compute pipeline
    NS::Error *error = nullptr;
    MTL::ComputePipelineState *kernel;

    if (mtl_function) {
        kernel = _device->newComputePipelineState(mtl_function, &error);
    }

    // Throw error if unable to compile metal function
    if (!mtl_function || !kernel) {
        ERROR("[metal::Device] Unable to load kernel {}", name);
        ERROR("{}", error->localizedDescription()->utf8String());
    }

    return kernel;
}

MTL::ComputePipelineState *Device::get_kernel_(const std::string &name,
                                               const MTL::Function *mtl_function,
                                               const MTL::LinkedFunctions *linked_functions) {
    // Check inputs
    if (!linked_functions) {
        return get_kernel_(name, mtl_function);
    }

    if (!mtl_function) {
        ERROR("[metal::Device] Unable to load kernel {}", name);
    }

    // Prepare compute pipeline state descriptor
    auto desc = MTL::ComputePipelineDescriptor::alloc()->init();
    desc->setComputeFunction(mtl_function);
    desc->setLinkedFunctions(linked_functions);

    // Compile kernel to compute pipeline
    NS::Error *error = nullptr;
    auto kernel = _device->newComputePipelineState(
        desc, MTL::PipelineOptionNone, nullptr, &error);

    // Throw error if unable to compile metal function
    if (!kernel) {
        ERROR("[metal::Device] Unable to load kernel {}", name);
        ERROR("{}", error->localizedDescription()->utf8String());
    }

    return kernel;
}

MTL::ComputePipelineState *Device::get_kernel(const std::string &base_name,
                                              MTL::Library *mtl_lib,
                                              const std::string &hash_name /* = "" */,
                                              const MTLFCList &func_consts /* = {} */,
                                              const std::vector<MTL::Function *> &linked_functions /* = {} */) {
    auto pool = new_scoped_memory_pool();

    // Look for cached kernel
    const auto &kname = hash_name.empty() ? base_name : hash_name;
    if (auto it = kernel_map_.find(kname); it != kernel_map_.end()) {
        return it->second;
    }

    // Pull kernel from library
    auto mtl_function = get_function_(base_name, kname, func_consts, mtl_lib);

    // Compile kernel to compute pipeline
    auto mtl_linked_funcs = get_linked_functions_(linked_functions);
    auto kernel = get_kernel_(kname, mtl_function, mtl_linked_funcs);
    mtl_function->release();
    mtl_linked_funcs->release();

    // Add kernel to cache
    kernel_map_.insert({kname, kernel});
    return kernel;
}

MTL::ComputePipelineState *Device::get_kernel(const std::string &base_name,
                                              const std::string &lib_name /* = "mlx" */,
                                              const std::string &hash_name /*  = "" */,
                                              const MTLFCList &func_consts /*  = {} */,
                                              const std::vector<MTL::Function *> &linked_functions /*  = {} */) {
    // Look for cached kernel
    const auto &kname = hash_name.empty() ? base_name : hash_name;
    if (auto it = kernel_map_.find(kname); it != kernel_map_.end()) {
        return it->second;
    }

    // Search for cached metal lib
    MTL::Library *mtl_lib = get_library_cache_(lib_name);

    return get_kernel(base_name, mtl_lib, kname, func_consts, linked_functions);
}

MTL::Library *Device::get_library_(const std::string &source_string) {
    auto pool = new_scoped_memory_pool();

    auto ns_code =
        NS::String::string(source_string.c_str(), NS::ASCIIStringEncoding);

    NS::Error *error = nullptr;
    auto mtl_lib = _device->newLibrary(ns_code, nullptr, &error);

    // Throw error if unable to compile library
    if (!mtl_lib) {
        ERROR("[metal::Device] Unable to build metal library from source");
        ERROR("{}", error->localizedDescription()->utf8String());
    }

    return mtl_lib;
}

MTL::Library *Device::get_library_(const MTL::StitchedLibraryDescriptor *desc) {
    auto pool = new_scoped_memory_pool();

    NS::Error *error = nullptr;
    auto mtl_lib = _device->newLibrary(desc, &error);

    // Throw error if unable to compile library
    if (!mtl_lib) {
        ERROR("[metal::Device] Unable to load build stitched metal library");
        ERROR("{}", error->localizedDescription()->utf8String());
    }

    return mtl_lib;
}

MTL::Library *Device::get_library(const std::string &name,
                                  const std::string &source,
                                  bool cache /* = true */) {
    if (cache) {
        if (auto it = library_map_.find(name); it != library_map_.end()) {
            return it->second;
        }
    }

    auto mtl_lib = get_library_(source);

    if (cache) {
        library_map_.insert({name, mtl_lib});
    }

    return mtl_lib;
}

MTL::Library *Device::get_library(const std::string &name,
                                  const MTL::StitchedLibraryDescriptor *desc,
                                  bool cache /* = true */) {
    if (cache) {
        if (auto it = library_map_.find(name); it != library_map_.end()) {
            return it->second;
        }
    }

    auto mtl_lib = get_library_(desc);

    if (cache) {
        library_map_.insert({name, mtl_lib});
    }

    return mtl_lib;
}

//----------------------------------------------------------------------------------------------------------------------
Device &device() {
    static Device metal_device;
    return metal_device;
}

Stream &stream(uint32_t index) {
    return device().stream(index);
}

void synchronize(bool wait, uint32_t index) {
    stream(index).synchronize(wait);
}

}// namespace vox