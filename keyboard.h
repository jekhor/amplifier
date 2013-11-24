//*****************************************************************************
void kbd_scan(void)
{
	cli();
	kbd_key = KBD_NOKEY;
// ROW1 -----------------------------------------------------------------------
	if ((!l_standby) && (!l_mute)) {
		KBD_PORT_ROW1 &= ~(_BV(KBD_PIN_ROW1));
		__asm__ volatile ("nop");
		if (debounce_sw_col1_1() == 1)
			kbd_key = KBD_SELECT1;
		if (debounce_sw_col2_1() == 1)
			kbd_key = KBD_SELECT2;
		if (debounce_sw_col3_1() == 1)
			kbd_key = KBD_SELECT3;
		KBD_PORT_ROW1 |= _BV(KBD_PIN_ROW1);
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
// ROW2 -----------------------------------------------------------------------
		KBD_PORT_ROW2 &= ~(_BV(KBD_PIN_ROW2));
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		if (debounce_sw_col1_2() == 1)
			kbd_key = KBD_LOND;
		if (debounce_sw_col2_2() >= 1)
			kbd_key = KBD_RIGHT;
		if (debounce_sw_col3_2() == 1)
			kbd_key = KBD_MENU;
		KBD_PORT_ROW2 |= _BV(KBD_PIN_ROW2);
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		KBD_PORT_ROW3 &= ~(_BV(KBD_PIN_ROW3));
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		if (debounce_sw_col1_3() == 1)
			kbd_key = KBD_UP;
		if (debounce_sw_col3_3() == 1)
			kbd_key = KBD_DOWN;
		KBD_PORT_ROW3 |= _BV(KBD_PIN_ROW3);
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
	}
// ROW3 -----------------------------------------------------------------------
	KBD_PORT_ROW3 &= ~(_BV(KBD_PIN_ROW3));
	__asm__ volatile ("nop");
	__asm__ volatile ("nop");
	if (debounce_sw_col2_3() == 1) {
		if (!l_standby)
			kbd_key = KBD_SET;
	}
	KBD_PORT_ROW3 |= _BV(KBD_PIN_ROW3);
	__asm__ volatile ("nop");
	__asm__ volatile ("nop");
// ROW4 -----------------------------------------------------------------------
	KBD_PORT_ROW4 &= ~(_BV(KBD_PIN_ROW4));
	__asm__ volatile ("nop");
	__asm__ volatile ("nop");
	if (debounce_sw_col1_4() == 1) {
		if (!l_standby)
			kbd_key = KBD_MUTE;
	}
	if (debounce_sw_col2_4() >= 1) {
		if ((!l_standby) && (!l_mute))
			kbd_key = KBD_LEFT;
	}
	if (debounce_sw_col3_4() == 1)
		kbd_key = KBD_STANDBY;
	KBD_PORT_ROW4 |= _BV(KBD_PIN_ROW4);
	__asm__ volatile ("nop");
	__asm__ volatile ("nop");
	sei();
}

//*****************************************************************************
static unsigned char debounce_sw_col1_1(void)
{
	static uint16_t state1 = 0;	//holds present state
	state1 =
	    (state1 << 1) | (!bit_is_clear(KBD_PORT_COL1, KBD_PIN_COL1)) |
	    0xE000;
	if (state1 == 0xF000)
		return 1;
	if (state1 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col2_1(void)
{
	static uint16_t state2 = 0;	//holds present state
	state2 =
	    (state2 << 1) | (!bit_is_clear(KBD_PORT_COL2, KBD_PIN_COL2)) |
	    0xE000;
	if (state2 == 0xF000)
		return 1;
	if (state2 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col3_1(void)
{
	static uint16_t state3 = 0;	//holds present state
	state3 =
	    (state3 << 1) | (!bit_is_clear(KBD_PORT_COL3, KBD_PIN_COL3)) |
	    0xE000;
	if (state3 == 0xF000)
		return 1;
	if (state3 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col1_2(void)
{
	static uint16_t state4 = 0;	//holds present state
	state4 =
	    (state4 << 1) | (!bit_is_clear(KBD_PORT_COL1, KBD_PIN_COL1)) |
	    0xE000;
	if (state4 == 0xF000)
		return 1;
	if (state4 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col2_2(void)
{
	static uint16_t state5 = 0;	//holds present state
	state5 =
	    (state5 << 1) | (!bit_is_clear(KBD_PORT_COL2, KBD_PIN_COL2)) |
	    0xE000;
	if (state5 == 0xF000)
		return 1;
	if (state5 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col3_2(void)
{
	static uint16_t state6 = 0;	//holds present state
	state6 =
	    (state6 << 1) | (!bit_is_clear(KBD_PORT_COL3, KBD_PIN_COL3)) |
	    0xE000;
	if (state6 == 0xF000)
		return 1;
	if (state6 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col1_3(void)
{
	static uint16_t state7 = 0;	//holds present state
	state7 =
	    (state7 << 1) | (!bit_is_clear(KBD_PORT_COL1, KBD_PIN_COL1)) |
	    0xE000;
	if (state7 == 0xF000)
		return 1;
	if (state7 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col2_3(void)
{
	static uint16_t state8 = 0;	//holds present state
	state8 =
	    (state8 << 1) | (!bit_is_clear(KBD_PORT_COL2, KBD_PIN_COL2)) |
	    0xE000;
	if (state8 == 0xF000)
		return 1;
	if (state8 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col3_3(void)
{
	static uint16_t state9 = 0;	//holds present state
	state9 =
	    (state9 << 1) | (!bit_is_clear(KBD_PORT_COL3, KBD_PIN_COL3)) |
	    0xE000;
	if (state9 == 0xF000)
		return 1;
	if (state9 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col1_4(void)
{
	static uint16_t state10 = 0;	//holds present state
	state10 =
	    (state10 << 1) | (!bit_is_clear(KBD_PORT_COL1, KBD_PIN_COL1)) |
	    0xE000;
	if (state10 == 0xF000)
		return 1;
	if (state10 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col2_4(void)
{
	static uint16_t state11 = 0;	//holds present state
	state11 =
	    (state11 << 1) | (!bit_is_clear(KBD_PORT_COL2, KBD_PIN_COL2)) |
	    0xE000;
	if (state11 == 0xF000)
		return 1;
	if (state11 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
static unsigned char debounce_sw_col3_4(void)
{
	static uint16_t state12 = 0;	//holds present state
	state12 =
	    (state12 << 1) | (!bit_is_clear(KBD_PORT_COL3, KBD_PIN_COL3)) |
	    0xE000;
	if (state12 == 0xF000)
		return 1;
	if (state12 == 0xE000)
		return 2;
	return 0;
}

//*****************************************************************************
