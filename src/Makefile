NAME	= avt3122xtd

MCU	= attiny2313
DMCU	= attiny2313
QUARTZ	= 8000000
OBJS	= main.o tmr0.o tmr1.o io.o dht.o ds.o dpy_decode.o
PROG	= usbasp
FUSEE	= 0xff
FUSEH	= 0xc9
FUSEL	= 0xc4
SUDO	= sudo

CFLAGS= -std=c99 -pipe -Wall -Wextra -mmcu=$(MCU) -Os -DF_CPU=$(QUARTZ)L -fshort-enums

default:	$(NAME).bin $(NAME).hex $(NAME).lst $(NAME)-remove $(NAME)-ls

$(NAME).bin:	$(NAME)
	avr-objcopy -j .text -j .data -O binary $(NAME) $(NAME).bin
	@chmod -x $(NAME).bin

$(NAME).hex:	$(NAME)
	avr-objcopy -j .text -j .data -O ihex $(NAME) $(NAME).hex
#	@sed '/^:......05/d' < $(NAME).hex > $(NAME).hex-; mv -f $(NAME).hex- $(NAME).hex

$(NAME).lst:	$(NAME)
	avr-objdump -h -S -D --stabs --disassemble-zeroes $(NAME) > $(NAME).lst
	avr-nm $(NAME) -n >> $(NAME).lst

.PHONY:	$(NAME)-remove

$(NAME)-remove:
	@rm -f $(NAME)

$(NAME)-ls:
	@ls -l $(NAME).bin

$(NAME):	$(OBJS)
	avr-gcc $(CFLAGS) $(OBJS) -o $(NAME)

%.o:	%.c	Makefile
	avr-gcc $(CFLAGS) $< -c -o $@
	@avr-gcc $(CFLAGS) $< -S 2> /dev/null

.PHONY:	clean

clean:
	rm -f $(NAME) *.s *.hex *.bin *.o *.lst *~ core

.PHONY:	poke

poke: $(NAME).hex
	$(SUDO) avrdude -i 5 -p $(DMCU) -c $(PROG) -e -U efuse:w:$(FUSEE):m -U hfuse:w:$(FUSEH):m -U lfuse:w:$(FUSEL):m -U flash:w:$(NAME).hex
