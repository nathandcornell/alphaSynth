#pragma once
#ifndef ASYNTH_VOICE_H
#define ASYNTH_VOICE_H

#include "daisysp.h"

using namespace daisysp;

class EnvelopeSettings {
    private:
        Oscillator osc1;
        Oscillator osc2;
        Oscillator noise;
        Adsr       env;

        bool       hardSync;
        float      glideAmt;
        float      noiseLevel;
        float      osc1Blend;
        float      osc2Blend;
        float      osc1Tune;
        float      osc2Tune;
        int        osc1Octave;
        int        osc2Octave;

    public:
        void Init(
            glide = 0.0f,
            blend = 50.0f,
            sync  = false,
            oct1  = 0,
            oct2  = 0,
            tune1 = 0.0f,
            tune2 = 0.0f,
            wave1 = Oscillator.WAVE_SIN,
            wave2 = Oscillator.WAVE_SIN,
                 );

        void SetBlend();
        void SetEnvelope();
        void SetGlide();
        void SetNoise();
        void SetOsc1Octave();
        void SetOsc2Octave();
        void SetOsc1Tune();
        void SetOsc2Tune();
        void SetOsc1Wave();
        void SetOsc2Wave();
        void SetSync();
}
#endif
