#include "AmpSettings.h"

void AmpSettings::Init(float volumeValue, float gainValue) {
    volumeLevel = volumeValue;
    gainLevel = gainValue;
}

void AmpSettings::SetVolume(float volumeValue) {
    volumeLevel = volumeValue;
}

void AmpSettings::SetGain(float gainValue) {
    gainLevel = gainValue;
}

float AmpSettings::GetVolume() {
    return volumeLevel;
}

float AmpSettings::GetGain() {
    return gainLevel;
}
