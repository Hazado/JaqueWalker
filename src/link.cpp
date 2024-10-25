#include "link.h"
#include <array>

//Definitions by Dimitry

namespace FMS::Link {
	u8* buffer;
	u8 connectionId = 0;
	bool connected = false;
	
	Handle recvFinishedEvent = 0;
	PrintConsole linkConsole;
	PrintConsole defaultConsole;
	Pcap* pcap;
	PokePacket pkt;
	
	Result resultGetStatus = 0xffffffff;
	u32 StatusIR = 0;
	
	//Give Watts function:
	
	void giveWatts() {
		const u32 TIMEOUT = 50 * 1000000llu;
		
		//Wait for Connection		
		std::cout << ":: Connecting to PokeWalker..." << std::endl;
		Result res = setConnection(TIMEOUT);

		//Error Handling
		if (R_FAILED(res)) {
			printIfError(res);
			resetConnectionId();
			return;
		}
		
		//If not error.
		std::cout << "PokeWalker Found!" << std::endl;
		std::cout << "\n-------------\n SUCCESS:\n---Result: '" << std::dec << osStrError(res) << "' (" << std::hex << res << ")." << std::endl;
		
		//Close Connection
		resetConnectionId();
	}

	void emulatePW() {
		u8 ADV[1] = { CMD_POKEWALKER_ADV };
		//u8 SYNACK[8] = { CMD_POKEWALKER_SYNACK, DETAIL_DIR_FROM_WALKER, 0x83, 0x03, 0x00, 0x86, 0x09, 0x04 };
		std::cout << ":: Connecting to DS..." << std::endl;
		
		const u32 TIMEOUT = 50 * 1000000llu;
		
		//Send ADV
		std::cout << "Sending ADV..." << std::endl;
		
		blockSendData(ADV, sizeof(ADV));
		
		//Wait for Connection		
		
		Result res = setConnection(TIMEOUT);

		//Error Handling
		if (R_FAILED(res)) {
			printIfError(res);
			resetConnectionId();
			return;
		}
		
		//If not error.
		std::cout << "DS Game Found!" << std::endl;
		
		resetConnectionId();
	}

	void printIfError(Result res) {
		if (R_FAILED(res)) {
			std::cout << "\n-------------\nERROR:\n---Result: '" << osStrError(res) << "' (" << std::hex << res << ")." << std::endl;
			//u32 level = R_LEVEL(res);
			//std::cout << "\n---Level: " << level << "; Summary: " << R_SUMMARY(res) << "; Module: " << R_MODULE(res) << "; Description: " << R_DESCRIPTION(res) << std::endl;
			//std::cout << "-------------" << std::endl;
		}
	}

	void printBytes(u8* bytes, size_t size, bool sender) {
		consoleSelect(&linkConsole);
		if (sender) {
			std::cout << "SENT: " << "\e[31m"; //Red
		} else {
			std::cout << "RECV: " << "\e[32m"; //Green
			if (pcap->isOpen()) {
				PcapRecord record(size, bytes);
				pcap->addRecord(record);
			}
		}
		
		for (u32 i = 0; i < size; i++){
			std::cout << std::hex << std::setw(2) << std::setfill('0') << (bytes[i] & 0xFF) << " ";
			
			// Add spacing and newlines.
			if ((i + 1) % 4 == 0) std::cout << " ";
			if ((i + 1) % 12 == 0) {
				std::cout << std::endl;
				if ((i + 1) < size) {
					std::cout << "   ";
				}
			}
		}
		std::cout << "\e[0m";
		// Add a newline in case the request wasn't a multiple of 8 bytes.
		if ((size) % 12 != 0) std::cout << std::endl;
		consoleSelect(&defaultConsole);
	}
	
	Result initialise() {
		consoleInit(GFX_BOTTOM, &linkConsole);
		consoleInit(GFX_TOP, &defaultConsole);
		
		buffer = (u8*) memalign(BUFFER_SIZE, BUFFER_SIZE);
		Result ret = 0;
		pcap = new Pcap("fms-" + std::to_string((int) time(nullptr)) + ".pcap");
		if (!pcap->isOpen()) std::cout << ":: Failed to open capture file" << std::endl;
		
		if (R_FAILED(ret = iruInit( (u32*) buffer, BUFFER_SIZE))) return ret;

		IRU_SetBitRate(0x3); // The frequency of the Pokewalker is 115,200. More info on values: http://3dbrew.org/wiki/IRU:SetBitRate
		
		if (R_FAILED(ret = getRecvFinishedEvent(&recvFinishedEvent))) return ret;
		
		return ret;
	}
	
