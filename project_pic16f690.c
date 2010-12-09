/* Digital speedo using GPS, Knudsen version */
/* Written by Axel Norstedt */

/* TODO:

Add checksum control
*/

/*
         _______________
        |       \_/       |
  +5V >-| Vdd         Vss |-< GND
       -| RA5         RA0 |-> LED1 (optional)
       -| RA4         RA1 |-> LED2 (optional)
       -| RA3         RA2 |-> LED3 (optional)
HEX 8 >-| RC5         RC0 |-
HEX 4 >-| RC4         RC1 |-
HEX 2 >-| RC3         RC2 |-
HEX 1 >-| RC6     SDA/RB4 |-> VFD SDL
       -| RC7      RX/RB5 |-< GPS TX
       -| RB7     SCL/RB6 |-> VFD SCL
        |_________________|

HEX are the inputs from brightness adjustment hex coding switch, this is the one I used https://www.elfa.se/elfa3~eu_en/elfa/init.do?item=35-300-52&toc=20482
*/

#include "16F690.h"
#pragma config |= 0x00D4

#include "speed2string.c"			/* Speed integer to string by Dan Norstedt */
#include "m12by02aa.c"				/* Driver for Futaba M12BY02AA */
#include "br-355.c"				/* Driver for GlobalSat BR-355 */

#define NMEA_MAX_LENGTH 82
#define TIME_MAX_LENGTH 10			/* Always has length 10 (123456.789) */
#define SPEED_MAX_LENGTH 6
#define SPEED_OUTPUT_LENGTH 3			/* Number of speed characters to send to VFD, use formula VFD SIZE - TIME - SPACER = SPEED_OUTPUT_LENGTH (12 - 8 - 1 = 3) */

#define debug_led_on() PORTC.0=1
#define debug_led_off() PORTC.0=0
#define debug_led2_on() PORTC.1=1
#define debug_led2_off() PORTC.1=0
#define debug_led3_on() PORTC.2=1
#define debug_led3_off() PORTC.2=0
//#define overclocking_on() OSCCON.4=1		/* Overclocking to 8MHz, disable EUSART first! */
//#define overclocking_off() OSCCON.4=0		/* Back to 4MHz */
#define eusart_on() CREN=1			/* Enable EUSART */
#define eusart_off() CREN=0			/* Disable EUSART */

void debug(char);				/* Debug function for printing a char and it's binary value to display */

void main(void)
{
	char c, i, field = 0, time_index = 0, speed_index = 0, speed_index_decimal = 0, brightness;
	char time[TIME_MAX_LENGTH];
	char speed[SPEED_MAX_LENGTH];
	int16 speed_int = 0;
	bit capture = 0;

	TRISC = 0b11111.000;
	PORTC = 0;
	vfd_init();
	vfd_brightness(0x01);
	vfd_send_led(0b00000.000);
	gps_init();

	for (;;)
	{
		debug_led_on();
		c = gps_get_char();
		debug_led_off();
		if (c == '$' && gps_get_char() == 'G' && gps_get_char() == 'P' && gps_get_char() == 'R' && gps_get_char() == 'M' && gps_get_char() == 'C')
		{
			field = 0;
			time_index = speed_index = 0;
			capture = 1;
			debug_led2_on();
		}
		else if (capture && c == '*')
		{
			debug_led2_off();
			debug_led3_on();
			eusart_off();

			/* We don't have to worry about getting another sentence for a long time and EUSART is off, do heavy stuff */
			vfd_send_led(0b00000.111);
			brightness = 0;
			brightness.0 = PORTC.6;
			brightness.1 = PORTC.3;
			brightness.2 = PORTC.4;
			brightness.3 = PORTC.5;
			vfd_brightness(brightness);

			/* Fix timezone and DST */
			time[1] += 1 + PORTC.7;
			if (time[1] > '9')
			{
				time[0] += 1;
				time[1] -= 10;
			}
			if (time[0] == '2' && time[1] > '3')
			{
				time[0] -= 2;
				time[1] -= 4;
			}

			for (i = 0; i < 6; i++)
			{
				if (i != 0)
				{
					vfd_send_char(':');
				}
				vfd_send_char(time[i++]);
				vfd_send_char(time[i]);
			}

			speed_int = 0;
			for (i = 0; i < speed_index_decimal+2; i++)
			{
				int16 speed_int2;			/* avoid multiplication */
				speed_int2 = speed_int + speed_int;	/* speed_int * 2 */
				speed_int2 = speed_int2 + speed_int2;	/* speed_int * 4 */
				speed_int2 = speed_int2 + speed_int;	/* speed_int * 5 */
				speed_int = speed_int2 + speed_int2;	/* speed_int * 10 */
				speed_int += speed[i] - '0';
			}

			print054(speed_int);

			/* Send speed to display */
			vfd_send_char(' ');
			if (speed_index > 0)
			{
				if (hundreds != '0')
				{
					vfd_send_char(hundreds);
					vfd_send_char(tens);
				}
				else if (tens != '0')
				{
					vfd_send_char(' ');
					vfd_send_char(tens);
				}
				else
				{
					vfd_send_char(' ');
					vfd_send_char(' ');
				}
				vfd_send_char(ones);
			}
			else
			{
				vfd_send_char(' ');
				vfd_send_char(' ');
				vfd_send_char(' ');
			}

			capture = 0;
			eusart_on();
			debug_led3_off();
		}
		else if (capture && c == ',')
		{
			field++;
		}
		else if (capture && c != ',' && field == 1)
		{
			
			time[time_index] = c;
			time_index++;
		}
		else if (capture && c != ',' && field == 7)
		{
			if (c != '.')
			{
				speed[speed_index] = c;
				speed_index++;
			}
			else
			{
				speed[speed_index+1] = '0';
				speed[speed_index+2] = '0';
				speed_index_decimal = speed_index;
			}
		}
	}
}

void debug(char c)
{
	vfd_current_address_offset = 0x00;
	vfd_send_char(' ');
	vfd_send_char(c);
	vfd_send_char(':');
	vfd_send_char(c.7 + '0');
	vfd_send_char(c.6 + '0');
	vfd_send_char(c.5 + '0');
	vfd_send_char(c.4 + '0');
	vfd_send_char(c.3 + '0');
	vfd_send_char(c.2 + '0');
	vfd_send_char(c.1 + '0');
	vfd_send_char(c.0 + '0');
	vfd_send_char(' ');
}

