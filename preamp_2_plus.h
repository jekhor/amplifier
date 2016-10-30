//*****************************************************************************
// Preamp project 
// Doynikov Andrew 
// 24.05.2010 ver: 3.0 plus
//*****************************************************************************
// MAIN PROGRAMM HEADER
//*****************************************************************************
#include <avr/io.h>
#include <inttypes.h>
//*****************************************************************************
#define PORT_BEEP	         PORTD
#define PIN_BEEP	         PD5
#define PORT_LED_RC5   	     PORTD
#define LED_RC5		         PD1
#define PORT_LED_INPUT       PORTB
#define LED_INPUT1	         PB2
#define LED_INPUT2	         PB3
#define LED_INPUT3	         PB4
#define PORT_LED_MUTE        PORTA
#define PIN_LED_MUTE         PA7
#define PORT_INPUT	         PORTB
#define PORT_LED_LONDNESS    PORTA
#define LED_LONDNESS         PA6
#define PORT_LED_STANDBY     PORTB
#define PIN_LED_STANDBY      PB0
#define PORT_PIN_STANDBY_OUT PORTB
#define PIN_STANDBY_OUT      PB1
#define PORT_VENTIL    	     PORTD
#define PIN_VENTIL    	     PD6
//*****************************************************************************
#define KBD_NOKEY            0xFF
#define KBD_PORT_COL1        PINC
#define KBD_PORT_COL2        PINC
#define KBD_PORT_COL3        PINC
#define KBD_PIN_COL1         PC7
#define KBD_PIN_COL2         PC6
#define KBD_PIN_COL3         PC5
#define KBD_PORT_ROW1        PORTC
#define KBD_PORT_ROW2        PORTC
#define KBD_PORT_ROW3        PORTC
#define KBD_PORT_ROW4        PORTD
#define KBD_PIN_ROW1         PC4
#define KBD_PIN_ROW2         PC3
#define KBD_PIN_ROW3         PC2
#define KBD_PIN_ROW4         PD7
#define KBD_DELAY            100
//*****************************************************************************
#define KBD_STANDBY     	 128
#define KBD_SET          	 129
#define KBD_MUTE        	 130
#define KBD_LEFT        	 131
#define KBD_RIGHT        	 132
#define KBD_MENU        	 133
#define KBD_SELECT1        	 134
#define KBD_SELECT2        	 135
#define KBD_SELECT3        	 136
#define KBD_UP          	 137
#define KBD_DOWN          	 138
#define KBD_LOND          	 139
#define KBD_BASS_UP        	 140
#define KBD_BASS_DOWN      	 141
#define KBD_TREBLE_UP      	 142
#define KBD_TREBLE_DOWN    	 143
#define KBD_TEST     	     200
//*****************************************************************************
#define MAXSENSORS             2
#define DS18X20_MAX           75
#define DS18X20_MIN           45
//*****************************************************************************
#define DS1307_ADDR_READ     0xD1
#define DS1307_ADDR_WRITE    0xD0
#define DS1307_CONTROL       0x07
#define DS1307_DATE          0x04
#define CH                   0x07
#define READ_TEMP_DATE          1
#define NO_READ_TEMP_DATE       0
//*****************************************************************************
#define TDA7313_ADDR        0x88
#define TDA7313_VOLUME_REG  0x00
#define TDA7313_ATT_LR_REG  0xC0
#define TDA7313_ATT_RR_REG  0xE0
#define TDA7313_ATT_LF_REG  0x80
#define TDA7313_ATT_RF_REG  0xA0
#define TDA7313_SWITCH_REG  0x40
#define TDA7313_BASS_REG    0x60
#define TDA7313_TREBLE_REG  0x70

#define TDA7313_MUTE_LR     0xDF
#define TDA7313_MUTE_RR     0xFF
#define TDA7313_MUTE_LF     0x9F
#define TDA7313_MUTE_RF     0xBF
#define TDA7313_ATT_CH_MAX    31

