/*
	By DeosamoX 
	License is GPLv3, although I don't really care about Copyright, do whatever you want with my code.
	
	Forked from Fit Meter Sync (by HenkKalkwater and MrKev312). Some bits borrowed from Dimitry's
*/


#include <3ds.h>
#include <iostream>

#include "link.h"

//TODO: Delete not needed stuf here:

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

//CODE

using namespace FMS;

int main(int argc, char** argv) {
	gfxInitDefault();
	hidInit();
	consoleInit(GFX_TOP, nullptr);
	Link::printIfError(Link::initialise());
	
	//Menu
	
	std::cout << "JaqueWalker for 3DS! By DeosamoX" << std::endl;
	std::cout << "Press (START) anytime to exit." << std::endl;
	std::cout <<  "--------------" << std::endl;
		
	std::string text = "Select desired action: \n (A) Try to Give Watts \n (B) Try to Emulate PokeWalker \n (Y) Record";
	std::cout << text << std::endl;
    
	
	while (aptMainLoop()) {
		hidScanInput();
		if (hidKeysDown() & KEY_START)
			break;
		if (hidKeysDown() & KEY_A) {
			//Link::startTransfer2();
			Link::giveWatts();
            std::cout << text << std::endl;
        }
		if (hidKeysDown() & KEY_B) {
			//Link::startTransfer(1, false);
			Link::emulatePW();
			std::cout << text << std::endl;
		}
		if (hidKeysDown() & KEY_X) {
			//Link::startTransfer(2, false);
			std::cout << text << std::endl;
		}
		if (hidKeysDown() & KEY_Y) {
			Link::startTransfer(3, false);
			std::cout << text << std::endl;
		}
		//copied from keyboard example
		if (hidKeysDown() & KEY_SELECT)
		{
			//Link::startTransfer(0, true);
			std::cout << text << std::endl;
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
		//Wait for VBlank
		gspWaitForVBlank();
	}
	Link::exit();
	gfxExit();
	hidExit();
}
