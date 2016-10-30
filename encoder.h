//*****************************************************************************
const int8_t enc_states[] = {
	/*
	 * old / new
	 * 00	00 01 10 11
	 * 01	00 01 10 11
	 * 10	00 01 10 11
	 * 11	00 01 10 11
	 */
	0,-1,1,0,
	1,0,0,-1,
	-1,0,0,1,
	0,1,-1,0
};
void encoder_scan(void)
{
	unsigned char enc_A = 0, enc_B = 0;
	static uint8_t old_AB = 0;

	cli();

	enc_A = !!bit_is_set(ENCODER_PIN_PORT, ENCODER_PIN_A);
	enc_B = !!bit_is_set(ENCODER_PIN_PORT, ENCODER_PIN_B);


	old_AB <<= 2;                   //remember previous state
	old_AB |= (enc_A << 1) | enc_B; //add current state

	if (enc_states[old_AB & 0x0f] == 1) {
		enc.code = ENCODER_RIGHT;
		enc.flip = 1;
	} else if (enc_states[old_AB & 0x0f] == -1) {
		enc.code = ENCODER_LEFT;
		enc.flip = 1;
	}

	sei();
}

//*****************************************************************************
void encoder_init(void)
{
	enc.code = ENCODER_NOKEY;
	enc.flip = -1;
}

//*****************************************************************************
