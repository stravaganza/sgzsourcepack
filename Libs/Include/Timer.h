// ==[ File ]===================================================================================
//
//  -Name     : Timer.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CTimer class definition.
//
// =============================================================================================

/*

	21/09/2001  -  File created.


*/


#ifndef __TIMER_H
#define __TIMER_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CTimer
//
//  - Purpose   : Provides high resolution timing.
//
//  - Notes     : Uses QueryPerformanceCounter()
//
// ---------------------------------------------------------------------------

class CTimer
{


public:


	CTimer();
	CTimer(CTimer &timer);
	~CTimer() { }

	enum ETimerState
	{
		STATE_STOPPED,
		STATE_PAUSED,
		STATE_TIMING
	};

	// Actions

	bool Start(double dSeconds = 0.0);
	bool Stop();
	bool Pause();
	bool Resume();

	// Access to members

	ETimerState State() const { return m_eState; }

	double Seconds()      const;
	double PauseSeconds() const;

	// Overloaded operators

	bool operator < (CTimer& timer) const;
	bool operator > (CTimer& timer) const;

	CTimer& operator = (CTimer& timer);


private:


	ETimerState m_eState;

	double  m_dStartTime;

	__int64 m_n64TickStart;
	__int64 m_n64TickFrecuency;
	__int64 m_n64TickPauseStart;
	__int64 m_n64TickPauseTime;

};


#endif