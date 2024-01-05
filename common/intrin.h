#pragma once

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_X86_64
#elif defined(__aarch64__)
#define ARCH_ARM64
#else
#error Unsupported architecture
#endif

#if defined(ARCH_X86_64)

#include <immintrin.h>
#include <cstdint>

#define INTRIN_PAUSE() _mm_pause()

namespace vox {
using float16_t = int16_t;
using float32x4_t = __m128;
}// namespace vox

#elif defined(ARCH_ARM64)

#include <arm_neon.h>

namespace vox {
using float16_t = ::float16_t;
using float32x4_t = ::float32x4_t;
}// namespace vox

#define INTRIN_PAUSE() asm volatile("isb")

#else

#include <thread>
#define INTRIN_PAUSE() std::this_thread::yield()

#endif
