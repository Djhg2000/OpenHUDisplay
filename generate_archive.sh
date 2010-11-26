#!/bin/sh

./generate_hex.sh
if [ $? -ne 0 ]; then
	echo "Compile failed. Please correct the error and try again."
	exit
fi
echo "Compressing..."
tar -vzcf hud.tar.gz project_pic16f690.c speed2string.c delays.h delays.c br-355.c m12by02aa.c i2c.c project_pic16f690.hex generate_archive.sh
echo "Done. Output archive is hud.tar.gz"
