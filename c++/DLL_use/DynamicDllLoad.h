#ifndef _DYNAMIC_DLL_LOADER_H_
#define _DYNAMIC_DLL_LOADER_H_

#include <string>
#include <fstream>

#define _DYNAMIC_LOAD
#define WINDOWS

#if defined(_DYNAMIC_LOAD)

#if defined(WINDOWS)
#include <Windows.h>
#define MODULE_HANDLER HINSTANCE
#elif defined(LINUX)
#include <dlfcn.h>
#define MODULE_HANDLER void*
#endif 

#endif


#ifdef WINDOWS
#define PLATFORM_PATH_SPLIT_CHAR "\\"
#define PLATFORM_DYNAMIC_LIBRARY_PRE ""
#define PLATFORM_DYNAMIC_LIBRARY_EXT ".dll"
#elif defined (LINUX)
#define PLATFORM_PATH_SPLIT_CHAR "/"
#define PLATFORM_DYNAMIC_LIBRARY_PRE "lib"
#define PLATFORM_DYNAMIC_LIBRARY_EXT ".so"
#endif // WINDOWS

// ����״̬s�Ƿ���سɹ�
enum DynamicModuleState
{
	DMS_UnLoaded = 0,
	DMS_Loaded = 1
};

class DynamicDllLoader
{
public:
	DynamicDllLoader();
	~DynamicDllLoader();

	// ����ļ�ʱ�����
	static bool IsFileExist(const std::string filePath);

	// ���ض�̬��
	bool LoadDynamicModule(const std::string dynamicModulePath);

	// �Ӽ��صĶ�̬���ȡ����ָ��
	void* GetFunction(const std::string functionName);

	// ж�ض�̬��
	bool UnloadDynamicModule();

	// ��ȡ��̬�����״̬
	bool GetDynamicModuleState();

	// ��ȡ������Ϣ�����ڵ�ʽ
	void GetInternalErrorMessge(int errorCode);
private:
	// ��̬����
	MODULE_HANDLER m_DynamicModulePtr;

	DynamicModuleState m_DynamicModuleState;
};


#endif // !_DYNAMIC_DLL_LOADER_H_


