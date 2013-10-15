// ==[ File ]===================================================================================
//
//  -Name     : SoundSystem.cpp
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CSoundModule/CSoundStream/CSoundSystem implementation.
//
//  -Notes    : Get BASS Sound System from http://www.un4seen.com
//
// =============================================================================================

/*

	05/11/2001  -  File created.
	27/10/2003  -  Ported from FMOD to BASS, since FMOD had some .it issues.

*/


// ==[ Headers ]================================================================================


#include "DemoSystem.h"


// ==[ Class implementation ]===================================================================


CSoundSystem* CSoundSystem::s_pSharedInstance = NULL;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : Free()
//
//  - Purpose   : Frees all memory allocated by the sound module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::Free()
{
	if(!IsValid())
	{
		return false;
	}

	BASS_MusicFree(m_handle);
	SAFE_DELETE(m_pBuffer);

	CSoundSystem::Instance().EraseSound(this);

	return BASS_ErrorGetCode() == BASS_OK;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool GetVolume(float* pfPercentage) const
//
//  - Purpose   : Gets the volume (percentage) of the sound module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::GetVolume(float* pfPercentage) const
{
	assert(pfPercentage);

	if(!IsValid())
	{
		return false;
	}

	DWORD vol;
	BASS_ChannelGetAttributes(m_handle, NULL, &vol, NULL);

	if(vol == -1)
	{
		return false;
	}

	*pfPercentage = (float)vol;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool SetVolume(float fPercentage)
//
//  - Purpose   : Sets the volume (percentage) of the sound module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::SetVolume(float fPercentage)
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelSetAttributes(m_handle, -1, MYROUND(fPercentage), -101) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool Play()
//
//  - Purpose   : Plays the module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::Play()
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_MusicPlay(m_handle) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool Pause()
//
//  - Purpose   : Pauses the module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::Pause()
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelPause(m_handle) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool Resume()
//
//  - Purpose   : Resumes a paused module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::Resume()
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelResume(m_handle) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool Stop()
//
//  - Purpose   : Stops a module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::Stop()
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelStop(m_handle) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool GetPlayPos(float* pfSeconds) const
//
//  - Purpose   : Returns the playing position in seconds of the module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::GetPlayPos(float* pfSeconds) const
{
	LOG.Write("\nWARNING - CSoundModule::GetPlayPos() not supported.");
	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool GetPlayLength(float* pfSeconds) const
//
//  - Purpose   : Returns the play length in seconds of the module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::GetPlayLength(float* pfSeconds) const
{
	LOG.Write("\nWARNING - CSoundModule::GetPlayLength() not supported.");
	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool SetPlayPos(float fSeconds)
//
//  - Purpose   : Sets the playing position in seconds of the module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::SetPlayPos(float fSeconds)
{
	LOG.Write("\nWARNING - CSoundModule::SetPlayPos() not supported.");
	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool SetPlayPos(int nOrder)
//
//  - Purpose   : Sets the playing position in orders of the module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::SetPlayPos(int nOrder)
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelSetPosition(m_handle, (QWORD)MAKELONG(nOrder, 0)) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundModule
//  - prototype : bool GetFFT512(float* pfBuffer) const
//
//  - Purpose   : Returns the current FFT representation (512 float values)
//                of the module.
//
// -----------------------------------------------------------------------------
bool CSoundModule::GetFFT512(float* pfBuffer) const
{
	return BASS_ChannelGetData(m_handle, pfBuffer, BASS_DATA_FFT1024) != -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : std::string GetName() const
//
//  - Purpose   : Returns the sound stream name.
//
// -----------------------------------------------------------------------------
std::string CSoundStream::GetName() const
{
	if(!IsValid())
	{
		return std::string("non initialized stream");
	}

	return std::string("Generic stream");
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool IsBeingPlayed() const
//
//  - Purpose   : Returns true if the sound stream is being played.
//
// -----------------------------------------------------------------------------
bool CSoundStream::IsBeingPlayed() const
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelIsActive(m_handle) == BASS_ACTIVE_PLAYING;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool HasFinished() const
//
//  - Purpose   : Returns true if the sound stream playing has finished.
//
// -----------------------------------------------------------------------------
bool CSoundStream::HasFinished() const
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelIsActive(m_handle) == BASS_ACTIVE_STOPPED;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool Free()
//
//  - Purpose   : Frees the sound stream.
//
// -----------------------------------------------------------------------------
bool CSoundStream::Free()
{
	if(!IsValid())
	{
		return false;
	}

	BASS_StreamFree(m_handle);
	SAFE_DELETE(m_pBuffer);

	CSoundSystem::Instance().EraseSound(this);

	return BASS_ErrorGetCode() == BASS_OK;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool GetVolume(float* pfPercentage) const
//
//  - Purpose   : Gets the current sound stream volume (percentage).
//
// -----------------------------------------------------------------------------
bool CSoundStream::GetVolume(float* pfPercentage) const
{
	if(!IsValid())
	{
		return false;
	}

	DWORD vol;
	BASS_ChannelGetAttributes(m_handle, NULL, &vol, NULL);

	if(vol == -1)
	{
		return false;
	}

	*pfPercentage = (float)vol;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool SetVolume(float fPercentage)
//
//  - Purpose   : Sets the current sound stream volume (percentage).
//
// -----------------------------------------------------------------------------
bool CSoundStream::SetVolume(float fPercentage)
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelSetAttributes(m_handle, -1, MYROUND(fPercentage), -101) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool Play()
//
//  - Purpose   : Plays the sound stream.
//
// -----------------------------------------------------------------------------
bool CSoundStream::Play()
{
	if(!IsValid())
	{
		return false;
	}

	BASS_StreamPreBuf(m_handle);
	return BASS_StreamPlay(m_handle, FALSE, 0) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool Pause()
//
//  - Purpose   : Pauses the playing sound stream.
//
// -----------------------------------------------------------------------------
bool CSoundStream::Pause()
{
	if(!IsValid())
	{
		return false;
	}
	return BASS_ChannelPause(m_handle) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool Resume()
//
//  - Purpose   : Resumes the paused sound stream.
//
// -----------------------------------------------------------------------------
bool CSoundStream::Resume()
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelResume(m_handle) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool Stop()
//
//  - Purpose   : Stops the sound stream.
//
// -----------------------------------------------------------------------------
bool CSoundStream::Stop()
{
	if(!IsValid())
	{
		return false;
	}

	return BASS_ChannelStop(m_handle) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool GetPlayPos(float* pfSeconds) const
//
//  - Purpose   : Gets the current playing position (seconds) of the stream.
//
// -----------------------------------------------------------------------------
bool CSoundStream::GetPlayPos(float* pfSeconds) const
{
	if(!IsValid())
	{
		return false;
	}

	QWORD pos = BASS_ChannelGetPosition(m_handle);

	if(pos == -1)
	{
		return false;
	}

	*pfSeconds = BASS_ChannelBytes2Seconds(m_handle, pos);
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool GetPlayLength(float* pfSeconds) const
//
//  - Purpose   : Gets the stream length (seconds).
//
// -----------------------------------------------------------------------------
bool CSoundStream::GetPlayLength(float* pfSeconds) const
{
	if(!IsValid())
	{
		return false;
	}

	QWORD len  = BASS_StreamGetLength(m_handle); // length in bytes

	if(len == -1)
	{
		return false;
	}

	*pfSeconds = BASS_ChannelBytes2Seconds(m_handle, len); // the time length
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool SetPlayPos(float fSeconds)
//
//  - Purpose   : Sets the stream play position (seconds).
//
// -----------------------------------------------------------------------------
bool CSoundStream::SetPlayPos(float fSeconds)
{
	if(!IsValid())
	{
		return false;
	}

	QWORD bytes = BASS_ChannelSeconds2Bytes(m_handle, fSeconds);
	return BASS_ChannelSetPosition(m_handle, bytes) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundStream
//  - prototype : bool GetFFT512(float* pfBuffer) const
//
//  - Purpose   : Gets the current FFT representation of the playing stream.
//                The buffer is filled with 512 floating point values.
//
// -----------------------------------------------------------------------------
bool CSoundStream::GetFFT512(float* pfBuffer) const
{
	return BASS_ChannelGetData(m_handle, pfBuffer, BASS_DATA_FFT1024) != -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : CSoundSystem()
//
//  - Purpose   : CSoundSystem's default constructor.
//
// -----------------------------------------------------------------------------
CSoundSystem::CSoundSystem()
{
	m_bActive = false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : CSoundSystem& Instance()
//
//  - Purpose   : Singleton pattern, gets the global instance.
//
// -----------------------------------------------------------------------------
CSoundSystem& CSoundSystem::Instance()
{
	static CSoundSystem soundSystem;

	if(s_pSharedInstance)
	{
		return *s_pSharedInstance;
	}

	return soundSystem;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool ShareFrom(CSoundSystem& soundSystem)
//
//  - Purpose   : Gets a shared global instance from a source. This is used
//				  to share global instaces between exe<->dll.
//
// -----------------------------------------------------------------------------
bool CSoundSystem::ShareFrom(CSoundSystem& soundSystem)
{
	s_pSharedInstance = &soundSystem;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : ~CSoundSystem()
//
//  - Purpose   : CSoundSystem's destructor.
//
// -----------------------------------------------------------------------------
CSoundSystem::~CSoundSystem()
{
	if(!s_pSharedInstance)
	{
		Close();
	}
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool Init(HWND hWnd)
//
//  - Purpose   : Initializes sound system.
//
// -----------------------------------------------------------------------------
bool CSoundSystem::Init(HWND hWnd)
{
	if(m_bActive)
	{
		Close();
	}

	if(BASS_Init(-1, 44100, 0, hWnd) != TRUE)
	{
		CLogger::ErrorWindow("CSoundSystem::Init(): BASS_Init() failed.");
		return false;
	}

	if(BASS_Start() != TRUE)
	{
		CLogger::ErrorWindow("CSoundSystem::Init(): BASS_Start() failed.");
		return false;
	}

	BASS_SetGlobalVolumes(100, 100, 100);

	m_bActive = true;
	return m_bActive;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool Close()
//
//  - Purpose   : Stops all currently playing songs, deallocates them and closes
//                the sound system.
//
// -----------------------------------------------------------------------------
bool CSoundSystem::Close()
{
	if(!IsActive())
	{
		return false;
	}

	BASS_Free();
	m_bActive = false;
	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : CSound* LoadSound(CInput* pInput, const std::string& strExt)
//
//  - Purpose   : Loads a sound,  setting it ready to play.
//                If succesfull, it returns a valid handle to operate
//                with, otherwise it returns NULL.
//
// -----------------------------------------------------------------------------
CSound* CSoundSystem::LoadSound(CInput* pInput, const std::string& strExt)
{
	if(m_bActive == false)
	{
		return NULL;
	}

	// Put it into a buffer:

	pInput->SetPosition(0, CInput::SEEKFROM_START);
	char* pBuffer = new char[pInput->GetLength()];

	assert(pBuffer);

	if(pInput->ReadRawData(pInput->GetLength(), 1, pBuffer) == 0)
	{
		CLogger::ErrorWindow("ERROR - CSoundSystem::LoadSound():\npInput->ReadRawData() failed");
		SAFE_DELETE_A(pBuffer);
		return NULL;
	}

	CSound* pSound = NULL;

	if(IsModule(strExt))
	{
		HMUSIC module = BASS_MusicLoad(TRUE, pBuffer, 0, pInput->GetLength(), 0);

		if(!module)
		{
			CLogger::ErrorWindow("ERROR - CSoundSystem::LoadSound():\nBASS_MusicLoad() failed");
			return NULL;
		}

		CSound* pSound = new CSoundModule(module, pBuffer);
		m_vecSounds.push_back(pSound);
		return pSound;
	}
	else if(IsStream(strExt))
	{
		HSTREAM stream = BASS_StreamCreateFile(TRUE, pBuffer, 0, pInput->GetLength(), BASS_MP3_SETPOS);

		if(!stream)
		{
			CLogger::ErrorWindow("ERROR - CSoundSystem::LoadSound():\nBASS_StreamCreateFile() failed");
			return NULL;
		}

		CSound* pSound = new CSoundStream(stream, pBuffer);
		m_vecSounds.push_back(pSound);
		return pSound;
	}

	SAFE_DELETE_A(pBuffer);
	CLogger::ErrorWindow("ERROR - CSoundSystem::LoadSound():\nUnknown format (%s)", strExt.data());
	return NULL;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool GetMasterVolume(float* pfPercentage) const
//
//  - Purpose   : Gets the master volume (percentage).
//
// -----------------------------------------------------------------------------
bool CSoundSystem::GetMasterVolume(float* pfPercentage) const
{
	if(m_bActive == false)
	{
		return false;
	}

	int nVol = BASS_GetVolume();
	*pfPercentage = nVol / 2.55f;

	return true;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool SetMasterVolume(float fPercentage)
//
//  - Purpose   : Sets the master volume (percentage).
//
// -----------------------------------------------------------------------------
bool CSoundSystem::SetMasterVolume(float fPercentage)
{
	if(m_bActive == false)
	{
		return false;
	}

	return BASS_SetVolume(MYROUND(fPercentage * 2.55f)) == TRUE;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool IsModule(const std::string& strExt) const
//
//  - Purpose   : Checks if the extension given is from a module.
//
// -----------------------------------------------------------------------------
bool CSoundSystem::IsModule(const std::string& strExt) const
{
	     if(!stricmp(strExt.data(), "mod")) return true;
	else if(!stricmp(strExt.data(), "s3m")) return true;
	else if(!stricmp(strExt.data(), "xm"))  return true;
	else if(!stricmp(strExt.data(), "it"))  return true;
	else if(!stricmp(strExt.data(), "mid")) return true;
	else if(!stricmp(strExt.data(), "rmi")) return true;
	else if(!stricmp(strExt.data(), "sgt")) return true;

	return false;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool IsStream(const std::string& strExt) const
//
//  - Purpose   : Checks if the extension given is from a sound stream.
//
// -----------------------------------------------------------------------------
bool CSoundSystem::IsStream(const std::string& strExt) const
{
	     if(!stricmp(strExt.data(), "raw")) return true;
	else if(!stricmp(strExt.data(), "wav")) return true;
	else if(!stricmp(strExt.data(), "mp2")) return true;
	else if(!stricmp(strExt.data(), "mp3")) return true;
	else if(!stricmp(strExt.data(), "ogg")) return true;
	else if(!stricmp(strExt.data(), "wma")) return true;
	else if(!stricmp(strExt.data(), "asf")) return true;

	return false;

}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : float GetCPUUsage() const
//
//  - Purpose   : Returns the percentage of CPU used by the whole sound system.
//
// -----------------------------------------------------------------------------
float CSoundSystem::GetCPUUsage() const
{
	 return BASS_GetCPU();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : int GetNumSounds() const
//
//  - Purpose   : Returns the number of sounds loaded.
//
// -----------------------------------------------------------------------------
int	CSoundSystem::GetNumSounds() const
{
	return m_vecSounds.size();
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : CSound* GetSound(int i)
//
//  - Purpose   : Returns a pointer to the i-th sound.
//
// -----------------------------------------------------------------------------
CSound*	CSoundSystem::GetSound(int i)
{
	return m_vecSounds[i];
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CSoundSystem
//  - prototype : bool EraseSound(CSound* pSound)
//
//  - Purpose   : Removes the current sound from the list.
//
// -----------------------------------------------------------------------------
bool CSoundSystem::EraseSound(CSound* pSound)
{
	std::vector<CSound*>::iterator it;

	for(it = m_vecSounds.begin(); it != m_vecSounds.end(); ++it)
	{
		if(*it == pSound)
		{
			m_vecSounds.erase(it);
			return true;
		}
	}

	return false;
}