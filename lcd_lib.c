//*****************************************************************************
// AudioProc programm 
// Doynikov Andrew 
// 24.05.2010 ver: 2.0 plus
//*****************************************************************************
// LCD FUNCTIONS
//*****************************************************************************

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <inttypes.h>
#include "lcd_lib.h"
//=============================================================================

const uint8_t LcdCustomChar[] PROGMEM =	//define 8 custom LCD chars
{
	0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00,	// 0. 0/5 full progress block
	0x10, 0x10, 0x10, 0x15, 0x10, 0x10, 0x10, 0x00,	// 1. 1/5 full progress block
	0x18, 0x18, 0x18, 0x1D, 0x18, 0x18, 0x18, 0x00,	// 2. 2/5 full progress block
	0x1C, 0x1C, 0x1C, 0x1D, 0x1C, 0x1C, 0x1C, 0x00,	// 3. 3/5 full progress block
	0x1E, 0x1E, 0x1E, 0x1F, 0x1E, 0x1E, 0x1E, 0x00,	// 4. 4/5 full progress block
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00,	// 5. 5/5 full progress block
	0x0E, 0x15, 0x15, 0x17, 0x11, 0x11, 0x0E, 0x00,	// 6. clock
//      0x03, 0x07, 0x1F, 0x1F, 0x1F, 0x07, 0x03, 0x00, // 5. dinamik fill
	0x03, 0x05, 0x19, 0x11, 0x19, 0x05, 0x03, 0x00,	// 5. dinamik fill
};

//=============================================================================
void lcd(unsigned char p)
{
	LCD_PORT &= 0xF0;
	LCD_PORT |= (p >> 4);
	_delay_ms(1);
	LCD_PORT &= ~_BV(LCD_EN);
	_delay_ms(1);
	LCD_PORT |= _BV(LCD_EN);
	LCD_PORT &= 0xF0;
	LCD_PORT |= (p & 0x0F);
	_delay_ms(1);
	LCD_PORT &= ~_BV(LCD_EN);
	_delay_ms(1);
}

//=============================================================================
void lcd_com(unsigned char p)
{
	LCD_PORT &= ~_BV(LCD_RS);
	LCD_PORT |= _BV(LCD_EN);
	lcd(p);
}

//=============================================================================
void lcd_dat(unsigned char p)
{
	LCD_PORT |= _BV(LCD_RS) | _BV(LCD_EN);
	lcd(p);
}

//=============================================================================
void lcd_init(void)
{
	unsigned char ch = 0, chn = 0;
	_delay_ms(200);
	lcd_com(0x33);
	_delay_ms(20);
	lcd_com(0x32);
	lcd_com(0x28);
	lcd_com(0x08);
	lcd_com(0x01);
	_delay_ms(50);
	lcd_com(0x06);
	lcd_com(0x0C);
	while (ch < 64) {
		lcd_definechar((LcdCustomChar + ch), chn++);
		_delay_ms(1);
		ch = ch + 8;
	}
}

//=============================================================================
//=============================================================================
void lcd_puts(unsigned char *data)
{
	while (*data)
		lcd_dat(*(data++));
}

//=============================================================================
void lcd_clear(void)
{
	lcd_com(1 << LCD_CLR);
	_delay_ms(100);
}

//=============================================================================
void lcd_definechar(const uint8_t * pc, uint8_t char_code)
{
	uint8_t a, pcc;
	uint16_t i;
	a = (char_code << 3) | 0x40;
	for (i = 0; i < 8; i++) {
		pcc = pgm_read_byte(&pc[i]);
		lcd_com(a++);
		lcd_dat(pcc);
	}
}

//=============================================================================
// END
//=============================================================================
void lcdprogress_bar(uint8_t progress, uint8_t maxprogress, uint8_t length)
{
	uint8_t i;
	uint16_t pixelprogress;
	uint8_t c;
	pixelprogress =
	    ((progress * (length * PROGRESSPIXELS_PER_CHAR)) / maxprogress);
	lcd_com(0xC0);
	for (i = 0; i < length; i++) {
		if (((i * (uint16_t) PROGRESSPIXELS_PER_CHAR) + 5) >
		    pixelprogress) {
			if (((i * (uint16_t) PROGRESSPIXELS_PER_CHAR)) >
			    pixelprogress) {
				c = 0;
			} else {
				c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
			}
		} else {
			c = 5;
		}
		lcd_dat(c);
	}

}

//=============================================================================
void lcd_space(unsigned char n_sp)
{
	unsigned char i;
	for (i = 1; i <= n_sp; i++) {
		lcd_dat(' ');
	}
}
