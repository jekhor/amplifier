###############################################################################
# Makefile for the project preamp_2_plus
###############################################################################

## General Flags
PROJECT = preamp_2_plus
MCU = atmega16
AVRDUDE_TARGET = m16
AVRDUDE_ADAPTER = usbtiny

PRG = preamp_2_plus
TARGET = $(PRG).elf

CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                                     -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=preamp_2_plus.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = preamp_2_plus.o lcd_lib.o rc5.o i2c.o ds18x20.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) preamp_2_plus.hex preamp_2_plus.eep preamp_2_plus.lss size

## Compile
preamp_2_plus.o: ../preamp_2_plus.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd_lib.o: ../lcd_lib.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rc5.o: ../rc5.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c.o: ../i2c.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ds18x20.o: ../ds18x20.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

flash: $(PRG).hex
	        avrdude -c $(AVRDUDE_ADAPTER) -p $(AVRDUDE_TARGET) -U flash:w:$<:i

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) preamp_2_plus.elf dep/* preamp_2_plus.hex preamp_2_plus.eep preamp_2_plus.lss preamp_2_plus.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

