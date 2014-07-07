#!/bin/bash

#use to define the IP
ip="192.168.70."

# This script requires root-privileges

echo "Enter the name of the interface you want to configure, followed by [ENTER]"
echo "Leave blank for default value of eth0:"

read nic

if [ -z "$nic" ]; then
	nic="eth0"
fi

echo "Setting up $nic on the victim machine, this will take just a moment..."

ifconfig $nic down
ifconfig $nic $ip$((( $RANDOM %254) +1)) up

#uncomment the following lines to start services on the machine
#/etc/init.d/ssh start
#/etc/init.d/apache2 start
#/etc/init.d/atftpd start

echo "The victim machine is now setup."
echo "The IP address is somewhere in the $ip0/24 network"
echo "You may now close this window and begin your attack... Good luck!"