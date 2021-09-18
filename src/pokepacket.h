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
