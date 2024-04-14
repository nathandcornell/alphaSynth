# alphaSynth
First attempt at a synth using the Daisy Seed hardware platform and DaisySP Libraries

## Getting Started

### Source
Clone this repo with all submodules:
```sh
git clone --recursive https://github.com/nathandcornell/alphaSynth
```
The submodules include our DaisySP and libDaisy dependencies, plus _their_ dependencies, so it's very important.

### Dependencies
Go to each dependency folder and build them:

```sh
cd <root folder>/alphaSynth/lib/libDaisy
make

cd <root folder>/alphaSynth/lib/DaisySP
make
```

(TODO: Add this step to the makefile)

### Hardware

Create a circuit including your Daisy Seed and a stereo (TRS) output jack.

tip    -> pin 18
ring   -> pin 19
sleeve -> pin 20 (Audio Ground)

Also attach your Audio Ground (pin 20) to the Digital Ground (pin 40).

(TODO: Circuit diagram)

## Building

To build the synth, just run `make`

## Deploying

To upload to your Daisy Seed, first put it into bootloader mode, then run

```sh
make program-dfu
```

## Usage

Plug a Midi controller into your computer
