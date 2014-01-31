/**********************************************************************************
Filename	: AlazarAcquireThread.h
Authors		: Kevin Wong, Yifan Jian, Jing Xu, Marinko Sarunic
Published	: January 30th, 2014

Copyright (C) 2014 Biomedical Optics Research Group - Simon Fraser University

This file is part of a free software. Details of this software has been described 
in the paper: 

"Jian Y, Wong K, Sarunic MV; Graphics processing unit accelerated optical coherence 
tomography processing at megahertz axial scan rate and high resolution video rate 
volumetric rendering. J. Biomed. Opt. 0001;18(2):026002-026002.  
doi:10.1117/1.JBO.18.2.026002."

Please refer to this paper for further information about this software. Redistribution 
and modification of this code is restricted to academic purposes ONLY, provided that 
the following conditions are met:
-	Redistribution of this code must retain the above copyright notice, this list of 
	conditions and the following disclaimer
-	Any use, disclosure, reproduction, or redistribution of this software outside of 
	academic purposes is strictly prohibited

*DISCLAIMER*
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
TORT (INCLUDING NEGLIGENCE OR OTHERWISE)ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied.
**********************************************************************************/

#pragma once
	
#include "tchar.h"

//include the Alazar header files
#include "AlazarApi.h"
#include "AlazarCmd.h"
#include "GlobalClass.h";
#include "ScanningProcs.h"
#include "thread.h"

#define _CRT_SECURE_NO_DEPRECATE

#define FWDSCAN				0x00000001UL
#define BKWDSCAN			0x00000002UL
#define BUFFER_COUNT 100
#define __SAVE_DURING_ACQ

extern "C" RETURN_CODE AlazarReadFpgaRegAtOffset(HANDLE	hDevice, unsigned int offset, unsigned int *retVal);    


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AcquireThreadAlazar : public Win32Thread
{
public:
    AcquireThreadAlazar() : Win32Thread() {}
    ~AcquireThreadAlazar();

    HANDLE	alazarBoardHandle;
	FILE * fid;
	BoardDef bd;

	// public variables
	bool paused; //The 'paused' boolean variable is a workaround for starting/stopping the alazarthread
	bool breakLoop;
	bool bExtClk;
	unsigned short *RawData;
	GlobalClass *globalOptions;
    ScanThreadLinear *ScanThrPtr;
	unsigned int ExtTriggerRange;
	unsigned int ExtTriggerCoupling;
	unsigned int mode;
	unsigned int AcqMode;
	unsigned short *BufferArray[BUFFER_COUNT];
	unsigned short output;
	int frameNumCount;

	// procedure definitions
	void UpdateTriggerVal (int TVal);
	void UpdateTriggerDelay (int TrigDelayVal);
	void UpdatePreTriggerPts (int PreDepthVal);
	//RETURN_CODE StopAlazarThr();
	int AlazarAcquireDMA (void);
	void SetupAlazarBoard(char *setupFileName);
	void checkAlazarError(RETURN_CODE status, char *alazarCmd);

private:
	// Now the thread execution code resides in the following
    void run( )
    {
		paused = false;
		AlazarAcquireDMA();
		return;
	};
};

