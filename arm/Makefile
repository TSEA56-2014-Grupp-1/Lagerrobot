AVRDUDEFLAGS=-p m1284p -P usb -c jtag2 -x jtagchain=0,2,0,8
CFLAGS=\
	-mmcu=atmega1284p\
	-O1\
	-ffunction-sections\
	-fdata-sections\
	-DF_CPU=160000000\
	-Wall\
	-Wstrict-prototypes\
	-std=c99\
	-I.\
	-mrelax

compile:
	avr-gcc $(CFLAGS) -o arm.bin servo.c ../shared/usart.c

preprocess:
	avr-gcc -E servo.c

backup:
	avrdude $(AVRDUDEFLAGS) -U flash:r:arm-backup.bin:r

restore:
	avrdude $(AVRDUDEFLAGS) -U flash:w:arm-backup.bin:r

flash:
	avrdude $(AVRDUDEFLAGS) -U flash:w:arm.bin:r

all: compile flash
