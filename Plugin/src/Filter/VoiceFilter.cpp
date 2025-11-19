#include "VoiceFilter.h"

VoiceFilter::VoiceFilter() {}

void VoiceFilter::setSampleRate(double sampleRate)
{
    filter.setSampleRate(sampleRate);
}

void VoiceFilter::setParameters(RCFilterType type, float cutoff)
{
    filter.setType(type);
    filter.setCutoff(cutoff);
}

float VoiceFilter::processSample(float x)
{
    return filter.processSample(x);
}

void VoiceFilter::reset()
{
    filter.reset();
}
