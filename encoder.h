//*****************************************************************************
void encoder_scan(void)
{
	cli();
	unsigned char enc_A, enc_B, bit_0, bit_1;
	if (bit_is_clear(ENCODER_PIN_PORT, ENCODER_PIN_A))
		enc_A = 0;
	else
		enc_A = 1;
	if (bit_is_clear(ENCODER_PIN_PORT, ENCODER_PIN_B))
		enc_B = 0;
	else
		enc_B = 1;
	if (bit_is_clear(encoder_status, 0))
		bit_0 = 0;
	else
		bit_0 = 1;
	if (bit_is_clear(encoder_status, 1))
		bit_1 = 0;
	else
		bit_1 = 1;
	if ((enc_A != bit_0) || (enc_B != bit_1)) {
		encoder_status = encoder_status << 2;
		if (enc_A == 0)
			encoder_status &= ~(_BV(0));
		else
			encoder_status |= _BV(0);
		if (enc_B == 0)
			encoder_status &= ~(_BV(1));
		else
			encoder_status |= _BV(1);
		if (encoder_status == enc_mask_minus) {
			enc.code = ENCODER_LEFT;
			enc.flip = 1;
		}
		if (encoder_status == enc_mask_plus) {
			enc.code = ENCODER_RIGHT;
			enc.flip = 1;
		}
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
