#include "HolisticTrack.h"

HolisticTrack::HolisticTrack()
{
	m_MediapipeHolisticTrackInit = nullptr;
	m_MediapipeHolisticTrackDetect = nullptr;
	m_MediapipeHolisticTrackRelease = nullptr;

}

HolisticTrack::~HolisticTrack()
{

}

bool HolisticTrack::LoadMediapipeHolisticTrackingDll(const std::string& dll_path)
{
	std::string dllPath = dll_path;
	if (DynamicDllLoader::IsFileExist(dllPath))
	{
		if (m_DynamicDLLLoader.LoadDynamicModule(dllPath))
		{
			return true;
		}
		return false;
	}

	std::cout << "DLL File not Exist, Check File Path" << std::endl;
	return false;
}

bool HolisticTrack::UnLoadMediapipeHolisticTrackingDll()
{
	if (m_DynamicDLLLoader.UnloadDynamicModule())
	{
		return true;
	}

	return false;
}

bool HolisticTrack::GetAllFunctions()
{
	if (m_DynamicDLLLoader.GetDynamicModuleState())
	{
		void* pMediapipeHolisticTrackingInit = m_DynamicDLLLoader.GetFunction("MediapipeHolisticTrackInit");
		if (pMediapipeHolisticTrackingInit != nullptr)
		{
			m_MediapipeHolisticTrackInit = (FuncMediapipeHolisticTrackInit)(pMediapipeHolisticTrackingInit);
			if (m_MediapipeHolisticTrackInit != nullptr)
			{

			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		void* pMediapipeHolisticTrackingDetectFrameDirect = m_DynamicDLLLoader.GetFunction("MediapipeHolisticTrackDetect");
		if (pMediapipeHolisticTrackingDetectFrameDirect != nullptr)
		{
			m_MediapipeHolisticTrackDetect = (FuncMediapipeHolisticTrackDetect)(pMediapipeHolisticTrackingDetectFrameDirect);
			if (m_MediapipeHolisticTrackDetect != nullptr)
			{

			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		void* pMediapipeHolisticTrackingRelease = m_DynamicDLLLoader.GetFunction("MediapipeHolisticTrackRelease");
		if (pMediapipeHolisticTrackingRelease != nullptr)
		{
			m_MediapipeHolisticTrackRelease = (FuncMediapipeHolisticTrackRelease)(pMediapipeHolisticTrackingRelease);
			if (m_MediapipeHolisticTrackRelease != nullptr)
			{

			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}
