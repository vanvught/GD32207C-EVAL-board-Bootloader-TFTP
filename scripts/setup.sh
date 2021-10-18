#!/bin/bash

chmod u+x do-tftp.sh

cd ../udp_send/Release
make clean && make
chmod u+x udp_send
ls -al udp_send
cd -

DIR=../GD32F20x_Demo_Suites_V2.2.1/*

for f in $DIR
do
	echo -e "\033[32m[$f]\033[0m"
	cd "$f"
	rm -fv do-tftp.sh
	ln -s ../../scripts/do-tftp.sh .
	rm -fv udp_send
	ln -s ../../udp_send/Release/udp_send .
	ls -al
	cd -
done