# Fingerprint Controlled Biometric Desk Lamp
All the files required to build the finger-print enabled LED super lamp.

Files include Arduino code, KiCad files, etc.

## 5V1A Power Supply
Used to power the Arduino Nano and the cooling fan, this power supply is slightly under powered due to the narrow trace width between the components.

## LED Driver
LEDs must be driven at a constant current (not voltage) in order to maximize their life expectancy. This driver produces a 1A constant ~30V supply for a high-powered LED module.  It is based on the Texas Instruments LM-3409 chip.  See: https://github.com/LeStarch/lm3409-calc for how the components were calculated.

## Nano Code
Code written for the Arduino Nano, which interfaces with the fingerprint reader and controls PWM signals used to drive the cooling fan and LED driver enable line.
