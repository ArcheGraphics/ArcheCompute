//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_device.h"
#include "metal_buffer.h"
#include "metal_stream.h"
#include "metal_kernel.h"
#include "metal_counter.h"
#include "extension/metal_debug_capture_ext.h"
#include "common/logging.h"
#include <dlfcn.h>
#include <filesystem>

namespace vox {
namespace {

constexpr const char *default_mtllib_path = METAL_PATH;

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

MetalDevice::MetalDevice()
    : _device{MTL::CreateSystemDefaultDevice()} {
    _device->retain();
}

MetalDevice::~MetalDevice() {
    _device->release();
}

std::string MetalDevice::name() {
    return _device->name()->utf8String();
}

std::string MetalDevice::builtin_lib_name() {
    return "metal_kernel";
}

std::unique_ptr<MetalDebugCaptureExt> MetalDevice::debug_capture() {
    return std::make_unique<MetalDebugCaptureExt>(_device);
}

std::shared_ptr<Buffer> MetalDevice::create_buffer(size_t element_stride, size_t size) {
    return std::make_shared<MetalBuffer>(_device, element_stride, size);
}

std::shared_ptr<Stream> MetalDevice::create_stream() {
    return std::make_shared<MetalStream>(_device);
}

std::shared_ptr<Kernel> MetalDevice::create_kernel_with_source(const std::string &code, const std::string &entry) {
    NS::Error *pError = nullptr;

    MTL::Library *pComputeLibrary = _device->newLibrary(NS::String::string(code.c_str(), NS::UTF8StringEncoding), nullptr, &pError);
    if (!pComputeLibrary) {
        ERROR("{}", pError->localizedDescription()->utf8String());
    }
    auto kernel = std::make_shared<MetalKernel>(_device, pComputeLibrary, entry);
    pComputeLibrary->release();
    return kernel;
}

std::shared_ptr<Kernel> MetalDevice::create_kernel_from_library(const std::string &lib_name, const std::string &entry) {
    if (library_map_.empty()) {
        register_library(builtin_lib_name());
    }

    if (auto it = library_map_.find(lib_name); it != library_map_.end()) {
        return std::make_shared<MetalKernel>(_device, it->second, entry);
    } else {
        ERROR("can't find library, register first");
    }
}

void MetalDevice::register_library(const std::string &lib_name) {
    if (auto it = library_map_.find(lib_name); it == library_map_.end()) {
        auto new_lib = load_library(_device, lib_name);
        library_map_.insert({lib_name, new_lib});
    }
}

std::shared_ptr<Counter> MetalDevice::create_counter(uint32_t count) {
    return std::make_shared<MetalCounter>(_device, count);
}

}// namespace vox