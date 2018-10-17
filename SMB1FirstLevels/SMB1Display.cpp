
#pragma once

#include "stdafx.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
using namespace std;

#include "cv.h"
#include <cxcore.h>
#include "highgui.h"


#include "SMB1Display.h"
;


SMB1Display::SMB1Display(IplImage * img)
{
	_marioLocale = cvRect(-1,-1,10,10);
	_printDebug = false;

	_threshVal = 200;
	_threshMult = 10;

	_gamePauseVal = 0.0;
	_gamePauseCnt = 0;

	_ctMario0 = new ColorTheory();
	_ctMario1 = new ColorTheory();
	_ctMario2 = new ColorTheory();

	_ctGoomba0 = new ColorTheory();
	_ctGoomba1 = new ColorTheory();
	_ctGoomba2 = new ColorTheory();

	_ctKoopa0 = new ColorTheory();

	_ctOneDim = new ColorTheory();


	_marioImg = cvCloneImage(img);
	_goombaImg = cvCloneImage(img);
	_koopaImg = cvCloneImage(img);
	_greenPipeImg = cvCloneImage(img);
	_blueHoleImg = cvCloneImage(img);
	_startScreenImg = cvCloneImage(img);
	_gamePausedImg = cvCloneImage(img);
	_waitGameImg = cvCloneImage(img);
	_tempImg = cvCloneImage(img);


	_goombaListPos = new CPtrList();
	if(!_goombaListPos->IsEmpty())
	{
		TRACE("GoombaListPos should be empty\n");
		exit(0);
	}

	ColorTheory * once;
	// Setup GoombaHist
	IplImage * enemyGoombaImg = cvLoadImage("smb_enemies_sheet.png");		
	cvSetImageROI(enemyGoombaImg,cvRect(0,3,16-0,21-3));
	once = new ColorTheory();
	_goombaHist = once->FaasHSNoVHistogram(enemyGoombaImg);

	*(cvGetHistValue_2D(_goombaHist,10,24)) = 0;
	cvNormalizeHist(_goombaHist,1.0);
	_goombaMat = SigFor2dEMD(_goombaHist);

	// Setup KoopaHist
	IplImage * enemyKoopaImg = cvLoadImage("smb_enemies_sheet.png");		
	cvSetImageROI(enemyKoopaImg,cvRect(149,0,166-149,25-0));
	once = new ColorTheory();
	_koopaHist = once->FaasHSNoVHistogram(enemyKoopaImg);

	*(cvGetHistValue_2D(_koopaHist,10,24)) = 0;	
	*(cvGetHistValue_2D(_koopaHist,6,24)) = 0;	//Taking shell out of place
	cvNormalizeHist(_koopaHist,1.0);
	_koopaMat = SigFor2dEMD(_koopaHist);


	//Setup MarioHist
	IplImage * marioImg = cvLoadImage("smb_mario_sheet.png");
	//cvSetImageROI(marioImg,cvRect(210,0,225-210,17-0));  standing mario
	cvSetImageROI(marioImg,cvRect(299,0,316-299,17-0)); //running motion mario...need to set threshold :(
	once = new ColorTheory();
	_marioHist = once->FaasHSNoVHistogram(marioImg);

	*(cvGetHistValue_2D(_marioHist,10,24)) = 0;
	cvNormalizeHist(_marioHist,1.0);
	_marioMat = SigFor2dEMD(_marioHist);


	//TODO: delete once;

	cvReleaseImage(&marioImg);
	cvReleaseImage(&enemyGoombaImg);
	cvReleaseImage(&enemyKoopaImg);
	//End setup
}

SMB1Display::~SMB1Display()
{
	delete _ctMario0;
	delete _ctMario1;
	delete _ctMario2;

	delete _ctGoomba0;
	delete _ctGoomba1;
	delete _ctGoomba2;

	delete _ctKoopa0;

	delete _ctOneDim;

	delete _goombaListPos;


	cvReleaseImage(&_marioImg);
	cvReleaseImage(&_goombaImg);
	cvReleaseImage(&_greenPipeImg);
	cvReleaseImage(&_blueHoleImg);
	cvReleaseImage(&_startScreenImg);
	cvReleaseImage(&_gamePausedImg);
	cvReleaseImage(&_waitGameImg);
	cvReleaseImage(&_tempImg);
	cvReleaseImage(&_koopaImg);
}


