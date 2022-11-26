# Beat Generator

Beat generation program with GPIO buttons using USB Audio and ALSA.

# Hardware

* Operates successfully on a Debian 11/Bullseye image (September 2022).

* For pin setup, refer to the BBG pin map at https://wiki.seeedstudio.com/BeagleBone_Green_Wireless/ for all relevant GPIO pins. GPIO buttons are connected to pins P8_15 through P8_18 for this program.

* GPIO buttons are to be connected to ground with a 1k resistor in addition to the 3.3V supply on the BBG.

# Setup/Configuration 

* Configure pins on the BBG for I2C and GPIO by running the following commands on the target:
```
config-pin i2c PX_XX
```
and 
```
config-pin gpio PX_XX
```
respectively, where X_XX is the relevant pin (eg. P9_17).

* This program uses ALSA to handle PCM (pulse-code modulation) audio output via USB connection to the BBG. To configure the target, you must install usbutils (for lsusb), alsa-utils, and i2c-tools for the 8x8 LED matrix. Run

```
sudo apt-get install usbutils alsa-utils i2c-tools
```
on your host.

* In order to play back PCM audio, install `asound` (which is required to compile this program) via
```
sudo apt-get install libasound2-dev
```
on your host.

* Move the libasound.so file to your target's running directory.

# Implementation

* This program is written entirely in C.
* This program uses a `makefile` and generates an exectutable on the target.

# Possible Improvements
### IMPORTANT NOTE: This program should only operate smoothly if you supply it with .wav files (refer to the signal handling and /sounds/ folder).
* It may be worthwhile to extend this usage to encompass multiple forms of audio encoding.
