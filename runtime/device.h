//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <string>

namespace vox {
class Stream;
class Kernel;

using MTLFCList = std::vector<std::tuple<const void *, MTL::DataType, NS::UInteger>>;

class Device {
public:
    Device();

    ~Device();

    MTL::Device *handle();

    std::string name();

    Stream &stream(uint32_t index);

private:
    friend class Kernel;

private:
    void register_library(
        const std::string &lib_name,
        const std::string &lib_path);

    void register_library(
        const std::string &lib_name,
        const std::function<std::string(const std::string &)> &lib_path_func);

    MTL::Library *get_library_cache_(const std::string &name);

private:
    static MTL::Function *get_function_(const std::string &name, MTL::Library *mtl_lib);

    static MTL::Function *get_function_(
        const std::string &name,
        const std::string &specialized_name,
        const MTLFCList &func_consts,
        MTL::Library *mtl_lib);

    static MTL::Function *get_function(
        const std::string &base_name,
        MTL::Library *mtl_lib,
        const std::string &specialized_name = "",
        const MTLFCList &func_consts = {});

    MTL::Function *get_function(
        const std::string &base_name,
        const std::string &lib_name = "metal_kernel",
        const std::string &specialized_name = "",
        const MTLFCList &func_consts = {});

private:
    static MTL::LinkedFunctions *get_linked_functions_(
        const std::vector<MTL::Function *> &funcs);

    MTL::ComputePipelineState *get_kernel_(
        const std::string &name,
        const MTL::Function *mtl_function);

    MTL::ComputePipelineState *get_kernel_(
        const std::string &name,
        const MTL::Function *mtl_function,
        const MTL::LinkedFunctions *linked_functions);

    MTL::ComputePipelineState *get_kernel(
        const std::string &base_name,
        MTL::Library *mtl_lib,
        const std::string &hash_name = "",
        const MTLFCList &func_consts = {},
        const std::vector<MTL::Function *> &linked_functions = {});

    MTL::ComputePipelineState *get_kernel(
        const std::string &base_name,
        const std::string &lib_name = "metal_kernel",
        const std::string &hash_name = "",
        const MTLFCList &func_consts = {},
        const std::vector<MTL::Function *> &linked_functions = {});

private:
    MTL::Library *get_library_(const std::string &source_string);

    MTL::Library *get_library_(const MTL::StitchedLibraryDescriptor *desc);

    MTL::Library *get_library(
        const std::string &name,
        const std::string &source_string,
        bool cache = true);

    MTL::Library *get_library(
        const std::string &name,
        const MTL::StitchedLibraryDescriptor *desc,
        bool cache = true);

private:
    MTL::Device *_device{nullptr};
    std::unordered_map<std::string, MTL::ComputePipelineState *> kernel_map_;
    std::unordered_map<std::string, MTL::Library *> library_map_;
    std::unordered_map<uint32_t, Stream *> stream_map_;
};

Device &device();

Stream &stream(uint32_t index);

void synchronize(bool wait = false, uint32_t index = 0);

}// namespace vox