int
SMB1Display::ProcessImage(IplImage * img)
{	
	SetItemLocations(img);
	return 0;
}

CvMat *
SMB1Display::SigFor2dEMD(CvHistogram * hist, CvMat * sigFill)
{
	CvMat * returnMat;
	int numrows = 450;//18*25;
	if(sigFill == NULL)
	{
		returnMat = cvCreateMat(numrows,3,CV_32FC1);
	}
	else
		returnMat = sigFill;


	int h_bins = 18,s_bins = 25;

	for(int h = 0; h<h_bins;h++)
	{
		for(int s = 0; s<s_bins;s++)
		{
			float bin_val = cvQueryHistValue_2D(hist,h,s);
			cvSet2D(returnMat,h*s_bins+s,0,cvScalar(bin_val));
			cvSet2D(returnMat,h*s_bins+s,1,cvScalar(h));
			cvSet2D(returnMat,h*s_bins+s,2,cvScalar(s));
		}
	}

	return returnMat;
}



int 
SMB1Display::SetItemLocations(IplImage * img)
{
	//Set Value < 100 || Saturation < 100 to CV_RGB(0,0,0)

	cvCopyImage(img,_tempImg);

	cvCopyImage(img,_waitGameImg);
	cvCopyImage(img,_gamePausedImg);
	cvCopyImage(img,_startScreenImg);
	cvCopyImage(img,_marioImg);
	cvCopyImage(img,_goombaImg);
	cvCopyImage(img,_koopaImg);
	cvCopyImage(img,_greenPipeImg);
	cvCopyImage(img,_blueHoleImg);

	_jumpGoomba = false;
	_nextToGreenPipe = false;
	_jumpBlueHole = false;
	_jumpKoopaTroopa = false;
	_startScreen = false;
	_gamePaused = false;
	_waitGame = false;
	_marioFound = false;
	_stairClimb = false;

	_marioLocalePrev.x = _marioLocale.x;
	_marioLocalePrev.y = _marioLocale.y;
	_marioLocalePrev.width = _marioLocale.width;
	_marioLocalePrev.height = _marioLocale.height;

	_marioLocale.x = -1;
	_blueHoleLocation.x = -1;


	if(_printDebug)
		TRACE("\tAbout to check for wait game screen\n");
	_waitGame = CheckForWaitGameScreen(_waitGameImg);
	if(_waitGame)
		return 1;


	if(_printDebug)
		TRACE("\tAbout to look at Start Screen\n");
	_startScreen = AtStartScreen(_startScreenImg);
	if(_startScreen)
		return 1;

	_gamePaused = CheckGamePaused(_gamePausedImg);
	if(_gamePaused)
		return 1;

	if(_printDebug)
		TRACE("\tAbout to look for Mario\n");
	_marioFound = LookForMario(_marioImg);
	//TODO: Below is terrible
	if(!_marioFound && _marioLocale.x!=-1)//This is him floating in air
	{
		_stairClimb = CheckForStairClimb(NULL);
		if(_stairClimb)
			return 1;
	}
	if(!_marioFound)//Just can't find him
		return 1;

	_nextToGreenPipe = NextToGreenPipe(img);
	if(_nextToGreenPipe)
		return 1;

	_jumpGoomba = LookForGoombas(_goombaImg);
	if(_jumpGoomba)
		return 1;

	_jumpBlueHole = ApproachingBlueHole(_blueHoleImg);
	if(_jumpBlueHole)
		return 1;

	_jumpKoopaTroopa = LookForKoopas(_koopaImg);
	if(_jumpKoopaTroopa)
		return 1;

	if(_printDebug)
		TRACE("Complete Look up\n");


	return 1;
}

