#include <inttypes.h>
#include "ds18x20.h"
#define CRC8INIT	0x00
#define CRC8POLY	0x18	//0X18 = X^8+X^5+X^4+X^0

void delayloop32(uint32_t loops)
{
	__asm__ volatile ("cp  %A0,__zero_reg__ \n\t"
			  "cpc %B0,__zero_reg__ \n\t"
			  "cpc %C0,__zero_reg__ \n\t"
			  "cpc %D0,__zero_reg__ \n\t"
			  "breq L_Exit_%=       \n\t"
			  "L_LOOP_%=:           \n\t"
			  "subi %A0,1           \n\t"
			  "sbci %B0,0           \n\t"
			  "sbci %C0,0           \n\t"
			  "sbci %D0,0           \n\t"
			  "brne L_LOOP_%=            \n\t"
			  "L_Exit_%=:           \n\t":"=w" (loops)
			  :"0"(loops)
	    );

	return;
}

uint8_t crc8(uint8_t * data_in, uint16_t number_of_bytes_to_read)
{
	uint8_t crc;
	uint16_t loop_count;
	uint8_t bit_counter;
	uint8_t data;
	uint8_t feedback_bit;

	crc = CRC8INIT;

	for (loop_count = 0; loop_count != number_of_bytes_to_read;
	     loop_count++) {
		data = data_in[loop_count];

		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;

			if (feedback_bit == 0x01) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if (feedback_bit == 0x01) {
				crc = crc | 0x80;
			}

			data = data >> 1;
			bit_counter--;

		} while (bit_counter > 0);
	}

	return crc;
}

#ifdef OW_ONE_BUS

#define OW_GET_IN()   ( OW_IN & (1<<OW_PIN))
#define OW_OUT_LOW()  ( OW_OUT &= (~(1 << OW_PIN)) )
#define OW_OUT_HIGH() ( OW_OUT |= (1 << OW_PIN) )
#define OW_DIR_IN()   ( OW_DDR &= (~(1 << OW_PIN )) )
#define OW_DIR_OUT()  ( OW_DDR |= (1 << OW_PIN) )

#else

/* set bus-config with ow_set_bus() */
uint8_t OW_PIN_MASK;
volatile uint8_t *OW_IN;
volatile uint8_t *OW_OUT;
volatile uint8_t *OW_DDR;

#define OW_GET_IN()   ( *OW_IN & OW_PIN_MASK )
#define OW_OUT_LOW()  ( *OW_OUT &= (uint8_t) ~OW_PIN_MASK )
#define OW_OUT_HIGH() ( *OW_OUT |= (uint8_t)  OW_PIN_MASK )
#define OW_DIR_IN()   ( *OW_DDR &= (uint8_t) ~OW_PIN_MASK )
#define OW_DIR_OUT()  ( *OW_DDR |= (uint8_t)  OW_PIN_MASK )
//#define OW_PIN2 PD6
//#define OW_DIR_IN2()   ( *OW_DDR &= ~(1 << OW_PIN2 ) )

void ow_set_bus(volatile uint8_t * in,
		volatile uint8_t * out, volatile uint8_t * ddr, uint8_t pin)
{
	OW_DDR = ddr;
	OW_OUT = out;
	OW_IN = in;
	OW_PIN_MASK = (1 << pin);
	ow_reset();
}

#endif

uint8_t ow_input_pin_state()
{
	return OW_GET_IN();
}

void ow_parasite_enable(void)
{
	OW_OUT_HIGH();
	OW_DIR_OUT();
}

void ow_parasite_disable(void)
{
	OW_OUT_LOW();
	OW_DIR_IN();
}

uint8_t ow_reset(void)
{
	uint8_t err;
	uint8_t sreg;

	OW_OUT_LOW();		// disable internal pull-up (maybe on from parasite)
	OW_DIR_OUT();		// pull OW-Pin low for 480us

	delay_us(480);

	sreg = SREG;
	cli();

	// set Pin as input - wait for clients to pull low
	OW_DIR_IN();		// input

	delay_us(66);
	err = OW_GET_IN();	// no presence detect
	// nobody pulled to low, still high

	SREG = sreg;		// sei()

	// after a delay the clients should release the line
	// and input-pin gets back to high due to pull-up-resistor
	delay_us(480 - 66);
	if (OW_GET_IN() == 0)	// short circuit
		err = 1;

	return err;
}

uint8_t ow_bit_io(uint8_t b)
{
	uint8_t sreg;

	sreg = SREG;
	cli();

	OW_DIR_OUT();		// drive bus low

	delay_us(1);		// Recovery-Time wuffwuff was 1
	if (b)
		OW_DIR_IN();	// if bit is 1 set bus high (by ext. pull-up)

	// wuffwuff delay was 15uS-1 see comment above
	delay_us(15 - 1 - OW_CONF_DELAYOFFSET);

	if (OW_GET_IN() == 0)
		b = 0;		// sample at end of read-timeslot

	delay_us(60 - 15);
	OW_DIR_IN();

	SREG = sreg;		// sei();

	return b;
}

