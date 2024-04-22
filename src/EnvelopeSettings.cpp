#include "EnvelopeSettings.h"

void EnvelopeSettings::Init(float attackValue,
          float decayValue,
          float sustainValue,
          float releaseValue) {
    attackTime   = attackValue;
    decayTime    = decayValue;
    sustainLevel = sustainValue;
    releaseTime  = releaseValue;
}

float EnvelopeSettings::GetAttack() {
    return attackTime;
}

float EnvelopeSettings::GetDecay() {
    return decayTime;
}

float EnvelopeSettings::GetSustain() {
    return sustainLevel;
}

float EnvelopeSettings::GetRelease() {
    return releaseTime;
}

void EnvelopeSettings::SetAttack(float attackValue) {
    attackTime = attackValue;
}

void EnvelopeSettings::SetDecay(float decayValue) {
    decayTime = decayValue;
}

void EnvelopeSettings::SetSustain(float sustainValue) {
    sustainLevel = sustainValue;
}

void EnvelopeSettings::SetRelease(float releaseValue) {
    releaseTime = releaseValue;
}
