//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_kernel.h"
#include <string>

namespace vox {
MetalKernel::MetalKernel(MTL::Device *device, const std::string &code, const std::string &entry) {
    NS::Error *pError = nullptr;

    MTL::Library *pComputeLibrary = device->newLibrary(NS::String::string(code.c_str(), NS::UTF8StringEncoding), nullptr, &pError);
    if (!pComputeLibrary) {
        __builtin_printf("%s", pError->localizedDescription()->utf8String());
        assert(false);
    }

    MTL::Function *pMandelbrotFn = pComputeLibrary->newFunction(NS::String::string("mandelbrot_set", NS::UTF8StringEncoding));
    _pso = NS::TransferPtr(device->newComputePipelineState(pMandelbrotFn, &pError));
    if (!_pso) {
        __builtin_printf("%s", pError->localizedDescription()->utf8String());
        assert(false);
    }

    pMandelbrotFn->release();
    pComputeLibrary->release();
}

}// namespace vox