uint8_t ow_byte_wr(uint8_t b)
{
	uint8_t i = 8, j;

	do {
		j = ow_bit_io(b & 1);
		b >>= 1;
		if (j)
			b |= 0x80;
	} while (--i);

	return b;
}

uint8_t ow_byte_rd(void)
{
	// read by sending 0xff (a dontcare?)
	return ow_byte_wr(0xFF);
}

uint8_t ow_rom_search(uint8_t diff, uint8_t * id)
{
	uint8_t i, j, next_diff;
	uint8_t b;

	if (ow_reset())
		return OW_PRESENCE_ERR;	// error, no device found

	ow_byte_wr(OW_SEARCH_ROM);	// ROM search command
	next_diff = OW_LAST_DEVICE;	// unchanged on last device

	i = OW_ROMCODE_SIZE * 8;	// 8 bytes

	do {
		j = 8;		// 8 bits
		do {
			b = ow_bit_io(1);	// read bit
			if (ow_bit_io(1)) {	// read complement bit
				if (b)	// 11
					return OW_DATA_ERR;	// data error
			} else {
				if (!b) {	// 00 = 2 devices
					if (diff > i
					    || ((*id & 1) && diff != i)) {
						b = 1;	// now 1
						next_diff = i;	// next pass 0
					}
				}
			}
			ow_bit_io(b);	// write bit
			*id >>= 1;
			if (b)
				*id |= 0x80;	// store bit

			i--;

		} while (--j);

		id++;		// next byte

	} while (i);

	return next_diff;	// to continue search
}

void ow_command(uint8_t command, uint8_t * id)
{
	uint8_t i;

	ow_reset();

	if (id) {
		ow_byte_wr(OW_MATCH_ROM);	// to a single device
		i = OW_ROMCODE_SIZE;
		do {
			ow_byte_wr(*id);
			id++;
		} while (--i);
	} else {
		ow_byte_wr(OW_SKIP_ROM);	// to all devices
	}

	ow_byte_wr(command);
}

void DS18X20_find_sensor(uint8_t * diff, uint8_t id[])
{
	for (;;) {
		*diff = ow_rom_search(*diff, &id[0]);
		if (*diff == OW_PRESENCE_ERR || *diff == OW_DATA_ERR ||
		    *diff == OW_LAST_DEVICE)
			return;
		if (id[0] == DS18B20_ID || id[0] == DS18S20_ID)
			return;
	}
}

//=======================================================================
uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;

	nSensors = 0;

	for (diff = OW_SEARCH_FIRST;
	     diff != OW_LAST_DEVICE && nSensors < MAXSENSORS;) {
		DS18X20_find_sensor(&diff, &id[0]);

		if (diff == OW_PRESENCE_ERR) {
			break;
		}

		if (diff == OW_DATA_ERR) {
			break;
		}

		for (i = 0; i < OW_ROMCODE_SIZE; i++)
			gSensorIDs[nSensors][i] = id[i];

		nSensors++;
	}

	return nSensors;
}

//=======================================================================
//=======================================================================
uint8_t DS18X20_meas_to_cel(uint8_t fc, uint8_t * sp,
			    uint8_t * subzero, uint8_t * cel,
			    uint8_t * cel_frac_bits)
{
	uint16_t meas;
	uint8_t i;

	meas = sp[0];		// LSB
	meas |= ((uint16_t) sp[1]) << 8;	// MSB
	//meas = 0xff5e; meas = 0xfe6f;

	//  only work on 12bit-base
	if (fc == DS18S20_ID) {	// 9 -> 12 bit if 18S20
		/* Extended measurements for DS18S20 contributed by Carsten Foss */
		meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
		meas <<= 3;	// Convert to 12-bit , now degrees are in 1/16 degrees units
		meas += (16 - sp[6]) - 4;	// Add the compensation , and remember to subtract 0.25 degree (4/16)
	}
	// check for negative 
	if (meas & 0x8000) {
		*subzero = 1;	// mark negative
		meas ^= 0xffff;	// convert to positive => (twos complement)++
		meas++;
	} else
		*subzero = 0;

	// clear undefined bits for B != 12bit
	if (fc == DS18B20_ID) {	// check resolution 18B20
		i = sp[DS18B20_CONF_REG];
		if ((i & DS18B20_12_BIT) == DS18B20_12_BIT) ;
		else if ((i & DS18B20_11_BIT) == DS18B20_11_BIT)
			meas &= ~(DS18B20_11_BIT_UNDF);
		else if ((i & DS18B20_10_BIT) == DS18B20_10_BIT)
			meas &= ~(DS18B20_10_BIT_UNDF);
		else {		// if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) { 
			meas &= ~(DS18B20_9_BIT_UNDF);
		}
	}

	*cel = (uint8_t) (meas >> 4);
	*cel_frac_bits = (uint8_t) (meas & 0x000F);

	return DS18X20_OK;
}