bool
SMB1Display::CheckForStairClimb(IplImage * img)
{
	return false;
}
bool
SMB1Display::CheckForWaitGameScreen(IplImage * img)
{


	cvCopyImage(img,_tempImg);
	cvSetImageROI(img,cvRect(100,300,600-100,400-300));
	cvSetImageROI(_tempImg,cvRect(100,300,600-100,400-300));

	cvCvtColor(img,_tempImg,CV_BGR2HSV);
	IplImage * hue = cvCreateImage(cvGetSize(img),8,1);
	cvSplit(_tempImg,hue,NULL,NULL,NULL);

	CvHistogram * _tempImgHist = _ctOneDim->FaasOneArrTenBinHue(hue);

	cvNormalizeHist(_tempImgHist,1.0);

	cvResetImageROI(img);
	cvResetImageROI(_tempImg);

	float gamePauseTemp = *(cvGetHistValue_1D(_tempImgHist,0));

	if(gamePauseTemp > .9)
		return true;


	return false;
}

bool
SMB1Display::CheckGamePaused(IplImage * img)
{


	cvCopyImage(img,_tempImg);
	cvSetImageROI(img,cvRect(610,62,615-610,66-62));
	cvSetImageROI(_tempImg,cvRect(610,62,615-610,66-62));

	cvCvtColor(img,_tempImg,CV_BGR2HSV);
	IplImage * hue = cvCreateImage(cvGetSize(img),8,1);
	cvSplit(_tempImg,hue,NULL,NULL,NULL);

	CvHistogram * _tempImgHist = _ctOneDim->FaasOneArrTenBinHue(hue);

	cvNormalizeHist(_tempImgHist,1.0);

	cvResetImageROI(img);
	cvResetImageROI(_tempImg);

	float gamePauseTemp = *(cvGetHistValue_1D(_tempImgHist,0));

	if(gamePauseTemp * 1.1 >= _gamePauseVal && gamePauseTemp * .9 <= _gamePauseVal)
		_gamePauseCnt++;
	else
	{
		_gamePauseVal = gamePauseTemp;
		_gamePauseCnt = 0;
	}

	if(_printDebug)
		TRACE("GameCount:%i:\n",_gamePauseCnt);
	if(_gamePauseCnt > 10)
	{
		_gamePauseCnt = 0;
		return true;
	}

	return false;
}

bool
SMB1Display::AtStartScreen(IplImage * img)
{


	cvCopyImage(img,_tempImg);
	cvSetImageROI(img,cvRect(150,80,575-150,225-80));
	cvSetImageROI(_tempImg,cvRect(150,80,575-150,225-80));

	cvCvtColor(img,_tempImg,CV_BGR2HSV);
	IplImage * hue = cvCreateImage(cvGetSize(img),8,1);
	cvSplit(_tempImg,hue,NULL,NULL,NULL);

	CvHistogram * _tempImgHist = _ctOneDim->FaasOneArrTenBinHue(hue);

	cvNormalizeHist(_tempImgHist,1.0);

	cvRectangle(img,cvPoint(0,0),cvPoint(100,100),CV_RGB(0,0,255),CV_FILLED);

	cvResetImageROI(img);
	cvResetImageROI(_tempImg);

	//TRACE("Hmm:%.3f:%.3f\n",*(cvGetHistValue_1D(_tempImgHist,2)),*(cvGetHistValue_1D(_tempImgHist,3)));

	if(*(cvGetHistValue_1D(_tempImgHist,0)) > .9)
		return true;	

	return false;
}

