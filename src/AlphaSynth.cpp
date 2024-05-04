#include "daisy_seed.h"
#include "daisysp.h"
#include "AmpSettings.h"
#include "EnvelopeSettings.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;
using namespace std;

// Declare a DaisySeed object called hardware, a volume envelope, a midi
// handler, and an oscillator
DaisySeed           hardware;

Adsr            env;
MidiUsbHandler  midi;
Oscillator      osc;

// Amplifier settings
AmpSettings ampSettings;

// Envelope settings
EnvelopeSettings envSettings;

int        CHANNELS = 2;
int ADC_CHANNEL_QTY = 5;

// Track if a note should be playing:
bool  gateOpen = false;
bool  ledState = false;
float velocity = 0.0;
enum AdcChannels {
    volumeLevel  = 0,
    attackTime  = 1,
    decayTime   = 2,
    sustainLevel = 3,
    releaseTime = 4,
};

float SaturatedSignal(float signal) {
    return tanh(signal * ampSettings.GetGain());
}

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t size) {
    float signal, envOut, saturatedSignal;

    // Fill the buffers with oscillator signals
    for (size_t i = 0; i < size; i++) {
        envOut = env.Process(gateOpen);
        osc.SetAmp(envOut);

        signal = osc.Process() * ampSettings.GetVolume() * velocity;
        saturatedSignal = SaturatedSignal(signal);

        for (int j = 0; j < CHANNELS; j++) {
            out[j][i] = saturatedSignal;
        }
    }

}

void UpdateEnvelope() {
  // Adjust ADSR settings as needed
  env.SetTime(ADSR_SEG_ATTACK, envSettings.GetAttack());
  env.SetTime(ADSR_SEG_DECAY, envSettings.GetDecay());
  env.SetSustainLevel(envSettings.GetSustain());
  env.SetTime(ADSR_SEG_RELEASE, envSettings.GetRelease());
}

void MidiCallback(MidiEvent event) {
    switch(event.type) {
        case NoteOn:
            {
                // Apply the value of the midi note to the oscillator
                // frequency
                auto noteEvent = event.AsNoteOn();
                float noteVelocity = noteEvent.velocity;

                if(noteVelocity > 0) {
                    float freq = mtof(noteEvent.note);
                    osc.SetFreq(freq);
                    velocity = noteVelocity / 100;

                    // Apply envelope changes
                    UpdateEnvelope();

                    // Open the gate
                    gateOpen = true;
                    ledState = true;
                }
            }
            break;
        case NoteOff:
            {
                // Close the gate
                gateOpen = false;
                ledState = false;
            }
            break;

            // Ignore non-note events (for now)
        default: break;
    }
}

void InitializeMidi() {
    MidiUsbHandler::Config midiConfig;
    // Configure the transport to use the internal midi implementation (as 
    // opposed to the peripheral config using pins 2-15)
    midiConfig.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    // Alternate external (peripheral) config:
    // midiConfig.transport_config.periph = MidiUsbTransport::Config::EXTERNAL

    // Initialize Midi using this configuration
    midi.Init(midiConfig);
}

void InitializeOscillator() {
    osc.Init(hardware.AudioSampleRate());
    osc.SetWaveform(osc.WAVE_SIN);
    osc.SetAmp(0.25f); // default
    osc.SetFreq(440.0f); // default
}

void InitializeEnvelope() {
    env.Init(hardware.AudioSampleRate());
    env.SetTime(ADSR_SEG_ATTACK, envSettings.GetAttack());
    env.SetTime(ADSR_SEG_DECAY, envSettings.GetDecay());
    env.SetSustainLevel(envSettings.GetSustain());
    env.SetTime(ADSR_SEG_RELEASE, envSettings.GetRelease());
}

void InitializeADC() {
    // Initialize the knobs
    AdcChannelConfig adcConfig[ADC_CHANNEL_QTY];
    // Configure pins 21-25 as ADC inputs
    adcConfig[volumeLevel].InitSingle(A0);
    adcConfig[attackTime].InitSingle(A1);
    adcConfig[decayTime].InitSingle(A2);
    adcConfig[sustainLevel].InitSingle(A3);
    adcConfig[releaseTime].InitSingle(A4);

    //Initialize the adc with the config we just made
    hardware.adc.Init(adcConfig, ADC_CHANNEL_QTY);
    //Start reading values
    hardware.adc.Start();
}

void ApplyADCUpdates() {
    ampSettings.SetVolume(hardware.adc.GetFloat(volumeLevel));
    envSettings.SetAttack(fmap(hardware.adc.GetFloat(attackTime), 0.01, 10));
    envSettings.SetDecay(fmap(hardware.adc.GetFloat(decayTime), 0.01, 10));
    envSettings.SetSustain(fmap(hardware.adc.GetFloat(sustainLevel), 0.1, 1));
    envSettings.SetRelease(fmap(hardware.adc.GetFloat(releaseTime), 0.1, 20));
}

int main(void) {
    // Initialize the hardware
    hardware.Configure();
    hardware.Init();

    // Initialize the ADSR settings:
    envSettings.Init();

    // Initialize the Amp settings:
    ampSettings.Init();

    InitializeMidi();
    InitializeOscillator();
    InitializeEnvelope();
    InitializeADC();

    // Start the audio callback
    hardware.StartAudio(AudioCallback);

    // Main loop
    while(true) {
        // Start listening for midi notes
        midi.Listen();
        hardware.SetLed(ledState);

        ApplyADCUpdates();

        // If midi events occur:
        while(midi.HasEvents()) {
            // Pop off the midi stack:
            auto event = midi.PopEvent();
            // Run the callback
            MidiCallback(event);
        }
    }
}
