#!/bin/bash

# this script will assist the user with interactively programming the trap
# cannot modify the base program with interactive dialog because all output will 
# be piped to crontab. This allows the user to interactively enter values that
# will then be input as config variables into crontab file generator

echo "This script will program the start and stop times of the trap."
echo "Values must be entered representing hours before/after sunrise/sunset."
echo "Must use whole numbers and not decimals or unpredictable behavior will result."
echo "Use negative numbers to represent hours before but do not use"
echo "whitespace. Example: -2 and not - 2"
echo " "
echo "Enter a number between -4 and 12 for Sunrise Offset"

read SRoffset

if ! [[ "$SRoffset" =~ ^[+-]?[0-9]+$ ]]
    then
        echo "Invalid input! Whole integers only. Terminating Script."
        exit
fi

if [ $SRoffset -lt -4 ] || [ $SRoffset -gt 12 ]
then
	echo "Invalid input entered! Terminating Script."
	exit
fi

ba="before"
if [ $SRoffset -gt 0 ] 
then
	ba="after"
fi

echo -e "Trap will stop collection $SRoffset hours $ba sunrise.\n"

echo "Enter a number between -12 and 5 for Sunset Offset"

read SSoffset

if ! [[ "$SSoffset" =~ ^[+-]?[0-9]+$ ]]
    then
        echo "Invalid input! Whole integers only. Terminating Script."
        exit
fi

if [ $SSoffset -lt -12 ] || [ $SSoffset -gt 5 ]
then
	echo "Invalid input entered! Terminating Script."
	exit
fi

ba="before"
if [ $SSoffset -gt 0 ]
then
        ba="after"
fi

echo -e "Trap will start collection $SSoffset hours $ba sunset.\n"

echo "Press enter to program the trap using Crontab."
read delay

runnables="/home/mosqtrap/mosquito/runnables"
file="/home/mosqtrap/mosquito/runnables/PC_sunfile.txt"

$runnables/crontab_file_gen.x $file $SRoffset $SSoffset | crontab

crontab -l


