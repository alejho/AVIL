#!/bin/bash 

#see docs for this example here: http://avil.altervista.org/Index/4_Examples/AVIL_shell/download_files.html

IP="192.168.1.177"
PORT="23"
#timeout, the download automatically ends after WAIT seconds
WAIT=2

#this is a quick and dirty way to download a file from the device using netcat
#i'm sure there is a better way to do it!!!

#open a netcat connection and send a cat command...
#then redirect data to a local file for WAIT seconds
echo -e 'cat "'$1'"\n\r' | nc -q $WAIT $IP $PORT > $1 
#remove the last two lines containing only terminal stuff...
sed -i '$ d' $1
sed -i '$ d' $1
