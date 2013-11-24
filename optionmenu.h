//*****************************************************************************
void run_optionmenu(unsigned char key)
{
	switch (key)		// switch
	{
	case KBD_STANDBY:
		l_standby = 1;
		set_standby(1);
		break;
	case KBD_MUTE:
		l_mute = !l_mute;
		set_mute();
		break;
	case KBD_DOWN:
		if (omenu == OPTIONMENU_MAX)
			omenu = 0;
		else
			omenu++;
		break;
	case KBD_UP:
		if (omenu > 0)
			omenu--;
		else
			omenu = OPTIONMENU_MAX;
		break;
	case KBD_LEFT:
		switch (omenu)	// switch
		{
		case SET_TIME_H_MENU:
			if (h1 > 0)
				h1--;
			else
				h1 = 23;
			break;
		case SET_TIME_M_MENU:
			if (m1 > 0)
				m1--;
			else
				m1 = 59;
			break;
		case SET_DATE_D_MENU:
			if (d1 > 1)
				d1--;
			else
				d1 = 31;
			break;
		case SET_DATE_M_MENU:
			if (mes1 > 1)
				mes1--;
			else
				mes1 = 12;
			break;
		case SET_DATE_Y_MENU:
			if (year1 > 10)
				year1--;
			break;
		case SET_TIMER_SLEEP:
			if (n_sleep_tmp == 0)
				n_sleep_tmp = SLEEP_MAX;
			else
				n_sleep_tmp--;
			break;
		case SET_STB_MENU:
			if (n_auto_stb_tmp == 0)
				n_auto_stb_tmp = AUTO_STB_MAX;
			else
				n_auto_stb_tmp--;
			break;
		case SET_DS1820_MENU:
			if (n_ds_found_cnt > 0) {
				if (n_ds_temp_max_tmp > DS18X20_MIN)
					n_ds_temp_max_tmp--;
				else
					n_ds_temp_max_tmp = DS18X20_MAX;
			}
			break;
		case SET_STB_OUT:
			if (stb_out_tmp == 0)
				stb_out_tmp = 1;
			else
				stb_out_tmp = 0;
			if (stb_out)
				PORT_PIN_STANDBY_OUT &= ~(_BV(PIN_STANDBY_OUT));
			else
				PORT_PIN_STANDBY_OUT |= _BV(PIN_STANDBY_OUT);
			break;
		}
		break;
	case KBD_RIGHT:
		switch (omenu)	// switch
		{
		case SET_TIME_H_MENU:
			if (h1 < 23)
				h1++;
			else
				h1 = 0;
			break;
		case SET_TIME_M_MENU:
			if (m1 < 59)
				m1++;
			else
				m1 = 0;
			break;
		case SET_DATE_D_MENU:
			if (d1 < 31)
				d1++;
			else
				d1 = 1;
			break;
		case SET_DATE_M_MENU:
			if (mes1 < 12)
				mes1++;
			else
				mes1 = 1;
			break;
		case SET_DATE_Y_MENU:
			year1++;
			break;
		case SET_TIMER_SLEEP:
			if (n_sleep_tmp == SLEEP_MAX)
				n_sleep_tmp = 0;
			else
				n_sleep_tmp++;
			break;
		case SET_STB_MENU:
			if (n_auto_stb_tmp == AUTO_STB_MAX)
				n_auto_stb_tmp = 0;
			else
				n_auto_stb_tmp++;
			break;
		case SET_DS1820_MENU:
			if (n_ds_found_cnt > 0) {
				if (n_ds_temp_max_tmp < DS18X20_MAX)
					n_ds_temp_max_tmp++;
				else
					n_ds_temp_max_tmp = DS18X20_MIN;
			}
			break;
		case SET_STB_OUT:
			if (stb_out_tmp == 0)
				stb_out_tmp = 1;
			else
				stb_out_tmp = 0;
			if (stb_out)
				PORT_PIN_STANDBY_OUT &= ~(_BV(PIN_STANDBY_OUT));
			else
				PORT_PIN_STANDBY_OUT |= _BV(PIN_STANDBY_OUT);
			break;
		}
		break;
	case KBD_SET:
		if (omenu < SET_DATE_D_MENU) {
			rtc_set_time(h1, m1, 0);
		}
		if (omenu < SET_STB_MENU) {
			rtc_set_date(d1, mes1, year1);
		}
		if (omenu == SET_RC5_KEY_DEF) {
			save_EEprom_Rc5key();
		}
		if (omenu == SET_TIMER_SLEEP) {
			n_sleep = n_sleep_tmp;
			sleep_flag = 0;
		}
		if (omenu == SET_STB_MENU) {
			n_auto_stb = n_auto_stb_tmp;
		}
		if (omenu == SET_DS1820_MENU) {
			n_ds_temp_max = n_ds_temp_max_tmp;
		}
		if (omenu == SET_STB_OUT)
			stb_out = stb_out_tmp;
		save_param();
		if (stb_out)
			PORT_PIN_STANDBY_OUT &= ~(_BV(PIN_STANDBY_OUT));
		else
			PORT_PIN_STANDBY_OUT |= _BV(PIN_STANDBY_OUT);
		break;
	case KBD_MENU:
		lcd_clear();
		nmenu = 0;
		reg_menu = REG_MAINMENU;
		timer_nokey_count = TIMER1_NOKEY_MAIN;
		lcd_mainmenu();
		break;
	}
	save_EEprom_param();
	if ((key != KBD_MENU) && (!l_standby) && (!l_mute))
		lcd_optionmenu();
}

