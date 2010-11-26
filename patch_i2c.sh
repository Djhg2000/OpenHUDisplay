#!/bin/sh

I2C_LIBRARY_URL="http://www.sparkfun.com/tutorial/coding/I2c.c"

echo "Checking enviroment..."
echo -n "I2c.c..."
if [ -e I2c.c ]; then
	echo "yes"
else
	echo "no"
	echo "You need to download the I2c.c library from SparkFun."
	echo -n "May I do it for you? [y/N]: "
	read DOWNLOAD
	if [ "x$DOWNLOAD" = "xy" ]; then
		wget "$I2C_LIBRARY_URL"
		if [ $? -ne 0 ]; then
			echo "Download failed, aborting..."
			exit
		fi
		echo "Done, proceeding with patch..."
	else
		exit
	fi
fi
echo -n "i2c.patch..."
if [ -e i2c.patch ]; then
	echo "yes"
else
	echo "no"
	echo "Patch not found, try \"git pull\"."
fi

patch -p0 < i2c.patch
echo -n "Renaming I2c.c to i2c.c..."
mv I2c.c i2c.c
echo "done"

