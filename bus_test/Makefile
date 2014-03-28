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
