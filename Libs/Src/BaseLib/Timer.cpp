// ==[ File ]===================================================================================
//
//  -Name     : Timer.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CTimer implementation (see CTimer.h).
//              QueryPerformanceCounter() is used for timing.
//
// =============================================================================================

/*

	21/09/2001  -  File created.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"


// ==[ Class implementations ]==================================================================


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : CTimer()
//
//  - Purpose   : CTimer's default constructor
//
// -----------------------------------------------------------------------------
CTimer::CTimer()
{
	LARGE_INTEGER li;

	m_n64TickStart      = 0;
	m_n64TickFrecuency  = 0;
	m_n64TickPauseStart = 0;
	m_n64TickPauseTime  = 0;

	// Get number of ticks per second:

	QueryPerformanceFrequency(&li);

	m_n64TickFrecuency = li.QuadPart;

	m_dStartTime = 0.0;

	m_eState = STATE_STOPPED;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : CTimer(CTimer &timer)
//
//  - Purpose   : CTimer's copy constructor.
//
// -----------------------------------------------------------------------------
CTimer::CTimer(CTimer &timer)
{
	*this = timer;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : Start(double dSeconds = 0.0f)
//
//  - Purpose   : Starts timing. dSeconds is an optional start time.
//
// -----------------------------------------------------------------------------
bool CTimer::Start(double dSeconds)
{
	LARGE_INTEGER li;

	QueryPerformanceCounter(&li);

	m_n64TickStart     = li.QuadPart;
	m_eState           = STATE_TIMING;
	m_n64TickPauseTime = 0;
	m_dStartTime       = dSeconds;

	return true;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : Stop()
//
//  - Purpose   : Stops and resets the timer..
//
// -----------------------------------------------------------------------------
bool CTimer::Stop()
{
	m_eState = STATE_STOPPED;

	return true;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : Pause()
//
//  - Purpose   : Pauses timing until CTimer::Resume() is called.
//
// ---------------------------------------------------------------------------
bool CTimer::Pause()
{
	if(m_eState != STATE_TIMING)
	{
		return false;
	}

	LARGE_INTEGER li;

	QueryPerformanceCounter(&li);

	m_eState            = STATE_PAUSED;
	m_n64TickPauseStart = li.QuadPart;

	return true;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : Resume()
//
//  - Purpose   : Resumes a timer paused with CTimer::Pause().
//
// ---------------------------------------------------------------------------
bool CTimer::Resume()
{
	if(m_eState != STATE_PAUSED)
	{
		return false;
	}

	LARGE_INTEGER li;

	QueryPerformanceCounter(&li);

	m_n64TickPauseTime += li.QuadPart - m_n64TickPauseStart;

	m_eState = STATE_TIMING;

	return true;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : double CTimer
//  - Method    : Seconds()
//
//  - Purpose   : Returns the number of seconds between the call to
//                CTimer::Start() and the current time, minus pause time.
//
// ---------------------------------------------------------------------------
double CTimer::Seconds() const
{
	LARGE_INTEGER li;

	QueryPerformanceCounter(&li);

	if(m_eState == STATE_TIMING)
	{
		return m_dStartTime + ((li.QuadPart - m_n64TickStart - m_n64TickPauseTime) / (double)m_n64TickFrecuency);
	}

	if(m_eState == STATE_PAUSED)
	{
		return m_dStartTime + (((li.QuadPart - m_n64TickStart) - (li.QuadPart - m_n64TickPauseStart) - m_n64TickPauseTime) / (double)m_n64TickFrecuency);
	}

	return 0.0f;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Method    : PauseSeconds()
//
//  - Purpose   : Returns total pause time in seconds.
//
// ---------------------------------------------------------------------------
double CTimer::PauseSeconds() const
{
	LARGE_INTEGER li;

	QueryPerformanceCounter(&li);

	if(m_eState != STATE_PAUSED)
	{
		return 0.0f;
	}

	return (li.QuadPart - m_n64TickPauseStart) / (double)m_n64TickFrecuency;
}

// --[ Method ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Method    : bool operator < (CTimer& timer)
//
//  - Purpose   : Less than operator.
//
// ---------------------------------------------------------------------------
bool CTimer::operator < (CTimer& timer) const
{
	return Seconds() < timer.Seconds();
}

// --[ Méthod ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : bool operator > (CTimer& timer)
//
//  - Purpose   : Greater than operator.
//
// ---------------------------------------------------------------------------
bool CTimer::operator > (CTimer& timer) const
{
	return Seconds () > timer.Seconds();
}

// --[ Méthod ]---------------------------------------------------------------
//
//  - Class     : CTimer
//  - Prototype : CTimer& operator = (CTimer& timer)
//
//  - Purpose   : Assignement operator.
//
// ---------------------------------------------------------------------------
CTimer& CTimer::operator = (CTimer& timer)
{
	m_eState = timer.m_eState;

	m_dStartTime        = timer.m_dStartTime;

	m_n64TickStart      = timer.m_n64TickStart;
	m_n64TickFrecuency  = timer.m_n64TickFrecuency;
	m_n64TickPauseStart = timer.m_n64TickPauseStart;
	m_n64TickPauseTime  = timer.m_n64TickPauseTime;

	return *this;
}