#
#   Copyright 2016 ProtoEng/Cory Casper
# 
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
# 
#       http://www.apache.org/licenses/LICENSE-2.0
# 
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#

# AVR Makefile
# Configure MCU, AVRDUDE_PROGRAMMER, and AVRDUDE_PORT for your specific device.
#   - Note: Default is configured for an Arduino Mega with Ubuntu client.
#   - If install is failing with Arduino, make sure there are no "!!!" in the
#     code.

TARGETNAME = avrfirmware

#MCU=atmega328p
#MCU:=atmega1281
MCU:=atmega2560
F_CPU:=16000000

# Type: "avrdude -c ?" to get a full listing.
#AVRDUDE_PROGRAMMER := stk500v2
#AVRDUDE_PROGRAMMER := stk600
#AVRDUDE_PROGRAMMER := jtag2
AVRDUDE_PROGRAMMER := stk600

#AVRDUDE_PORT := /dev/ttyUSB0
#AVRDUDE_PORT := usb
AVRDUDE_PORT := /dev/ttyACM0

CC=avr-gcc
AR=@avr-ar

## Begin sources definition
SRC = timer_example.c
INCL = -I./
#///////////////////////////////////////////////////////////////////////////////
SRC += timer_lib.c
## End sources definition



# Flags for the linker and the compiler
COMMON_FLAGS = -DF_CPU=$(F_CPU)
COMMON_FLAGS += $(INCL)
COMMON_FLAGS += -g -Os -Wall -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
COMMON_FLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections
COMMON_FLAGS += -Wl,--relax -mcall-prologues
CFLAGS := $(COMMON_FLAGS) -std=gnu99 -Wstrict-prototypes

OBJ = $(SRC:.c=.o)

.SUFFIXES: .elf .hex .dis

# Targets
.PHONY: all
all:  COMMON_FLAGS += -mmcu=$(MCU)
all:  CFLAGS += -mmcu=$(MCU)
all:  $(TARGETNAME).dis $(TARGETNAME).hex 

.PHONY: install
install: $(TARGETNAME).dis $(TARGETNAME).hex
	avrdude -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -D DBOARD -v -U flash:w:$(TARGETNAME).hex
	avr-size $(TARGETNAME).elf

.PHONY: clean
clean:  
	$(RM) $(TARGETNAME).hex $(TARGETNAME).elf $(TARGETNAME).a $(TARGETNAME).dis $(OBJ)
	$(RM) -f *.o *.rom *.elf *.map *~ *.lst
	$(RM) -rf .dep
	$(RM) -f $(TARGETNAME) $(OBJ) $(DEPENDS)
	find . -type d -name "obj" -delete

# implicit rules
.elf.hex:
	avr-objcopy -O ihex -R .eeprom $< $@

# explicit rules
$(TARGETNAME).elf: $(TARGETNAME).a($(OBJ)) 
	$(LINK.o) $(COMMON_FLAGS) $(TARGETNAME).a $(LOADLIBES) $(LDLIBS) -o $@

$(TARGETNAME).dis: $(TARGETNAME).elf
	avr-objdump -S $< > $@
