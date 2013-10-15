// ==[ File ]===================================================================================
//
//  -Name     : Factory.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : Factory template declaration.
//
// =============================================================================================

/*

	27/04/2003  -  File created.


*/


#ifndef __FACTORY_H
#define __FACTORY_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : TFactory
//
//  - Purpose   : Factory template.
//
// ---------------------------------------------------------------------------
template <class T>
class TFactory
{


public:

	// Instantiator

	class CObjectCreator
	{

	public:

		virtual std::string InstanceClassType() = 0;
		virtual std::string InstanceClassName() = 0;
		virtual T*          CreateInstance()    = 0;
	};

	// Instantiator registerer

	template<class R>
	class TRegisterCreator
	{

	public:

		TRegisterCreator(TFactory<T>& factory)
		{
			m_pCreator = new R;
			factory.RegisterClass(m_pCreator);
		}

		~TRegisterCreator() { SAFE_DELETE(m_pCreator); }

	private:

		TRegisterCreator() { }

		CObjectCreator* m_pCreator;
	};

	// Factory

	TFactory();
	~TFactory();

	bool		RegisterClass		(CObjectCreator* pCreator);
	bool		UnregisterClass		(const std::string& strClassType, const std::string& strClassName);
	T*			GetNew				(const std::string& strClassType, const std::string& strClassName);
	bool		UnregisterAll		();

	int			GetClassTypeCount	();
	std::string	GetClassType		(int i);
	int			GetClassCount		(const std::string& strType);
	std::string	GetClassName		(const std::string& strType, int i);


private:


	typedef std::map<std::string, CObjectCreator*>   MAPCREATORSBYNAME;
	typedef std::map<std::string, MAPCREATORSBYNAME> MAPCREATORSBYTYPE;

	MAPCREATORSBYTYPE m_mapCreators;

};

/////////////////////////////////////////////////////////////////////////////

template <class T>
TFactory<T>::TFactory()
{

}

/////////////////////////////////////////////////////////////////////////////

template <class T>
TFactory<T>::~TFactory()
{
	UnregisterAll();
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
bool TFactory<T>::RegisterClass(CObjectCreator* pCreator)
{
	MAPCREATORSBYTYPE::iterator it;

	it = m_mapCreators.find(pCreator->InstanceClassType());

	if(it != m_mapCreators.end())
	{
		it->second.insert(MAPCREATORSBYNAME::value_type(pCreator->InstanceClassName(), pCreator));
	}
	else
	{
		MAPCREATORSBYNAME newMap;
		newMap.insert(MAPCREATORSBYNAME::value_type(pCreator->InstanceClassName(), pCreator));
		m_mapCreators.insert(MAPCREATORSBYTYPE::value_type(pCreator->InstanceClassType(), newMap));
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
bool TFactory<T>::UnregisterClass(const std::string& strClassType, const std::string& strClassName)
{
	MAPCREATORSBYTYPE::iterator it;
	MAPCREATORSBYNAME::iterator itAux;

	// Look if we have registered this type

	it = m_mapCreators.find(strClassType);

	if(it != m_mapCreators.end())
	{
		// Look if we have a class name of this type

		itAux = it->second.find(strClassName);

		if(itAux != it->second.end())
		{
			// Erase the entry

			it->erase(itAux);

			// If it's the last one, erase the type

			if(it->empty())
			{
				m_mapCreators.erase(it);
			}

			return true;
		}
	}

#ifdef _DEBUG
	CLogger::ErrorWindow("TFactory::UnregisterClass()\nClass %s (type %s) not found!", strClassName.data(), strClassType.data());
#endif

	return false;
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
T* TFactory<T>::GetNew(const std::string& strClassType, const std::string& strClassName)
{
	MAPCREATORSBYTYPE::iterator it;
	MAPCREATORSBYNAME::iterator itAux;

	// Look if we have registered this type

	it = m_mapCreators.find(strClassType);

	if(it != m_mapCreators.end())
	{
		// Look if we have a class name of this type
		itAux = it->second.find(strClassName);

		if(itAux != it->second.end())
		{
			// Call the instantiation method
			return itAux->second->CreateInstance();
		}
	}

#ifdef _DEBUG
	CLogger::ErrorWindow("TFactory::GetNew()\nClass %s (type %s) not found!", strClassName.data(), strClassType.data());
#endif

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
bool TFactory<T>::UnregisterAll()
{
	MAPCREATORSBYTYPE::iterator it;

	for(it = m_mapCreators.begin(); it != m_mapCreators.end(); ++it)
	{
		it->second.clear();
	}

	m_mapCreators.clear();

	return true;
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
int TFactory<T>::GetClassTypeCount()
{
	return m_mapCreators.size();
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
std::string TFactory<T>::GetClassType(int i)
{
	// TODO: use another way to access i-th element

	MAPCREATORSBYTYPE::iterator it;

	int n = 0;

	for(it = m_mapCreators.begin(); it != m_mapCreators.end(); ++it, n++)
	{
		if(n == i)
		{
			return it->first;
		}
	}

#ifdef _DEBUG
	CLogger::ErrorWindow("TFactory::GetClassType()\nCan't access %dth element!", i);
#endif

	return std::string("");
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
int TFactory<T>::GetClassCount(const std::string& strType)
{
	// Look if we have registered this type

	MAPCREATORSBYTYPE::iterator it;

	it = m_mapCreators.find(strType);

	if(it != m_mapCreators.end())
	{
		return it->second.size();
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

template <class T>
std::string TFactory<T>::GetClassName(const std::string& strType, int i)
{
	// TODO: use another way to access i-th element

	MAPCREATORSBYTYPE::iterator it;
	MAPCREATORSBYNAME::iterator itAux;

	// Look if we have registered this type

	it = m_mapCreators.find(strType);

	if(it != m_mapCreators.end())
	{
		// Access the i-th element

		int n = 0;

		for(itAux = it->second.begin(); itAux != it->second.end(); ++itAux, n++)
		{
			if(n == i)
			{
				// Return the class name

				return itAux->first;
			}
		}
	}

#ifdef _DEBUG
	CLogger::ErrorWindow("TFactory::GetClassName()\nCan't access %dth element!", i);
#endif

	return std::string("");
}


#endif