#define TDA7313_SEL_1       0x00
#define TDA7313_SEL_2       0x01
#define TDA7313_SEL_3       0x02
#define TDA7313_SET_ALL     0xFF
//*****************************************************************************
#define ENCODER_PIN_A    	 PD3
#define ENCODER_PIN_B    	 PD0
#define ENCODER_PIN_PORT 	 PIND
#define ENCODER_LEFT     	 KBD_LEFT
#define ENCODER_RIGHT    	 KBD_RIGHT
#define ENCODER_NOKEY    	 KBD_NOKEY
#define ENC_FLASH        	 1
#define COUNT_ENCODER        3
//*****************************************************************************
// Timer1 Setup 0.5 sec !!! Warning
#define TIMER1_CNT1H  0x85
#define TIMER1_CNT1L  0xEE
#define TIMER1_OCR1H  0x7A
#define TIMER1_OCR1L  0x11
#define TIMER1_NOKEY_MAIN    20
#define TIMER1_NOKEY_OPTION  40
#define TIMER1_SLEEP    60
#define TIMER1_1SEC     1
#define TIMER1_STANDBY  300
#define TIMER1_STOP   TCCR1B = 0x00
#define TIMER1_START  TCCR1B = 0x04
// Timer2 Setup 0.5 mks !!! Warning
#define TIMER2_STOP       TCCR2 = 0x00
#define TIMER2_START      TCCR2 = 0x04
#define TIMER2_CNT        0xE1	// 500 us
#define TIMER2_OCR        0x1E	// 500 us
#define RC5_WAIT        16
//*****************************************************************************
#define ENC_TIMER_CNT      	 6
#define KBD_TIMER_CNT      	 4
//*****************************************************************************
#define C0_5        0
#define C1_5        1
#define C2_5        2
#define C3_5        3
#define C4_5        4
#define C5_5        5
#define CLOCK       6
#define ZVUK_ON     7
//*****************************************************************************
#define TIMER_SLEEP_MAX     9
#define TIMER_SLEEP_MIN     1
#define TIMER_SLEEP_STEP    1
#define STANDBY_OUT_0       0
#define STANDBY_OUT_1       1
#define TIMER_AUTO_STB_MAX  30
#define TIMER_AUTO_STB_MIN  10
#define TIMER_AUTO_STB_STEP 5
//*****************************************************************************
#define BEEP_LONG           8
//*****************************************************************************
#define RC5_STB_CNT     6
#define RC5_MUTE_CNT    6
#define RC5_SET_CNT     3
#define RC5_CNT         2
#define RC5_MAX        16
// 
unsigned char rc5_def_save[] =
    { 12, 59, 13, 17, 16, 48, 1, 2, 3, 32, 33, 34, 5, 9, 6, 0 };
unsigned char rc5_def[] =
    { 12, 59, 13, 17, 16, 48, 1, 2, 3, 32, 33, 34, 5, 9, 6, 0 };
unsigned char rc5_key[] =
    { KBD_STANDBY, KBD_SET, KBD_MUTE, KBD_LEFT, KBD_RIGHT, KBD_MENU,
KBD_SELECT1, KBD_SELECT2, KBD_SELECT3,
	KBD_UP, KBD_DOWN, KBD_LOND, KBD_BASS_UP, KBD_BASS_DOWN, KBD_TREBLE_UP,
	    KBD_TREBLE_DOWN
};

unsigned char rc5_key_stb[] =
    { KBD_STANDBY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY,
KBD_NOKEY, KBD_NOKEY, KBD_NOKEY,
	KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY,
	    KBD_NOKEY
};

unsigned char rc5_key_mute[] =
    { KBD_STANDBY, KBD_SET, KBD_MUTE, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY,
KBD_NOKEY, KBD_NOKEY, KBD_NOKEY,
	KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY, KBD_NOKEY,
	    KBD_NOKEY
};

