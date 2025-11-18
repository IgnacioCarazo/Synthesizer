#include "Envelope.h"
#include <algorithm>

/**
 * @brief Default constructor initializing internal parameters.
 */
Envelope::Envelope()
    : stage(Stage::Idle),
      sampleRate(44100.0),
      attackTime(0.01f),
      decayTime(0.10f),
      sustainLevel(0.8f),
      releaseTime(0.20f),
      stageCounter(0),
      currentLevel(0.0f),
      releaseStartLevel(0.0f)
{
    setSampleRate(sampleRate);
}

void Envelope::setSampleRate(double sr)
{
    sampleRate = sr;

    // Convert seconds → samples
    attackSamples = std::max(1, int(attackTime * sampleRate));
    decaySamples = std::max(1, int(decayTime * sampleRate));
    releaseSamples = std::max(1, int(releaseTime * sampleRate));
}

void Envelope::setParameters(float a, float d, float s, float r)
{
    attackTime = a;
    decayTime = d;
    sustainLevel = s;
    releaseTime = r;

    setSampleRate(sampleRate);
}

void Envelope::enterAttack()
{
    stage = Stage::Attack;
    stageCounter = 0;
    currentLevel = 0.0f;
}

void Envelope::enterRelease()
{
    releaseStartLevel = currentLevel;
    stage = Stage::Release;
    stageCounter = 0;
}

void Envelope::reset()
{
    stage = Stage::Idle;
    stageCounter = 0;
    currentLevel = 0.0f;
}

bool Envelope::isActive() const
{
    return stage != Stage::Idle;
}

float Envelope::processSample()
{
    switch (stage)
    {
    case Stage::Idle:
        return 0.0f;

    case Stage::Attack:
        stageCounter++;
        currentLevel = float(stageCounter) / float(attackSamples);

        if (stageCounter >= attackSamples)
        {
            stage = Stage::Decay;
            stageCounter = 0;
            currentLevel = 1.0f;
        }
        break;

    case Stage::Decay:
        stageCounter++;
        currentLevel = 1.0f - (1.0f - sustainLevel) *
                                  (float(stageCounter) / float(decaySamples));

        if (stageCounter >= decaySamples)
        {
            stage = Stage::Sustain;
            currentLevel = sustainLevel;
            stageCounter = 0;
        }
        break;

    case Stage::Sustain:
        currentLevel = sustainLevel;
        break;

    case Stage::Release:
        stageCounter++;
        currentLevel = releaseStartLevel *
                       (1.0f - float(stageCounter) / float(releaseSamples));

        if (stageCounter >= releaseSamples || currentLevel <= 1e-6f)
        {
            currentLevel = 0.0f;
            reset();
        }
        break;
    }

    return currentLevel;
}
