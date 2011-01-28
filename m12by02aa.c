/* Driver for Futaba M12BY02AA, compile with knudsen cc5x */
/* Written by Axel Norstedt */

/*
       _______________
      |       \_/       |
+5V >-| Vdd         Vss |-< GND
     -| RA5         RA0 |-
     -| RA4         RA1 |-
     -| RA3         RA2 |-
     -| RC5         RC0 |-> LED Debug (demo code only)
     -| RC4         RC1 |-
     -| RC3         RC2 |-
     -| RC6     SDA/RB4 |-> VFD SDL
     -| RC7      RX/RB5 |-
     -| RB7     SCL/RB6 |-> VFD SCL
      |_________________|
*/

/* VFD command list

Comand bytes are preceeded by device ID byte (VFD_I2C_ID) and the address byte is followed by a data byte, e.g. the following will display '0' as first character:
0b10100000 (VFD_I2C_ID)
0b01000000 (write to display)
0b00110000 (0x30, ASCII '0')

? == ignored

Display mode:
0b00.??.1.b3-b0
b3-b0 = unknown, poorly documented command (spec says default is 12 digits and 16 segments)

Data setting:
0b01.??.b3.b2.?.b0
b0 = control VFD (0) or LEDs (1)
b2 = increment address after data write (0) or stay at last address (1)
b3 = test mode, spec says always use 0

Address setting:
0b11.b5-b0
b5-b0 = address (must be 0x00~0x23, otherwise ignored and waits for next valid address)

Display control:
0b10.?.b4.b3-b0
b4 = display on (1) or off (0)
b3-b0 = brightness, note 1111 has same effect as 1110
*/

/* Character table:
0	0b00111111, 0b00000000, 0b00000000
1	0b00110000, 0b00000000, 0b00000000
2	0b01011011, 0b10000100, 0b00000000
3	0b01111001, 0b10000100, 0b00000000
4	0b01110100, 0b10000100, 0b00000000
5	0b01101101, 0b10000100, 0b00000000
6	0b01101111, 0b10000100, 0b00000000
7	0b00111000, 0b00000000, 0b00000000
8	0b01111111, 0b10000100, 0b00000000
9	0b01111101, 0b10000100, 0b00000000
:	0b00000000, 0b01000000, 0b00000000

Horizontally mirrored characters:
0	0b00111111, 0b00000000, 0b00000000
1	0b00110000, 0b00000000, 0b00000000
2	0b01101101, 0b10000100, 0b00000000
3	0b01001111, 0b10000100, 0b00000000
4	0b01110010, 0b10000100, 0b00000000
5	0b01011011, 0b10000100, 0b00000000
6	0b01011111, 0b10000100, 0b00000000
7	0b00110001, 0b00000000, 0b00000000
8	0b01111111, 0b10000100, 0b00000000
9	0b01111011, 0b10000100, 0b00000000
:	0b00000000, 0b01000000, 0b00000000
*/

/* Special includes and config */
//#include "16F690.h"
#pragma config |= 0x00D4

#define VFD_I2C_ID 0b1010.0.0.0.0	/* Spec states the I2C ID is 0b1010.A2.A1.A0 where A2-0 is state of pin 5-7. The ID is followed by a zero, indicating I2C mode */

#include "i2c.c"			/* Modified SparkFun i2c library */

void vfd_init(void);
void vfd_brightness(char);
void vfd_set_address(char);
void vfd_send_char(char);
void vfd_send_segments(char, char, char, char);
void vfd_send_led(char);

char vfd_current_address_offset;

