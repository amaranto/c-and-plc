# Some C/C++ little projects

## CCIA Project
[Official  edu-ciaa web page](http://www.proyecto-ciaa.com.ar/)

### Requirements
- gcc
- gdb
- openlpc
- gcc-arm-none-eabi
- picocom 

[ Framework installation ](http://www.proyecto-ciaa.com.ar/devwiki/doku.php?id=desarrollo:firmware:instalacion_sw)

## Usage
From a terminal run :

`make clean`
`make download`

In order to interact with the board via keyboard, you have to run :

`picocom -b 115200 /dev/ttyUSB1`

- The outcome should be like this :

`picocom v1.7`
`port is        : /dev/ttyUSB1`

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

`

- Increasing and decreasing the shine on the leds using keyboard and buttons.

`
Press ENTER to check the current value of the LEDS

Press : to insert a hex value between 00 and ff

Press button 1 to deacrease the shine to 0x00

Press button 2 to increase the shien to 0xff

Press button 3 to increase the value in 0x10

Press button 4 to deacrease the value in 0x10

`
