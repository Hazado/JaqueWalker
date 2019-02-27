#include "link.h"

#include <sstream>

u8 FMS::curByte = 0;
u8 FMS::curByte2 = 1;

u8 secondShake[8] = { 0xA5, 0x00, 0x84, 0x01, 0x04, 0x04, 0x57, 0xd2 };
u8 firstShake[8] = { 0xA5, 0x00, 0x84, 0x01, 0x03, 0x04, 0x70, 0x31 };

//copied from keyboard example:
static SwkbdState swkbd;
static char mybuf[60];
static SwkbdStatusData swkbdStatus;
static SwkbdLearningData swkbdLearning;
SwkbdButton button = SWKBD_BUTTON_NONE;
bool didit = false;

void FMS::startTransfer(int first = 0, bool editdata = false) {
    std::cout << ":: Start listening" << std::endl;
    
    const size_t SIZE = 0x1000;
    u8* data;
    data = (u8*) memalign(SIZE, SIZE);

	int firstshakedone = 0;
    
	u8 dataSend[8];
	std::copy(firstShake, firstShake + 8, dataSend);
	Result ret = iruInit((u32*) data, SIZE);
    printIfError(ret);

	if (editdata == true) {
		consoleClear();
		std::string temp = FMS::u8tostring(firstShake, 8);
		const char * c = temp.c_str();
		std::cout << "original: " << temp << std::endl;
		didit = true;
		swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 3, -1);
		swkbdSetInitialText(&swkbd, mybuf);
		swkbdSetHintText(&swkbd, c);
		swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Cancel", false);
		swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Modify", true);
		static bool reload = false;
		swkbdSetStatusData(&swkbd, &swkbdStatus, reload, true);
		swkbdSetLearningData(&swkbd, &swkbdLearning, reload, false);
		reload = true;
		button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
		std::cout << button << std::endl;
		std::cout << mybuf << std::endl;
		return;
	}

    std::cout << "   Please turn on the Fit Meter and long press \n   the Fit Meter middle button within 5 seconds." << std::endl;
    // The frequency of the Pokewalker is about 116 279,07 and it looks a lot like a Wii Fit U meter.
    // Let's try that frequency, because you have to do something.
    // The closest 
    IRU_SetBitRate(3);
    
    u32 receivedSize;
    
    printIfError(IRU_StartSendTransfer(firstShake, 8));
    printIfError(IRU_WaitSendTransfer());
    Handle waitEvent;
    printIfError(getRecvFinishedEvent(&waitEvent));
    while (1) {
        printIfError(IRU_StartRecvTransfer(SIZE, 0));
		
        // Wait a maximum of 5 seconds.
		Result res = svcWaitSynchronization(waitEvent, 5000000000);
        printIfError(svcClearEvent(waitEvent));

		if (first == 0) { //data[4] == 0x4) {
			//std::cout << "   increasing 5th byte." << std::endl;
			//writableData[3] = 0x0;
			//writableData[4] = 0x3;
			//std::cout << ":: Sending the received data back..." << std::endl;
			if (data[4] == 0x3 || data[4] == 0x4) {
				// Skip 4, because it makes the meter think the connection failed.
				if (curByte == 4 && curByte2 == 4)
					curByte2 = 5;
				if (curByte == 0xFF)
					curByte2++;
				if (firstshakedone == 1)
					std::copy(secondShake, secondShake + 8, dataSend);
				//firstShake[1] = curByte++;
				//firstShake[5] = curByte2;
				dataSend[7] = crc8_arr(dataSend, 7);
				printBytes(dataSend, 8, true);
				printIfError(IRU_StartSendTransfer(dataSend, receivedSize));
				printIfError(IRU_WaitSendTransfer());
				firstshakedone++;
			}
			//std::cout << "   Done sending data back" << std::endl;
		}

		
        // Where's my sleep?
        //for (int i = 0; i < 9999; i++)
            //std::cout << i << "\r" << std::flush;
        // Sleep for a little.
        
        hidScanInput();
        printIfError(IRU_WaitRecvTransfer(&receivedSize));
        
        if (receivedSize == 0 && R_DESCRIPTION(res) == 0x3FE)  {
            std::cout << "   No data detected within 5 seconds. " << std::endl;
            break;
        } else if (receivedSize == 0 ){
            std::cout << "   No data detected and no timeout." << std::endl;
            printIfError(res);
            break;
        }
        //std::cout << std::endl << ":: Received " << receivedSize << " bytes!" << std::endl;
        
        //if(data[4] == 0x3) {
            //memcpy(writableData, data, receivedSize);
        //}
        printBytes(data, receivedSize, false);
        // Prevent implicit uint8_t to char conversions
        
		if (first == 1) { //data[4] == 0x4) {
			//std::cout << "   increasing 5th byte." << std::endl;
			//writableData[3] = 0x0;
			//writableData[4] = 0x3;
			//std::cout << ":: Sending the received data back..." << std::endl;
			if (data[4] == 0x3 || data[4] == 0x4) {
				// Skip 4, because it makes the meter think the connection failed.
				if (curByte == 4 && curByte2 == 4)
					curByte2 = 5;
				if (curByte == 0xFF)
					curByte2++;
				if (firstshakedone == 1)
					std::copy(secondShake, secondShake + 8, dataSend);
				//firstShake[1] = curByte++;
				//firstShake[5] = curByte2;
				dataSend[7] = crc8_arr(dataSend, 7);
				printBytes(dataSend, 8, true);
				printIfError(IRU_StartSendTransfer(dataSend, receivedSize));
				printIfError(IRU_WaitSendTransfer());
				firstshakedone++;
			}
			//std::cout << "   Done sending data back" << std::endl;
		}
		if (first == 2) {
			std::copy(data, data + 8, dataSend);
			dataSend[7] = crc8_arr(dataSend, 7);
			printBytes(dataSend, 8, true);
			printIfError(IRU_StartSendTransfer(dataSend, receivedSize));
			printIfError(IRU_WaitSendTransfer());
		}
    }
    std::cout << ":: Stopped listening" << std::endl;
    iruExit();
}

