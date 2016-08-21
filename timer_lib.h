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
#ifndef TIMER_LIB_H_
#define TIMER_LIB_H_

#include <avr/io.h>
#include <stdio.h>

// See timer_lib.c for full definitions of these function prototypes.

void timer_begin_setup(uint8_t timer);

void timer2_set_external_clock(void);
void timer2_set_internal_clock(void);

void timer_setup(uint8_t timer, uint8_t waveGenMode, int prescale,
                 uint8_t compareOutModeA, uint8_t compareOutModeB,
                 uint8_t compareOutModeC, uint8_t captureMode);

void timer_set_interrupts(uint8_t timer, uint8_t outputCompare);

#endif