# MIDI2CV
Convert MIDI messages to control voltage signals for use with a modular synthesizer

---


##Contents
- [Motivation](#motivation)
- [Plan](#plan)
- [Details](#details)
	- [Digital-to-Analog Converter Options](#digital-to-analog-converter-options)
		- [Filtered PWM](#filtered-pwm)
			- [1-pole LPF math](#1-pole-lpf-math)
			- [1-pole LPF for Pitch CV](#1-pole-lpf-for-pitch-cv)
			- [Sallen-Key Filter](#sallen-key-filter)
			- [2-pole Sallen-Key LPF for Pitch CV](#2-pole-sallen-key-lpf-for-pitch-cv)
- [Results](#results)
- [Further Plans](#further-plans)

---

##Motivation

Today, most music keyboards and synthesizers use [MIDI](https://www.midi.org/) (Musical Instrument Digital Interface), a digital communication protocal almost universally accepted in the electronic music industry. However, before MIDI's emergence in the 1980s, analog synthesizers often used _control voltage_ (CV) signals for control (surprisingly!) and communication.

I recently started building a modular synthesizer, using the [Music From Outer Space](http://musicfromouterspace.com/index.php?MAINTAB=SYNTHDIY&VPW=1910&VPH=844) (MFOS) series of modules as a starting point, and I wanted to play it with my keyboards and the DAW (digital audio workstation) on my computer. 

With a device to convert MIDI messages to CV signals, I could play my modular synth using any MIDI-capable keyboard or my computer. While using a keyboard to play the modular synth makes improvising and jamming easier and more fun, I'm especially excited about the possibilty of writing music on my computer and playing it back on the modular synth, because this will greatly expand my toolset for writing and recording music.

##Plan

This project is essentially a specialized digital-to-analog converter (DAC). While there are already many options for MIDI-to-CV conversion, I wanted to make my own converter. 

In retrospect, this was a great decision
* I learned a lot more about MIDI and about the CV method it replaced.
* I gained an appreciation for the level of complexity required to generate some very common synth sounds (like [this one](https://youtu.be/SwYN7mTi6HM)!). I also _really_ gained an appreciation for the complexity required for polyphony in synthesizers!
* Designing and building this converter gave me an opportunity to apply what I'd learned in my coursework and improve my troubleshooting skills.

While there are [some](http://musicfromouterspace.com/index.php?MAINTAB=SYNTHDIY&VPW=1910&VPH=844) MIDI-to-CV converters out there that do the job without resorting to a microcontroller, I wanted to go the microcontroller route, for a few reasons:
* it's easier to reprogram a &mu;C than it is to rebuild a circuit
* more documentation available
* more room for error/experimentation
* I already had an Arduino Uno on hand

Given the last bullet point above, my converter could be represented like this:

![Block Diagram](/images/blockdiagram.png)

-------

##Details

Sort of. Rather than try to explain the MIDI communication protocal myself, here are some other websites and people who've done a great job explaining it:

* This [Instructable](http://www.instructables.com/id/What-is-MIDI/) gives a comprehensive and comprehensible overview of MIDI. The author also wrote an [Instructable](http://www.instructables.com/id/Send-and-Receive-MIDI-with-Arduino/) about using an Arduino to send and receive MIDI messages; this particular Instructable was an invaluable resource throughout my project.
* This [blog post](http://www.notesandvolts.com/2015/02/midi-and-arduino-build-midi-input.html) also gives a clear description of the MIDI input circuit.

Briefly, MIDI uses bytes to convey a wide array of musical information, such as
* **note On/Off** (are you pressing a key?)
* **pitch** (what key are you pressing?)
* **velocity** (how hard are you pressing the key?)
* **pitchbend** (most MIDI keyboards have a control wheel for bending a note up or down)
* **modulation** (most MIDI keyboards also have a modulation wheel that can be assigned to modulate another parameter, such as vibrato, filter cutoff frequency, etc.)
* **sustain** (are you pressing down on a sustain pedal, like on a piano?)

There are many, many more MIDI messages available, but these are some of the most common and are also the ones I use most often when playing.

CV conveys the same information as MIDI but does it differently, using both analog and logic-level signals.
* MIDI **note on/off** messages map to logic-level **GATE** and **TRIGGER** CV signals.
* Pretty much every other MIDI message maps to an analog CV signal
    * pitch
    * velocity
    * modulation
The beauty of CV is that these analog CV signals are interchangeable. This flexibility opens more possibilities for sound design. Here are just a few examples:
* velocity CV -> filter cutoff frequency (sound brightens with harder keypresses)
* pitch CV -> oscillator mix (use pitch to blend between two different waveforms)
* modulation -> low frequency oscillator (LFO) amplitude (LFOs are another signal generator that will also output CV signals and can modulate more aspects of your sound)

###Digital-to-Analog Converter Options

I considered two methods for converting the Arduino's digital output to analog CV signals, and I actually wound up using both methods.

1. Dedicated DAC chip
2. Pulse Width Modulation (PWM) + Low-Pass Filter (LPF).

Before this project, I never used a dedicated DAC, and because I initially felt hesitant about that method, I decided to see if I could do all my digital-to-analog conversion using filtered PWM.

####Filtered PWM

The Arduino's analogWrite() function uses PWM to approximate a voltage between 0V and 5V. While some devices (LEDs, DC motors) can be controlled directly by PWM, the synthesizer is not one of those devices (unless you want to generate some interesting 'talking-robot' sounds). For PWM to interface nicely with my modular, it had to be low-pass filtered to 'smooth out' the signal. The simplest LPF is a resistor-capacitor (RC) circuit, as shown below, so I started there, with the expectation that a more sophisticated filter might be necessary.

![RC circuit](/images/lpf_1pole_circuit.png)

A buffer is necessary to prevent whatever is connected to the filter's output from loading down the filter. This can be done with a voltage-follower, shown below with the original RC circuit.

![Buffered RC circuit](/images/lpf_1pole_buffer.png)

Note: I might use "bandwidth frequency" and "cutoff frequency" interchangeably, because for low-pass filters, they mean the same thing.

An RC circuit is a 1-pole filter, which means that above its cutoff frequency, the filter has a slope of -20 dB per decade (i.e. if the filter attenuates a 100 Hz input signal by 20 dB, it will attenuate a 1000 Hz input signal by 40 dB). The Bode plot below illustrates the frequency response of this circuit.

![RC bode](/images/lpf_1pole.png)

This Bode plot is also quite helpful for understanding my design process; the process applies for higher-order filters, too, and the only difference is that the Bode plot would look different (it would have a steeper slope).

For clarity, I want to list the parameters relevant to using an RC circuit to filter a PWM signal from the Arduino.
* Arduino PWM frequency
* LPF cutoff frequency
* PWM amplitude
* Ripple amplitude (amplitude of filtered PWM signal)

For the RC circuit described above, the cutoff frequency is a function of the resistor and capacitor values, R and C. Also, the frequency and amplitude of the Arduino's PWM signal are fixed: 490 Hz (980 Hz on pins 5 and 6) and 5V, respectively.

The design procedure then is this:
```
1. Determine a desirable ripple amplitude.
2. Given the filter's -20 dB/dec slope, calculate the cutoff frequency required to achieve the ripple amplitude.
```

#####1-pole LPF math
  The transfer function for an RC LPF is

  ![lpf_1pole_TF](/images/equations/lpf_1pole_TF.JPG)		[1]

  The bandwidth frequency or cutoff frequency of a filter is the frequency at which the filter's output is 3 dB less than its DC level. For an RC LPF, this is simply -3 dB.

  ![lpf_1pole_BW_TF](/images/equations/lpf_1pole_BW_TF.JPG)		[2]

  Solving for &omega; gives the cutoff frequency as a function of R and C:

  ![lpf_1pole_BW_freq](/images/equations/lpf_1pole_BW_freq.JPG)		[3]

  Define RC as a time constant &tau;, and the equation above can be rewritten as

  ![lpf_1pole_BW_tau](/images/equations/lpf_1pole_BW_tau.JPG)		[4]

Below, I'll describe how I followed this procedure to design a filter to smooth out a CV signal which could be used for pitch control:

#####1-pole LPF for Pitch CV
  **Spoiler Alert!** This design doesn't work (as I'll explain below). That's why there are two section below labeled [Sallen-Key Filter](#sallen-key-filter) and [2-pole Sallen-Key LPF for Pitch CV](#2-pole-sallen-key-lpf-for-pitch-cv). Nevertheless, it's important to understand why this filter didn't work and why a more sophisticated filter was required.

  Recall that step 1 of the design procedure required me to determine a desirable ripple amplitude. Since I was designing a filter to generate a pitch CV signal, this ripple amplitude corresponded to a variability in pitch, kind of like a vibrato, so I had to decide how much vibrato I was willing to tolerate (not much).
 
  * I measured pitch variation in _cents_. A cent is 1/100th of a semitone (the difference in pitch between two adjacent keys on a piano, like C and C#).
  * I decided to tolerate +/- 5 cents variation in pitch, or a magnitude of 10 cents.
  * The voltage-controlled oscillator (VCO) of my synth is calibrated to 1V/octave, so two pitch CV signals 1V apart will make the VCO generate sounds 1 octave apart.
  * There are 12 semitones in an octave, so 1V/octave is equivalent to 1/12V per semitone.
  * Thus, 1 cent equals 0.8333 mV, and 10 cents equals 8.333 mV.
  * ![pitch_var_magnitude](/images/equations/pitch_var_magnitude.JPG)
  * Recall that for most pins on the Arduino, the PWM frequency is 490 Hz or 3079 rad/s, and the PWM signal switches between 0V and 5V.
  * Solving the equation above for time constant &tau; gives &tau; = 0.196 s.
  * Plugging this &tau; into equation 3 above gives a cutoff frequency of 5.11 rad/s or 0.813 Hz. That's pretty low!
  * Using the MATLAB script [lpf1p.m](/matlab/lpf1p.m), I examined the frequency response and 0-5V step response of this filter, for a PWM frequency of 490 Hz, both of which are plotted below. Using the MATLAB function [stepinfo()](https://www.mathworks.com/help/control/ref/stepinfo.html), I found the settling time of this filter to be about 0.76 seconds. Considering the input PWM frequency is 490 Hz, that's an unacceptably slow settling time. Even if I used pins 5 or 6 (980 Hz PWM instead of 490 Hz), the settling time would have been around 0.38 s, still way too slow.

  ![1-Pole LPF Bode](/images/matlab_plots/lpf_1pole_bode.png)
  ![1-Pole LPF 0-5V Step](/images/matlab_plots/lpf_1pole_5V_step_response.png)

  If I was determined to use a 1-pole RC LPF, I would have to tolerate more pitch variability. Instead, I decided to try a 2-pole filter, which would have a -40 db/dec slope instead of the RC LPF's -20 db/dec slope.

#####Sallen-Key Filter
A [Sallen-Key filter](https://en.wikipedia.org/wiki/Sallen%E2%80%93Key_topology) is an active 2-pole filter that can be designed to work as a cascade of two 1-pole RC filters.

![Sallen-Key LPF](/images/sallen-key_lpf.png)

The generic transfer function for a Sallen-Key filter is

![sallen-key_TF_generic](/images/equations/sallen-key_TF_generic.JPG)		[5]

To make a Sallen-Key LPF,

![sallen-key_LPF_Zs](/images/equations/sallen-key_LPF_Zs.JPG)		[6]

The generic transfer function above becomes

![sallen-key_LPF_TF](/images/equations/sallen-key_LPF_TF.JPG)		[7]

where

![sallen-key_LPF_comp2canon](/images/equations/sallen-key_LPF_comp2canon.JPG)		[8]

#####2-pole Sallen-Key LPF for Pitch CV

---
##Results
##Further Plans

--------

![MIDI circuit](/images/midicircuit.gif)