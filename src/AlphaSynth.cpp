#include "daisy_seed.h"
#include "daisysp.h"

// libDaisy alias
using namespace daisy;

// Declare a DaisySeed object called hardware
// and an Oscillator called osc
DaisySeed           hardware;
daisysp::Oscillator osc;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t size)
{
    float sine_signal = osc.Process();

    // Fill the buffers with oscillator samples
    std::fill(&out[0][0], &out[0][size], sine_signal);
    std::fill(&out[1][0], &out[1][size], sine_signal);
}


int main(void)
{
    hardware.Init();
    osc.Init(hardware.AudioSampleRate());
    hardware.StartAudio(AudioCallback);

    // Loop endlessly
    while(true) {}
}
