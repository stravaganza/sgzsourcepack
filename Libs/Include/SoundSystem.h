// ==[ File ]===================================================================================
//
//  -Name     : SoundSystem.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSoundModule, CSoundStream & CSoundSystem definition.
//
// =============================================================================================

/*

	03/11/2001  -  File created.
	02/05/2003  -  Some modifications made.
	27/10/2003  -  Ported from FMOD to BASS, since FMOD had some .it issues.


*/


#ifndef __SOUNDSYSTEM_H
#define __SOUNDSYSTEM_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSound
//
//  - Purpose   : Base sound class
//
// ---------------------------------------------------------------------------
class CSound
{


public:


	CSound()          { }
	virtual ~CSound() { }

	virtual std::string GetName			() const					= 0;
	virtual bool		IsValid			() const					= 0;
	virtual bool		IsBeingPlayed	() const					= 0;
	virtual bool		HasFinished		() const					= 0;
	virtual bool		Free			()							= 0;

	virtual bool		GetVolume		(float* pfPercentage) const	= 0;
	virtual bool		SetVolume		(float fPercentage)			= 0;

	virtual bool		Play			()							= 0;
	virtual bool		Pause			()							= 0;
	virtual bool		Resume			()							= 0;
	virtual bool		Stop			()							= 0;

	virtual bool		GetPlayPos		(float* pfSeconds) const	= 0;
	virtual bool        GetPlayLength   (float* pfSeconds) const    = 0;
	virtual bool		SetPlayPos		(float  fSeconds)			= 0;

	virtual bool		GetFFT512		(float* pfBuffer) const	    = 0;

};

// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSoundModule
//
//  - Purpose   : Sound Module (.MOD/.S3M/.XM/.IT/.MID/.RMI/.SGT)
//
// ---------------------------------------------------------------------------
class CSoundModule : public CSound
{


public:


	CSoundModule (HMUSIC handle, void* pBuffer) : m_handle(handle), m_pBuffer(pBuffer) { }
	~CSoundModule()
	{
		Free();
	}

	// From CSound

	std::string	GetName			() const { return BASS_MusicGetName(m_handle);					 		 }
	bool		IsValid			() const { return m_handle != NULL && m_pBuffer != NULL;				 }
	bool		IsBeingPlayed	() const { return BASS_ChannelIsActive(m_handle) == BASS_ACTIVE_PLAYING; }
	bool		HasFinished		() const { return BASS_ChannelIsActive(m_handle) == BASS_ACTIVE_STOPPED; }
	bool		Free			();

	bool		GetVolume		(float* pfPercentage) const;
	bool		SetVolume		(float  fPercentage);

	bool		Play			();
	bool		Pause			();
	bool		Resume			();
	bool		Stop			();

	bool		GetPlayPos		(float* pfSeconds) const;
	bool        GetPlayLength   (float* pfSeconds) const;
	bool		SetPlayPos		(float  fSeconds);

	bool		GetFFT512		(float* pfBuffer) const;

	// CSoundModule

	bool SetPlayPos				(int nOrder);


private:


	CSoundModule() { }
	HMUSIC m_handle;
	void*  m_pBuffer;

};

// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSoundStream
//
//  - Purpose   : Sound Stream (.RAW/.WAV/.MP2/.MP3/.OGG/.WMA/.ASF)
//
// ---------------------------------------------------------------------------
class CSoundStream : public CSound
{

public:

	CSoundStream (HSTREAM handle, void* pBuffer) : m_handle(handle), m_pBuffer(pBuffer) { }
	~CSoundStream() { Free(); }

	// From CSound

	std::string	GetName			() const;
	bool		IsValid			() const { return m_handle != NULL && m_pBuffer != NULL; }
	bool		IsBeingPlayed	() const;
	bool		HasFinished		() const;
	bool		Free			();

	bool		GetVolume		(float* pfPercentage) const;
	bool		SetVolume		(float  fPercentage);

	bool		Play			();
	bool		Pause			();
	bool		Resume			();
	bool		Stop			();

	bool		GetPlayPos		(float* pfSeconds) const;
	bool        GetPlayLength   (float* pfSeconds) const;
	bool		SetPlayPos		(float  fSeconds);

	bool		GetFFT512		(float* pfBuffer) const;


private:


	CSoundStream() { }

	HSTREAM	 m_handle;
	void*    m_pBuffer;

};

// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CSoundSystem
//
//  - Purpose   : Class to handle sound stream/modules output.
//
// ---------------------------------------------------------------------------
class CSoundSystem
{

	friend class CSoundStream; // TODO: change
	friend class CSoundModule; // TODO: change


private:


	CSoundSystem();
	CSoundSystem(const CSoundSystem& soundSystem)              { }
	CSoundSystem& operator = (const CSoundSystem& soundSystem) { }

	static CSoundSystem* s_pSharedInstance;


public:


	~CSoundSystem();

	// Singleton

	static CSoundSystem& Instance();
	static bool			 ShareFrom(CSoundSystem& soundSystem);

	// Init / Close:

	bool			Init			(HWND hWnd);
	bool			Close			();
	bool			IsActive		() const { return m_bActive; }

	// Sound loading:

	CSound*			LoadSound		(CInput* pInput, const std::string& strExt);

	// Volume

	bool			GetMasterVolume	(float* pfPercentage) const;
	bool            SetMasterVolume	(float  fPercentage);

	// Misc

	bool			IsModule		(const std::string& strExt) const;
	bool			IsStream		(const std::string& strExt) const;
	float			GetCPUUsage		() const;

	// Enum sounds:

	int				GetNumSounds	() const;
	CSound*			GetSound		(int i);


private:


	bool			EraseSound(CSound* pSound);

	bool				 m_bActive;
	HWND				 m_hWnd;
	std::vector<CSound*> m_vecSounds;
};



#endif
