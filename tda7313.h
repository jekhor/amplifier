//*****************************************************************************
void send_tda7313(void)
{
	unsigned char sw_reg = TDA7313_SWITCH_REG, k;

	k = param_tda[VOLUME_MENU] - 1;
	i2c_start();
	i2c_write(TDA7313_ADDR);
#if (VOLUME_MAX == DEF_VOLUME_MAX_16)
	i2c_write(TDA7313_VOLUME_REG + (0x3F - (k * 4)));
	i2c_stop();
#endif
#if (VOLUME_MAX == DEF_VOLUME_MAX_32)
	i2c_write(TDA7313_VOLUME_REG + (0x3F - (k * 2)));
	i2c_stop();
#endif
#if (VOLUME_MAX == DEF_VOLUME_MAX_64)
	i2c_write(TDA7313_VOLUME_REG + (0x3F - k));
	i2c_stop();
#endif

	k = param_tda[ATTENS_MENU] - 1;
	sw_reg = sw_reg + (n_select - 1) + attens[k];
	if (l_londness)
		sw_reg &= ~(_BV(2));
	else
		sw_reg |= _BV(2);
	i2c_start();
	i2c_write(TDA7313_ADDR);
	i2c_write(sw_reg);
	i2c_stop();

	k = param_tda[ATTENS_FL_MENU] - 1;
	i2c_start();
	i2c_write(TDA7313_ADDR);
	i2c_write(TDA7313_ATT_LF_REG + (TDA7313_ATT_CH_MAX - k));
	i2c_stop();

	k = param_tda[ATTENS_FR_MENU] - 1;
	i2c_start();
	i2c_write(TDA7313_ADDR);
	i2c_write(TDA7313_ATT_RF_REG + (TDA7313_ATT_CH_MAX - k));
	i2c_stop();

	k = param_tda[ATTENS_RL_MENU] - 1;
	i2c_start();
	i2c_write(TDA7313_ADDR);
	i2c_write(TDA7313_ATT_LR_REG + (TDA7313_ATT_CH_MAX - k));
	i2c_stop();

	k = param_tda[ATTENS_RR_MENU] - 1;
	i2c_start();
	i2c_write(TDA7313_ADDR);
	i2c_write(TDA7313_ATT_RR_REG + (TDA7313_ATT_CH_MAX - k));
	i2c_stop();

	k = param_tda[BASS_MENU] - 1;
	i2c_start();
	i2c_write(TDA7313_ADDR);
	i2c_write(TDA7313_BASS_REG + bass_treb[k]);
	i2c_stop();

	k = param_tda[TREBLE_MENU] - 1;
	i2c_start();
	i2c_write(TDA7313_ADDR);
	i2c_write(TDA7313_TREBLE_REG + bass_treb[k]);
	i2c_stop();
}

//*****************************************************************************
void set_mute(void)
{
	unsigned char n;
	nmenu = 0;
	omenu = 0;
	reg_menu = REG_MAINMENU;
	if (l_mute) {
		lcd_com(0x80);
		lcd_mainmenu();
		save_volume_mute = param_tda[VOLUME_MENU];
		for (n = save_volume_mute; n > 0; n--) {
			param_tda[VOLUME_MENU] = n;
			send_tda7313();
			lcdprogress_bar(param_tda[VOLUME_MENU],
					menu_max[VOLUME_MENU], 16);
			lcd_com(0x8D);
			print_dec(param_tda[VOLUME_MENU], 2, '0');
		}
		i2c_start();
		i2c_write(TDA7313_ADDR);
		i2c_write(TDA7313_MUTE_LR);
		i2c_stop();	// Mute LF
		i2c_start();
		i2c_write(TDA7313_ADDR);
		i2c_write(TDA7313_MUTE_RR);
		i2c_stop();	// Mute RR
		i2c_start();
		i2c_write(TDA7313_ADDR);
		i2c_write(TDA7313_MUTE_LF);
		i2c_stop();	// Mute RF
		i2c_start();
		i2c_write(TDA7313_ADDR);
		i2c_write(TDA7313_MUTE_RF);
		i2c_stop();	// Mute LR
		PORT_LED_MUTE &= ~(_BV(PIN_LED_MUTE));
		lcd_clear();
		lcd_puts(msg[MSG_MUTE]);
		main_delay(32);
	} else {
		lcd_com(0x80);
		lcd_mainmenu();
		for (n = 1; n < save_volume_mute + 1; n++) {
			param_tda[VOLUME_MENU] = n;
			send_tda7313();
			lcdprogress_bar(param_tda[VOLUME_MENU],
					menu_max[VOLUME_MENU], 16);
			lcd_com(0x8D);
			print_dec(param_tda[VOLUME_MENU], 2, '0');
		}
		PORT_LED_MUTE |= _BV(PIN_LED_MUTE);
		sleep_flag = 0;
	}
}

//*****************************************************************************
void set_select(void)
{
	PORT_LED_INPUT |= _BV(LED_INPUT1);
	PORT_LED_INPUT |= _BV(LED_INPUT2);
	PORT_LED_INPUT |= _BV(LED_INPUT3);
	if (!l_standby) {
		switch (n_select)	// switch
		{
		case 1:
			PORT_LED_INPUT &= ~(_BV(LED_INPUT1));
			break;
		case 2:
			PORT_LED_INPUT &= ~(_BV(LED_INPUT2));
			break;
		case 3:
			PORT_LED_INPUT &= ~(_BV(LED_INPUT3));
			break;
		}
	}
}

//*****************************************************************************
void set_londness(unsigned char l_lond)
{
	if ((l_lond) && (!l_standby)) {
		PORT_LED_LONDNESS &= ~(_BV(LED_LONDNESS));
	} else {
		PORT_LED_LONDNESS |= _BV(LED_LONDNESS);
	}
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
