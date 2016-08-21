/*
 *   Copyright 2016 ProtoEng/Cory Casper
 * 
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 * 
 *       http://www.apache.org/licenses/LICENSE-2.0
 * 
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#include "timer_lib.h"
#include <avr/interrupt.h>

// Simple pin control macros (designed with avr8 and avr32 in mind).
#define setpin_in(port, pin) \
  port &= ~_BV(pin)
#define setpin_out(port, pin) \
  port |= _BV(pin)
#define setpin(port, pin) \
  port |= _BV(pin)
#define clearpin(port, pin) \
  port &= ~_BV(pin)

// LOCK/UNLOCK are useful for maintaining previous interrupt state.
//   Note: Each Lock/Unlock pair need their own scope: { ... }
// Disable interrupts and maintain interrupt enable bit.
#define LOCK_INTERRUPTS \
  uint8_t __old_SREG = SREG; \
  cli()
// Restore interrupts and preserve interrupt enable bit.
#define UNLOCK_INTERRUPTS \
  SREG = __old_SREG

#define LOCK_TIMER2_COMPA \
  TIMSK2 &= ~(1<< OCIE2A)
#define UNLOCK_TIMER2_COMPA \
  TIMSK2 |= (1<< OCIE2A)

static inline void onboard_led_enable(void) {
  setpin_out(DDRB, 7);
}
static inline void onboard_led_on(void) {
  setpin(PORTB, 7);
}
static inline void onboard_led_off(void) {
  clearpin(PORTB, 7);
}

static volatile uint32_t counter = 0;
static uint32_t ledState = 0;

/**
 * Initialize the Timer for counting to a set value and producing an
 * interrupt each time the value is reached.
 *
 * Timer2 configured to produce an interrupt at 62.5kHz (16us period) and
 *   update the Arduino LED every 1 second.
 */
int main(void) {
  
  // Enable the LED pin for flashing LED.
  onboard_led_enable();
      
  timer_begin_setup(2 /* timer2 */);
  
  timer2_set_internal_clock();
  
  // Setup the timer appropriately.
  timer_setup(
    2 /* timer2 */,
    2 /* CTC */,
    64 /* prescale */,
    0 /* disconnect A output pin */,
    0 /* disconnect B output pin */,
    0 /* disconnect C output pin */,
    0 /* captureMode */);
  /* Update the timer values.
   * 
   * We need to calculate a proper value to load the timer counter.
   * (CPU frequency: 16Mhz) / (prescaler value: 64)
   *   = 250KHz = 4us (per timer count).
   * 
   * (desired period: 16us) / (per timer count: 4us) = 4 cycles.
   * Using Fast: [MAX(uint8): 255] + 1 - (cycles: 4) = 252 counts.
   * Using CTC: (cycles: 4) - 1 = 3 counts.
   */
  OCR2A = 3; /* counts */
  // Reset the start of the timer counter.
  TCNT2 = 0; 
  
  /* Clear Interrupt flags and enable specific timer interrupts. */
  timer_set_interrupts(2 /* timer */, 2 /* comp A interrupt */);

  // Enable global interrupts.
  sei();
  
  // Loop forever and update the led based on the timer2 interrupt counter.
  while (1 == 1) {
    
    // Synchronously read the counter.
    LOCK_INTERRUPTS;
    uint32_t counterCopy = counter;
    UNLOCK_INTERRUPTS;
    
    if (counterCopy > 62500 /*1sec (1s / .016us = 62500 cnts)*/) {
      
      // Synchronously reset the counter.
      LOCK_INTERRUPTS;
      counter = 0;
      UNLOCK_INTERRUPTS;
      
      ledState = (ledState + 1) % 2;
      if (ledState == 1) {
        onboard_led_on();
      } else {
        onboard_led_off();
      } 
    }
  }
  return 0;
}


/** Timer 2 Comparator A ISR. */
ISR(TIMER2_COMPA_vect) {
  /// Disable interrupt within this ISR. From experience, when execution takes
  /// slightly over the ISR period, the ISR will be called recursively!
  LOCK_TIMER2_COMPA;

  // Count how many interrupts were called for updating the led approprately.
  counter++;

  // Reenable Timer2 interrupt.
  UNLOCK_TIMER2_COMPA;
}
