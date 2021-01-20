SET target="ultasonic Distance LCD"
SET mcu=at90s2313
avr-gcc -g -Os -mmcu=%mcu% -c %target%.c -DF_CPU=4000000UL
avr-gcc -g -mmcu=%mcu% -o %target%.elf %target%.o -DF_CPU=4000000UL
avr-objdump -h -S %target%.elf > %target%.lst
avr-gcc -g -mmcu=%mcu% -Wl,-Map,%target%.map -o %target%.elf %target%.o -DF_CPU=4000000UL
avr-objcopy -j .text -j .data -O ihex %target%.elf %target%.hex