bool
SMB1Display::NextToGreenPipe(IplImage * img)
{
	if(_printDebug || true)
		TRACE("Starting Next To Green Pipe check\n");
	if(_marioLocale.x == -1)
		return false;

	int greenPipeLook = 100;

	int fullWidth = 30;
	int fullHeight = 55;

	cvCopyImage(img,_tempImg);
	cvSetImageROI(_tempImg,cvRect(_marioLocale.x+_marioLocale.width+25,_marioLocale.y-_marioLocale.height,fullWidth,fullHeight));

	for(int iXStart = _marioLocale.x+_marioLocale.width;iXStart< 650 && iXStart<_marioLocale.x+_marioLocale.width+greenPipeLook;iXStart+=fullWidth)
	{
		cvSetImageROI(img,cvRect(iXStart,_marioLocale.y-_marioLocale.height,fullWidth,fullHeight));

		cvCvtColor(img,_tempImg,CV_BGR2HSV);
		IplImage * hue = cvCreateImage(cvGetSize(img),8,1);
		cvSplit(_tempImg,hue,NULL,NULL,NULL);

		CvHistogram * _tempImgHist = _ctOneDim->FaasOneArrTenBinHue(hue);

		cvNormalizeHist(_tempImgHist,1.0);

		if(_printDebug || true)
			TRACE("Hmm:%.3f:%.3f\n",*(cvGetHistValue_1D(_tempImgHist,2)),*(cvGetHistValue_1D(_tempImgHist,3)));

		cvRectangle(img,cvPoint(0,0),cvPoint(fullWidth*.5,fullHeight*.5),CV_RGB(100,100,100),2);
		

		if(*(cvGetHistValue_1D(_tempImgHist,2)) > .3 && *(cvGetHistValue_1D(_tempImgHist,3)) > .3)
		{
			cvRectangle(img,cvPoint(0,0),cvPoint(fullWidth*.5,fullHeight*.5),CV_RGB(255,255,0),2);
			cvResetImageROI(_tempImg);
			cvResetImageROI(img);
			return true;	
		}
		cvResetImageROI(img);
	}
	cvResetImageROI(_tempImg);


	return false;
}

bool
SMB1Display::ApproachingBlueHole(IplImage * img)
{
	if(_marioLocale.x == -1)
		return false;

	_blueHoleLocation.y = 420;
	_blueHoleLocation.width = 20;
	_blueHoleLocation.height = 10;

	cvCopyImage(img,_tempImg);	

	int xInterval = 30;

	cvSetImageROI(_tempImg,cvRect(0,0,20,10));
	//TODO: This is a big waste
	IplImage * hue = cvCreateImage(cvGetSize(_tempImg),8,1);


	for(int iXStart = _marioLocale.x+_marioLocale.width;iXStart < 650 && iXStart < _marioLocale.x+_marioLocale.width + _marioEndLook;iXStart+=xInterval)
	{
		cvSetImageROI(img,cvRect(iXStart,420,20,10));		

		cvCvtColor(img,_tempImg,CV_BGR2HSV);		
		cvSplit(_tempImg,hue,NULL,NULL,NULL);

		CvHistogram * _tempImgHist = _ctOneDim->FaasOneArrTenBinHue(hue);
		cvNormalizeHist(_tempImgHist,1.0);

		//TODO: reset this to make since
		//cvRectangle(img,cvPoint(0,0),cvPoint(40,55),CV_RGB(100,100,100),8);

		if(_printDebug)
			TRACE("%.3f:\n",*(cvGetHistValue_1D(_tempImgHist,6)));

		if(*(cvGetHistValue_1D(_tempImgHist,6)) > .7)
		{
			_blueHoleLocation.x = iXStart;

			cvResetImageROI(img);
			cvResetImageROI(_tempImg);
			cvReleaseImage(&hue);

			return true;
		}

		cvResetImageROI(img);
	}
	cvResetImageROI(_tempImg);
	cvReleaseImage(&hue);		

	return false;
}

