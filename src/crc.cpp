/*  
 *	Based on Dimitry's CRC functions
 */

#include "crc.h"
#include "pokepacket.h"

namespace FMS {
    	
	//Dimitry CRC Stuff
	uint16_t commsPrvCrc(const void *data, uint8_t len, uint16_t crcStart)
	{
		const uint8_t *src = (const uint8_t*)data;
		uint32_t crc = crcStart;
		uint8_t i;
		
		for (i = 0; i < len; i++) {
			uint16_t v = src[i];
			
			if (!(i & 1))
				v <<= 8;
			
			crc += v;
		}
		
		while (crc >> 16)
			crc = (uint16_t)crc + (crc >> 16);
		
		return crc;
	}

	uint16_t commsPrvChecksumPacket(const struct PokePacket *pkt, const void *data, uint8_t len)		//assumes pkt->crc == 0
	{
		return commsPrvCrc(data, len, commsPrvCrc(pkt, sizeof(struct PokePacket), POKEWALKER_CRC_START));
	}

	void commsPrvChecksumPacketAndRecordSum(struct PokePacket *pkt, const void *data, uint8_t len)
	{
		uint16_t crc;
		
		pkt->crc[0] = 0;
		pkt->crc[1] = 0;
		
		crc = commsPrvChecksumPacket(pkt, data, len);
		
		pkt->crc[0] = crc;
		pkt->crc[1] = crc >> 8;
	}

	bool commsPrvChecksumPacketAndCheck(struct PokePacket *pkt, const void *data, uint8_t len)
	{
		uint16_t rxedCrc = (((uint16_t)pkt->crc[1]) << 8) + pkt->crc[0];
		
		pkt->crc[0] = 0;
		pkt->crc[1] = 0;
		
		return rxedCrc == commsPrvChecksumPacket(pkt, data, len);
	}
	
void setPKTCRC(struct PokePacket *pkt){
		uint16_t crc;
		
		pkt->crc[0] = 0;
		pkt->crc[1] = 0;
		
		crc = setChecksumPacket(pkt);
		
		pkt->crc[0] = crc;
		pkt->crc[1] = crc >> 8;
	}
	
uint16_t setChecksumPacket(const struct PokePacket *pkt){
		return commsPrvCrc(pkt, sizeof(struct PokePacket), POKEWALKER_CRC_START);
	}
}