//=======================================================================
uint16_t DS18X20_temp_to_decicel(uint8_t subzero, uint8_t cel,
				 uint8_t cel_frac_bits)
{
	uint16_t h;
	uint8_t i;
	uint8_t need_rounding[] = { 1, 3, 4, 6, 9, 11, 12, 14 };

	h = cel_frac_bits * DS18X20_FRACCONV / 1000;
	h += cel * 10;
	if (!subzero) {
		for (i = 0; i < sizeof(need_rounding); i++) {
			if (cel_frac_bits == need_rounding[i]) {
				h++;
				break;
			}
		}
	}
	return h;
}

//=======================================================================
int8_t DS18X20_temp_cmp(uint8_t subzero1, uint16_t cel1,
			uint8_t subzero2, uint16_t cel2)
{
	int16_t t1 = (subzero1) ? (cel1 * (-1)) : (cel1);
	int16_t t2 = (subzero2) ? (cel2 * (-1)) : (cel2);

	if (t1 < t2)
		return -1;
	if (t1 > t2)
		return 1;
	return 0;
}

//=======================================================================
uint8_t DS18X20_get_power_status(uint8_t id[])
{
	uint8_t pstat;
	ow_reset();
	ow_command(DS18X20_READ_POWER_SUPPLY, id);
	pstat = ow_bit_io(1);	// pstat 0=is parasite/ !=0 ext. powered
	ow_reset();
	return (pstat) ? DS18X20_POWER_EXTERN : DS18X20_POWER_PARASITE;
}

//=======================================================================
uint8_t DS18X20_start_meas(uint8_t with_power_extern, uint8_t id[])
{
	ow_reset();		//**
	if (ow_input_pin_state()) {	// only send if bus is "idle" = high
		ow_command(DS18X20_CONVERT_T, id);
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_enable();
		return DS18X20_OK;
	} else {
		return DS18X20_START_FAIL;
	}
}

//=======================================================================
uint8_t DS18X20_read_meas(uint8_t id[], uint8_t * subzero,
			  uint8_t * cel, uint8_t * cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];

	ow_reset();		//**
	ow_command(DS18X20_READ, id);
	for (i = 0; i < DS18X20_SP_SIZE; i++)
		sp[i] = ow_byte_rd();
	if (crc8(&sp[0], DS18X20_SP_SIZE))
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(id[0], sp, subzero, cel, cel_frac_bits);
	return DS18X20_OK;
}

//=======================================================================
uint8_t DS18X20_read_meas_single(uint8_t familycode, uint8_t * subzero,
				 uint8_t * cel, uint8_t * cel_frac_bits)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];

	ow_command(DS18X20_READ, NULL);
	for (i = 0; i < DS18X20_SP_SIZE; i++)
		sp[i] = ow_byte_rd();
	if (crc8(&sp[0], DS18X20_SP_SIZE))
		return DS18X20_ERROR_CRC;
	DS18X20_meas_to_cel(familycode, sp, subzero, cel, cel_frac_bits);
	return DS18X20_OK;
}

//=======================================================================
#ifdef DS18X20_EEPROMSUPPORT

uint8_t DS18X20_write_scratchpad(uint8_t id[],
				 uint8_t th, uint8_t tl, uint8_t conf)
{
	ow_reset();		//**
	if (ow_input_pin_state()) {	// only send if bus is "idle" = high
		ow_command(DS18X20_WRITE_SCRATCHPAD, id);
		ow_byte_wr(th);
		ow_byte_wr(tl);
		if (id[0] == DS18B20_ID)
			ow_byte_wr(conf);	// config avail. on B20 only
		return DS18X20_OK;
	} else {
		return DS18X20_ERROR;
	}
}

uint8_t DS18X20_read_scratchpad(uint8_t id[], uint8_t sp[])
{
	uint8_t i;

	ow_reset();		//**
	if (ow_input_pin_state()) {	// only send if bus is "idle" = high
		ow_command(DS18X20_READ, id);
		for (i = 0; i < DS18X20_SP_SIZE; i++)
			sp[i] = ow_byte_rd();
		return DS18X20_OK;
	} else {
		return DS18X20_ERROR;
	}
}

uint8_t DS18X20_copy_scratchpad(uint8_t with_power_extern, uint8_t id[])
{
	ow_reset();		//**
	if (ow_input_pin_state()) {	// only send if bus is "idle" = high
		ow_command(DS18X20_COPY_SCRATCHPAD, id);
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_enable();
		delay_ms(DS18X20_COPYSP_DELAY);	// wait for 10 ms 
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_disable();
		return DS18X20_OK;
	} else {
		return DS18X20_START_FAIL;
	}
}

uint8_t DS18X20_recall_E2(uint8_t id[])
{
	ow_reset();		//**
	if (ow_input_pin_state()) {	// only send if bus is "idle" = high
		ow_command(DS18X20_RECALL_E2, id);
		// TODO: wait until status is "1" (then eeprom values
		// have been copied). here simple delay to avoid timeout 
		// handling
		delay_ms(DS18X20_COPYSP_DELAY);
		return DS18X20_OK;
	} else {
		return DS18X20_ERROR;
	}
}
#endif
//=======================================================================