bool
SMB1Display::LookForKoopas(IplImage * img)
{
	if(_printDebug)
		TRACE("Start of look for Koopas\n");
	int koopaStartLook = 50;
	int koopaEndLook = 225;

	CvPoint startPoint = cvPoint(_marioLocale.x+_marioLocale.width + koopaStartLook,370);
	int xBoxes = 100;

	int halfWidth = 20;
	int koopaLocale = -1;
	double tempMatCmp = -1;
	double matCmp = -3;
	CvHistogram * tempHist;
	CvMat * tempMat = NULL;


	//BlackOutUsingLaplace(img);


	int minArr[3] = {-1,181,-1};
	int maxArr[3] = {100,-1,256};

	minArr[0] = 30;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 256;maxArr[2] = 256;
	_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	minArr[0] = -1;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 256;maxArr[2] = 50;
	_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	//Or low saturation(White)
	//minArr[0] = -1;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 50;maxArr[2] = 256;
	//_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);


	for(int iXStart = startPoint.x+halfWidth;iXStart < 650 && iXStart < startPoint.x + _marioEndLook;iXStart+=halfWidth)
	{
		cvSetImageROI(img,cvRect(iXStart,startPoint.y,halfWidth*2,30));

		tempHist = _ctKoopa0->FaasHSNoVHistogram(img);

		cvNormalizeHist(tempHist,1.0);
		tempMat = SigFor2dEMD(tempHist,tempMat);
		tempMatCmp = cvCalcEMD2(tempMat,_koopaMat,CV_DIST_L1);
		if(_printDebug)		
			TRACE("\t%.2f\n",tempMatCmp);
		if(_printDebug)
			TRACE("\t\t%i\n",cvRound(255.0*tempMatCmp));
		int colour = cvRound(255.0*tempMatCmp)/22.0;
		if(tempMatCmp<_threshVal)
			cvRectangle(img,cvPoint(0,0),cvPoint(halfWidth-10,30),CV_RGB(colour,colour,colour),8);
		if(tempMatCmp < 19.0 && tempMatCmp > 17.5)
		{		
			//TODO: Stop utilizing same PtrList for thing that can only be 1 anyway
			_goombaListPos->AddTail(new CvRect(cvRect(iXStart,startPoint.y,halfWidth*2,30)));

			cvResetImageROI(img);
			return true;
		}
		cvResetImageROI(img);
	}

	return false;
}
bool
SMB1Display::LookForGoombas(IplImage * img)
{
	int goombaStartLook = 50;
	int goombaEndLook = 225;

	CvPoint startPoint = cvPoint(_marioLocale.x+_marioLocale.width + goombaStartLook,382);
	int xBoxes = 100;

	int halfWidth = 15;
	int goombaLocale = -1;
	double tempMatCmp = -1;
	double matCmp = -3;
	CvHistogram * tempHist;
	CvMat * tempMat = NULL;



	int minArr[3] = {-1,181,-1};
	int maxArr[3] = {100,-1,256};


	minArr[0] = 20;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 256;maxArr[2] = 256;
	_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	//TODO: This part is probably detrimentatl to differentiatting mario and goomba
	minArr[0] = -1;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 256;maxArr[2] = 50;
	_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	//TODO: This too
	minArr[0] = -1;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 50;maxArr[2] = 256;
	_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	int addCheck = 4;



	for(int iXStart = startPoint.x-halfWidth*5;iXStart < 650 && iXStart < startPoint.x + _marioEndLook;iXStart+=halfWidth)
	{
		if(iXStart == startPoint.x-halfWidth)
			iXStart+=halfWidth*2;

		cvSetImageROI(img,cvRect(iXStart,startPoint.y,halfWidth*2,30));

		addCheck++;

		tempHist = _ctGoomba0->FaasHSNoVHistogram(img);

		cvNormalizeHist(tempHist,1.0);
		tempMat = SigFor2dEMD(tempHist,tempMat);
		tempMatCmp = cvCalcEMD2(tempMat,_goombaMat,CV_DIST_L1);
		if(_printDebug)
			TRACE("\t%.2f\n",tempMatCmp);
		if(_printDebug)
			TRACE("\t\t%i\n",cvRound(255.0*tempMatCmp));
		int colour = cvRound(255.0*tempMatCmp)/22.0;
		if(tempMatCmp<_threshVal)
			cvRectangle(img,cvPoint(0,0),cvPoint(halfWidth-10,30),CV_RGB(colour,colour,colour),8);
		if(tempMatCmp < 10.5)
		{		
			_goombaListPos->AddTail(new CvRect(cvRect(iXStart,startPoint.y,halfWidth*2,30)));

			if(addCheck>3)
				addCheck = 0;

		}
		cvResetImageROI(img);
	}
	if(_goombaListPos->GetCount()>0)
		return true;

	return false;
}
bool
SMB1Display::LookForMario(IplImage * img)
{
	if(_printDebug)
		TRACE("Start Look for Mario\n");

	CvPoint startPoint = cvPoint(130,382);

	int halfWidth = 15;
	int fullHeight = 30;
	int marioLocale = -1;
	double tempMatCmp = -1;
	double matCmp = -3;
	CvHistogram * tempHist;
	CvMat * tempMat = NULL;

	_marioLocale.x = -1;
	_marioLocale.y = -1;
	_marioLocale.width = halfWidth*2;
	_marioLocale.height = fullHeight;

	int minArr[3] = {-1,181,-1};
	int maxArr[3] = {100,-1,256};


	//This is done in 2 places
	minArr[0] = 20;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 256;maxArr[2] = 256;
	_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	//TODO: Delete low value if not used
	//minArr[0] = -1;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 256;maxArr[2] = 50;
	//_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	//TODO: also high sat
	//minArr[0] = -1;minArr[1] = -1;minArr[2] = -1;maxArr[0] = 181;maxArr[1] = 50;maxArr[2] = 256;
	//_ctMario0->SetPixelsToColor(img,_ctOneDim->CONST_SET_MAGENTA,false,minArr,maxArr);

	cvSetImageROI(img,cvRect(startPoint.x+halfWidth*-1,startPoint.y,halfWidth*2,30));
	cvRectangle(img,cvPoint(0,0),cvPoint(halfWidth-10,30),CV_RGB(0,0,255),2);
	cvResetImageROI(img);

	int xBoxes = 25;
	if(_marioLocalePrev.y == startPoint.y && _marioLocalePrev.x!=-1)
	{
		xBoxes = 8;
		startPoint.x = _marioLocalePrev.x;		
	}

	for(int i = -2;i<xBoxes;i++)
	{
		cvSetImageROI(img,cvRect(startPoint.x+halfWidth*i,startPoint.y,halfWidth*2,30));

		tempHist = _ctMario1->FaasHSNoVHistogram(img);

		cvNormalizeHist(tempHist,1.0);
		tempMat = SigFor2dEMD(tempHist,tempMat);
		tempMatCmp = cvCalcEMD2(tempMat,_marioMat,CV_DIST_L1);

		if(_printDebug)
			TRACE("\t%.2f\n",tempMatCmp);
		if(_printDebug)
			TRACE("\t\t%i\n",cvRound(255.0*tempMatCmp));
		int colour = cvRound(255.0*tempMatCmp)/22.0;
		if(tempMatCmp<_threshVal)
			cvRectangle(img,cvPoint(0,0),cvPoint(halfWidth-10,30),CV_RGB(colour,colour,colour),2);
		if((tempMatCmp < matCmp  || matCmp < -2) && tempMatCmp < 9.5)
		{
			_marioLocale.x = startPoint.x+halfWidth*i;
			_marioLocale.y = startPoint.y;
			matCmp = tempMatCmp;
		}
		cvResetImageROI(img);
	}


	if(_marioLocale.x == -1)
	{
		//check if mario is in air somewhere
		CvPoint startPoint = cvPoint(330,350);//x is expected, y is one level up


		int yBoxes = 11;

		marioLocale = -1;
		tempMatCmp = -1;
		matCmp = -3;

		cvSetImageROI(img,cvRect(startPoint.x-15*2,startPoint.y+30,30,fullHeight));
		cvRectangle(img,cvPoint(0,0),cvPoint(30,30),CV_RGB(0,0,255),8);
		cvResetImageROI(img);


		//TODO: Optimize before this point, look with Laplace.
		int xBoxes = 5;
		int xInterval = 15;
		for(int g = 0;g<xBoxes;g++)
		{
			for(int i = yBoxes*-1+1;i<=0;i++)
			{
				cvSetImageROI(img,cvRect(startPoint.x+xInterval*g,startPoint.y+fullHeight*i,30,fullHeight));

				tempHist = _ctMario1->FaasHSNoVHistogram(img);

				cvNormalizeHist(tempHist,1.0);
				tempMat = SigFor2dEMD(tempHist,tempMat);
				tempMatCmp = cvCalcEMD2(tempMat,_marioMat,CV_DIST_L1);

				if(_printDebug)
					TRACE("\t%.2f\n",tempMatCmp);
				if(_printDebug)
					TRACE("\t\t%i\n",cvRound(255.0*tempMatCmp));
				int colour = cvRound(255.0*tempMatCmp)/22.0;
				if(tempMatCmp<_threshVal)
					cvRectangle(img,cvPoint(0,0),cvPoint(10,20),CV_RGB(colour,colour,colour),2);
				if((tempMatCmp < matCmp  || matCmp < -2) && tempMatCmp < 9.5)
				{
					_marioLocale.x = startPoint.x+xInterval*g;
					_marioLocale.y = startPoint.y+fullHeight*i;
					matCmp = tempMatCmp;
				}
				cvResetImageROI(img);
			}
		}
		cvSetImageROI(img,cvRect(startPoint.x+xInterval*xBoxes,startPoint.y+fullHeight*yBoxes*-1,30,30));
		cvRectangle(img,cvPoint(0,0),cvPoint(30,30),CV_RGB(0,0,255),8);
		cvResetImageROI(img);
	}

	if(_marioLocale.x == -1)
		return false;

	else
		return true;
}




