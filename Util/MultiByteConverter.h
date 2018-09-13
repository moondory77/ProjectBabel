#ifndef __MULTIBYTE_CONVERTER__
#define __MULTIBYTE_CONVERTER__

#include "cocos2d.h"
#include <string>
#include <stdexcept>

using namespace std;
USING_NS_CC;

class MultiByteConverter
{
public:
	MultiByteConverter(const char* pSrc, int nSrcCP, int nDstCP):m_pBuffer(NULL)
	{
		// check validation
		if (nSrcCP == nDstCP)
			throw std::logic_error("same code page.");

		// convert to wide character
		int nNum = ::MultiByteToWideChar(nSrcCP, 0, pSrc, -1, NULL, NULL);
		WCHAR* szWCS = new WCHAR[nNum];
		if (!szWCS)
			throw std::logic_error("cannot allocate memory anymore.");
		if (nNum != ::MultiByteToWideChar(nSrcCP, 0, pSrc, -1, szWCS, nNum))
			throw std::logic_error("failed MultiByteToWideChar call.");

		// convert to multi byte character
		nNum = ::WideCharToMultiByte(nDstCP, 0, szWCS, -1, NULL, NULL, NULL, NULL);
		m_pBuffer = new char[nNum];
		if (!m_pBuffer)
			throw std::logic_error("cannot allocate memory anymore.");
		if (nNum != ::WideCharToMultiByte(nDstCP, 0, szWCS, -1, m_pBuffer, nNum, NULL, NULL))
			throw std::logic_error("failed WideCharToMultiByte call.");

		delete[] szWCS;
	}

	~MultiByteConverter()
	{
		if (m_pBuffer)
		{
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}
	}

	const char* operator * ()
	{
		return m_pBuffer;
	}

private:
	char*	m_pBuffer;
};



#define _AtoU8( str )	*MultiByteConverter( str, CP_ACP, CP_UTF8 )
#define _U8toA( str )	*MultiByteConverter( str, CP_UTF8, CP_ACP )


#endif  // __MULTIBYTE_CONVERTER__