//*****************************************************************************
// ............................................................................
// EEPROM
#define TDA_PARAM_START_ADR   0x20
#define LONDNESS_ADR          0x40
#define DS18X20_ADR           0x41
#define INPUT_SEL_ADR         0x42
#define STANDBY_OUT           0x50
#define STANDBY_AUTO_STB      0x52
#define SLEEP_TIMER           0x53
#define TYPE_MUTE_SHOW        0x5D
// ............................................................................
#define EEPROM_WR  	             1
#define EEPROM_NO_WR             0
// ............................................................................
#define RC5_KEY_SAVE_ADR      0x5F
#define RC5_KEY_START_ADR     0x60
#define RC5_KEY_ADR_DEF       0x80

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
void main_init(void);
void main_delay(unsigned char i);
void beep(void);
void beep_long(void);
void flash(unsigned char ms);
void beep_flash(void);
void print_dec(unsigned int x, unsigned char n, unsigned char fillch);
void kbd_scan(void);
void encoder_scan(void);
void encoder_init(void);
void timers_init(void);
//*****************************************************************************
void rtc_init(void);
void rtc_get_time(unsigned char *hour, unsigned char *min, unsigned char *sec);
void rtc_set_time(unsigned char hour, unsigned char min, unsigned char sec);
void rtc_print_time(unsigned char hour, unsigned char min, unsigned char sec);
void rtc_print_time_read(void);
void rtc_get_date(unsigned char *date, unsigned char *month,
		  unsigned char *year);
void rtc_set_date(unsigned char date, unsigned char month, unsigned char year);
void rtc_print_date(unsigned char date, unsigned char month,
		    unsigned char year);
void rtc_print_date_read(void);
unsigned char bcd2bin(unsigned char bcd);
unsigned char bin2bcd(unsigned char bin);
static unsigned char debounce_sw_col1_1(void);
static unsigned char debounce_sw_col2_1(void);
static unsigned char debounce_sw_col3_1(void);
static unsigned char debounce_sw_col1_2(void);
static unsigned char debounce_sw_col2_2(void);
static unsigned char debounce_sw_col3_2(void);
static unsigned char debounce_sw_col1_3(void);
static unsigned char debounce_sw_col2_3(void);
static unsigned char debounce_sw_col3_3(void);
static unsigned char debounce_sw_col1_4(void);
static unsigned char debounce_sw_col2_4(void);
static unsigned char debounce_sw_col3_4(void);
void run_mainmenu(unsigned char key);
void run_optionmenu(unsigned char key);
void lcd_mainmenu(void);
void lcd_optionmenu(void);
void set_standby(unsigned char pgb);
void set_mute(void);
void set_select(void);
void set_londness(unsigned char l_lond);
void send_tda7313(void);
void print_temperatura(unsigned char nd);
void lcd_show_temp_sleep_mute_stb(unsigned char typelcd);
void get_temp(unsigned char nd);
void get_all(void);
void gettemp(void);
static unsigned char rc5_to_kbd(unsigned char rc5code);
void save_EEprom_param(void);
void load_EEprom_param(void);
void load_EEprom_Rc5key(void);
void save_EEprom_Rc5key(void);
void save_param(void);
//*****************************************************************************
volatile char encoder_status;
const char enc_mask_plus = 0x1E; // b00011110;
const char enc_mask_minus = 0x2D; // b00101101;
typedef struct {
	uint8_t code;
	volatile signed char flip;
} enc_t;

enc_t enc;
volatile unsigned char enc_count_p, enc_count_m;
//*****************************************************************************
volatile unsigned char kbd_timer_cnt, enc_timer_cnt, kbd_key = KBD_NOKEY;
volatile unsigned char l_standby, l_mute, l_londness, n_select =
    1, stb_out, stb_out_tmp, l_tcheck, l_info;