CvRect
SMB1Display::GetMarioLocation()
{
	return _marioLocale;
}


CvRect
SMB1Display::GetBlueHoleLocation()
{
	return _blueHoleLocation;
}	
CPtrList *
SMB1Display::GetGoombaLocation()
{		
	return _goombaListPos;
}



bool
SMB1Display::IsNextToGreenPipe()
{
	return _nextToGreenPipe;
}
bool
SMB1Display::IsNearGoomba()
{
	return _jumpGoomba;
}
bool
SMB1Display::IsNearBlueHole()
{
	return _jumpBlueHole;
}
bool
SMB1Display::IsNearKoopaTroopa()
{
	return _jumpKoopaTroopa;
}
bool
SMB1Display::IsAtStartScreen()
{
	return _startScreen;
}
bool
SMB1Display::IsGamePaused()
{
	return _gamePaused;
}
bool
SMB1Display::IsAtWaitGameScreen()
{
	return _waitGame;
}
bool
SMB1Display::IsAtStairClimb()
{
	return _stairClimb;
}
void
SMB1Display::ThreshValModify(bool upDown)
{
	int posNeg = -1;
	if(upDown)
		posNeg = 1;

	_threshVal += (posNeg*_threshMult);

	TRACE("Thresh Val: %i\n",_threshVal);
}



