
#pragma once

#include "cv.h"
#include <cxcore.h>
#include "highgui.h"
;
#include "..\..\..\..\GenericFunctions\StartVideo.h"
;
#include "..\..\..\..\GenericFunctions\FaasStringFunctions.h"
;
#include "SMB1Display.h"
;
#include "SMB1Controller.h"
;



class SMB1Central
{

public:
	SMB1Central(int vidNumber, char * fileName, int frames = -1);
	~SMB1Central();

	CvRect NoThreadVideo();

	void StartSMB1();
	void StopSMB1();

	void ModifyThresh(bool upDown);



private:
	int _vidNumber;
	bool _exitThread;
	bool _printDebug;
	SMB1Display * _smb1Display;
	SMB1Controller * _smb1C;
	StartVideo * _svMain;
	StartVideo * _svProcessImg;
	StartVideo * _svExtraDisp;
	IplImage * _img2;
	IplImage * _img3;
	IplImage * _imgExtraDisplay;
	IplImage * _imgProcessImg;

	void ImagesAndActions();

	void ExtraDisplay(int valInt,IplImage * img);
	int DoSomethingBasedOnOneFrame(IplImage * img);

	bool IsMarioToCloseToGoomba();

}