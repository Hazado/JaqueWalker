/*
	By DeosamoX 
	License is GPLv3, although I don't really care about Copyright, do whatever you want with my code.
	
	Forked from Fit Meter Sync (by HenkKalkwater and MrKev312). Some bits borrowed from Dimitry's
*/


#include <3ds.h>
#include <iostream>

#include "link.h"

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
		if (hidKeysDown() & KEY_A){
			Link::giveWatts();
            std::cout << text << std::endl;
        }
		if (hidKeysDown() & KEY_B){
			Link::emulatePW();
			std::cout << text << std::endl;
		}
		if (hidKeysDown() & KEY_X){
			std::cout << text << std::endl;
		}
		if (hidKeysDown() & KEY_Y){
			std::cout << text << std::endl;
		}
		if (hidKeysDown() & KEY_SELECT){
			std::cout << text << std::endl;
		}

		//Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
		//Wait for VBlank
		gspWaitForVBlank();
	}
	Link::exit();
	gfxExit();
	hidExit();
}
