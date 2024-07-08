#include "DynamicDllLoad.h"
#include <iostream>

DynamicDllLoader::DynamicDllLoader() :m_DynamicModulePtr(NULL), m_DynamicModuleState(DynamicModuleState::DMS_UnLoaded)
{
}

DynamicDllLoader::~DynamicDllLoader()
{
}

bool DynamicDllLoader::IsFileExist(const std::string filePath)
{
	std::fstream file;
	file.open(filePath, std::ios::in);
	if (file)
	{
		file.close();
		return true;
	}
	else
	{
		return false;
	}
}

bool DynamicDllLoader::LoadDynamicModule(const std::string dynamicModulePath)
{
	if (IsFileExist(dynamicModulePath))
	{
#ifdef WINDOWS
		m_DynamicModulePtr = LoadLibraryA(dynamicModulePath.c_str());
#elif LINUX
		m_DynamicModulePtr = dlopen(dynamicModulePath.c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif // WINDOWS

		if (m_DynamicModulePtr != NULL)
		{
			m_DynamicModuleState = DynamicModuleState::DMS_Loaded;
			return true;
		}
#ifdef WINDOWS
		int errorCode = GetLastError();
#endif
		GetInternalErrorMessge(errorCode);
		std::cout << "Failed to Load Dynamic DLL !!" << std::endl;
		return false;
	}
	return false;
}

void* DynamicDllLoader::GetFunction(const std::string functionName)
{
	if (m_DynamicModulePtr)
	{
		void* tempFunctionPtr = NULL;
#ifdef WINDOWS
		tempFunctionPtr = GetProcAddress(m_DynamicModulePtr, functionName.c_str());
#elif LINUX
		tempFunctionPtr = dlsym(m_DynamicModulePtr, functionName.c_str());
#endif 
		if (tempFunctionPtr != NULL)
		{
			return tempFunctionPtr;
		}
	}

	return NULL;
}

bool DynamicDllLoader::UnloadDynamicModule()
{
	if (m_DynamicModulePtr)
	{
#ifdef WINDOWS
		if (FreeLibrary(m_DynamicModulePtr) == 0)
		{
			return false;
		}
#elif LINUX
		dlclose(m_DynamicModulePtr);
#endif
		m_DynamicModuleState = DynamicModuleState::DMS_UnLoaded;
		return true;
	}
	return false;
}

bool DynamicDllLoader::GetDynamicModuleState()
{
	return (bool)m_DynamicModuleState;
}


void DynamicDllLoader::GetInternalErrorMessge(int errorCode)
{
	//std::wstring errorMessge = L"";  // FormatMessageW  LPWSTR
	std::string errorMessge = ""; // FormatMessageA LPSTR
	LPVOID lpMsgBuf;
	if (FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	//	(LPTSTR)&lpMsgBuf,
	//	(LPTSTR)&lpMsgBuf,
		(LPSTR)&lpMsgBuf,
		0,
		NULL
	))
	{
		errorMessge = (LPSTR)(lpMsgBuf);
		LocalFree(lpMsgBuf);
	}

	std::cerr << "Error code: " << errorCode << ", Message: " << errorMessge << std::endl;
}


