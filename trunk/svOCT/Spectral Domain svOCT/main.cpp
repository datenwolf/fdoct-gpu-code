/**********************************************************************************
Filename	: cuda_FilterKernels.cu
Authors		: Jing Xu, Kevin Wong, Yifan Jian, Marinko Sarunic
Published	: Janurary 6th, 2014

Copyright (C) 2014 Biomedical Optics Research Group - Simon Fraser University
This software contains source code provided by NVIDIA Corporation.

This file is part of a Open Source software. Details of this software has been described 
in the papers titled: 

"Jing Xu, Kevin Wong, Yifan Jian, and Marinko V. Sarunic.
'Real-time acquisition and display of flow contrast with speckle variance OCT using GPU'
In press (JBO)
and
"Jian, Yifan, Kevin Wong, and Marinko V. Sarunic. 'GPU accelerated OCT processing at 
megahertz axial scan rate and high resolution video rate volumetric rendering.' 
In SPIE BiOS, pp. 85710Z-85710Z. International Society for Optics and Photonics, 2013."


Please refer to these papers for further information about this software. Redistribution 
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

#include "thread.hpp"
#include "FileReadThread.hpp"
#include "ProcessThread.hpp"
#include <iostream>
#include <math.h>

#define		LAMBDA_MIN					763.412f
#define		D_LAMBDA					0.100571f
#define		SECOND_ORDER_CORRECTION		-2.57900E-06f
#define		THIRD_ORDER_CORRECTION		-6.99779E-11f
#define		FOURTH_ORDER_CORRECTION		0.0f

using namespace std;

int main()
{
	bool fileRead = true;

	int frameWidth = 1024;
	int frameHeight =300;
	int framesPerBuffer = 30; //This value needs to be divisble by 3, and a factor of the total number of frames
	int samplingMethod  = 0; //0 for Interpolation, 1 for Fast Cubic, 2 for Pref Cubic
	int volumeFrames = 900; //Useless for this code

//OpenGL ONLY Variables to be transferred
	bool processData = true;  //MUST be true
	bool fundusRender = true; //Can be false or true
	bool volumeRender = false; //Volume render currently does not work with speckle variance in this version of the code
	int fileLen;
	int bufferLen;
	int windowWidth = 1080;
	int windowHeight = 720;

	buffer *h_buffer = new buffer[BUFFNUM];
	int buffCtr = 0;
	unsigned short *h_ProcBuffer;

	bool *registeredHost;

	enum volumeDisplay {DownSize, Crop};
	volumeDisplay volumeMode = Crop;

//CUDA ONLY Variables to be transferred
	int fftLenMult = 2;
	float dispMag = 10;
	float dispVal = -6.5f * pow(0.1f,5);
	float dispValThird = 0;

	//Calculation of lookupLambda Coordinates
	float lambda_Max= LAMBDA_MIN	+	(frameWidth-1) * D_LAMBDA	+
					pow((frameWidth-1.0f),2) * SECOND_ORDER_CORRECTION	+
					pow((frameWidth-1.0f),3) * THIRD_ORDER_CORRECTION	+
					pow((frameWidth-1.0f),4) * FOURTH_ORDER_CORRECTION;
// END OF DEFINING CUDA VARIABLES

	//Define the filename of the file to be used for acquisition simulation
	char *fileName = new char[100];
//	fileName = "c:\\OCTViewer\\SDD\\fullvolume"; 
	dispVal = 2.3f * pow(0.1f,5);
//fileName = "F:\\RightEye(042213)\\WT_Scratchback_PV_R_4-ON---1.unp"; dispVal = 2.3f * pow(0.1f,5);
//fileName = "F:\\SS-OCT\\SS-OCT\\For Robert\\ssdata.dat";
	fileName = "f:\\Jing\\SPIE\\mouse data\\800middle-ON---1";	
	//fileName = "G:\\randow\\09192013\\2500-ON---1.unp";
		//  fileName = "g:\\randow\\SVTest3-ON---1.unp";
//fileName = "c:\\OCTViewer\\SDD\\fullvolume"; dispVal = 2.3f * pow(0.1f,5);


	FILE *file = fopen(fileName, "rb");
	if (file==NULL)
	{	printf("Unable to Open file. Exiting Program...\n"); exit(1);}
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file)/(int)sizeof(unsigned short);
	fclose(file);


	if (fileRead) {
		bufferLen = fileLen;
		volumeFrames = fileLen / (frameWidth*frameHeight);
	} 
	else if (volumeRender || fundusRender) {
		bufferLen = frameWidth*frameHeight*volumeFrames;
	} else {
		bufferLen = frameWidth*frameHeight*framesPerBuffer;
	}

	for (int i=0; i<BUFFNUM; i++) {
		h_buffer[i].data = (unsigned short *)malloc(bufferLen * sizeof(unsigned short));
		h_buffer[i].regHost = false;
	}


//Initiate FileRead Thread
	FileReadThread FileRead;
	FileRead.create();
	FileRead.InitFileRead(fileName, h_buffer, &buffCtr, &bufferLen);

//Initiate the Process Thread
	ProcessThread Proc;
	Proc.create();
	Proc.InitProcess(	h_buffer, &buffCtr, processData, volumeRender, fundusRender,
						frameWidth, frameHeight, framesPerBuffer,volumeFrames, bufferLen, 
						windowWidth, windowHeight, samplingMethod, (int)volumeMode, fftLenMult,
						LAMBDA_MIN, lambda_Max, dispMag, dispVal, dispValThird);
//END OF INITIALIZATION
//Begin Process Thread
	FileRead.start();
	Proc.start();
	FileRead.join();
	Proc.join();

	return 0;
}


