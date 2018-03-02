## tongue program
## argument $1 is PORT


##Flashing stage, can skip
###for  USPasp
#avrdude -Cavrdude.conf -v -pattiny85 -cusbasp -e -Uefuse:w:0xFF:m -Uhfuse:w:0xD7:m -Ulfuse:w:0xE2:m 

### for arduino as isp
avrdude -Cavrdude.conf -v -pattiny85 -cstk500v1 -P/dev/$1 -b19200 -e -Uefuse:w:0xFF:m 
-Uhfuse:w:0xD7:m -Ulfuse:w:0xE2:m 

##burn firmware
### as usbasp
#avrdude -Cavrdude.conf -v -pattiny85 -cusbasp -Uflash:w:firmware.ino.hex:i 
### as arduino as isp
avrdude -Cavrdude.conf -v -pattiny85 -cstk500v1 -P/dev/$1 -b19200 -Uflash:w:firmware.ino.hex:i 
