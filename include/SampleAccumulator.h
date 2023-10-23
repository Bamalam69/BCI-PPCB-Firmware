#ifndef SAMPLE_ACCUMULATOR_H_
#define SAMPLE_ACCUMULATOR_H_

#include <Arduino.h>
#include "SampleData.h"
#include <queue>

class SampleAccumulator
{
public:
    SampleAccumulator();
    // Non-copyable:
    SampleAccumulator(const SampleAccumulator &) = delete;
    SampleAccumulator &operator=(const SampleAccumulator &) = delete;

    void addSample(Sample sample);
    std::queue<Sample> &getSamples();
    uint8_t hasSamples() { return _samples.size(); }
    // void reset();

private:
    std::queue<Sample> _samples;
};

#endif // SAMPLE_ACCUMULATOR_H_