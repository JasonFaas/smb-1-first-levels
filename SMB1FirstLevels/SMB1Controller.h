#pragma once
;
#include "..\..\..\..\GenericFunctions\XIMCore.h"
;
#include "..\..\..\..\GenericFunctions\XIMExecute.h"
;
#include "SMB1Controller.h"
;


class SMB1Controller
{
public:
	SMB1Controller();
	~SMB1Controller();

	bool PauseGame();
	bool HoldRight();
	bool JumpAndRight(int jumpTimeMS);
	bool JumpOnToPipe();
	bool JumpOverBlueHole(int jumpTimeMS);
	bool StartGame();
	bool WaitAndLeft();
	bool SpeedJump();
	bool ShortJumpStop();


private:

	XIMXbox360Input ** _ximInput;
	int * _actionTimes;
	int _maxActions;
	XIMExecute * _ximExe;
	//_ximExe->XIMExe_CONST_Right
	int JumpRightAndHoldAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart, int jumpTimeMS);
	int HoldDirectionAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart ,int direction, int timeMS = 100);
	int JumpOnToPipeAndHoldAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart);
	int SpeedJumpAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart);	
	int StandStillAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart, int timeMS);
	int HoldReleaseButtonAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStar, int button);
	int ShortHopAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart);
}
