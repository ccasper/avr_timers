# avr_timers
Simple helpers to make working with Atmel AVR timers easier.

<p>The timer documentation for AVR (atmega) processors is very complicated and hard
to follow. This code makes setting up any of the timers easier while keeping 
the logic easy to follow without any magical equations being done behind the 
scenes.

<p>The code example is fully functional and works with Arduino devices to generate
an interrupt every 16us and uses this to simply toggle the LED every 1 second.

<p>This code is an excellent base for adding more complex scheduling and real-time
control of specific hardware on exactly timed intervals.
