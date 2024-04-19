#include "daisy_seed.h"
#include "daisysp.h"

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

float DEFAULT_VOLUME = 0.2f;
int   CHANNELS = 2;
float DEFAULT_GAIN = 0.9f;

// Track if a note should be playing:
bool  gateOpen = false;
float velocity = 0.0;
bool ledState = false;
float volume = DEFAULT_VOLUME;

float SaturatedSignal(float signal, float gain = DEFAULT_GAIN) {
    return tanh(signal * gain);
}

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t size) {
    float signal, envOut, saturatedSignal;

    // Fill the buffers with oscillator signals
    for (size_t i = 0; i < size; i++) {
        envOut = env.Process(gateOpen);
        osc.SetAmp(envOut);

        signal = osc.Process() * volume * velocity;
        saturatedSignal = SaturatedSignal(signal);

        for (int j = 0; j < CHANNELS; j++) {
            out[j][i] = saturatedSignal;
        }
    }

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

int main(void) {
    hardware.Configure();
    hardware.Init();

    // Initialize a midi configuration
    MidiUsbHandler::Config midiConfig;
    // Configure the transport to use the internal midi implementation (as 
    // opposed to the peripheral config using pins 2-15)
    midiConfig.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    // Alternate external (peripheral) config:
    // midiConfig.transport_config.periph = MidiUsbTransport::Config::EXTERNAL

    // Initialize Midi using this configuration
    midi.Init(midiConfig);

    // Initialize the oscillator
    osc.Init(hardware.AudioSampleRate());
    osc.SetWaveform(osc.WAVE_SIN);
    osc.SetAmp(0.25f); // default
    osc.SetFreq(440.0f); // default

    // Initialize the envelope
    env.Init(hardware.AudioSampleRate());
    env.SetTime(ADSR_SEG_ATTACK, .1);
    env.SetTime(ADSR_SEG_DECAY, .25);
    env.SetTime(ADSR_SEG_RELEASE, .25);
    env.SetSustainLevel(.7);

    // Initialize the volume knob
    AdcChannelConfig adcConfig;
    // Configure pin 21 as an ADC input
    adcConfig.InitSingle(A0);

    //Initialize the adc with the config we just made
    hardware.adc.Init(&adcConfig, 1);
    //Start reading values
    hardware.adc.Start();

    // Start the audio callback
    hardware.StartAudio(AudioCallback);

    // Main loop
    while(true) {
        // Start listening for midi notes
        midi.Listen();
        hardware.SetLed(ledState);
        volume = hardware.adc.GetFloat(0);

        // If midi events occur:
        while(midi.HasEvents()) {
            // Pop off the midi stack:
            auto event = midi.PopEvent();
            // Run the callback
            MidiCallback(event);
        }
    }
}