//*****************************************************************************
void lcd_optionmenu(void)
{
//-----------------------------------------------------------------------------
	lcd_com(0x80);
	lcd_puts(optionmenu[omenu]);
//-----------------------------------------------------------------------------
	lcd_com(0xC0);
	switch (omenu)		// switch
	{
	case SET_TIME_H_MENU:
		if (sec_flag)
			print_dec(h1, 2, '0');
		else
			lcd_space(2);
		lcd_dat(':');
		print_dec(m1, 2, '0');
		lcd_dat(' ');
		print_dec(d1, 2, '0');
		lcd_dat('/');
		print_dec(mes1, 2, '0');
		lcd_dat('/');
		print_dec(20, 2, '0');
		print_dec(year1, 2, '0');
		break;
	case SET_TIME_M_MENU:
		print_dec(h1, 2, '0');
		lcd_dat(':');
		if (sec_flag)
			print_dec(m1, 2, '0');
		else
			lcd_space(2);
		lcd_dat(' ');
		print_dec(d1, 2, '0');
		lcd_dat('/');
		print_dec(mes1, 2, '0');
		lcd_dat('/');
		print_dec(20, 2, '0');
		print_dec(year1, 2, '0');
		break;
	case SET_DATE_D_MENU:
		print_dec(h1, 2, '0');
		lcd_dat(':');
		print_dec(m1, 2, '0');
		lcd_dat(' ');
		if (sec_flag)
			print_dec(d1, 2, '0');
		else
			lcd_space(2);
		lcd_dat('/');
		print_dec(mes1, 2, '0');
		lcd_dat('/');
		print_dec(20, 2, '0');
		print_dec(year1, 2, '0');
		break;
	case SET_DATE_M_MENU:
		print_dec(h1, 2, '0');
		lcd_dat(':');
		print_dec(m1, 2, '0');
		lcd_dat(' ');
		print_dec(d1, 2, '0');
		lcd_dat('/');
		if (sec_flag)
			print_dec(mes1, 2, '0');
		else
			lcd_space(2);
		lcd_dat('/');
		print_dec(20, 2, '0');
		print_dec(year1, 2, '0');
		break;
	case SET_DATE_Y_MENU:
		print_dec(h1, 2, '0');
		lcd_dat(':');
		print_dec(m1, 2, '0');
		lcd_dat(' ');
		print_dec(d1, 2, '0');
		lcd_dat('/');
		print_dec(mes1, 2, '0');
		lcd_dat('/');
		if (sec_flag) {
			print_dec(20, 2, '0');
			print_dec(year1, 2, '0');
		} else
			lcd_space(4);
		break;
	case SET_TIMER_SLEEP:
		if (n_sleep_tmp == 0) {
			lcd_puts(msg[MSG_SLEEP_OFF]);
		} else {
			lcd_space(5);
			print_dec(n_sleep_tmp, 2, ' ');
			lcd_puts(msg[MSG_MIN]);
			lcd_space(5);
		}
		break;
	case SET_STB_MENU:
		if (n_auto_stb_tmp == 0) {
			lcd_puts(msg[MSG_AUTO_STB]);
		} else {
			lcd_space(4);
			print_dec(n_auto_stb_tmp, 3, ' ');
			lcd_puts(msg[MSG_MIN]);
			lcd_space(5);
		}
		break;
	case SET_DS1820_MENU:
		if (n_ds_found_cnt == 0) {
			lcd_puts(msg[MSG_DSNOTFOUND]);
		} else {
			lcd_space(6);
			lcd_dat('+');
			print_dec(n_ds_temp_max_tmp, 2, ' ');
			lcd_dat('C');
			lcd_space(6);
		}
		break;
	case SET_RC5_KEY_DEF:
		lcd_puts(msg[MSG_PRESS_SET]);
		break;
	case SET_STB_OUT:
		lcd_puts(msg[MSG_SET_STB_OUT]);
		if (stb_out_tmp == 0)
			lcd_dat('0');
		else
			lcd_dat('1');
		break;
	}
}

//*****************************************************************************
