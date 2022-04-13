MCU   = atmega328p
F_CPU = 20000000UL 
CPPFLAGS = -DF_CPU=$(F_CPU) -I.

CC=avr-gcc
OBJCOPY=avr-objcopy 

## Compiler/linker options 
CFLAGS = -Os -g -std=gnu99 -Wall 
CFLAGS += -ffunction-sections -fdata-sections
 
TARGET_ARCH = -mmcu=$(MCU)
 
LDFLAGS = -Wl,-Map,mmap.map 
LDFLAGS += -Wl,--gc-sections 

## Targets and Rules

%.o: %.c $(HEADERS) Makefile
	 $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<

flash: clean mrr.hex
	avrdude -c usbtiny -p $(MCU) -U flash:w:mrr.hex

flash_cal: clean cal.hex
	avrdude -c usbtiny -p $(MCU) -U flash:w:cal.hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.elf: %.o
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@

clean: 
	rm -f $(wildcard *.elf) $(wildcard *.hex) $(wildcard *.o) $(wildcard *.map)