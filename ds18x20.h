#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#define F_OSC 16000000

uint8_t crc8(uint8_t * data_in, uint16_t number_of_bytes_to_read);
static inline void delayloop16(uint16_t count)
{
	asm volatile ("cp  %A0,__zero_reg__ \n\t"
		      "cpc %B0,__zero_reg__ \n\t"
		      "breq L_Exit_%=       \n\t"
		      "L_LOOP_%=:           \n\t"
		      "sbiw %0,1            \n\t"
		      "brne L_LOOP_%=       \n\t"
		      "L_Exit_%=:           \n\t":"=w" (count)
		      :"0"(count)
	    );
}

#define DELAY_US_CONV(us) ((uint16_t)(((((us)*1000L)/(1000000000/F_OSC))-1)/4))
#define delay_us(us)	  delayloop16(DELAY_US_CONV(us))
void delayloop32(uint32_t l);	// not inline
#define DELAY_MS_CONV(ms) ( (uint32_t) (ms*(F_OSC/6000L)) )
#define delay_ms(ms)  delayloop32(DELAY_MS_CONV(ms))
#define DS18X20_EEPROMSUPPORT

// enable extended output via UART by defining:
#define DS18X20_VERBOSE

/* return values */
#define DS18X20_OK          0x00
#define DS18X20_ERROR       0x01
#define DS18X20_START_FAIL  0x02
#define DS18X20_ERROR_CRC   0x03

#define DS18X20_POWER_PARASITE 0x00
#define DS18X20_POWER_EXTERN   0x01

/* DS18X20 specific values (see datasheet) */
#define DS18S20_ID 0x10
#define DS18B20_ID 0x28

#define DS18X20_CONVERT_T	0x44
#define DS18X20_READ		0xBE
#define DS18X20_WRITE		0x4E
#define DS18X20_EE_WRITE	0x48
#define DS18X20_EE_RECALL	0xB8
#define DS18X20_READ_POWER_SUPPLY 0xB4

#define DS18B20_CONF_REG    4
#define DS18B20_9_BIT       0
#define DS18B20_10_BIT      (1<<5)
#define DS18B20_11_BIT      (1<<6)
#define DS18B20_12_BIT      ((1<<6)|(1<<5))

// indefined bits in LSB if 18B20 != 12bit
#define DS18B20_9_BIT_UNDF       ((1<<0)|(1<<1)|(1<<2))
#define DS18B20_10_BIT_UNDF      ((1<<0)|(1<<1))
#define DS18B20_11_BIT_UNDF      ((1<<0))
#define DS18B20_12_BIT_UNDF      0

// conversion times in ms
#define DS18B20_TCONV_12BIT      750
#define DS18B20_TCONV_11BIT      DS18B20_TCONV_12_BIT/2
#define DS18B20_TCONV_10BIT      DS18B20_TCONV_12_BIT/4
#define DS18B20_TCONV_9BIT       DS18B20_TCONV_12_BIT/8
#define DS18S20_TCONV            DS18B20_TCONV_12_BIT

// constant to convert the fraction bits to cel*(10^-4)
#define DS18X20_FRACCONV         625

#define DS18X20_SP_SIZE  9

// DS18X20 EEPROM-Support
#define DS18X20_WRITE_SCRATCHPAD  0x4E
#define DS18X20_COPY_SCRATCHPAD   0x48
#define DS18X20_RECALL_E2         0xB8
#define DS18X20_COPYSP_DELAY      10	/* ms */
#define DS18X20_TH_REG      2
#define DS18X20_TL_REG      3

extern void DS18X20_find_sensor(uint8_t * diff, uint8_t id[]);

extern uint8_t DS18X20_get_power_status(uint8_t id[]);

extern uint8_t DS18X20_start_meas(uint8_t with_external, uint8_t id[]);
extern uint8_t DS18X20_read_meas(uint8_t id[],
				 uint8_t * subzero, uint8_t * cel,
				 uint8_t * cel_frac_bits);
extern uint8_t DS18X20_read_meas_single(uint8_t familycode, uint8_t * subzero,
					uint8_t * cel, uint8_t * cel_frac_bits);

extern uint8_t DS18X20_meas_to_cel(uint8_t fc, uint8_t * sp,
				   uint8_t * subzero, uint8_t * cel,
				   uint8_t * cel_frac_bits);
extern uint16_t DS18X20_temp_to_decicel(uint8_t subzero, uint8_t cel,
					uint8_t cel_frac_bits);
extern int8_t DS18X20_temp_cmp(uint8_t subzero1, uint16_t cel1,
			       uint8_t subzero2, uint16_t cel2);
#ifdef DS18X20_EEPROMSUPPORT
// write th, tl and config-register to scratchpad (config ignored on S20)
uint8_t DS18X20_write_scratchpad(uint8_t id[],
				 uint8_t th, uint8_t tl, uint8_t conf);
// read scratchpad into array SP
uint8_t DS18X20_read_scratchpad(uint8_t id[], uint8_t sp[]);
// copy values th,tl (config) from scratchpad to DS18x20 eeprom
uint8_t DS18X20_copy_scratchpad(uint8_t with_power_extern, uint8_t id[]);
// copy values from DS18x20 eeprom to scratchpad
uint8_t DS18X20_recall_E2(uint8_t id[]);
#endif

#define OW_ONE_BUS

#ifdef OW_ONE_BUS

#define OW_PIN  PD4
#define OW_IN   PIND
#define OW_OUT  PORTD
#define OW_DDR  DDRD
#define OW_CONF_DELAYOFFSET 0
#else
#if F_OSC<1843200
#warning | experimental multi-bus-mode is not tested for
#warning | frequencies below 1,84MHz - use OW_ONE_WIRE or
#warning | faster clock-source (i.e. internal 2MHz R/C-Osc.)
#endif
#define OW_CONF_CYCLESPERACCESS 13
#define OW_CONF_DELAYOFFSET ( (uint16_t)( ((OW_CONF_CYCLESPERACCESS)*1000000L) / F_OSC  ) )
#endif
#define OW_MATCH_ROM	0x55
#define OW_SKIP_ROM	    0xCC
#define	OW_SEARCH_ROM	0xF0

#define	OW_SEARCH_FIRST	0xFF	// start new search
#define	OW_PRESENCE_ERR	0xFF
#define	OW_DATA_ERR	    0xFE
#define OW_LAST_DEVICE	0x00	// last device found
#define OW_ROMCODE_SIZE 8

extern uint8_t ow_reset(void);

extern uint8_t ow_bit_io(uint8_t b);
extern uint8_t ow_byte_wr(uint8_t b);
extern uint8_t ow_byte_rd(void);

extern uint8_t ow_rom_search(uint8_t diff, uint8_t * id);

extern void ow_command(uint8_t command, uint8_t * id);

extern void ow_parasite_enable(void);
extern void ow_parasite_disable(void);
extern uint8_t ow_input_pin_state(void);

#ifndef OW_ONE_BUS
extern void ow_set_bus(volatile uint8_t * in,
		       volatile uint8_t * out,
		       volatile uint8_t * ddr, uint8_t pin);
#endif

#define MAXSENSORS 2
uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];
uint8_t search_sensors(void);
