#include "RCFilter.h"
#include <cmath>
#include <algorithm>

RCFilter::RCFilter() {}

void RCFilter::setSampleRate(double sampleRate)
{
    fs = sampleRate;

    generateLowpassBank();
    generateHighpassBank();
    generateBandpassBank();
}

void RCFilter::setType(RCFilterType newType)
{
    type = newType;
}

void RCFilter::setCutoff(float newCutoff)
{
    cutoff = newCutoff;
}

// -------------------------------------------------------
// Bank generation
// -------------------------------------------------------
void RCFilter::generateLowpassBank()
{
    lpBank.clear();

    const int N = 50;
    const float fMin = 20.0f;
    const float fMax = (fs * 0.95f * 0.5f);

    for (int i = 0; i < N; ++i)
    {
        float fc = fMin * std::pow(fMax / fMin, (float)i / (N - 1));
        float RC = 1.0f / (2.0f * float(M_PI) * fc);
        float dt = 1.0f / fs;
        float alpha = dt / (RC + dt);

        RCLowpass s;
        s.cutoff = fc;
        s.b = alpha;
        s.a = 1.0f - alpha;
        lpBank.push_back(s);
    }
}

void RCFilter::generateHighpassBank()
{
    hpBank.clear();

    const int N = 50;
    const float fMin = 20.0f;
    const float fMax = (fs * 0.95f * 0.5f);

    for (int i = 0; i < N; ++i)
    {
        float fc = fMin * std::pow(fMax / fMin, (float)i / (N - 1));
        float RC = 1.0f / (2.0f * float(M_PI) * fc);
        float dt = 1.0f / fs;
        float alpha = RC / (RC + dt);

        RCHighpass s;
        s.cutoff = fc;
        s.alpha = alpha;
        hpBank.push_back(s);
    }
}

void RCFilter::generateBandpassBank()
{
    bpBank.clear();

    const int N = 50;
    const float fMin = 20.0f;
    const float fMax = (fs * 0.95f * 0.5f);

    for (int i = 0; i < N; ++i)
    {
        float fc = fMin * std::pow(fMax / fMin, (float)i / (N - 1));
        float RC = 1.0f / (2.0f * float(M_PI) * fc);
        float dt = 1.0f / fs;

        float alpha_lp = dt / (RC + dt);
        float alpha_hp = RC / (RC + dt);

        RCBandpass s;
        s.cutoffLow = fc;
        s.cutoffHigh = fc;

        s.b_hp = 1.0f;
        s.a_hp = alpha_hp;

        s.b_lp = alpha_lp;
        s.a_lp = 1.0f - alpha_lp;

        bpBank.push_back(s);
    }
}

// -------------------------------------------------------
// Index helpers
// -------------------------------------------------------
int RCFilter::findClosestIndexLP(float c) const
{
    float bestDist = 1e9f;
    int best = 0;

    for (int i = 0; i < (int)lpBank.size(); ++i)
    {
        float d = std::abs(lpBank[i].cutoff - c);
        if (d < bestDist)
        {
            bestDist = d;
            best = i;
        }
    }
    return best;
}

int RCFilter::findClosestIndexHP(float c) const
{
    float bestDist = 1e9f;
    int best = 0;

    for (int i = 0; i < (int)hpBank.size(); ++i)
    {
        float d = std::abs(hpBank[i].cutoff - c);
        if (d < bestDist)
        {
            bestDist = d;
            best = i;
        }
    }
    return best;
}

int RCFilter::findClosestIndexBP(float c) const
{
    float bestDist = 1e9f;
    int best = 0;

    for (int i = 0; i < (int)bpBank.size(); ++i)
    {
        float d = std::abs(bpBank[i].cutoffLow - c);
        if (d < bestDist)
        {
            bestDist = d;
            best = i;
        }
    }
    return best;
}

// -------------------------------------------------------
// Sample processing
// -------------------------------------------------------
float RCFilter::processSample(float x)
{
    switch (type)
    {
    case RCFilterType::Lowpass:
    {
        int i = findClosestIndexLP(cutoff);
        auto &s = lpBank[i];
        float y = s.b * x + s.a * s.state;
        s.state = y;
        return y;
    }

    case RCFilterType::Highpass:
    {
        int i = findClosestIndexHP(cutoff);
        auto &s = hpBank[i];

        float y = s.alpha * (s.state + x - s.xPrev);
        s.state = y;
        s.xPrev = x;
        return y;
    }

    case RCFilterType::Bandpass:
    {
        int i = findClosestIndexBP(cutoff);
        auto &s = bpBank[i];

        float y_hp = (x - s.xPrev_hp) + s.a_hp * s.state_hp;
        s.state_hp = y_hp;
        s.xPrev_hp = x;

        float y_lp = s.b_lp * y_hp + s.a_lp * s.state_lp;
        s.state_lp = y_lp;

        return y_lp;
    }
    }

    return x;
}

void RCFilter::reset()
{
    // Lowpass
    for (auto &s : lpBank)
    {
        s.state = 0.0f;
    }

    // Highpass
    for (auto &s : hpBank)
    {
        s.state = 0.0f;
        s.xPrev = 0.0f;
    }

    // Bandpass
    for (auto &s : bpBank)
    {
        s.state_hp = 0.0f;
        s.xPrev_hp = 0.0f;
        s.state_lp = 0.0f;
    }
}