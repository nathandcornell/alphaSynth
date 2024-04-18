#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

// Declare a DaisySeed object called hardware, a volume envelope, a midi
// handler, and an oscillator
DaisySeed           hardware;

Adsr            env;
MidiUsbHandler  midi;
Oscillator      osc;

float DEFAULT_VOLUME = 0.2f;
int   CHANNELS = 2;

// Track if a note should be playing:
bool  gate_open = false;
float velocity = 0.0;
bool led_state = false;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t size) {
    float signal, env_out;

    // Fill the buffers with oscillator signals
    for (size_t i = 0; i < size; i++) {
        env_out = env.Process(gate_open);
        osc.SetAmp(env_out);

        signal = osc.Process() * DEFAULT_VOLUME * velocity;

        for (int j = 0; j < CHANNELS; j++) {
            out[j][i] = signal;
        }
    }

}

void MidiCallback(MidiEvent event) {
    switch(event.type) {
        case NoteOn:
            {
                // Apply the value of the midi note to the oscillator
                // frequency
                auto note_event = event.AsNoteOn();
                float note_velocity = note_event.velocity;

                if(note_velocity > 0) {
                    float freq = mtof(note_event.note);
                    osc.SetFreq(freq);

                    velocity = note_velocity / 100;

                    // Open the gate
                    gate_open = true;
                    led_state = true;
                }
            }
            break;

        case NoteOff:
            {
                // Close the gate
                gate_open = false;
                led_state = false;
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
    MidiUsbHandler::Config midi_cfg;
    // Configure the transport to use the internal midi implementation (as 
    // opposed to the peripheral config using pins 2-15)
    midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    // Alternate external (peripheral) config:
    // midi_cfg.transport_config.periph = MidiUsbTransport::Config::EXTERNAL

    // Initialize Midi using this configuration
    midi.Init(midi_cfg);

    // Initialize the oscillator
    osc.Init(hardware.AudioSampleRate());
    osc.SetWaveform(osc.WAVE_SAW);
    osc.SetAmp(0.25f); // default
    osc.SetFreq(440.0f); // default

    // Initialize the envelope
    env.Init(hardware.AudioSampleRate());
    env.SetTime(ADSR_SEG_ATTACK, .1);
    env.SetTime(ADSR_SEG_DECAY, .25);
    env.SetTime(ADSR_SEG_RELEASE, .25);
    env.SetSustainLevel(.7);

    // Start the audio callback
    hardware.StartAudio(AudioCallback);

    // Main loop
    while(true) {
        // Start listening for midi notes
        midi.Listen();
        hardware.SetLed(led_state);

        // If midi events occur:
        while(midi.HasEvents()) {
            // Pop off the midi stack:
            auto event = midi.PopEvent();
            // Run the callback
            MidiCallback(event);
        }
    }
}
