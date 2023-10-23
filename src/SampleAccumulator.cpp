#include "SampleAccumulator.h"

SampleAccumulator::SampleAccumulator()
{
}

void SampleAccumulator::addSample(Sample sample)
{
    _samples.push(sample);
}

std::queue<Sample> &SampleAccumulator::getSamples()
{
    // noInterrupts();
    // std::queue<Sample> samples = _samples;
    // std::queue<Sample> empty;
    // _samples.swap(empty);
    // interrupts();
    return _samples;
}
