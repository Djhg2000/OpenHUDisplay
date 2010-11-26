#!/bin/sh

echo "Cheking enviroment..."
echo -n "project_pic16f690.c..."
if [ -e project_pic16f690.c ]; then
	echo "yes"
else
	echo "no"
	exit 1
fi
echo -n "speed2string.c..."
if [ -e speed2string.c ]; then
        echo "yes"
else
        echo "no"
        exit 1
fi
echo -n "br-355.c..."
if [ -e br-355.c ]; then
        echo "yes"
else
        echo "no"
        exit 1
fi
echo -n "m12by02aa.c..."
if [ -e m12by02aa.c ]; then
        echo "yes"
else
        echo "no"
        exit 1
fi
echo -n "i2c.c..."
if [ -e i2c.c ]; then
        echo "yes"
else
	echo "run patch_i2c.sh"
	exit 1
fi
echo -n "cc5x..."
if [ `which cc5x` ]; then
        echo "yes"
else
        echo "no"
	echo "Make sure cc5x is installed and the wrapper script for wine is located at $HOME/bin/cc5x"
        exit 1
fi

echo "Compiling..."
cc5x project_pic16f690.c

