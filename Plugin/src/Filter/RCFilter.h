#pragma once
#include <vector>

enum class RCFilterType
{
    Lowpass,
    Highpass,
    Bandpass
};

struct RCLowpass
{
    float cutoff;
    float b;
    float a;
    float state = 0.0f;
};

struct RCHighpass
{
    float cutoff;
    float alpha;
    float state = 0.0f;
    float xPrev = 0.0f;
};

struct RCBandpass
{
    float cutoffLow;
    float cutoffHigh;

    // HP stage
    float b_hp;
    float a_hp;
    float state_hp = 0.0f;
    float xPrev_hp = 0.0f;

    // LP stage
    float b_lp;
    float a_lp;
    float state_lp = 0.0f;
};

class RCFilter
{
public:
    RCFilter();
    void setSampleRate(double sampleRate);
    void setType(RCFilterType newType);
    void setCutoff(float newCutoff);
    float processSample(float x);

    void reset();

private:
    double fs = 44100.0;
    RCFilterType type = RCFilterType::Lowpass;
    float cutoff = 1000.0f;

    std::vector<RCLowpass> lpBank;
    std::vector<RCHighpass> hpBank;
    std::vector<RCBandpass> bpBank;

    int findClosestIndexLP(float c) const;
    int findClosestIndexHP(float c) const;
    int findClosestIndexBP(float c) const;

    void generateLowpassBank();
    void generateHighpassBank();
    void generateBandpassBank();
};
