CC=avr-gcc
OBJCPY=avr-objcopy
CFLAGS += -Os -mmcu=atmega16m1 -DF_CPU=16000000UL -std=c99 -Wall -Wextra #-pedantic

TARGET=batteryWatch
SOURCE=$(wildcard *.c)
DEPS=
LIBS=can/libcan.a
LDFLAG=-Lcan -lcan

OBJS=$(SOURCE:.c=.o)

$(TARGET): $(OBJS) $(DEPS)
	@printf "LINKING \033\13301;33m-\033\13301;37m %-20s\n\033\13300;39m" $@
	@$(CC) $(CFLAGS) $(LDFLAG) -o $(TARGET).elf $(OBJS) $(LIBS)

	@printf "MAKING HEX \033\13301;33m-\033\13301;37m %-20s\n\033\13300;39m" $@
	@avr-objcopy -O ihex $(TARGET).elf $(TARGET).hex
	@printf "OBJCPY  \033\13301;33m-\033\13301;37m %-20s %s\033\13300;39m\n" $@
	@$(OBJCPY) -O binary $(TARGET).elf $(TARGET).bin

%.o: %.c $(DEPS)
	@printf "CC \033\13301;33m-\033\13301;37m %-20s\n\033\13300;39m" $@
	@$(CC) -c $(CFLAGS) -o $@ $< $(INCLUDES)

#####
# ObjCopy
$(BR)/$(BINARY).bin: $(BR)/$(BINARY).elf


flash:
#	avrdude -c mcprog   -p m16m1 -x clockrate=125000  -U flash:w:$(TARGET).hex
#	avrdude -v -c usbasp -p m16m1 -U flash:w:$(TARGET).hex
	avrdude -B 0.1 -v -c usbtiny -p m16m1 -U flash:w:$(TARGET).hex

fuse:
	avrdude -B 100 -v -c usbtiny -p m16m1 -U lfuse:w:0xfe:m -U hfuse:w:0xd1:m -U efuse:w:0xfe:m
#	python2 megaHidProg.py -C -S -c 125 -L FF -H D9 -E F

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.hex
	rm -f *.bin
#	rm can/*.a

.PHONY: doc clean
