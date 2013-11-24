//*****************************************************************************
void run_mainmenu(unsigned char key)
{
	unsigned char n;
//-----------------------------------------------------------------------------
	switch (key)		// switch
	{
	case KBD_STANDBY:
		l_standby = !l_standby;
		beep_flash();
		if (l_mute)
			set_standby(0);
		else
			set_standby(1);
		break;
	case KBD_MUTE:
		beep_flash();
		l_mute = !l_mute;
		set_mute();
		break;
	case KBD_DOWN:
		if (nmenu == MENU_MAX)
			nmenu = 0;
		else
			nmenu++;
		break;
	case KBD_UP:
		if (nmenu > 0)
			nmenu--;
		else
			nmenu = MENU_MAX;
		break;
	case KBD_SELECT1:
		n_select = 1;
		set_select();
		break;
	case KBD_SELECT2:
		n_select = 2;
		set_select();
		break;
	case KBD_SELECT3:
		n_select = 3;
		set_select();
		break;
	case KBD_LEFT:
		switch (nmenu)	// switch
		{
		case VOLUME_MENU:
			if (param_tda[VOLUME_MENU] > 1)
				param_tda[VOLUME_MENU]--;
			break;
		case ATTENS_MENU:
			if (param_tda[ATTENS_MENU] > 1)
				param_tda[ATTENS_MENU]--;
			break;
		case BASS_MENU:
			if (param_tda[BASS_MENU] > 1)
				param_tda[BASS_MENU]--;
			break;
		case TREBLE_MENU:
			if (param_tda[TREBLE_MENU] > 1)
				param_tda[TREBLE_MENU]--;
			break;
		case ATTENS_FL_MENU:
			if (param_tda[ATTENS_FL_MENU] > 1)
				param_tda[ATTENS_FL_MENU]--;
			break;
		case ATTENS_FR_MENU:
			if (param_tda[ATTENS_FR_MENU] > 1)
				param_tda[ATTENS_FR_MENU]--;
			break;
		case ATTENS_RL_MENU:
			if (param_tda[ATTENS_RL_MENU] > 1)
				param_tda[ATTENS_RL_MENU]--;
			break;
		case ATTENS_RR_MENU:
			if (param_tda[ATTENS_RR_MENU] > 1)
				param_tda[ATTENS_RR_MENU]--;
			break;
		}
		break;
	case KBD_RIGHT:
		switch (nmenu)	// switch
		{
		case VOLUME_MENU:
			if (param_tda[VOLUME_MENU] < menu_max[VOLUME_MENU])
				param_tda[VOLUME_MENU]++;
			break;
		case ATTENS_MENU:
			if (param_tda[ATTENS_MENU] < menu_max[ATTENS_MENU])
				param_tda[ATTENS_MENU]++;
			break;
		case BASS_MENU:
			if (param_tda[BASS_MENU] < menu_max[BASS_MENU])
				param_tda[BASS_MENU]++;
			break;
		case TREBLE_MENU:
			if (param_tda[TREBLE_MENU] < menu_max[TREBLE_MENU])
				param_tda[TREBLE_MENU]++;
			break;
		case ATTENS_FL_MENU:
			if (param_tda[ATTENS_FL_MENU] <
			    menu_max[ATTENS_FL_MENU])
				param_tda[ATTENS_FL_MENU]++;
			break;
		case ATTENS_FR_MENU:
			if (param_tda[ATTENS_FR_MENU] <
			    menu_max[ATTENS_FR_MENU])
				param_tda[ATTENS_FR_MENU]++;
			break;
		case ATTENS_RL_MENU:
			if (param_tda[ATTENS_RL_MENU] <
			    menu_max[ATTENS_RL_MENU])
				param_tda[ATTENS_RL_MENU]++;
			break;
		case ATTENS_RR_MENU:
			if (param_tda[ATTENS_RR_MENU] <
			    menu_max[ATTENS_RR_MENU])
				param_tda[ATTENS_RR_MENU]++;
			break;
		}
		break;
	case KBD_SET:
		if (!l_standby) {
			l_info = 1;
			beep_flash();
			lcd_clear();
			if (!l_mute) {
				for (n = 0; n < 16; n++) {
					lcd_show_temp_sleep_mute_stb(1);
					main_delay(8);
					nokey_timer_cnt = 0;
					nokey_flag = 0;
					sleep_flag = 0;
					sleep_timer_cnt = 0;
				}
			} else {
				l_mute_show = !l_mute_show;
				if (!l_mute_show) {
					lcd_clear();
					lcd_puts(msg[MSG_MUTE]);
				}
				eeprom_write_byte((uint8_t *) TYPE_MUTE_SHOW,
						  l_mute_show);
			}
			l_info = 0;
			lcd_clear();
			lcd_mainmenu();
		}
		break;
	case KBD_MENU:
		lcd_clear();
		omenu = 0;
		reg_menu = REG_OPTIONMENU;
		timer_nokey_count = TIMER1_NOKEY_OPTION;
		rtc_get_time(&h1, &m1, &s1);
		rtc_get_date(&d1, &mes1, &year1);
		n_sleep_tmp = n_sleep;
		n_auto_stb_tmp = n_auto_stb;
		n_ds_temp_max_tmp = n_ds_temp_max;
		stb_out_tmp = stb_out;
		lcd_optionmenu();
		break;
	case KBD_LOND:
		l_londness = !l_londness;
		set_londness(l_londness);
		break;
	case KBD_BASS_UP:
		nmenu = BASS_MENU;
		if (param_tda[BASS_MENU] > 1)
			param_tda[BASS_MENU]--;
		break;
	case KBD_BASS_DOWN:
		nmenu = BASS_MENU;
		if (param_tda[BASS_MENU] < menu_max[BASS_MENU])
			param_tda[BASS_MENU]++;
		break;
	case KBD_TREBLE_UP:
		nmenu = TREBLE_MENU;
		if (param_tda[TREBLE_MENU] > 1)
			param_tda[TREBLE_MENU]--;
		break;
	case KBD_TREBLE_DOWN:
		nmenu = TREBLE_MENU;
		if (param_tda[TREBLE_MENU] < menu_max[TREBLE_MENU])
			param_tda[TREBLE_MENU]++;
		break;
	}
	if ((key != KBD_MENU) && (key != KBD_SET) && (!l_mute) && (!l_standby))
		send_tda7313();
	if ((key != KBD_STANDBY) && (key != KBD_MUTE))
		save_EEprom_param();
	if ((key != KBD_MENU) && (!l_standby) && (!l_mute)) {
		lcd_mainmenu();
	}
//-----------------------------------------------------------------------------
}

//*****************************************************************************
void lcd_mainmenu(void)
{
	lcd_com(0x80);
	lcd_dat('[');
	lcd_dat(0x30 + n_select);
	lcd_dat(']');
	if (n_auto_stb > 0)
		lcd_dat(0xEB);
	else
		lcd_dat(' ');
	lcd_puts(mainmenu[nmenu]);
	lcd_com(0x8D);
	print_dec(param_tda[nmenu], 2, '0');
	if (l_londness)
		lcd_dat('L');
	else
		lcd_dat(' ');
	lcdprogress_bar(param_tda[nmenu], menu_max[nmenu], 16);
}

//*****************************************************************************
