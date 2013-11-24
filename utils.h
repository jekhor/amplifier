//*****************************************************************************
void main_delay(unsigned char i)
{
	unsigned char n;
	for (n = 0; n < i; n++) {
		_delay_ms(250);
	}
}

//*****************************************************************************
void beep(void)
{
	PORT_BEEP &= ~(_BV(PIN_BEEP));
	_delay_ms(50);
	PORT_BEEP |= _BV(PIN_BEEP);
}

//*****************************************************************************
void beep_long(void)
{
	PORT_BEEP &= ~(_BV(PIN_BEEP));
	main_delay(BEEP_LONG);
	PORT_BEEP |= _BV(PIN_BEEP);
}

//*****************************************************************************
void flash(unsigned char ms)
{
	PORT_LED_RC5 &= ~(_BV(LED_RC5));
	_delay_ms(30);
	PORT_LED_RC5 |= _BV(LED_RC5);
}

//*****************************************************************************
void beep_flash(void)
{
	PORT_BEEP &= ~(_BV(PIN_BEEP));
	PORT_LED_RC5 &= ~(_BV(LED_RC5));
	_delay_ms(50);
	PORT_BEEP |= _BV(PIN_BEEP);
	PORT_LED_RC5 |= _BV(LED_RC5);
}

//*****************************************************************************
unsigned char bin2bcd(unsigned char bin)
{
	unsigned Temp = 0;
	while (bin > 9) {
		Temp += 0x10;
		bin -= 10;
	}
	return Temp + bin;
}

//*****************************************************************************
void print_dec(unsigned int x, unsigned char n, unsigned char fillch)
{
	unsigned char i;
	unsigned char s[10];
	for (i = 0; i < n; i++) {
		s[n - i - 1] = '0' + (x % 10);
		x /= 10;
	}
	for (i = 0; i < (n - 1); i++) {
		if (s[i] == '0')
			s[i] = fillch;
		else
			break;
	}
	for (i = 0; i < n; i++)
		lcd_dat(s[i]);
}

//*****************************************************************************
