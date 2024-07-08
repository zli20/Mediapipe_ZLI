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

// 加载状态s是否加载成功
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

	// 检查文件时候存在
	static bool IsFileExist(const std::string filePath);

	// 加载动态库
	bool LoadDynamicModule(const std::string dynamicModulePath);

	// 从加载的动态库获取函数指针
	void* GetFunction(const std::string functionName);

	// 卸载动态库
	bool UnloadDynamicModule();

	// 获取动态库加载状态
	bool GetDynamicModuleState();

	// 获取错误消息，用于调式
	void GetInternalErrorMessge(int errorCode);
private:
	// 动态库句柄
	MODULE_HANDLER m_DynamicModulePtr;

	DynamicModuleState m_DynamicModuleState;
};


#endif // !_DYNAMIC_DLL_LOADER_H_