void
SMB1Display::BlackOutUsingLaplace(IplImage * img)
{
	TRACE("Enter Blackout\n");
	//cvCopyImage(img,_temp);

	IplImage * hueImg = cvCreateImage(cvGetSize(img),8,1);	
	IplImage * satImg = cvCreateImage(cvGetSize(img),8,1);	
	IplImage * valImg = cvCreateImage(cvGetSize(img),8,1);	
	IplImage * tempFree = cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_16S,1);
	IplImage * conScaleImg = cvCreateImage(cvSize(img->width,img->height),8,1);
	IplImage * threshImg = cvCreateImage(cvSize(img->width,img->height),8,1);


	cvCvtColor(img,_tempImg,CV_BGR2HSV);
	cvSplit(_tempImg,hueImg,satImg,valImg,NULL);
	TRACE("Before Complex\n");
	cvLaplace(hueImg,tempFree);
	TRACE("\t1\n");
	cvConvertScaleAbs(tempFree,conScaleImg);
	TRACE("\t2\n");
	cvThreshold(conScaleImg,threshImg,35,256,CV_THRESH_BINARY);
	TRACE("\t3\n");
	cvMin(threshImg,valImg,conScaleImg);
	TRACE("\t4\n");
	cvMerge(hueImg,satImg,conScaleImg,NULL,_tempImg);

	cvCvtColor(_tempImg,img,CV_HSV2BGR);

	TRACE("Exit Blackout\n");
}