/* Driver for GlobalSat BR-355, compile with knudsen cc5x */
/* Based on some concepts and code from "echo690u.c" by William Sandqvist, modified by Axel Norstedt */

/*
       _______________
      |       \_/       |
+5V >-| Vdd         Vss |-< GND
     -| RA5         RA0 |-
     -| RA4         RA1 |-
     -| RA3         RA2 |-
     -| RC5         RC0 |-> LED Debug
     -| RC4         RC1 |-
     -| RC3         RC2 |-
     -| RC6     SDA/RB4 |-
     -| RC7      RX/RB5 |-< GPS TX
     -| RB7     SCL/RB6 |-
      |_________________|
*/

#pragma config |= 0x00D4

void gps_init(void);
char gps_get_char(void);

void gps_init(void)
{
	/* Init gps, setup pins */
	ANSELH.3 = 0;	/* Disable analog output for RX pin */
	SYNC = 0;	/* Asyncronous */
	SPEN = 1;	/* Enable EUSART */
	BRGH = 0;	/* Multiplier for Baud Rate Generator, look up for 4800 baud in documentation */
	BRG16 = 1;	/* Complement to above */
	SPBRG = 51;	/* The tricky part, ((16000000/4800 baud)/64)-1 = 51 for 4MHz, 103 for 8MHz */
	CREN = 1;	/* Quoted from documentation, "Setting the CREN bit of the RCSTA register enables the receiver circuitry of the EUSART." */
	RX9 = 0;	/* We want 8-bit reception, not 9-bit */
}

char gps_get_char(void)
{
	/* Looks a bit like getchar() from echo690u.c */
	OERR = 0;
	CREN = 1;
	while (RCIF == 0);	/* Read recieve interrupt flag, 1 when character(s) are avalible for read */
	return RCREG;
}

