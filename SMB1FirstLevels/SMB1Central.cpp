
#include "stdafx.h"

#include "SMB1Central.h"
;

SMB1Central::SMB1Central(int vidNumber, char * fileName, int frames)
{
	_vidNumber = vidNumber;

	_printDebug = false;

	_svMain = new StartVideo(_vidNumber,fileName,frames);
	_svProcessImg = new StartVideo(_vidNumber,"CV View");
	_svExtraDisp = new StartVideo(_vidNumber,"SMB1_1-1.avi",7);
	_img2 = NULL;
	_img3 = NULL;
	_smb1Display = NULL;
	_imgExtraDisplay = NULL;
	_imgProcessImg = NULL;

	_exitThread = false;
}

SMB1Central::~SMB1Central()
{	
	if(_img2!=NULL)
		cvReleaseImage(&_img2);
	if(_img3!=NULL)
		cvReleaseImage(&_img3);
	if(_imgExtraDisplay!=NULL)
		cvReleaseImage(&_imgExtraDisplay);
	if(_imgProcessImg!=NULL)
		cvReleaseImage(&_imgProcessImg);

	if(_smb1Display!=NULL)
		delete _smb1Display;

	delete _svExtraDisp;
	delete _svMain;
	delete _svProcessImg;
}

CvRect
SMB1Central::NoThreadVideo()
{
	CvRect rectRet;
	rectRet.x = 0;
	rectRet.y = 0;
	IplImage* frame;
	do
	{
		frame = _svMain->GetIplImage();
	}
	while(!_svMain->DisplayIplImage(frame));

	rectRet.width = frame->width;
	rectRet.height = frame->height;

	return rectRet;
}

void
SMB1Central::StopSMB1()
{
	_exitThread = true;
}

void
SMB1Central::StartSMB1()
{		
	//TODO: Memory Leak about here, don't know why
	//Is a 16 byte block, all zeros.
	IplImage * releaseAtSMB1DisplayDelete = cvCloneImage(_svMain->GetIplImage());
	_smb1Display = new SMB1Display(releaseAtSMB1DisplayDelete);
	cvReleaseImage(&releaseAtSMB1DisplayDelete);

	_smb1C = new SMB1Controller();
	TRACE("Got through Initilization\n");

	ImagesAndActions();

	delete _smb1Display;
	_smb1Display = NULL;
	delete _smb1C;
	_smb1C = NULL;
}

void
SMB1Central::ImagesAndActions()
{
	IplImage* frame;
	int retInt = 0;
	bool escExit = false;

	_img2 = cvCloneImage(_svMain->GetIplImage());
	_img3 = cvCloneImage(_svMain->GetIplImage());

	_imgExtraDisplay = cvCloneImage(_svMain->GetIplImage());

	_imgProcessImg = cvCloneImage(_svMain->GetIplImage());
	_smb1C->PauseGame();

	while(!_smb1C->HoldRight()){}

	do
	{
		//Get Image and Copy for other uses
		frame = _svMain->GetIplImage();
		escExit = _svMain->DisplayIplImage(frame);
		cvCopyImage(frame,_imgProcessImg);
		cvCopyImage(frame,_imgExtraDisplay);

		//Process Image and Display result of process - May uncomment block to display another image before actions and Extra Display
		retInt = _smb1Display->ProcessImage(_imgProcessImg);
		_svProcessImg->DisplayIplImage(_imgProcessImg);
		//frame = _svMain->GetIplImage();
		//_svMain->DisplayIplImage(frame);

		//Do Actions
		if(_printDebug)
			TRACE("About to Do Actions\n");
		retInt = DoSomethingBasedOnOneFrame(frame);

		//Show Image with Computer understanding
		if(_printDebug)
			TRACE("About to go to extra display\n");
		ExtraDisplay(retInt,_imgExtraDisplay);
		_svExtraDisp->DisplayIplImage(_imgExtraDisplay);				
	}
	while(!escExit && !_exitThread && retInt >= 0);

	_smb1C->PauseGame();
}