	void exit() {
		iruExit();
		free(buffer);
		std::cout << ":: Saving capture file" << std::endl;
		pcap->flush();
		delete pcap;
	}

	Result getRecvFinishedEvent(Handle* handle) {
		Result ret = 0;
		u32 *cmdbuf = getThreadCommandBuffer();
		
		cmdbuf[0] = IPC_MakeHeader(0xE,0,0); // 0xE0000
		
		if(R_FAILED(ret = svcSendSyncRequest(iruGetServHandle())))return ret;
		
		ret = (Result)cmdbuf[1];
		
		*handle = (Handle) cmdbuf[3];
		
		return ret;
	}
	
	Result blockReceiveData(u8* data, size_t size, u32* length, u64 timeout) {
		Result ret = 0;
		
		/*if (R_FAILED(ret = IRU_StartRecvTransfer(BUFFER_SIZE, 0))) { 
			std::cout << "StartRecvTransfer failed." << std::endl;
			IRU_WaitRecvTransfer(length);
			return ret;
		}
		
		svcWaitSynchronization(recvFinishedEvent, timeout);
		
		if (R_FAILED(ret = svcClearEvent(recvFinishedEvent))) return ret;
		
		if (R_FAILED(ret = IRU_WaitRecvTransfer(length))) return ret;
		
		if (*length == 0) return ret;
		
		std::copy(buffer, buffer + std::min((size_t) *length, size), data);
		
		//TODO: I'm missing this CRC stuff:
		
		if (dst[0] >= 0xfe) {		//no packet begins with 0x55 or 0x54
		memmove(dst + 0, dst + 1, expectedLen - 1);
		(void)iruRecvData(dst + expectedLen - 1, 1, 0, 0, 0);	//crc will tell if it happened
		}
		*/
        
        ret = iruRecvData(data, size, 0, length, true);
		
		for (u32 i = 0; i < *length; i++)
			data[i] ^= POKEWALKER_KEY;
		
		printBytes(data, (size_t) *length, false);

		return ret;
	}
	
	Result blockReceivePacket(u8* data, size_t size, u32* length, u64 timeout) {
		u8 received[8];
		u32 receivedSize = 0;
		blockReceiveData(received, size, &receivedSize, timeout);
		
		u8 packetSize = 0;
		
		std::cout << "Received:" <<  received[0] << std::endl;
        printBytes(received, receivedSize, false);
        std::cout << "ReceivedSize:" <<  receivedSize << std::endl;

        //std::fill(std::begin(received), std::end(received), 0x00);
		
		if (receivedSize == 0) {
			std::cout << "   Timeout." << std::endl;
			*length = 0;
			return MAKERESULT(RL_INFO, RS_NOP, RM_LINK, RD_TIMEOUT);
		}
		
		if (receivedSize < 4) {
			// We need at least 4 bytes: Magic number, ConnectionID, Flags/size and a CRC-8.
			*length = 0;
			// Return error if no data was received.
			std::cout << "   Received too few bytes" << std::endl;
			return MAKERESULT(RL_STATUS, RS_NOP, RM_LINK, RD_NO_DATA);
		}
		
		if (received[0] != MAGIC) {
			return MAKERESULT(RL_STATUS, RS_NOP, RM_LINK, RD_NOT_FOUND);
		}
		
		if (received[1] != connectionId && received[1] != 0x00) {
			std::cout << "   Incorrect connectionId" << std::endl;
			return MAKERESULT(RL_STATUS, RS_NOP, RM_LINK, RD_NOT_FOUND);
		}
		
		/*
		if (crc8_arr(received, receivedSize - 1) != received[receivedSize - 1]) {
			return MAKERESULT(RL_STATUS, RS_NOP, RM_LINK, RD_NOT_FOUND);
		}
		*/
		
		if ((received[2] & 0x80) == 0x80 && received[1] != 0x00) {
			// The other client would like to close the connection.
			connectionId = 0;
		}
		
		if ((received[2] & 0x40) == 0x40) {
			// Use the extended size format
			packetSize = received[3];
			memcpy(data, received + 4, packetSize);
		} else {
			// Standard size.
			packetSize = received[2] & 0b00111111;
			memcpy(data, received + 3, packetSize);
		}
		*length = packetSize;
		
		return 0;
	}
	
