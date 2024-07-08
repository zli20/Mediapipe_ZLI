#ifndef _HOLISTIC_TRACK_H_
#define _HOLISTIC_TRACK_H_

#include "DynamicDllLoad.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

// ���庯��ָ�����ͣ���Ӧ��̬���е�API
typedef bool (*FuncMediapipeHolisticTrackInit)(
	const char* model_path,
	bool is_need_video_outputstream,
	bool is_need_pose_outputstream,
	bool is_need_pose3d_outputstream,
	bool is_need_hand_outputstream,
	bool is_need_face_outputstream,
	bool is_debug);

typedef bool (*FuncMediapipeHolisticTrackDetect)(
	int image_width, 
	int image_height, 
	void* image_data,
	char*** keys_out,
	int* num_keys,
	float*** points_out,
	int** num_points,
	bool show_result_image,
	bool is_debug);

typedef bool (*FuncMediapipeHolisticTrackRelease)();

class HolisticTrack
{
public:
	HolisticTrack();
	~HolisticTrack();

public:
	// ���ؿ�
	bool LoadMediapipeHolisticTrackingDll(const std::string& dll_path);
	bool UnLoadMediapipeHolisticTrackingDll();
	bool GetAllFunctions();

public:
	// ��̬��API ����ָ��
	FuncMediapipeHolisticTrackInit m_MediapipeHolisticTrackInit;
	FuncMediapipeHolisticTrackDetect m_MediapipeHolisticTrackDetect;
	FuncMediapipeHolisticTrackRelease m_MediapipeHolisticTrackRelease;

private:
	// ��̬�������
	DynamicDllLoader m_DynamicDLLLoader;
};
#endif // !_HOLISTIC_TRACK_H_
