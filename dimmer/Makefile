PRG            = main
OBJ            = main.o uart.o #twi.o touch.c
MCU_TARGET     = attiny2313
OPTIMIZE       = -O2
ISP            = usbtiny

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS =  -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: hex

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf $(PRG).hex

hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

install: load

load: $(PRG).hex
	avrdude -P usb -p t2313 -c $(ISP) -U flash:w:$<

status:
	avrdude -P usb -p t2313 -c $(ISP) -v