	Result setConnection(u64 timeout) {
		std::array<u8, sizeof(struct PokePacket) + POKEWALKER_DATA_MAX_LEN> receiveBuffer;
        std::fill(std::begin(receiveBuffer), std::end(receiveBuffer), 0x00);
		
		Result ret = 0;	
		srand(time(0));	
		
		u8 received[8];
		u32 receivedSize = 0;
		
		uint32_t ourSeed = (((uint32_t)rand()) << 16) + rand();
		
		// Error if there is already an ongoing connection.
		if (connected) {
			return MAKERESULT(RL_STATUS, RS_INVALIDSTATE, RM_LINK, RD_ALREADY_EXISTS);
		}
		
		if (R_FAILED(ret = blockReceiveData(received, 8, &receivedSize, 10 * timeout))){
			std::cout << "Failed to receive ADV data." << std::endl;
			return ret;
		}
		
		std::cout << "Debug: Data Received..." << std::endl;
		
		if (received[0] != CMD_POKEWALKER_ADV){
			std::cout << "Received value is not ADV." << std::endl;
			return ret;
		}
        std::fill(std::begin(received), std::end(received), 0x00);
        receivedSize = 0;
		
		std::cout << "Debug: Received ADV. Sending SYN..." << std::endl;
		
		//Create Packet to send SYN
		pkt.cmd = CMD_POKEWALKER_SYN;
		pkt.detail = DETAIL_DIR_TO_WALKER;
		
		//Create Session ID		
		for (u32 i = 0; i < 4; i++, ourSeed >>= 8)
			pkt.session[i] = ourSeed;
		
		//Send SYN		
		if (R_FAILED(ret = sendPacket(&pkt, timeout))){
			std::cout << "Failed to send SYN." << std::endl;
			return ret;
		}
		
		std::cout << "SYN sent. Receiving SYNACK" << std::endl;
		
		//Receive SYNACK
		if (R_FAILED(ret = blockReceiveData(received, 8 + POKEWALKER_DATA_MAX_LEN, &receivedSize, timeout))){
			std::cout << "Failed to receive SYNACK data." << std::endl;
			return ret;
		}
		
		std::cout << "Debug: Data Received..." << std::endl;
		std::cout << "Is this SYNACK?" << std::hex << receiveBuffer[0] << std::endl;
		
		/* 		
		if (commsPrvPacketRxLL(&pkt, NULL, 0) != 0 || pkt.cmd != CMD_POKEWALKER_SYNACK || pkt.detail != DETAIL_DIR_FROM_WALKER || !commsPrvChecksumPacketAndCheck(&pkt, NULL, 0))
			return false;
		*/
		
		//Set Connection as True
		
		//connectionId = receiveBuffer[6];
		//connected = true;
		/*
				for (i = 0; i < 4; i++)
			comms->session[i] ^= pkt.session[i];
			
		comms->connected = true;*/
		
		return ret;
	}
	
	Result blockSendData(u8* data, u32 length) {
        std::cout << "blockSendData:" << std::endl;
        printBytes(data, length, true);
		const uint8_t *ptr = (const uint8_t*)data;
		uint8_t txBuf[length], i;
	
		for (i = 0; i < length; i++)
			txBuf[i] = ptr[i] ^ POKEWALKER_KEY;
        
        std::cout << "txBuf: " << txBuf << std::endl;
        printBytes(txBuf, length, true);
		
		Result ret = iruSendData(txBuf, length, 0x1);

		return ret;
	}
	
	Result sendPacket(struct PokePacket *pkt, u64 timeout) {
        std::cout << "sendPacket:" << std::endl;
        
		u8* packet = new u8[8];
        
        commsPrvChecksumPacketAndRecordSum(pkt, packet, sizeof(packet));
		
		packet[0] = pkt->cmd;
		packet[1] = pkt->detail;
		packet[2] = pkt->crc[0];	
		packet[3] = pkt->crc[1];
		packet[4] = pkt->session[0];
		packet[5] = pkt->session[1];
		packet[6] = pkt->session[2];
		packet[7] = pkt->session[3];
        
		
		Result res = blockSendData(packet, sizeof(packet)); // && blockSendData(data, length);
		/*delete[] packet;
        
 		svcWaitSynchronization(recvFinishedEvent, timeout);
		
		if (R_FAILED(ret = svcClearEvent(recvFinishedEvent))) return ret;*/
		
		return res;
	}
	
	void resetConnectionId() {
		connectionId = 0;
		connected = 0;
	}

}