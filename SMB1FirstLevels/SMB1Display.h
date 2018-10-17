#pragma once

#include "cv.h"
#include <cxcore.h>
#include "highgui.h"
;
#include "..\..\..\..\GenericFunctions\FaasMySQLFunctions.h"
;
#include "..\..\..\..\GenericFunctions\ColorTheory.h"
;

class SMB1Display{

public:
	bool _exitThread;

	SMB1Display(IplImage * img);
	~SMB1Display(); 

	int ProcessImage(IplImage * img);

	CvRect GetMarioLocation();
	CvRect GetBlueHoleLocation();
	CPtrList * GetGoombaLocation();

	bool IsNextToGreenPipe();
	bool IsNearGoomba();
	bool IsNearKoopaTroopa();
	bool IsNearBlueHole();
	bool IsAtStartScreen();
	bool IsGamePaused();
	bool IsAtWaitGameScreen();
	bool IsAtStairClimb();

	

	bool DetectColision(IplImage* img);

	void ThreshValModify(bool upDown);

private:


	CvRect _marioLocale;
	CvRect _marioLocalePrev;
	IplImage * _marioImg;
	IplImage * _goombaImg;
	IplImage * _koopaImg;
	IplImage * _greenPipeImg;
	IplImage * _blueHoleImg;
	IplImage * _startScreenImg;
	IplImage * _gamePausedImg;
	IplImage * _waitGameImg;

	IplImage * _tempImg;

	CvHistogram * _goombaHist;
	CvHistogram * _koopaHist;
	CvHistogram * _marioHist;

	static const int _marioEndLook = 225;

	bool _printDebug;
	int _threshVal;
	int _threshMult;
	CvMat * _goombaMat;
	CvMat * _koopaMat;
	CvMat * _marioMat;
	bool _nextToGreenPipe;
	bool _jumpGoomba;
	bool _jumpKoopaTroopa;
	bool _jumpBlueHole;
	bool _startScreen;
	bool _gamePaused;
	bool _waitGame;
	bool _stairClimb;
	bool _marioFound;

	float _gamePauseVal;
	int _gamePauseCnt;

	CPtrList * _goombaListPos;
	CvRect _blueHoleLocation;

	ColorTheory * _ctMario0;
	ColorTheory * _ctMario1;	
	ColorTheory * _ctMario2;

	ColorTheory * _ctGoomba0;
	ColorTheory * _ctGoomba1;	
	ColorTheory * _ctGoomba2;

	ColorTheory * _ctKoopa0;

	ColorTheory * _ctOneDim;	


	int SetItemLocations(IplImage * img);
	bool LookForMario(IplImage * img);
	bool LookForGoombas(IplImage * img);
	bool LookForKoopas(IplImage * img);
	bool NextToGreenPipe(IplImage * img);	
	bool ApproachingBlueHole(IplImage * img);
	bool AtStartScreen(IplImage * img);
	bool CheckGamePaused(IplImage * img);
	bool CheckForWaitGameScreen(IplImage * img);
	bool CheckForStairClimb(IplImage * img);


	
	void BlackOutUsingLaplace(IplImage * img);

	CvPoint WhichOfNine(IplImage * img, int scale, CvPoint start);
	//CvPoint WhichOfThree(IplImage * img, int scale, CvPoint start, bool horizontalVertical, CvHistogram * hist = NULL);



	CvMat * SigFor2dEMD(CvHistogram * hist, CvMat * sigArr = NULL);

}