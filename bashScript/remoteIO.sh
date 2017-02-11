#!/bin/bash 

#see docs for this example here: http://avil.altervista.org/Index/4_Examples/AVIL_shell/remote_digital_IO_from_bash_script.html

IP="192.168.1.177"
PORT="23"


exec 2>/dev/null

#defining a function to remotely read a digital input from Arduino
function dioR {
    lastResp=`ncat -i 100 $IP $PORT<<<"dioR "$1` 
    lastResp=($lastResp)
    if echo "$lastResp" | grep -q "true" ; then
		return 0
	else
		return 1
	fi;
}  

#defining a function to remotely set an Arduino's pin as input or output
function setPM {
	nc $IP $PORT<<<"setPM "$1" "$2  
	#nc $IP $PORT<<<"setPM 7 OUTPUT"  
}   

#defining a function to remotely write a digital output of Arduino
function dioW {
	nc $IP $PORT<<<"dioW "$1" "$2  
}



#running the example
#connect a button as input on pin 6
#connect a led as output on pin 7

setPM 6 "INPUT"
sleep 1
setPM 7 "OUTPUT"
sleep 1

while true
do
	
	if dioR 6 ; then
		#do something on your PC whe the button is pressed
		#...you can print some output
		echo "button pressed!"
		#...or send an email:
		#echo "hello world" | mail -s "a subject" someone@somewhere.com
		#...or just turn on a led
		dioW 7 true
	fi;
	sleep 0.1
done 
