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
2. Pulse Width Modulation (PWM) + Low-pass filter (LPF).

Before this project, I never used a dedicated DAC, and because I initially felt hesitant about that method, I decided to see if I could do all my digital-to-analog conversion using filtered PWM.

--------

![MIDI circuit](/images/midicircuit.gif)

<dl>
  <dt>Definition list</dt>
  <dd>Is something people use sometimes.</dd>
  <dd>&omega;</dd>

  <dt>Markdown in HTML</dt>
  <dd>Does *not* work **very** well. Use HTML <em>tags</em>.</dd>
</dl>