void
SMB1Central::ExtraDisplay(int valInt,IplImage * img)
{
	if(valInt == 0)
	{
		return;
	}
	else if(valInt == 1)
	{
		TRACE("Shouldn't get here\n");
		exit(0);

		cvSet(_img2,CV_RGB(0,0,0));
		cvSet(_img3,CV_RGB(0,0,0));
		cvSet(_imgExtraDisplay,CV_RGB(0,0,0));

		CvRect rect = _smb1Display->GetMarioLocation();
		cvRectangle(_img3,cvPoint(rect.x+40,rect.y),cvPoint(rect.x+rect.width+20,rect.y+rect.height),CV_RGB(80,0,0),CV_FILLED);
		cvAdd(_img2,_img3,_imgExtraDisplay);


		cvCopyImage(_imgExtraDisplay,_img2);
		cvSetZero(_img3);		
		cvSetZero(_imgExtraDisplay);
		/* Err, as above, shouldn't get here
		rect = _smb1Display->GetGoombaLocation();
		cvRectangle(_img3,cvPoint(rect.x-20,rect.y-20),cvPoint(rect.x+rect.width+40,rect.y+rect.height+20),CV_RGB(0,80,0),CV_FILLED);
		cvAdd(_img2,_img3,_imgExtraDisplay);


		_svProcessImg->DisplayIplImage(_imgExtraDisplay);
		*/		
	}
	else if(valInt == 2)
	{
		CvRect boxRect = cvRect(8,8,50,75);
		//Display Mario Location
		CvRect mRect = _smb1Display->GetMarioLocation();
		if(mRect.x!=-1)
		{
			cvRectangle(img,cvPoint(mRect.x,mRect.y),cvPoint(mRect.x+mRect.width+250,mRect.y+mRect.height),CV_RGB(255,0,255),2);
			cvRectangle(img,cvPoint(mRect.x,mRect.y),cvPoint(mRect.x+mRect.width,mRect.y+mRect.height),CV_RGB(0,0,0),2);
		}
		else
			cvRectangle(img,cvPoint(boxRect.x,boxRect.y),cvPoint(boxRect.width+boxRect.x,boxRect.y+boxRect.height),CV_RGB(255,0,0),8);
		boxRect.x+=boxRect.width*1.1;


		//Check and display green pipe
		if(_smb1Display->IsNextToGreenPipe())
			cvRectangle(img,cvPoint(mRect.x+mRect.width+25,355),cvPoint(mRect.x+mRect.width+25+45,355+55),CV_RGB(255,255,255),10);
		else
			cvRectangle(img,cvPoint(boxRect.x,boxRect.y),cvPoint(boxRect.width+boxRect.x,boxRect.y+boxRect.height),CV_RGB(255,255,0),8);
		boxRect.x+=boxRect.width*1.1;

		//TODO: rewrite for only one possible goomba since currently maxed at one goomba
		//TODO: Better differentiate Koopa and Goomba
		CPtrList * gList = _smb1Display->GetGoombaLocation();
		CvRect * rectG;
		if(gList->GetCount() >0)
		{
			while(gList->GetCount() > 0)
			{
				rectG = (CvRect*)gList->GetHead();
				if(_smb1Display->IsNearKoopaTroopa())
					cvRectangle(img,cvPoint(rectG->x,rectG->y),cvPoint(rectG->x+rectG->width,rectG->y+rectG->height),CV_RGB(0,150,150),6);
				else
					cvRectangle(img,cvPoint(rectG->x,rectG->y),cvPoint(rectG->x+rectG->width,rectG->y+rectG->height),CV_RGB(0,150,0),6);

				free(rectG);			
				gList->RemoveHead();
			}
		}
		else
			cvRectangle(img,cvPoint(boxRect.x,boxRect.y),cvPoint(boxRect.width+boxRect.x,boxRect.y+boxRect.height),CV_RGB(0,255,0),8);	
		boxRect.x+=boxRect.width*1.1;


		//Check and Display Blue Hole
		CvRect blueHole = _smb1Display->GetBlueHoleLocation();
		if(blueHole.x!=-1)
			cvRectangle(img,cvPoint(blueHole.x,blueHole.y),cvPoint(blueHole.x+blueHole.width,blueHole.y+blueHole.height),CV_RGB(255,255,0),10);
		else
			cvRectangle(img,cvPoint(boxRect.x,boxRect.y),cvPoint(boxRect.width+boxRect.x,boxRect.y+boxRect.height),CV_RGB(0,0,255),8);
		boxRect.x+=boxRect.width*1.1;

		//Display If at Start Screen
		if(_smb1Display->IsAtStartScreen())
			cvRectangle(img,cvPoint(150,80),cvPoint(575,225),CV_RGB(255,0,255),10);
		else
			cvRectangle(img,cvPoint(boxRect.x,boxRect.y),cvPoint(boxRect.width+boxRect.x,boxRect.y+boxRect.height),CV_RGB(0,255,255),8);
		boxRect.x+=boxRect.width*1.1;

		//Display If Game Paused
		if(_smb1Display->IsGamePaused())
			cvRectangle(img,cvPoint(350,80),cvPoint(500,225),CV_RGB(200,100,255),10);
		else
			cvRectangle(img,cvPoint(boxRect.x,boxRect.y),cvPoint(boxRect.width+boxRect.x,boxRect.y+boxRect.height),CV_RGB(255,255,255),8);
		boxRect.x+=boxRect.width*1.1;

	}	

}




int
SMB1Central::DoSomethingBasedOnOneFrame(IplImage * img)
{	
	//TODO: Remove all debug statements in this block

	if(_smb1Display->IsAtWaitGameScreen())
		_smb1C->WaitAndLeft();	
	else if(_smb1Display->IsAtStartScreen() || _smb1Display->IsGamePaused())
	{
		while(_smb1C->StartGame()){}
	}
	else if(_smb1Display->IsNextToGreenPipe())	
	{
		_smb1C->JumpOnToPipe();

	}
	else if(_smb1Display->IsNearKoopaTroopa())
		_smb1C->JumpAndRight(400);
		
	else if(_smb1Display->IsNearGoomba())
	{

		if(_smb1Display->GetGoombaLocation()->GetCount() > 1)
			_smb1C->JumpAndRight(400);
		else
			_smb1C->SpeedJump();
	}
	else if(_smb1Display->IsNearBlueHole())
	{
		CvRect marioRect = _smb1Display->GetMarioLocation();
		CvRect blueRect = _smb1Display->GetBlueHoleLocation();
			

		_smb1C->JumpOverBlueHole(blueRect.x-marioRect.x);
	}
	else if(_smb1Display->IsAtStairClimb())
		_smb1C->ShortJumpStop();

	return 2;
}



void 
SMB1Central::ModifyThresh(bool upDown)
{
	if(_smb1Display!=NULL)
		_smb1Display->ThreshValModify(upDown);
}



