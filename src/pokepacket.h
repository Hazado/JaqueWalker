#ifndef POKEPACKET_H
#define POKEPACKET_H

#include <3ds.h>

#define POKEWALKER_DATA_MAX_LEN			128		//data per packet max, else we overflow pokewalker's buffer
#define POKEWALKER_CRC_START			0x0002
#define POKEWALKER_KEY					0xAA //According to dimitry "every byte sent is simply being XORed with 0xAA before being sent"

#define DETAIL_DIR_TO_WALKER			0x01
#define DETAIL_DIR_FROM_WALKER			0x02

#define CMD_POKEWALKER_ADV				0xfc
#define CMD_POKEWALKER_SYN				0xfa
#define CMD_POKEWALKER_SYNACK			0xf8
#define CMD_POKEWALKER_DIS_REQ			0x66
#define CMD_POKEWALKER_DIS_RSP			0x68
#define CMD_EEPROM_READ_REQ				0x0c
#define CMD_EEPROM_READ_RSP				0x0e
#define CMD_EEPROM_WRITE_REQ			0x0a
#define CMD_EEPROM_WRITE_RSP			0x04
#define CMD_EVENT_POKE_RXED				0xc2
#define CMD_EVENT_ITEM_RXED				0xc4
#define CMD_EVENT_ROUTE_RXED			0xc6
#define CMD_WRITE						0x06

//Dimitry's Exploits

static const uint8_t add_watts_expoint_upload_to_0xF956[] = {
	0x56,
		
	0x79, 0x00, 0x27, 0xf,		//mov.w		#9999, r0
	0x5E, 0x00, 0x1f, 0x40,		//jsr		addWatts_word ( == addWatts + 2)
	
	0x79, 0x00, 0x08, 0xd6,		//mov.w		#&irHandleRxedByteIfAnyHasBeenRxed, r0
	
	0x5a, 0x00, 0x69, 0x3a,		//jmp		setProcToCallByMainInLoop
};

//set step count high too (optional)
static const char set_high_stepcount_exploit_upload_to_0xF79C[] = {
	0x9C,
	
	0x00, 0x01, 0x86, 0x9f
};

/* static const uint8_t rom_dump_exploit_upload_to_0xF956[] = {
	0x56,
	
	0x5E, 0x00, 0xBA, 0x42,		//jsr		common_prologue
	
	0x19, 0x55,					//sub.w		r5, r5
	
//lbl_big_loop:
	0x79, 0x06, 0xf8, 0xd6,		//mov.w		0xf8d6, r6
	
	0xfc, 0x80,					//mov.b		0x80, r4l
	
	0x7b, 0x5c, 0x59, 0x8f,		//eemov.b
	
	0x79, 0x00, 0xaa, 0x80,		//mov.w		#0xaa80, r0
	
	0x5e, 0x00, 0x07, 0x72,		//jsr		sendPacket
	
	0x5E, 0x00, 0x25, 0x9E,		//jsr		wdt_pet

	0x79, 0x25, 0xc0, 0x00,		//cmp.w		r5, #0xc000
	
	0x46, 0xe4,					//bne		$-0x1c		//lbl_big_loop
	
	0x79, 0x00, 0x08, 0xd6,		//mov.w		#&irHandleRxedByteIfAnyHasBeenRxed, r0
	
	0x5e, 0x00, 0x69, 0x3a,		//jsr		setProcToCallByMainInLoop
	
	0x5a, 0x00, 0xba, 0x62		//jmp		common_epilogue
};
*/


static const uint8_t trigger_uploaded_code_upload_to_0xF7E0[] = {
	0xe0,

	0xf9, 0x56
};

namespace FMS {
	struct PokePacket {
		uint8_t cmd;
		uint8_t detail;
		uint8_t crc[2];
		uint8_t session[4];
		//data here
	};
}

#endif //POKEPACKET_H