volatile unsigned char nokey_timer_cnt, sec_cnt, l_mute_show;
volatile unsigned char nokey_flag, sleep_flag, sec_flag, standby_avto_flag;
volatile uint16_t standby_avto_cnt, sleep_timer_cnt;
volatile char n_sleep, n_sleep_tmp, n_auto_stb, n_auto_stb_tmp;
volatile unsigned char timer_nokey_count = TIMER1_NOKEY_MAIN;
unsigned char save_volume_stb, save_volume_mute;
unsigned char rc5_stb_cnt, rc5_mute_cnt, rc5_set_cnt, rc5_cnt;
unsigned char h1, m1, s1, d1, mes1, year1;
unsigned char n_ds_found_cnt, n_ds_temp_max = DS18X20_MIN, n_ds_temp_max_tmp;
volatile unsigned char temperature1, temperature2, sz1, sz2;
#define DS_WAIT_CNT     2
volatile unsigned char ds_state, ds_wait_cnt, show_temp_cnt;
unsigned char nmenu, omenu;
// ............................................................................
#define REG_MAINMENU     1
#define REG_OPTIONMENU   2
#define REG_SHOWTEMP     3
// ............................................................................
unsigned char reg_menu = REG_MAINMENU;
//*****************************************************************************
#define PARAM_TDA_MAX     8
unsigned char param_tda[] = { 1, 0, 1, 4, 5, 6, 7, 8 };
unsigned char bass_treb[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 15, 14, 13, 12, 11, 10, 9, 8 };
unsigned char attens[] = { 0x18, 0x10, 0x08, 0x00 };

//*****************************************************************************
#define SLEEP_MAX     5
#define AUTO_STB_MAX  60
#define TEMP_GIS	  2
#define DS_CNV_CNT	  3
#define DS_SHOWTEMP   10
//#############################################################################
#define DEF_VOLUME_MAX_16 16
#define DEF_VOLUME_MAX_32 32
#define DEF_VOLUME_MAX_64 64
//#############################################################################
#define VOLUME_MAX    DEF_VOLUME_MAX_32	// если нужно другое количество шагов, то изменить и перекомпелит
//#############################################################################
#define ATTENS_MAX    16
#define BASS_MAX      16
#define TREBLE_MAX    16
#define ATTENS_FL_MAX 32
#define ATTENS_FR_MAX 32
#define ATTENS_RL_MAX 32
#define ATTENS_RR_MAX 32
#if (VOLUME_MAX == DEF_VOLUME_MAX_16)
unsigned char menu_max[] = { 16, 4, 16, 16, 31, 31, 31, 31 };
#endif
#if (VOLUME_MAX == DEF_VOLUME_MAX_32)
unsigned char menu_max[] = { 32, 4, 16, 16, 31, 31, 31, 31 };
#endif
#if (VOLUME_MAX == DEF_VOLUME_MAX_64)
unsigned char menu_max[] = { 64, 4, 16, 16, 31, 31, 31, 31 };
#endif
//*****************************************************************************
// Define menus & lang
//*****************************************************************************
// ENGLISH
#include "language-en.h"
// RUSSIAN
//#include "language-ru.h"
// UKRAINE
//#include "language-ua.h"
//*****************************************************************************
#define MSG_STANDBY    0
#define MSG_MUTE       1
#define MSG_SLEEP      2
#define MSG_LOGO1      3
#define MSG_LOGO2      4
#define MSG_DSNOTFOUND 5
#define MSG_DS1        6
#define MSG_DS2        7
#define MSG_DSFOUND    8
#define MSG_SLEEP_OFF  9
#define MSG_AUTO_STB   10
#define MSG_MIN        11
#define MSG_PRESS_SET  12
#define MSG_SAVE_SET   13
#define MSG_SET_STB_OUT 14
#define MSG_DS1_SLEEP  15
#define MSG_DS2_SLEEP  16
//*****************************************************************************
#define MENU_MAX 7
#define VOLUME_MENU    0
#define ATTENS_MENU    1
#define BASS_MENU      2
#define TREBLE_MENU    3
#define ATTENS_FL_MENU 4
#define ATTENS_FR_MENU 5
#define ATTENS_RL_MENU 6
#define ATTENS_RR_MENU 7
// ............................................................................
#define OPTIONMENU_MAX 9
#define SET_TIME_H_MENU  0
#define SET_TIME_M_MENU  1
#define SET_DATE_D_MENU  2
#define SET_DATE_M_MENU  3
#define SET_DATE_Y_MENU  4
#define SET_TIMER_SLEEP  5
#define SET_STB_MENU     6
#define SET_DS1820_MENU  7
#define SET_RC5_KEY_DEF  8
#define SET_STB_OUT      9
// ............................................................................
const char *msg_1[] = {
//0123456789ABCDEF
	" ---"			// 1
};

// ............................................................................
