int24 font[256];

void font_init()
{
	font[' '] = 0b0000.0000.0000.0000;
	font['\''] = 0b0000.0000.0000.0001;
	font['+'] = 0b0100.0000.1101.0101;
	font[','] = 0b0000.0000.0001.0000;
	font['-'] = 0b0100.0000.1000.0100;
	font['.'] = 0b0000.0000.1000.0000;
	font['0'] = 0b0011.1111.0000.0000;
	font['1'] = 0b0011.0000.0000.0000;
	font['2'] = 0b0110.1101.1000.0100;
	font['3'] = 0b0111.1001.1000.0100;
	font['4'] = 0b0110.0010.1000.0100;
	font['5'] = 0b0101.1011.1000.0100;
	font['6'] = 0b0101.1111.1000.0100;
	font['7'] = 0b0011.0001.0000.0000;
	font['8'] = 0b0111.1111.1000.0100;
	font['9'] = 0b0111.1011.1000.0100;
	font[':'] = 0b0000.0000.0100.0000;
	font['='] = 0b0100.1000.1000.0100;
	font['A'] = 0b0111.0111.1000.0100;
	font['B'] = 0b0111.1001.1000.1010;
	font['C'] = 0b0000.1111.0000.0000;
	font['D'] = 0b0011.1001.0000.1010;
	font['E'] = 0b0100.1111.1000.0100;
	font['F'] = 0b0100.0111.1000.0100;
	font['G'] = 0b0101.1111.0000.0000;
	font['H'] = 0b0111.0110.1000.0100;
	font['I'] = 0b0000.0000.1101.0001;
	font['J'] = 0b0011.1100.0000.0000;
	font['K'] = 0b1101.0110.1000.0100;
	font['L'] = 0b0000.1110.0000.0000;
	font['M'] = 0b1011.0110.1000.0010;
	font['N'] = 0b0011.0110.1010.0010;
	font['O'] = 0b1000.1111.0010.0000;
	font['P'] = 0b0110.0111.1000.0100;
	font['Q'] = 0b1000.1111.0011.0000;
	font['R'] = 0b0110.0111.1010.0100;
	font['S'] = 0b0000.1011.1010.0100;
	font['T'] = 0b0000.0001.1101.0001;
	font['U'] = 0b0011.1110.0000.0000;
	font['V'] = 0b0010.1110.0010.0000;
	font['W'] = 0b0011.0110.1010.1000;
	font['X'] = 0b1000.0000.1010.1010;
	font['Y'] = 0b0110.0010.1101.0100;
	font['Z'] = 0b1000.1001.1000.1000;
	font['_'] = 0b0000.1000.0000.0000;
}

