readslave:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=0,1,0,4 -U flash:r:slave-backup.bin:r

readmaster:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=1,0,4,0 -U flash:r:master-backup.bin:r

slave:
	avr-gcc -mmcu=atmega1284p -o slave.bin -iquote../shared slave.c ../shared/bus.c

master:
	avr-gcc -mmcu=atmega1284p -o master.bin -iquote../shared master.c ../shared/bus.c

flashslave:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=0,1,0,4 -U flash:w:slave.bin

flashmaster:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=1,0,4,0 -U flash:w:master.bin

love:
	avr-gcc -mmcu=atmega1284p -o arbm.bin -iquote../shared arbm.c ../shared/bus.c
	avr-gcc -mmcu=atmega1284p -o arbs1.bin -iquote../shared arbs1.c ../shared/bus.c
	avr-gcc -mmcu=atmega1284p -o arbs2.bin -iquote../shared arbs2.c ../shared/bus.c

flove:
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=0,2,0,8 -U flash:w:arbs2.bin
	sleep 2
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=1,1,4,4 -U flash:w:arbs1.bin
	sleep 2
	avrdude -p m1284p -P usb -c jtag2 -x jtagchain=2,0,8,0 -U flash:w:arbm.bin

all: master slave
