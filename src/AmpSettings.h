#pragma once
#ifndef ASYNTH_AMP_SETTINGS_H
#define ASYNTH_AMP_SETTINGS_H

class AmpSettings {
    public:
        static constexpr float DEFAULT_VOLUME = 0.2f;
        static constexpr float DEFAULT_GAIN = 0.9f;

        void Init(float volumeValue = DEFAULT_VOLUME,
                  float gainValue = DEFAULT_GAIN);

        void SetVolume(float volumeValue);
        void SetGain(float gainValue);

        float GetVolume();
        float GetGain();

    private:
        float volumeLevel;
        float gainLevel;
};

#endif
