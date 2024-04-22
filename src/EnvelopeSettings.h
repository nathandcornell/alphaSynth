#pragma once
#ifndef ASYNTH_ENV_SETTINGS_H
#define ASYNTH_ENV_SETTINGS_H

class EnvelopeSettings {
    public:
        static constexpr float DEFAULT_ATTACK = 0.1f;
        static constexpr float DEFAULT_DECAY = 0.25f;
        static constexpr float DEFAULT_SUSTAIN = 0.7f;
        static constexpr float DEFAULT_RELEASE = 0.25f;

        void Init(float attackValue  = DEFAULT_ATTACK,
                  float decayValue   = DEFAULT_DECAY,
                  float sustainValue = DEFAULT_SUSTAIN,
                  float releaseValue = DEFAULT_RELEASE);

        float GetAttack();
        float GetDecay();
        float GetSustain();
        float GetRelease();

        void SetAttack(float attackValue);
        void SetDecay(float decayValue);
        void SetSustain(float sustainValue);
        void SetRelease(float releaseValue);

    private:
        float attackTime;
        float decayTime;
        float sustainLevel;
        float releaseTime;
};

#endif
