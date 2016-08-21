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
#include <avr/io.h>
#include "timer_lib.h"

/**
 * Turn off timer specific interrupts to begin setup.
 */
void timer_begin_setup(uint8_t timer) {
  /* First disable all timer interrupts while we're configuring. */
  switch(timer) {
    case 0: 
      #ifdef TIMSK0
      TIMSK0 &= ~(1<<TOIE0 | 1<< OCIE0A | 1<<OCIE0B);
      #endif
      break;
    case 1: 
      #ifdef TIMSK1
      TIMSK1 &= ~(1<<TOIE1 | 1<< OCIE1A | 1<<OCIE1B);
      #endif
      break;
    case 2: 
      #ifdef TIMSK2
      TIMSK2 &= ~(1<<TOIE2 | 1<< OCIE2A | 1<<OCIE2B);
      #endif
      break;
    case 3: 
      #ifdef TIMSK3
      TIMSK3 &= ~(1<<TOIE3 | 1<< OCIE3A | 1<<OCIE3B);
      #endif
      break;
    case 4: 
      #ifdef TIMSK4
      TIMSK4 &= ~(1<<TOIE4 | 1<< OCIE4A | 1<<OCIE4B);
      #endif
      break;
    case 5:
      #ifdef TIMSK5
      TIMSK5 &= ~(1<<TOIE5 | 1<< OCIE5A | 1<<OCIE5B);
      #endif
      break;
  }
}

/*
 * When using Timer2, an external clock can be configured to be used.
 */
void timer2_set_external_clock() {
  ASSR |= (1<<AS2);
}

/*
 * When using Timer2, the internal clock can be configured for use
 * (recommended method).
 */
void timer2_set_internal_clock() {
  ASSR &= ~(1<<AS2);
}

/**
 * Configure the timer config register values with a simpler interface.
 * 
 * @param timer
 *   0 = Timer0
 *   1 = Timer1
 *   2 = Timer2
 *   3 = Timer3 (Note: Not all avr devices support all listed timers)
 *   4 = Timer4
 *   5 = Timer5
 * @param waveGenMode = WGM13 << 3 | WGM12/CTC1 << 2 |  WGM11 << 1 | WGM10 << 0
 *   0 = Normal 0xFF Immediate MAX
 *   1 = PWM Phase Correct 0xFF TOP BOTTOM
 *   2 = CTC OCRA Immediate MAX (recommended for basic counter)
 *   3 = PWM Fast 0xFF TOP MAX
 *   4 = Reserved – – –
 *   5 = PWM Phase Correct OCRA TOP BOTTOM
 *   6 = Reserved – – –
 *   7 = PWM Fast OCRA BOTTOM TOP
 * @param prescale => clockSelect = CS22 << 2 | CS21 << 1 | CS20 << 0
 *   See code below for prescale.
 * @param compareOutModeA = COM1A1 << 1 | COM1A0 << 0
 *   0 = normal port operation (disconnected from pins) (recommended).
 *   1 = toggle OCnA/OCnB/OCnC on compare match.
 *   2 = clear OCnA/OCnB/OCnC on compare match (set output to low level).
 *   3 = set OCnA/OCnB/OCnC on compare match (set output to high level).
 * @param compareOutModeB = COM1B1 << 1 | COM1B0 << 0
 *     - See compareOutModeA above.
 * @param compareOutModeC = COM1C1 << 1 | COM1C0 << 0
 *     - See compareOutModeA above.
 * @param captureMode = ICNC1 << 1 | ICES1 << 0
 *     - 0 = disable noise canceller, input capture negative edge select
 *     - 1 = enable noise canceller, input capture negative edge select
 *     - 2 = disable noise canceller, input capture postive edge select
 *     - 3 = enable noise canceller, input capture postive edge select
 */
