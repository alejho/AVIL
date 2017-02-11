#!/bin/bash 

##see docs for this example here: http://avil.altervista.org/Index/4_Examples/AVIL_shell/upload_files.html

IP="192.168.1.177"
PORT="23"

#this is way to upload a file to the device

function send {
    echo $1|nc $IP $PORT 
}

send 'frec "'$1'"'
sleep 1
nc $IP $PORT < $1 
echo -e '\x1B'|nc $IP $PORT 