void FMS::printIfError(Result ret) {
    if (ret != 0) {
        std::cout << "   Result " << std::hex << ret << ": " << std::dec << osStrError(ret) << std::endl;
        u32 level = R_LEVEL(ret);
        std::cout << "   Lvl: " << level << "; Smry: " << R_SUMMARY(ret) << "; Mdl: " << R_MODULE(ret) << "; Desc: " << R_DESCRIPTION(ret) << std::endl;
    }
}

void FMS::printBytes(u8* bytes, size_t size, bool sender) {
    if (sender) {
        std::cout << "-> ";
    } else {
        std::cout << "<- ";
    }
    for (u32 i = 0; i < size; i++){
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (bytes[i] & 0xFF) << " ";
        
        if ((i + 1) % 4 == 0) std::cout << " ";
        if ((i + 1) % 8 == 0) std::cout << std::endl;
    }
}
std::string FMS::u8tostring(u8* bites, size_t syze) {
	std::string returnstring;
	std::stringstream buffer;
	for (u32 i = 0; i < syze; i++) {
		buffer << std::hex << std::setw(2) << std::setfill('0') << (bites[i] & 0xFF) << " ";

		if ((i + 1) % 4 == 0) buffer << " ";
	}
	returnstring = buffer.str();
	return returnstring;
}

Result FMS::getRecvFinishedEvent(Handle* handle) {
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	
	cmdbuf[0] = IPC_MakeHeader(0xE,0,0); // 0xE0000
	
	if(R_FAILED(ret = svcSendSyncRequest(iruGetServHandle())))return ret;
	
	ret = (Result)cmdbuf[1];
	
	*handle = (Handle) cmdbuf[3];
	
	return ret;
}