void timer_setup (uint8_t timer, uint8_t waveGenMode, int prescale, uint8_t compareOutModeA, uint8_t compareOutModeB, uint8_t compareOutModeC, uint8_t captureMode)
{
  // Sanitize inputs.
  waveGenMode &= 0x0F; // Waveform generation mode.
  compareOutModeA &= 0x03; // Compare Output mode A.
  compareOutModeB &= 0x03; // Compare Output mode B.
  compareOutModeC &= 0x03; // Compare Output mode B.
  captureMode &= 0x03; // Input capture.
  
  // Choose the clock based on which timer and the requested prescaler.
  uint8_t clockSelect = 0;
  if (timer == 0) {
    // Convert prescale to clockSelect for Timer 0 (unique).
    switch (prescale) {
      case 0: clockSelect = 0; break; // no clocking, frozen timer.
      case 1: clockSelect = 1; break;
      case 8: clockSelect = 2; break;
      case 64: clockSelect = 3; break;
      case 256: clockSelect = 4; break;
      case 1024: clockSelect = 5; break;
      default: clockSelect = 7; break; // external clock source rising edge.
    }
  } else {
    // Convert prescale to clockSelect for Timer 1-5.
    switch (prescale) {
      case 1: clockSelect = 1; break;
      case 8: clockSelect = 2; break;
      case 32: clockSelect = 3; break;
      case 64: clockSelect = 4; break;
      case 128: clockSelect = 5; break;
      case 256: clockSelect = 6; break;
      case 1024: clockSelect = 7; break;
      default: clockSelect = 0; break; // no clocking, frozen timer.
    }
  }
  
  uint8_t tccrNa = (compareOutModeA << 6)
      | (compareOutModeB << 4)
      | (compareOutModeC << 2)
      | (waveGenMode & 3);
  uint8_t tccrNb = (captureMode << 6)
      | ((waveGenMode & 0xC) << 1)
      | clockSelect;
      
  // Set the TCCR values for the applicable/available timer.
  switch (timer) {
    case 0: 
      #ifdef TCCR0A
        TCCR0A = tccrNa; TCCR0B = tccrNb;
      #endif
      break;
    case 1: 
      #ifdef TCCR1A
        TCCR1A = tccrNa; TCCR1B = tccrNb;
      #endif
      break;
    case 2: 
      #ifdef TCCR2A
        TCCR2A = tccrNa; TCCR2B = tccrNb;
      #endif
      break;
    case 3: 
      #ifdef TCCR3A
        TCCR3A = tccrNa; TCCR3B = tccrNb;
      #endif
      break;
    case 4: 
      #ifdef TCCR4A
        TCCR4A = tccrNa; TCCR4B = tccrNb;
      #endif
      break;
    case 5: 
      #ifdef TCCR5A
        TCCR5A = tccrNa; TCCR5B = tccrNb;
      #endif
      break;
  }
}

/**
 * Configure the desired interrupts for the timer to trigger.
 * 
 * Note: SREG will need to be set to get global interrupts running.
 * 
 * @param timer Timer to configure 0-5
 * @param isrSelection = OCF2B << 2 | OCF2A << 1 | TOV2 << 0
 *   Controls three different interrupts, each of which can be set. (|) the 
 *   conditions together for enabling multiple interrupts.
 *   0 = no interrupt flags set (No ISR vectors will be called)
 *   1<<0 = (TIMER2_OVF_vect) TOV2 bit is set (one) when an overflow occurs
 *   1<<1 = (TIMER2_COMPA_vect) OCF2A bit is set (one) when a compare match
 *           occurs between the Timer/Counter2 and the data in OCR2A - Output
 *           Compare Register2.
 *   1<<2 = (TIMER2_COMPA_vect) OCF2B bit is set (one) when a compare match
 *           occurs between the Timer/Counter2 and the data in OCR2B - Output
 *           Compare Register2.
 */
void timer_set_interrupts(uint8_t timer, uint8_t isrSelection) {
  // Sanitize inputs.
  isrSelection &= 0x07; // Interrupt selections.
  
  // Set the timer interrupts to be enabled (pending SREG) and clear the
  // existing timer interrupt.
  switch (timer) {
    case 0: 
      #ifdef TIMSK0
      TIFR0 = isrSelection;
      TIMSK0 = isrSelection;
      #endif
      break;
    case 1: 
      #ifdef TIMSK1
      TIFR1 = isrSelection;
      TIMSK1 = isrSelection;
      #endif
      break;
    case 2: 
      #ifdef TIMSK2
      TIFR2 = isrSelection;
      TIMSK2 = isrSelection;
      #endif
      break;
    case 3: 
      #ifdef TIMSK3
      TIFR3 = isrSelection;
      TIMSK3 = isrSelection;
      #endif
      break;
    case 4: 
      #ifdef TIMSK4
      TIFR4 = isrSelection;
      TIMSK4 = isrSelection;
      #endif
      break;
    case 5: 
      #ifdef TIMSK5
      TIFR5 = isrSelection;
      TIMSK5 = isrSelection;
      #endif
      break;
  }
}