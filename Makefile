# Project Name
TARGET = AlphaSynth

# Sources
CPP_SOURCES += src/AmpSettings.cpp
CPP_SOURCES += src/EnvelopeSettings.cpp
CPP_SOURCES += src/AlphaSynth.cpp

# Library Locations
LIBDAISY_DIR = lib/libDaisy
DAISYSP_DIR = lib/DaisySP

# Core location, and generic makefile
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
