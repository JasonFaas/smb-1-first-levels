#pragma once

#include "stdafx.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
using namespace std;
;
#include "..\..\..\..\GenericFunctions\XIMCore.h"
;
#include "SMB1Controller.h"
;

SMB1Controller::SMB1Controller()
{

	_ximExe = new XIMExecute();
	_ximExe->StartWaitForAction();

	_maxActions = 100;
	_actionTimes = (int *)malloc(sizeof(int)*_maxActions);
	_ximInput = (XIMXbox360Input**)malloc(sizeof(XIMXbox360Input*)*_maxActions);	
	for(int i = 0;i<_maxActions;i++)
	{
		*(_actionTimes+i) = 1;
		*(_ximInput+i) = (XIMXbox360Input*)malloc(sizeof(XIMXbox360Input));
		_ximExe->InputDefault(*(_ximInput+i));
	}



}

SMB1Controller::~SMB1Controller()
{
	_ximExe->EndWaitForAction();
	delete _ximExe;

	free(_actionTimes);
	for(int i = 0;i<_maxActions;i++)
		free(*(_ximInput+i));
	free(_ximInput);
}

bool
SMB1Controller::PauseGame()
{
	if(_ximExe->IsActionHappening())
	{		
		TRACE("Action did not happen, not done with the last action yet...\n\n");
		return false;
	}

	int curXIM = 0;
	int time1 = 200;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Start,true);
	*(_actionTimes+curXIM) = time1;
	curXIM++;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Start,false);
	*(_actionTimes+curXIM) = time1;
	curXIM++;


	_ximExe->ExecuteActions(_ximInput,_actionTimes,curXIM);

	return true;
}

bool
SMB1Controller::SpeedJump()
{
	if(_ximExe->IsActionHappening())
		return false;

	int actions = 0;
	actions = SpeedJumpAction(_ximInput,_actionTimes, actions);
	actions = StandStillAction(_ximInput,_actionTimes, actions,250);
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Right);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
	return true;
}


bool
SMB1Controller::JumpAndRight(int jumpTimeMS)
{
	if(_ximExe->IsActionHappening())
		return false;

	int actions = 0;
	actions = JumpRightAndHoldAction(_ximInput,_actionTimes, actions,jumpTimeMS);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
	return true;
}

bool
SMB1Controller::WaitAndLeft()
{
	if(_ximExe->IsActionHappening())
		return false;

	int actions = 0;
	actions = StandStillAction(_ximInput,_actionTimes,actions,6000);
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Left,1000);
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Right,1000);
	//followed by jumpoverbluehold
	actions = JumpRightAndHoldAction(_ximInput,_actionTimes,actions,400);
	actions = StandStillAction(_ximInput,_actionTimes,actions, 700);
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Right);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
	return true;
}

bool
SMB1Controller::JumpOverBlueHole(int jumpTimeMS)
{
	if(_ximExe->IsActionHappening())
		return false;

	int actions = 0;
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Right,jumpTimeMS);
	actions = JumpRightAndHoldAction(_ximInput,_actionTimes,actions,400);
	actions = StandStillAction(_ximInput,_actionTimes,actions, 750);
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Right);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
	return true;
}

bool
SMB1Controller::StartGame()
{
	if(_ximExe->IsActionHappening())
		return false;

	int actions = 0;
	actions = HoldReleaseButtonAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Start);
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions,_ximExe->XIMExe_CONST_Right);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
	return true;

}

bool
SMB1Controller::ShortJumpStop()
{

	if(_ximExe->IsActionHappening())
		return false;

	int actions = 0;
	actions = ShortHopAction(_ximInput,_actionTimes,actions);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
	return true;
}


bool
SMB1Controller::JumpOnToPipe()
{
	if(_ximExe->IsActionHappening())
		return false;

	int actions = 0;
	actions = JumpRightAndHoldAction(_ximInput,_actionTimes,actions,400);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
	return true;
}

bool
SMB1Controller::HoldRight()
{
	if(_ximExe->IsActionHappening())
		return false;
	int actions = 0;
	actions = HoldDirectionAction(_ximInput,_actionTimes,actions, _ximExe->XIMExe_CONST_Right);

	_ximExe->ExecuteActions(_ximInput,_actionTimes,actions);
}



int
SMB1Controller::HoldReleaseButtonAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart, int button)
{
	int curXIM = curXIMStart;
	int time1 = 100;
	int time2 = 100;

	_ximExe->InputDefault(*(_ximInput+curXIM));	
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),button,true);
	*(_actionTimes+curXIM) = time1;
	curXIM++;

	_ximExe->InputDefault(*(_ximInput+curXIM));	
	*(_actionTimes+curXIM) = time2;
	curXIM++;

	return curXIM;

}


int
SMB1Controller::StandStillAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart, int timeMS)
{
	int curXIM = curXIMStart;
	int time1 = timeMS;

	_ximExe->InputDefault(*(_ximInput+curXIM));	
	*(_actionTimes+curXIM) = time1;
	curXIM++;

	return curXIM;
}

int
SMB1Controller::SpeedJumpAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart)
{
	int curXIM = curXIMStart;
	int time1 = 400;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Right,true);
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_X,true);
	*(_actionTimes+curXIM) = 50;
	curXIM++;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Right,true);
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_A,true);
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_X,true);
	*(_actionTimes+curXIM) = time1;
	curXIM++;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Right,true);
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_X,true);
	*(_actionTimes+curXIM) = 50;
	curXIM++;


	return curXIM;
}



int
SMB1Controller::ShortHopAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart)
{
	int curXIM = curXIMStart;
	int time1 = 100;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Right,true);
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_A,true);
	*(_actionTimes+curXIM) = time1;
	curXIM++;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Right,true);
	*(_actionTimes+curXIM) = 50;
	curXIM++;

	return curXIM;

}

int
SMB1Controller::JumpRightAndHoldAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart, int jumpTimeMS)
{
	int curXIM = curXIMStart;
	int time1 = jumpTimeMS;		

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Right,true);
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_A,true);
	*(_actionTimes+curXIM) = time1;
	curXIM++;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),_ximExe->XIMExe_CONST_Right,true);
	*(_actionTimes+curXIM) = 100;
	curXIM++;

	return curXIM;
}


int
SMB1Controller::HoldDirectionAction(XIMXbox360Input ** ximMe,int * delayTimes, int curXIMStart, int direction, int timeMS)
{
	int curXIM = curXIMStart;
	int time1 = timeMS;

	_ximExe->InputDefault(*(_ximInput+curXIM));
	_ximExe->SimpleControllerHit(*(_ximInput+curXIM),direction,true);
	*(_actionTimes+curXIM) = time1;
	curXIM++;

	return curXIM;
}