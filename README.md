# MIDI2CV
Convert MIDI messages to control voltage signals for use with a modular synthesizer

---

###Motivation

Today, most music keyboards and synthesizers use [MIDI](https://www.midi.org/) (Musical Instrument Digital Interface), a digital communication protocal almost universally accepted in the electronic music industry. However, before MIDI's emergence in the 1980s, analog synthesizers often used _control voltage_ (CV) signals for control (surprisingly!) and communication.

I recently started building a modular synthesizer, using the [Music From Outer Space](http://musicfromouterspace.com/index.php?MAINTAB=SYNTHDIY&VPW=1910&VPH=844) (MFOS) series of modules as a starting point, and I wanted to play it with my keyboards and the DAW (digital audio workstation) on my computer. 

With a device to convert MIDI messages to CV signals, I could play my modular synth using any MIDI-capable keyboard or my computer. While using a keyboard to play the modular synth makes improvising and jamming easier and more fun, I'm especially excited about the possibilty of writing music on my computer and playing it back on the modular synth, because this will greatly expand my toolset for writing and recording music.

###Plan

This project is essentially a specialized digital-to-analog converter (DAC). While there are already many options for MIDI-to-CV conversion, I wanted to make my own converter. 

In retrospect, this was a great decision
* I learned a lot more about MIDI and about the CV method it replaced.
* I gained an appreciation for the level of complexity required to generate some very common synth sounds (like [this one](https://youtu.be/SwYN7mTi6HM)!). I also _really_ gained an appreciation for the complexity required for polyphony in synthesizers!
* Designing and building this converter gave me an opportunity to apply what I'd learned in my coursework and improve my troubleshooting skills.

While there are [some](http://musicfromouterspace.com/index.php?MAINTAB=SYNTHDIY&VPW=1910&VPH=844) MIDI-to-CV converters out there that do the job without resorting to a microcontroller, I wanted to go the microcontroller route, for a few reasons:
* it's easier to reprogram a uC than it is to rebuild a circuit
* more documentation available
* more room for error/experimentation
* I already had an Arduino Uno on hand

Given the last bullet point above, my converter could be represented like this:

![Block Diagram](/images/blockdiagram.png)

-------

###Let's Get Into the Details

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

####Digital-to-Analog Converter (DAC) Options

I considered two methods for converting the Arduino's digital output to analog CV signals, and I actually wound up using both methods.

1. Dedicated DAC chip
2. Pulse Width Modulation (PWM) + Low-Pass Filter (LPF).

Before this project, I never used a dedicated DAC, and because I initially felt hesitant about that method, I decided to see if I could do all my digital-to-analog conversion using filtered PWM.

#####Filtered PWM

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
2. Given the -20 dB slope of the filter, calculate the filter cutoff frequency required to achieve the desired ripple amplitude.
```

Below, I'll describe how I followed this procedure to design a filter to smooth out a CV signal which could be used for pitch control:

--------

![MIDI circuit](/images/midicircuit.gif)

<dl>
  <dt>Definition list</dt>
  <dd>Is something people use sometimes.</dd>
  <dd>&omega;</dd>

  <dt>Markdown in HTML</dt>
  <dd>Does *not* work **very** well. Use HTML <em>tags</em>.</dd>
</dl>