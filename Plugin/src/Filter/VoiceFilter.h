#pragma once
#include "RCFilter.h"

class VoiceFilter
{
public:
    VoiceFilter();
    void setSampleRate(double sampleRate);
    void setParameters(RCFilterType type, float cutoff);
    float processSample(float x);

    void reset();

private:
    RCFilter filter;
};
