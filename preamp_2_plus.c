//*****************************************************************************
// Preamp project 
// Doynikov Andrew 
// 24.05.2010 ver: 3.0 plus
//*****************************************************************************
// MAIN PROGRAMM
//*****************************************************************************
//*****************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "intsoft.h"
#include "lcd_lib.h"
#include "i2c.h"
#include "rc5.h"
#include "ds18x20.h"
#include "preamp_2_plus.h"
//*****************************************************************************
#include "encoder.h"
#include "ds1307.h"
#include "keyboard.h"
#include "utils.h"
#include "mainmenu.h"
#include "optionmenu.h"
#include "tda7313.h"
//*****************************************************************************
void main_init(void)
{
// Input/Output Ports initialization
	DDRA = 0xFF;
	PORTA = 0xFF;
	DDRB = 0xFF;
	PORTB = 0xFF;
	DDRC = 0x1C;
	PORTC = 0xFF;
	DDRD = 0xE2;
	PORTD = 0xA6;
// Timer/Counter 0 initialization
	TCCR0 = 0x00;
	TCNT0 = 0x00;
	OCR0 = 0x00;
// Timer/Counter 1 initialization
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1H = 0x00;
	ICR1L = 0x00;
	OCR1AH = 0x00;
	OCR1AL = 0x00;
	OCR1BH = 0x00;
	OCR1BL = 0x00;
// Timer/Counter 2 initialization
	ASSR = 0x00;
	TCCR2 = 0x00;
	TCNT2 = 0x00;
	OCR2 = 0x00;
// External Interrupt(s) initialization
	GICR |= 0x40;
	MCUCR = 0x02;
	MCUCSR = 0x00;
	GIFR = 0x40;		// - INT0
// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK = 0x00;
// Analog Comparator initialization
	ACSR = 0x80;
	SFIOR = 0x00;
//.............................................................................
	lcd_init();
//.............................................................................
	lcd_clear();
	lcd_puts(msg[MSG_LOGO1]);
	lcd_com(0xC0);
	lcd_puts(msg[MSG_LOGO2]);
	i2c_init();
	rc5_init(RC5_ALL);
	rtc_init();
	timers_init();
	encoder_init();
	n_ds_found_cnt = search_sensors();
	main_delay(32);

	ds_state = 0;
	temperature1 = 0;
	temperature2 = 0;

	lcd_com(0xC0);
	if (n_ds_found_cnt > 0) {
		lcd_puts(msg[MSG_DSFOUND]);
		print_dec(n_ds_found_cnt, 2, ' ');
	} else
		lcd_puts(msg[MSG_DSNOTFOUND]);

	l_tcheck = 0;
	if (n_ds_found_cnt == 1) {
		get_temp(1);
		main_delay(4);
		get_temp(1);
	}
	if (n_ds_found_cnt == 2) {
		get_temp(2);
		main_delay(4);
		get_temp(2);
	}
	l_tcheck = 1;
	lcd_clear();
//.............................................................................
//.............................................................................
}

//*****************************************************************************
void timers_init(void)
{
	TIMER1_STOP;
	TCNT1H = TIMER1_CNT1H;
	TCNT1L = TIMER1_CNT1L;
	OCR1AH = TIMER1_OCR1H;
	OCR1AL = TIMER1_OCR1L;
	TIMER1_START;
	TIMER2_STOP;
	TCNT2 = TIMER2_CNT;
	OCR2 = TIMER2_OCR;
	TIMSK |= _BV(TOIE2);
	TIMER2_START;
	TIMSK |= (1 << TOIE1);
	TIMSK |= (1 << TOIE2);
}

//*****************************************************************************
//ISR(SIG_OVERFLOW1)
ISR(TIMER0_OVF_vect)
{
	TCNT1H = TIMER1_CNT1H;
	TCNT1L = TIMER1_CNT1L;
	OCR1AH = TIMER1_OCR1H;
	OCR1AL = TIMER1_OCR1L;
	if ((!standby_avto_flag) && (n_auto_stb > 0) && (!l_standby)) {
		standby_avto_cnt++;
		if (standby_avto_cnt == n_auto_stb * 120) {
			standby_avto_flag = 1;
			standby_avto_cnt = 0;
			sleep_flag = 0;
			sleep_timer_cnt = 0;
			nokey_flag = 0;
			nokey_timer_cnt = 0;
		}
	}
	if (!nokey_flag) {
		nokey_timer_cnt++;
		if (nokey_timer_cnt == timer_nokey_count) {
			nokey_flag = 1;
			nokey_timer_cnt = 0;
		}
	}
	if ((!sleep_flag) && (n_sleep > 0) && (!l_standby)) {
		sleep_timer_cnt++;
		if (sleep_timer_cnt == n_sleep * 120) {
			lcd_clear();
			lcd_com(0x80);
			sleep_flag = 1;
			sleep_timer_cnt = 0;
		}
	}
	ds_wait_cnt++;
	sec_cnt++;
	if (sec_cnt == TIMER1_1SEC) {
		sec_flag = !sec_flag;
		sec_cnt = 0;
	}
	if (ds_wait_cnt == DS_WAIT_CNT) {
		gettemp();
		ds_wait_cnt = 0;
	}

}

