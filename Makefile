CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
CFLAGS = -fno-common -O0 -g -I include -std=c99 -Wall -mcpu=cortex-m3 -mthumb  \
         -T linker/STM32F407.ld -nostartfiles

TARGET = ProIn.bin

.PHONY: all clean
vpath %.c src

all: $(TARGET)

$(TARGET): startup.c main.c core.c exception.c peripheral.c thread.c lock.c heap.c
	$(CC) $(CFLAGS) -o ProIn.elf $^
	$(CROSS_COMPILE)objcopy -Obinary ProIn.elf $@

clean:
	rm -f *.bin *.elf
