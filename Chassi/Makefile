TARGET = chassi

SRC = Chassi.c automatic_steering.c engine_control.c ../shared/usart.c ../shared/bus.c ../shared/LCD_interface.c
OBJ = $(SRC:.c=.o)

MCU = "atmega1284p"
F_CPU = "16000000UL"

AVRDUDEFLAGS=-p m1284p -P usb -c jtag2 -x jtagchain=1,1,4,4
CFLAGS=\
	-mmcu=$(MCU)\
	-O1\
	-funsigned-char\
	-funsigned-bitfields\
	-ffunction-sections\
	-fdata-sections\
	-fpack-struct\
	-fshort-enums\
	-DF_CPU=$(F_CPU)\
	-Wall\
	-Wstrict-prototypes\
	-std=c99\
	-I.\
	-mrelax

LDFLAGS=\
	-Wl,--start-group\
	-Wl,-lm\
	-Wl,--end-group\
	-Wl,--gc-sections\
	-mrelax\
	-mmcu=$(MCU)

.PHONY: compile flash backup restore flash clean

compile: $(TARGET).hex $(TARGET).eep

backup:
	avrdude $(AVRDUDEFLAGS) -U flash:r:$(TARGET)-backup.hex:i

restore:
	avrdude $(AVRDUDEFLAGS) -U flash:w:$(TARGET)-backup.hex:i

flash: compile
	avrdude $(AVRDUDEFLAGS) -U flash:w:$(TARGET).hex:i

clean:
	rm $(TARGET).{elf,hex,eep}

# Pseudorules
%.eep: %.hex
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

%.elf: $(OBJ)
	avr-gcc $^ $(LDFLAGS) -o "$@"

%.o: %.c
	avr-gcc $(CFLAGS) -c -o "$@" "$(@:%.o=%.c)"
