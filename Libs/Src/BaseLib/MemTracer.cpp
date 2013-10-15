// ==[ File ]===================================================================================
//
//  -Name     : MemTracer.cpp
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CMemTracer implementation. Global memtracer instance.
//              Overloaded new/delete operators body.
//
// =============================================================================================

/*

	01/10/2001  -  File created.
	26/04/2004  -  Changed internal list from vector to hash table.


*/


// ==[ Headers ]================================================================================


#include "BaseLib.h"
#include "DemoSystem.h"


// ==[ Operators ]==============================================================================


#ifdef USE_MEMTRACER

//#define CMEMTRACER_VERBOSE

#ifdef new
  #undef new // undef new for this file. At the end of it we restore it
#endif

// Our own new/delete

void* MyNew(size_t size, char *szFile, int nLine)
{
	void *pPointer = malloc(size);

	if(CMemTracer::Instance().IsEnabled())
	{
		if(pPointer == NULL)
		{
			CLogger::ErrorWindow("Critical error! Out of memory while allocating.\nFile: %s Line: %u Alloc size: %u bytes.", szFile, nLine, size);
		}
		else
		{
			CMemTracer::Instance().RegisterNewCall(szFile, nLine, size, pPointer);
		}
	}

	return pPointer;
}

void MyDelete(void* pPointer)
{
	if(pPointer)
	{
		if(CMemTracer::Instance().IsEnabled())
		{
			CMemTracer::Instance().RegisterDeleteCall(pPointer);
		}

		free(pPointer);
	}
}

// operator new overloaded.

void* operator new(size_t size, char *szFile, int nLine)
{
	return MyNew(size, szFile, nLine);
}

void* operator new[](size_t size, char *szFile, int nLine)
{
	return MyNew(size, szFile, nLine);
}

// operator delete.

void operator delete(void *pPointer)
{
	MyDelete(pPointer);
}

void operator delete[](void *pPointer)
{
	MyDelete(pPointer);
}

// Matching operators delete for overloaded new

void operator delete(void *pPointer, char *szFile, int nLine)
{
	MyDelete(pPointer);
}

void operator delete[](void *pPointer, char *szFile, int nLine)
{
	MyDelete(pPointer);
}


#endif // USE_MEMTRACER


// ==[ Class implementation ]===================================================================


CMemTracer* CMemTracer::s_pSharedInstance = NULL;


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMemTracer
//  - Prototype : CMemTracer()
//
//  - Purpose   : CMemTracer's default constructor, it's specified as private
//                for this is a singleton.
//                Use CMemTracer::GetInstance() to access the unique instance.
//
// -----------------------------------------------------------------------------
CMemTracer::CMemTracer()
{
	m_nNewCalls      = 0;
	m_nDeleteCalls   = 0;
	m_nCurAllocBytes = 0;
	m_nMaxAllocBytes = 0;

	m_bEnabled = true;
}


// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMemTracer
//  - Prototype : ~CMemTracer()
//
//  - Purpose   : CMemTracer's destructor.
//                Deallocates all memory leaked by our program.
//
// -----------------------------------------------------------------------------
CMemTracer::~CMemTracer()
{
#ifdef USE_MEMTRACER
	if(!s_pSharedInstance)
	{
		DumpEndProgramStats();

		// Don't free all leaks here because this is only used for debugging purposed and
		// also some static objects may free the memory after the memtracer is destroyed.
	}

	s_pSharedInstance = NULL;
	this->SetEnabled(false);
#endif
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMemTracer
//  - Prototype : static CMemTracer& Instance()
//
//  - Purpose   : Returns the global instance (singleton pattern).
//
// -----------------------------------------------------------------------------
CMemTracer& CMemTracer::Instance()
{
	static CMemTracer s_instance;

	if(s_pSharedInstance)
	{
		return *s_pSharedInstance;
	}

	return s_instance;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMemTracer
//  - Prototype : void ShareFrom(CMemTracer& memTracer)
//
//  - Purpose   : Allows the global instance to be shared between other modules,
//				  such as a dll.
//
// -----------------------------------------------------------------------------
void CMemTracer::ShareFrom(CMemTracer& memTracer)
{
	memTracer.m_nNewCalls      += CMemTracer::Instance().m_nNewCalls;
	memTracer.m_nDeleteCalls   += CMemTracer::Instance().m_nDeleteCalls;
	memTracer.m_nCurAllocBytes += CMemTracer::Instance().m_nCurAllocBytes;

	if(memTracer.m_nCurAllocBytes > memTracer.m_nMaxAllocBytes)
	{
		memTracer.m_nMaxAllocBytes = memTracer.m_nCurAllocBytes;
	}

	if(memTracer.m_nMaxAllocBytes < CMemTracer::Instance().m_nMaxAllocBytes)
	{
		memTracer.m_nMaxAllocBytes = CMemTracer::Instance().m_nMaxAllocBytes;
	}

	// One should do a simple std::map::insert(iteratorbegin, iteratorend) call,
	// but that seems to be broken with VC6

	MAPCALLLIST::iterator it;

	for(it = CMemTracer::Instance().m_mapCallList.begin(); it != CMemTracer::Instance().m_mapCallList.end(); ++it)
	{
		memTracer.m_mapCallList.insert(MAPCALLLIST::value_type(it->first, it->second));
	}

	s_pSharedInstance = &memTracer;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMemTracer
//  - Prototype : DumpEndProgramStats()
//
//  - Purpose   : Dumps some statistics to our global log, including info
//                about maximum allocated memory at a time and memory leaks.
//
//  - Note      : Important! this method should be called at the end of our
//                program, when all the memory we allocated *should* have been
//                freed.
//
// -----------------------------------------------------------------------------
void CMemTracer::DumpEndProgramStats()
{
	if(!CMemTracer::Instance().IsEnabled()) return;

	LOG.Write("\n");
	LOG.Write("\nMemory tracer stats:");
	LOG.Write("\n");
	LOG.Write("\n  -Max. allocated : %u bytes.", m_nMaxAllocBytes);
	LOG.Write("\n  -New calls      : %u times.", m_nNewCalls);
	LOG.Write("\n  -Delete calls   : %u times.", m_nDeleteCalls);
	LOG.Write("\n  -Memory leaked  : %u pointers, %u bytes.", m_mapCallList.size(), m_nCurAllocBytes);
	LOG.Write("\n");

	if(!m_mapCallList.empty())
	{
		LOG.Write("\nMemory leaked info:");

		MAPCALLLIST::iterator iterator;
		int i;
		for(i = 0, iterator = m_mapCallList.begin(); i < 100 && iterator != m_mapCallList.end(); ++iterator, i++)
		{
			SAllocRecord *pElement = iterator->second;
			LOG.Write("\n-File %s, line %u, size %u, dir %u.", pElement->szFile, pElement->nLine, pElement->nSize, pElement->pPointer);
		}
	}
	else
	{
		LOG.Write("\nNo memory leaks.");
	}

	LOG.Write("\n\nEnd of memory tracer stats.");
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMemTracer
//  - Prototype : RegisterNewCall(char *szFile, int nLine, int nNumBytes, void *pPointer)
//
//  - Purpose   : Registers a call to operator new.
//                Adds a new entry to our allocation list.
//
// -----------------------------------------------------------------------------
void CMemTracer::RegisterNewCall(char *szFile, int nLine, int nNumBytes, void *pPointer)
{
#ifdef CMEMTRACER_VERBOSE
	LOG.Write("\nNew: file %s, line=%u, size=%u, direction=%u:", szFile, nLine, nNumBytes, pPointer);
#endif

	SAllocRecord *pNewRecord = (SAllocRecord *)malloc(sizeof(SAllocRecord));

	if(pNewRecord == NULL)
	{
		CLogger::ErrorWindow("Error! Out of memory while registering an allocation.\nMemory tracer will loose allocation calls.");
		return;
	}

	pNewRecord->szFile = (char*)malloc(strlen(szFile) + 1);
	strcpy(pNewRecord->szFile, szFile);

	pNewRecord->nLine    = nLine;
	pNewRecord->nSize    = nNumBytes;
	pNewRecord->pPointer = pPointer;

	m_mapCallList.insert(MAPCALLLIST::value_type(pPointer, pNewRecord));

	m_nCurAllocBytes += nNumBytes;

	if(m_nCurAllocBytes > m_nMaxAllocBytes)
	{
		m_nMaxAllocBytes = m_nCurAllocBytes;
	}

#ifdef CMEMTRACER_VERBOSE
	LOG.Write(" Done!");
#endif

	m_nNewCalls++;
}

// --[  Method  ]---------------------------------------------------------------
//
//  - Class     : CMemTracer
//  - Prototype : RegisterDeleteCall(void *pPointer);
//
//  - Purpose   : Registers a call to operator delete.
//                Removes the allocation entry of the list.
//
// -----------------------------------------------------------------------------
void CMemTracer::RegisterDeleteCall(void *pPointer)
{
	MAPCALLLIST::iterator iterator;

	if(m_mapCallList.empty())
	{
		LOG.Write("\nWARNING (CMemTracer) - Call list empty while registering delete call.");
		return;
	}

	iterator = m_mapCallList.find(pPointer);

	if(iterator == m_mapCallList.end())
	{
#ifdef CMEMTRACER_VERBOSE
		LOG.Write("\nWARNING (CMemTracer) - Pointer not found while registering delete call.");
#endif
	}
	else
	{
#ifdef CMEMTRACER_VERBOSE
		LOG.Write("\nDelete: direction=%u", pPointer);
#endif
		SAllocRecord *pElement = iterator->second;

		m_nCurAllocBytes -= pElement->nSize;

		if(pElement->szFile)
		{
			free(pElement->szFile);
		}

		free(pElement);

		m_mapCallList.erase(iterator);
		m_nDeleteCalls++;
	}	
}


#ifndef new
  #define new new(__FILE__, __LINE__)
#endif
