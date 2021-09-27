#ifndef CRC_H
#define CRC_H

#include <3ds.h>

namespace FMS {
	uint16_t commsPrvCrc(const void *data, uint8_t len, uint16_t crcStart);
	uint16_t commsPrvChecksumPacket(const struct PokePacket *pkt, const void *data, uint8_t len);		//assumes pkt->crc == 0
	void commsPrvChecksumPacketAndRecordSum(struct PokePacket *pkt, const void *data, uint8_t len);
	bool commsPrvChecksumPacketAndCheck(struct PokePacket *pkt, const void *data, uint8_t len);
	
	void setPKTCRC(struct PokePacket *pkt);
	uint16_t setChecksumPacket(const struct PokePacket *pkt);
}

#endif //CRC_H