/*
void main(void)
{
	// Demo code for testing/debugging the library
	// Set all pins to inputs except for RC0, RB4 & RB6, this prevents frying the pic since we have no idea what might actually be connected
	TRISA = 0b11111111;
	TRISB = 0b10101111;
	TRISC = 0b11111110;

	int i;

	// Test sequence start
	PORTC.0 = 0;
	for (i = 0; i < 2; i++)
	{
		delay(250);
	}
	PORTC.0 = 1;
	vfd_init();
	PORTC.0 = 0;
	while (1)
	{
		PORTC.0 = 1;
		vfd_send_led(0b00000.111);
		vfd_send_char('1');
		vfd_send_char('2');
		vfd_send_char(':');
		vfd_send_char('3');
		vfd_send_char('4');
		vfd_send_char(':');
		vfd_send_char('5');
		vfd_send_char('6');
		vfd_send_char(' ');
		vfd_send_char('7');
		vfd_send_char('8');
		vfd_send_char('9');
		PORTC.0 = 0;
		delay(250);	// This is where we sleep for 1 sec, turn off debug LED
		delay(250);
		delay(250);
		delay(250);
	}
}
*/

void vfd_init(void)
{
	/* Init display, sets display to on */
	ANSEL = 0;		/* Disable all analog pins or else some read-modify-write operations will fail, spent hours debugging this */
	ANSELH = 0;
	vfd_current_address_offset = 0x00;
	start();
	send_byte(VFD_I2C_ID);
	send_byte(0b10.0.1.0000);
	stop();
}

void vfd_brightness(char brightness)
{
	/* Since we have 15 levels of brightness and a power bit, brightness is shifted up one step, i.e. 0x00 will turn off display and 0x0F is full power */
	if (brightness == 0)
	{
		brightness = 0b10.0.0.0000;
	}
	else
	{
		brightness &= 0b00.0.0.1111;
		brightness += 0b10.0.1.0000;
	}
	start();
	send_byte(VFD_I2C_ID);
	send_byte(brightness);
	stop();
}

void vfd_set_address(char address)
{
	vfd_current_address_offset = address;
}

