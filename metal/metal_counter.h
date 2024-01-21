//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "metal_device.h"
#include "rhi/counter.h"

namespace vox {
class MetalCounter : public Counter {
public:
    MetalCounter(MTL::Device *device, uint32_t sampleCount,
                 MTL::CommonCounterSet counterSetName = MTL::CommonCounterSetTimestamp);

    static bool is_counter_support(MTL::CounterSet *counterSet, MTL::CommonCounter counterName);

    MTL::CounterSet *get_counter_set(MTL::CommonCounterSet counterSetName);

    std::vector<MTL::CounterSamplingPoint> sampling_boundaries();

public:
    MTL::CounterSampleBuffer *get_handle() const;

    void update_start_times() override;

    void update_final_times() override;

    double calculate_elapsed_seconds_between(uint32_t begin, uint32_t end) override;

private:
    [[nodiscard]] double absolute_time_in_microseconds(MTL::Timestamp timestamp) const;

    [[nodiscard]] double micro_seconds_between(MTL::Timestamp begin, MTL::Timestamp end) const;

private:
    MTL::Device *device;
    MTL::CounterSampleBuffer *buffer;

    double cpuStart{};
    double gpuStart{};
    double cpuTimeSpan{};
    double gpuTimeSpan{};
};

}// namespace vox