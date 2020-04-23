# Some C/C++ little projects

## CCIA Project
You have to install gcc, gdb, openlpc ,gcc-arm-none-eabi and picocom before start.
You can do it via official repos.
Official  edu-ciaa web page http://www.proyecto-ciaa.com.ar/devwiki/doku.php?id=desarrollo:firmware:instalacion_sw .
Now, you have to connect your EDU-CIA ( cortex m4 ) through USB-Debug port.
From a terminal run :

make clean
make download

If the download was successfully then you should see "Download done." at the end of the output.
In order to interact with the board via keyboard, you have to run :

picocom -b 115200 /dev/ttyUSB1

The outcome should be like this :

picocom v1.7

port is        : /dev/ttyUSB1
flowcontrol    : none
baudrate is    : 115200
parity is      : none
databits are   : 8
escape is      : C-a
local echo is  : no
noinit is      : no
noreset is     : no
nolock is      : no
send_cmd is    : sz -vv
receive_cmd is : rz -vv
imap is        : 
omap is        : 
emap is        : crcrlf,delbs,

Terminal ready
Press button 1 to continue ... 

Increase and decrease the shine on the leds using keyboard and buttons.
Press ENTER to check the current value of the LEDS
Press : to insert a hex value between 00 and ff 
Press button 1 to deacrease the shine to 0x00
Press button 2 to increase the shien to 0xff
Press button 3 to increase the value in 0x10
Press button 4 to deacrease the value in 0x10