#pragma codepage 1
void vfd_send_char(char c)
{
	if (vfd_current_address_offset > 0x21)
	{
		vfd_current_address_offset = 0x00;
	}

	switch(c)
	{
		case ' ':	vfd_send_segments(vfd_current_address_offset, 0b00000000, 0b00000000, 0b00000000);	break;
		case '\'':	vfd_send_segments(vfd_current_address_offset, 0b00000000, 0b00000001, 0b00000000);	break;
		case '+':	vfd_send_segments(vfd_current_address_offset, 0b01000000, 0b11010101, 0b00000000);	break;
		case ',':	vfd_send_segments(vfd_current_address_offset, 0b00000000, 0b00010000, 0b00000000);	break;
		case '-':	vfd_send_segments(vfd_current_address_offset, 0b01000000, 0b10000100, 0b00000000);	break;
		case '.':	vfd_send_segments(vfd_current_address_offset, 0b00000000, 0b10000000, 0b00000000);	break;
		case '0':	vfd_send_segments(vfd_current_address_offset, 0b00111111, 0b00000000, 0b00000000);	break;
		case '1':	vfd_send_segments(vfd_current_address_offset, 0b00110000, 0b00000000, 0b00000000);	break;
		case '2':	vfd_send_segments(vfd_current_address_offset, 0b01101101, 0b10000100, 0b00000000);	break;
		case '3':	vfd_send_segments(vfd_current_address_offset, 0b01111001, 0b10000100, 0b00000000);	break;
		case '4':	vfd_send_segments(vfd_current_address_offset, 0b01110010, 0b10000100, 0b00000000);	break;
		case '5':	vfd_send_segments(vfd_current_address_offset, 0b01011011, 0b10000100, 0b00000000);	break;
		case '6':	vfd_send_segments(vfd_current_address_offset, 0b01011111, 0b10000100, 0b00000000);	break;
		case '7':	vfd_send_segments(vfd_current_address_offset, 0b00110001, 0b00000000, 0b00000000);	break;
		case '8':	vfd_send_segments(vfd_current_address_offset, 0b01111111, 0b10000100, 0b00000000);	break;
		case '9':	vfd_send_segments(vfd_current_address_offset, 0b01111011, 0b10000100, 0b00000000);	break;
		case ':':	vfd_send_segments(vfd_current_address_offset, 0b00000000, 0b01000000, 0b00000000);	break;
		case '=':	vfd_send_segments(vfd_current_address_offset, 0b01001000, 0b10000100, 0b00000000);	break;
		case 'A':	vfd_send_segments(vfd_current_address_offset, 0b01110111, 0b10000100, 0b00000000);	break;
		case 'B':	vfd_send_segments(vfd_current_address_offset, 0b01111001, 0b10001010, 0b00000000);	break;
		case 'C':	vfd_send_segments(vfd_current_address_offset, 0b00001111, 0b00000000, 0b00000000);	break;
		case 'D':	vfd_send_segments(vfd_current_address_offset, 0b00111001, 0b00001010, 0b00000000);	break;
		case 'E':	vfd_send_segments(vfd_current_address_offset, 0b01001111, 0b10000100, 0b00000000);	break;
		case 'F':	vfd_send_segments(vfd_current_address_offset, 0b01000111, 0b10000100, 0b00000000);	break;
		case 'G':	vfd_send_segments(vfd_current_address_offset, 0b01011111, 0b00000000, 0b00000000);	break;
		case 'H':	vfd_send_segments(vfd_current_address_offset, 0b01110110, 0b10000100, 0b00000000);	break;
		case 'I':	vfd_send_segments(vfd_current_address_offset, 0b00000000, 0b11010001, 0b00000000);	break;
		case 'J':	vfd_send_segments(vfd_current_address_offset, 0b00111100, 0b00000000, 0b00000000);	break;
		case 'K':	vfd_send_segments(vfd_current_address_offset, 0b11010110, 0b10000100, 0b00000000);	break;
		case 'L':	vfd_send_segments(vfd_current_address_offset, 0b00001110, 0b00000000, 0b00000000);	break;
		case 'M':	vfd_send_segments(vfd_current_address_offset, 0b10110110, 0b10000010, 0b00000000);	break;
		case 'N':	vfd_send_segments(vfd_current_address_offset, 0b00110110, 0b10100010, 0b00000000);	break;
		case 'O':	vfd_send_segments(vfd_current_address_offset, 0b10001111, 0b00100000, 0b00000000);	break;
		case 'P':	vfd_send_segments(vfd_current_address_offset, 0b01100111, 0b10000100, 0b00000000);	break;
		case 'Q':	vfd_send_segments(vfd_current_address_offset, 0b10001111, 0b00110000, 0b00000000);	break;
		case 'R':	vfd_send_segments(vfd_current_address_offset, 0b01100111, 0b10100100, 0b00000000);	break;
		case 'S':	vfd_send_segments(vfd_current_address_offset, 0b00001011, 0b10100100, 0b00000000);	break;
		case 'T':	vfd_send_segments(vfd_current_address_offset, 0b00000001, 0b11010001, 0b00000000);	break;
		case 'U':	vfd_send_segments(vfd_current_address_offset, 0b00111110, 0b00000000, 0b00000000);	break;
		case 'V':	vfd_send_segments(vfd_current_address_offset, 0b00101110, 0b00100000, 0b00000000);	break;
		case 'W':	vfd_send_segments(vfd_current_address_offset, 0b00110110, 0b10101000, 0b00000000);	break;
		case 'X':	vfd_send_segments(vfd_current_address_offset, 0b10000000, 0b10101010, 0b00000000);	break;
		case 'Y':	vfd_send_segments(vfd_current_address_offset, 0b01100010, 0b11010100, 0b00000000);	break;
		case 'Z':	vfd_send_segments(vfd_current_address_offset, 0b10001001, 0b10001000, 0b00000000);	break;
		case '_':	vfd_send_segments(vfd_current_address_offset, 0b00001000, 0b00000000, 0b00000000);	break;
		case 'a':	vfd_send_segments(vfd_current_address_offset, 0b01110111, 0b10000100, 0b00000000);	break;
		case 'b':	vfd_send_segments(vfd_current_address_offset, 0b01111001, 0b10001010, 0b00000000);	break;
		case 'c':	vfd_send_segments(vfd_current_address_offset, 0b00001111, 0b00000000, 0b00000000);	break;
		case 'd':	vfd_send_segments(vfd_current_address_offset, 0b00111001, 0b00001010, 0b00000000);	break;
		case 'e':	vfd_send_segments(vfd_current_address_offset, 0b01001111, 0b10000100, 0b00000000);	break;
		case 'f':	vfd_send_segments(vfd_current_address_offset, 0b01000111, 0b10000100, 0b00000000);	break;
		case 'g':	vfd_send_segments(vfd_current_address_offset, 0b01011111, 0b00000000, 0b00000000);	break;
		case 'h':	vfd_send_segments(vfd_current_address_offset, 0b01110110, 0b10000100, 0b00000000);	break;
		case 'i':	vfd_send_segments(vfd_current_address_offset, 0b00000000, 0b11010001, 0b00000000);	break;
		case 'j':	vfd_send_segments(vfd_current_address_offset, 0b00111100, 0b00000000, 0b00000000);	break;
		case 'k':	vfd_send_segments(vfd_current_address_offset, 0b11010110, 0b10000100, 0b00000000);	break;
		case 'l':	vfd_send_segments(vfd_current_address_offset, 0b00001110, 0b00000000, 0b00000000);	break;
		case 'm':	vfd_send_segments(vfd_current_address_offset, 0b10110110, 0b10000010, 0b00000000);	break;
		case 'n':	vfd_send_segments(vfd_current_address_offset, 0b00110110, 0b10100010, 0b00000000);	break;
		case 'o':	vfd_send_segments(vfd_current_address_offset, 0b10001111, 0b00100000, 0b00000000);	break;
		case 'p':	vfd_send_segments(vfd_current_address_offset, 0b01100111, 0b10000100, 0b00000000);	break;
		case 'q':	vfd_send_segments(vfd_current_address_offset, 0b10001111, 0b00110000, 0b00000000);	break;
		case 'r':	vfd_send_segments(vfd_current_address_offset, 0b01100111, 0b10100100, 0b00000000);	break;
		case 's':	vfd_send_segments(vfd_current_address_offset, 0b00001011, 0b10100100, 0b00000000);	break;
		case 't':	vfd_send_segments(vfd_current_address_offset, 0b00000001, 0b11010001, 0b00000000);	break;
		case 'u':	vfd_send_segments(vfd_current_address_offset, 0b00111110, 0b00000000, 0b00000000);	break;
		case 'v':	vfd_send_segments(vfd_current_address_offset, 0b00101110, 0b00100000, 0b00000000);	break;
		case 'w':	vfd_send_segments(vfd_current_address_offset, 0b00110110, 0b10101000, 0b00000000);	break;
		case 'x':	vfd_send_segments(vfd_current_address_offset, 0b10000000, 0b10101010, 0b00000000);	break;
		case 'y':	vfd_send_segments(vfd_current_address_offset, 0b01100010, 0b11010100, 0b00000000);	break;
		case 'z':	vfd_send_segments(vfd_current_address_offset, 0b10001001, 0b10001000, 0b00000000);	break;
		default:	vfd_send_segments(vfd_current_address_offset, 0b11111111, 0b11111111, 0b00000000);	break;
	}
	vfd_current_address_offset += 0x03;
}
#pragma codepage 0

void vfd_send_segments(char address, char c, char d, char e)
{
	start();
	send_byte(VFD_I2C_ID);
	send_byte(0b11.000000 | address);
	send_byte(c);
	send_byte(d);
	send_byte(e);
	stop();
}

void vfd_send_led(char c)
{
	start();
	send_byte(VFD_I2C_ID);
	send_byte(0b01.00.0.0.0.1);
	send_byte(c);
	stop();
}

