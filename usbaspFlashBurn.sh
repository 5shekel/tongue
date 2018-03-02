avrdude -Cavrdude.conf -v -pattiny85 -cusbasp -e -Uefuse:w:0xFF:m -Uhfuse:w:0xD7:m -Ulfuse:w:0xE2:m 

avrdude -Cavrdude.conf -v -pattiny85 -cusbasp -Uflash:w:firmware.ino.hex:i 
