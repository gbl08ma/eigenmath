#include <fxcg/display.h>
#include <fxcg/file.h>
#include <fxcg/keyboard.h>
#include <fxcg/system.h>
#include <fxcg/misc.h>
#include <fxcg/app.h>
#include <fxcg/serial.h>
#include <fxcg/rtc.h>
#include <fxcg/heap.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "aboutGUI.hpp"
#include "textGUI.hpp"
#include "graphicsProvider.hpp"
#include "versionProvider.hpp"

void showAbout() {
 int key;
 Bdisp_AllClr_VRAM();
 DefineStatusMessage((char*)"About Eigenmath", 1, 0, 0);
 DisplayStatusArea();
 //y increment between each line: 17; between paragraphs: 20
 int textX = 0;
 int textY = 5;
 char verBuffer[100];
 getVersion(verBuffer);
 PrintMini(&textX, &textY, (char*)"Version ", 0, 0xFFFFFFFF, 0, 0, COLOR_BLUE, COLOR_WHITE, 1, 0);
 PrintMini(&textX, &textY, verBuffer, 0, 0xFFFFFFFF, 0, 0, COLOR_BLUE, COLOR_WHITE, 1, 0);
 textY = textY + 17; textX = 0;
 getTimestamp(verBuffer);
 PrintMini(&textX, &textY, verBuffer, 0, 0xFFFFFFFF, 0, 0, COLOR_GRAY, COLOR_WHITE, 1, 0);
 textY = 42;
 textX = 0;
 PrintMini(&textX, &textY, (char*)"Developed by gbl08ma at", 0, 0xFFFFFFFF, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
 drawSegvaultLogo(0, 65+24); //24 pixels for the status bar
 textY = 125;
 textX = 0;
 PrintMini(&textX, &textY, (char*)"http://segvault.tny.im", 0, 0xFFFFFFFF, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
 textY = 142;
 textX = 0;
 PrintMini(&textX, &textY, (char*)"gabriel@tny.im | http://gbl08ma.com", 0, 0xFFFFFFFF, 0, 0, COLOR_BLACK, COLOR_WHITE, 1, 0);
 
 textX = 0; textY = 175;
 PrintMini(&textX, &textY, (char*)"Press any key", 0, 0xFFFFFFFF, 0, 0, COLOR_BLUE, COLOR_WHITE, 1, 0);
 GetKey(&key);

 Bdisp_AllClr_VRAM();
 Bdisp_EnableColor(1);
 DisplayStatusArea();
 
  textArea text;

  textElement elem[23];
  text.elements = elem;

  elem[0].text = (char*)"This is a port of Eigenmath v137 to the Prizm. The Eigenmath core was developed by George Weigt and its official website is at http://gweigt.net/";

  elem[1].newLine = 1;
  elem[1].lineSpacing = 8;
  elem[1].text = (char*)"At the time of writing, its source code is not available at the official website.";
  
  elem[2].newLine = 1;
  elem[2].text = (char*)"However, the project used to be hosted at SourceForge and licensed under the GNU GPL.";
  elem[2].spaceAtEnd = 1;

  elem[3].text = (char*)"The Eigenmath code used here is from those times, and thus the following applies:";
  
  elem[4].newLine = 1;
  elem[4].lineSpacing = 8;
  elem[4].text = (char*)"Eigenmath, Copyright (C) 2004-2013 George Weigt";

  elem[5].newLine = 1;
  elem[5].text = (char*)"This port, Copyright (C) 2013-2017 Gabriel 'gbl08ma' Maia";
  
  elem[6].newLine = 1;
  elem[6].text = (char*)"This program is free software; you can redistribute it and/or modify it under the terms of the second version of the GNU General Public License as published by the Free Software Foundation; you are not allowed to choose another version.";

  elem[7].newLine = 1;
  elem[7].text = (char*)"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.";
  
  elem[8].newLine = 1;
  elem[8].lineSpacing = 8;
  elem[8].color = COLOR_RED;
  elem[8].text = (char*)"In no event will the authors be held liable for any damages arising from the use of this software.";
  
  elem[9].newLine = 1;
  elem[9].lineSpacing = 8;
  elem[9].color = COLOR_BLUE;
  elem[9].text = (char*)"Licenses for imported software, other than Eigenmath:";
  
  elem[10].newLine = 1;
  elem[10].text = (char*)"mystrtod - Convert string to double";
  
  elem[11].newLine = 1;
  elem[11].text = (char*)"Copyright (C) 2002 Michael Ringgaard. All rights reserved.";
  
  elem[12].newLine = 1;
  elem[12].text = (char*)"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:";
  
  elem[13].newLine = 1;
  elem[13].text = (char*)"1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.";
  
  elem[14].newLine = 1;
  elem[14].text = (char*)"2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.";
  
  elem[15].newLine = 1;
  elem[15].text = (char*)"3. Neither the name of the project nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.";
  
  elem[16].newLine = 1;
  elem[16].text = (char*)"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.";
  elem[16].spaceAtEnd = 1;
  elem[17].text = (char*)"IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;";
  elem[17].spaceAtEnd = 1;
  elem[18].text = (char*)"LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,";
  elem[18].spaceAtEnd = 1;
  elem[19].text = (char*)"EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";
  
  elem[20].newLine = 1;
  elem[20].lineSpacing = 8;
  elem[20].text = (char*)"Press EXIT";

  text.numelements = 21;
  doTextArea(&text);
 DefineStatusMessage((char*)"", 1, 0, 0);
}