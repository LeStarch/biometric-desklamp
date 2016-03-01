# Fingerprint Controlled Biometric Desk Lamp
All the files required to build the finger-print enabled LED super lamp.

Files include Arduino code, KiCad files, etc.

## Functional Diagram

![Lamp Functional Design](https://docs.google.com/drawings/d/1FzfYlJimZiMFY7TzZ2o8irCIButJ6qyPn2TqXGDPiBw/pub?w=1375&h=750,"Functional design diagram")

## 5V1A Power Supply
Used to power the Arduino and the cooling fan, this power supply is slightly under powered due to the narrow trace width between the components. Thus it is capible of supplying ~600mA. This is appears to be sufficient for the Teensy 3.2, finger-print scanner, and LED cooling fan.

**Needed improvments:** Trace widths need to be increased to reliably supply the advertised 1A power. 100k resistor should be replaced with surface mount equivalent.

## LED Driver
LEDs must be driven at a constant current (not voltage) in order to maximize their life expectancy. This driver produces a 1A constant ~30V supply for a high-powered LED module.  It is based on the Texas Instruments LM-3409 chip.  See: https://github.com/LeStarch/lm3409-calc for how the components were calculated.

**Needed Improvments:** Trace widths may be too small to reliably cary 1A for LED driving. In practice, the lamp is stable, but trace widths should be increased in "an abundance of caution".

P-type MOSFET has a reversed layout and thus should be corrected in version 2.

## Nano Code (and now Teensy as well)
Code written for the Arduino Nano, which interfaces with the fingerprint reader and controls PWM signals used to drive the cooling fan and LED driver enable line.
