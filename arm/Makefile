FLASHFLAGS="-p m1284p -P usb -c jtag2 -x jtagchain=0,2,0,8"

compile:
	avr-gcc -mmcu=atmega1284p -o arm.bin -O1 -DF_CPU=160000000 arm.c servo.c

backup:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=0,2,0,8 -U flash:r:arm-backup.bin:r

restore:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=0,2,0,8 -U flash:w:arm-backup.bin:r

flash:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=0,2,0,8 -U flash:w:arm.bin:r

all: compile flash
