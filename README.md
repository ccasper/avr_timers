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

The code comes down to the following and works for timer 1 - 5 for most avr devices.
```C
  // This begins configuration by turning off the timers applicable interrupts.
  timer_begin_setup(2 /* timer2 */);

  timer2_set_internal_clock();
  
  // Setup the timer appropriately.
  timer_setup(2 /* timer2 */,
              2 /* CTC */,
              64 /* prescale */,
              0 /* disconnect A output pin */,
              0 /* disconnect B output pin */,
              0 /* disconnect C output pin */,
              0 /* captureMode */);
              
  // Update any timer values.
  OCR2A = 3; /* counts */
  // Reset the start of the timer counter.
  TCNT2 = 0;
  
  // Clear Interrupt flags and enable specific timer interrupts.
  timer_set_interrupts(2 /* timer */, 2 /* comp A interrupt */);
  
  // Enable global interrupts.
  sei();
```