//*****************************************************************************
//ISR(SIG_OVERFLOW2)
ISR(TIMER2_OVF_vect)
{
	TCNT2 = TIMER2_CNT;
	OCR2 = TIMER2_OCR;
	kbd_timer_cnt++;
	if ((!l_standby) && (!l_mute)) {
		if (enc.flip == -1)
			encoder_scan();
	}
	if (kbd_timer_cnt == KBD_TIMER_CNT) {
		if (kbd_key == KBD_NOKEY)
			kbd_scan();
		kbd_timer_cnt = 0;
	}
}

//*****************************************************************************
void set_standby(unsigned char pgb)
{
	unsigned char n;
	PORT_LED_MUTE |= _BV(PIN_LED_MUTE);
	nmenu = 0;
	omenu = 0;
	reg_menu = REG_MAINMENU;
	if (l_standby) {
		if (pgb) {
			lcd_com(0x80);
			lcd_mainmenu();
			save_volume_stb = param_tda[VOLUME_MENU];
			for (n = save_volume_stb; n > 0; n--) {
				param_tda[VOLUME_MENU] = n;
				lcdprogress_bar(param_tda[VOLUME_MENU],
						menu_max[VOLUME_MENU], 16);
				send_tda7313();
				lcd_com(0x8D);
				print_dec(param_tda[VOLUME_MENU], 2, '0');
			}
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
		PORT_LED_INPUT |= _BV(LED_INPUT1);
		PORT_LED_INPUT |= _BV(LED_INPUT2);
		PORT_LED_INPUT |= _BV(LED_INPUT3);
		PORT_LED_LONDNESS |= _BV(LED_LONDNESS);
		PORT_LED_STANDBY &= ~(_BV(PIN_LED_STANDBY));
		if (pgb) {
			if (stb_out)
				PORT_PIN_STANDBY_OUT |= _BV(PIN_STANDBY_OUT);
			else
				PORT_PIN_STANDBY_OUT &= ~(_BV(PIN_STANDBY_OUT));
		}
		lcd_clear();
		lcd_puts(msg[MSG_STANDBY]);
		sleep_flag = 0;
		main_delay(16);
	} else {
		lcd_com(0x80);
		lcd_mainmenu();
		if (stb_out)
			PORT_PIN_STANDBY_OUT &= ~(_BV(PIN_STANDBY_OUT));
		else
			PORT_PIN_STANDBY_OUT |= _BV(PIN_STANDBY_OUT);
		for (n = 1; n < save_volume_stb + 1; n++) {
			param_tda[VOLUME_MENU] = n;
			lcdprogress_bar(param_tda[VOLUME_MENU],
					menu_max[VOLUME_MENU], 16);
			send_tda7313();
			lcd_com(0x8D);
			print_dec(param_tda[VOLUME_MENU], 2, '0');
		}
		PORT_LED_STANDBY |= _BV(PIN_LED_STANDBY);
		set_select();
		set_londness(l_londness);
		standby_avto_flag = 0;
	}
	l_mute = 0;
}

//*****************************************************************************
void print_temperatura(unsigned char nd)
{
	if (nd == 1) {
		if (temperature1 == 0) {
			lcd_puts(msg_1[0]);
		} else {
			lcd_dat((sz1) ? '-' : '+');
			print_dec(temperature1, 2, ' ');
			lcd_dat('C');
		}
	}
	if (nd == 2) {
		if (temperature2 == 0) {
			lcd_puts(msg_1[0]);
		} else {
			lcd_dat((sz2) ? '-' : '+');
			print_dec(temperature2, 2, ' ');
			lcd_dat('C');
		}
	}
}

//*****************************************************************************
void lcd_show_temp_sleep_mute_stb(unsigned char typelcd)
{
	rtc_get_time(&h1, &m1, &s1);
	rtc_get_date(&d1, &mes1, &year1);
	if (typelcd == 1) {
		lcd_com(0x80);
		if (l_info)
			lcd_dat(CLOCK);
		if (sleep_flag)
			lcd_dat(0xED);
		if (l_standby)
			lcd_dat(0xEB);
		if (l_mute)
			lcd_dat(ZVUK_ON);
		else
			lcd_dat(' ');
		lcd_space(2);
		rtc_print_time(h1, m1, s1);
		lcd_space(4);
		print_temperatura(1);
		lcd_com(0xC0);
		rtc_print_date(d1, mes1, year1);
		lcd_space(2);
		print_temperatura(2);
	} else {
		lcd_com(0x80);
		lcd_puts(msg[MSG_MUTE]);
		lcd_com(0xC0);
		rtc_print_time(h1, m1, s1);
		lcd_dat(' ');
		rtc_print_date(d1, mes1, year1);
	}
}

//*****************************************************************************
static unsigned char rc5_to_kbd(unsigned char rc5code)
{
	unsigned char i, ret = KBD_NOKEY;
	for (i = 0; i < RC5_MAX; i++) {
		if (l_standby) {
			if (rc5code == rc5_def[i])
				ret = rc5_key_stb[i];
		}
		if (l_mute) {
			if (rc5code == rc5_def[i])
				ret = rc5_key_mute[i];
		}
		if ((!l_mute) && (!l_standby)) {
			if (rc5code == rc5_def[i])
				ret = rc5_key[i];
		}
	}
	return ret;
}

//*****************************************************************************
void save_EEprom_param(void)
{
	unsigned char n;
	for (n = 0; n < PARAM_TDA_MAX; n++) {
		eeprom_write_byte((uint8_t *) TDA_PARAM_START_ADR + n,
				  param_tda[n]);
	}
	eeprom_write_byte((uint8_t *) LONDNESS_ADR, l_londness);
	eeprom_write_byte((uint8_t *) DS18X20_ADR, n_ds_temp_max);
	eeprom_write_byte((uint8_t *) INPUT_SEL_ADR, n_select);
	eeprom_write_byte((uint8_t *) STANDBY_AUTO_STB, n_auto_stb);
	eeprom_write_byte((uint8_t *) SLEEP_TIMER, n_sleep);
	eeprom_write_byte((uint8_t *) STANDBY_OUT, stb_out);
	eeprom_write_byte((uint8_t *) TYPE_MUTE_SHOW, l_mute_show);
}

//*****************************************************************************
void load_EEprom_param(void)
{
	unsigned char n;
	for (n = 0; n < PARAM_TDA_MAX; n++) {
		param_tda[n] =
		    eeprom_read_byte((uint8_t *) TDA_PARAM_START_ADR + n);
		if ((param_tda[n] < 1) || (param_tda[n] > menu_max[n]))
			param_tda[n] = 1;
		eeprom_write_byte((uint8_t *) TDA_PARAM_START_ADR + n,
				  param_tda[n]);
	}
	save_volume_stb = param_tda[VOLUME_MENU];
	save_volume_mute = param_tda[VOLUME_MENU];
	l_londness = eeprom_read_byte((uint8_t *) LONDNESS_ADR);
	if (l_londness > 1) {
		l_londness = 0;
		eeprom_write_byte((uint8_t *) LONDNESS_ADR, l_londness);
	}
	set_londness(l_londness);

	n_ds_temp_max = eeprom_read_byte((uint8_t *) DS18X20_ADR);
	if ((n_ds_temp_max > DS18X20_MAX) || (n_ds_temp_max < DS18X20_MIN)) {
		n_ds_temp_max = DS18X20_MIN;
		eeprom_write_byte((uint8_t *) DS18X20_ADR, n_ds_temp_max);
	}
// Load INPUT chanel
	n_select = eeprom_read_byte((uint8_t *) INPUT_SEL_ADR);
	if ((n_select < 1) || (n_select > 3)) {
		n_select = 1;
	}
	eeprom_write_byte((uint8_t *) INPUT_SEL_ADR, n_select);

	stb_out = eeprom_read_byte((uint8_t *) STANDBY_OUT);
	if (stb_out > 1) {
		stb_out = STANDBY_OUT_1;
		eeprom_write_byte((uint8_t *) STANDBY_OUT, stb_out);
	}
	n_auto_stb = eeprom_read_byte((uint8_t *) STANDBY_AUTO_STB);
	if (n_auto_stb > TIMER_AUTO_STB_MAX) {
		n_auto_stb = 0;
		eeprom_write_byte((uint8_t *) STANDBY_AUTO_STB, n_auto_stb);
	}
	n_sleep = eeprom_read_byte((uint8_t *) SLEEP_TIMER);
	if ((n_sleep > TIMER_SLEEP_MAX) || (n_sleep < TIMER_SLEEP_MIN)) {
		n_sleep = 0;
		eeprom_write_byte((uint8_t *) SLEEP_TIMER, n_sleep);
	}
	l_mute_show = eeprom_read_byte((uint8_t *) TYPE_MUTE_SHOW);
	if (l_mute_show > 1) {
		l_mute_show = 0;
		eeprom_write_byte((uint8_t *) TYPE_MUTE_SHOW, l_mute_show);
	}
}

//*****************************************************************************
void save_EEprom_Rc5key(void)
{
	unsigned char n;
	for (n = 0; n < RC5_MAX; n++) {
		eeprom_write_byte((uint8_t *) RC5_KEY_START_ADR + n,
				  rc5_def_save[n]);
		eeprom_write_byte((uint8_t *) RC5_KEY_SAVE_ADR, 'T');
	}
}

//*****************************************************************************
void load_EEprom_Rc5key(void)
{
	unsigned char n;
	for (n = 0; n < RC5_MAX; n++) {
		rc5_def[n] =
		    eeprom_read_byte((uint8_t *) RC5_KEY_START_ADR + n);
	}
}

//*****************************************************************************
//*****************************************************************************
void get_all(void)
{
	if (rc5.flip != -1) {
		kbd_key = KBD_NOKEY;
		kbd_key = rc5_to_kbd(rc5.code);
		switch (kbd_key)	// switch
		{
		case KBD_STANDBY:
			rc5_stb_cnt++;
			if (rc5_stb_cnt < RC5_STB_CNT)
				kbd_key = KBD_NOKEY;
			else
				rc5_stb_cnt = 0;
			break;
		case KBD_MUTE:
			rc5_mute_cnt++;
			if (rc5_mute_cnt < RC5_MUTE_CNT)
				kbd_key = KBD_NOKEY;
			else
				rc5_mute_cnt = 0;
			break;
		case KBD_SET:
			rc5_set_cnt++;
			if (rc5_set_cnt < RC5_SET_CNT)
				kbd_key = KBD_NOKEY;
			else
				rc5_set_cnt = 0;
			break;
		case KBD_LEFT:
			break;
		case KBD_RIGHT:
			break;
		default:
			rc5_cnt++;
			if (rc5_cnt < RC5_CNT)
				kbd_key = KBD_NOKEY;
			else
				rc5_cnt = 0;
			break;
		}
		rc5.flip = -1;
	} else if (enc.flip == 1) {
		kbd_key = enc.code;
		enc.flip = -1;
	}
}

//*****************************************************************************
void save_param(void)
{
	lcd_com(0xC0);
	lcd_puts(msg[MSG_SAVE_SET]);
	beep_long();
	main_delay(16);
	lcd_optionmenu();
}

//*****************************************************************************
void get_temp(unsigned char nd)
{
	uint8_t subzero, cel = 0, cel_frac_bits;
	if (n_ds_found_cnt > 0) {
		if (nd == 2)
			if (n_ds_found_cnt == 1)
				nd = 1;

		if (n_ds_found_cnt == 1)
			temperature2 = 0;

		DS18X20_start_meas(DS18X20_POWER_PARASITE,
				   &gSensorIDs[nd - 1][0]);

		delay_ms(DS18B20_TCONV_12BIT);

		DS18X20_read_meas(&gSensorIDs[nd - 1][0], &subzero, &cel,
				  &cel_frac_bits);

		if (nd == 1) {
			sz1 = subzero;
			temperature1 = cel;
		}

		if (nd == 2) {
			sz2 = subzero;
			temperature2 = cel;
		}

		if (l_tcheck) {
			if ((temperature1 > n_ds_temp_max)
			    || (temperature2 > n_ds_temp_max)) {
				PORT_VENTIL |= _BV(PIN_VENTIL);
			}
			if ((temperature1 <= n_ds_temp_max - TEMP_GIS)
			    && (temperature2 <= n_ds_temp_max - TEMP_GIS)) {
				PORT_VENTIL &= ~(_BV(PIN_VENTIL));
			}
		}
	} else {
		temperature1 = 0;
		temperature2 = 0;
	}

}

//*****************************************************************************
void gettemp(void)
{
	cli();
	uint8_t subzero, cel = 0, cel_frac_bits;
	if (n_ds_found_cnt > 0) {
		switch (ds_state)	// switch
		{
		case 0:
			DS18X20_start_meas(DS18X20_POWER_EXTERN,
					   &gSensorIDs[0][0]);
			ds_state = 1;
			break;
		case 1:
			DS18X20_read_meas(&gSensorIDs[0][0], &subzero, &cel,
					  &cel_frac_bits);
			sz1 = subzero;
			temperature1 = cel;
			ds_state = 2;
			break;
		case 2:
			if (n_ds_found_cnt == 2) {
				DS18X20_start_meas(DS18X20_POWER_EXTERN,
						   &gSensorIDs[1][0]);
			}
			ds_state = 3;
			break;
		case 3:
			if (n_ds_found_cnt == 2) {
				DS18X20_read_meas(&gSensorIDs[1][0], &subzero,
						  &cel, &cel_frac_bits);
				sz2 = subzero;
				temperature2 = cel;
			}
			ds_state = 4;
			break;
		case 4:
			if ((temperature1 > n_ds_temp_max)
			    || (temperature2 > n_ds_temp_max)) {
				PORT_VENTIL |= _BV(PIN_VENTIL);
			}
			if ((temperature1 <= n_ds_temp_max - TEMP_GIS)
			    && (temperature2 <= n_ds_temp_max - TEMP_GIS)) {
				PORT_VENTIL &= ~(_BV(PIN_VENTIL));
			}
			ds_state = 0;
			break;
		}
	} else {
		temperature1 = 0;
		temperature2 = 0;
	}
	sei();
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
int main()
{
	cli();
	main_init();
	load_EEprom_param();
	if (eeprom_read_byte((uint8_t *) RC5_KEY_SAVE_ADR) != 'T') {
		save_EEprom_Rc5key();
	}
	load_EEprom_Rc5key();
	l_standby = 1;
	if (stb_out)
		PORT_PIN_STANDBY_OUT |= _BV(PIN_STANDBY_OUT);
	else
		PORT_PIN_STANDBY_OUT &= ~(_BV(PIN_STANDBY_OUT));
	set_standby(0);
	set_select();
	beep();
	sei();

	while (1)		// While
	{
//.............................................................................
		if (kbd_key == KBD_NOKEY)
			get_all();
//.............................................................................
		if ((standby_avto_flag)) {
			beep();
			if (reg_menu == REG_OPTIONMENU)
				run_optionmenu(KBD_STANDBY);
			if (reg_menu == REG_MAINMENU)
				run_mainmenu(KBD_STANDBY);
			standby_avto_flag = 0;
		}
		if ((nokey_flag) && (!l_standby) && (!l_mute)) {
			omenu = 0;
			if (nmenu != VOLUME_MENU) {
				nmenu = 0;
				lcd_clear();
				lcd_mainmenu();
			}
			if (reg_menu == REG_OPTIONMENU)
				run_optionmenu(KBD_MENU);
		}
//.............................................................................
		if ((sleep_flag) && (!l_standby) && (!l_mute)) {
			reg_menu = REG_MAINMENU;
		}
//.............................................................................
		if ((l_standby) || (sleep_flag) || (l_mute)) {
			if (l_mute)
				lcd_show_temp_sleep_mute_stb(l_mute_show);
			else
				lcd_show_temp_sleep_mute_stb(1);
			nokey_timer_cnt = 0;
			sleep_timer_cnt = 0;
			nokey_flag = 0;
		}
//.............................................................................
		if ((reg_menu == REG_OPTIONMENU) && (!l_standby) && (!l_mute)
		    && (!sleep_flag)) {
			lcd_optionmenu();
		}
//.............................................................................
//.............................................................................
		if (kbd_key == KBD_NOKEY)
			get_all();
//.............................................................................
//.............................................................................
		if (kbd_key != KBD_NOKEY) {
			switch (reg_menu)	// switch
			{
			case REG_MAINMENU:
				run_mainmenu(kbd_key);
				break;
			case REG_OPTIONMENU:
				run_optionmenu(kbd_key);
				break;
			}
			if ((kbd_key != KBD_STANDBY) && (kbd_key != KBD_MUTE)
			    && (kbd_key != KBD_SET))
				beep_flash();
			kbd_key = KBD_NOKEY;
			sleep_flag = 0;
			sleep_timer_cnt = 0;
			nokey_flag = 0;
			nokey_timer_cnt = 0;
			rc5_stb_cnt = 0;
			rc5_mute_cnt = 0;
			rc5_set_cnt = 0;
		}
	}
}

//*****************************************************************************
//*****************************************************************************
