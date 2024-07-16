#ifndef MEDIAPIPE_HOLIST_TRACK_API_H
#define MEDIAPIPE_HOLIST_TRACK_API_H

#define EXPORT

/* ���嶯̬���ӿ�dll�ĵ��� */
#include <malloc.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#ifdef _WIN32
#ifdef EXPORT
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif
#else
#include <stdlib.h>

#ifdef EXPORT
#define EXPORT_API __attribute__((visibility ("default")))
#else
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef EXPORT_API
#define EXPORT_API
#endif

	/*
	@brief ��ʼ��Google Mediapipe
	@param[in] model_path ��Ҫ���ص�ģ��·��,pbtx�ļ�
	@param[in] is_need_video_outputstream �Ƿ���Graph�������Ƶ�����
	@param[in] is_need_pose_outputstream �Ƿ���Graph�����pose�����
	@param[in] is_need_pose3d_outputstream �Ƿ���Graph�����pose3d�����
	@param[in] is_need_hand_outputstream �Ƿ���Graph�����hand�����
	@param[in] is_need_face_outputstream �Ƿ���Graph�����face�����
	@return ���ز����ɹ�����ʧ��
	@note: pose�����Ҳ������3d���꣬��ͷ��Ϊ�ο�����
	*/
	EXPORT_API bool MediapipeHolisticTrackInit(
		const char* model_path,
		bool is_need_video_outputstream = true,
		bool is_need_pose_outputstream = true,
		bool is_need_pose3d_outputstream = true,
		bool is_need_hand_outputstream = true,
		bool is_need_face_outputstream = true);

	/*
	@brief ���ͼ��
	@param[in] image_width ͼ��֡���
	@param[in] image_height ͼ��֡�߶�
	@param[in] image_data ͼ��֡����
	@param[in] show_result_image �Ƿ���ʾ���ͼƬ
	@param[out] holist_result - �ؼ�������������map��ʽ����
	@return ���ز����ɹ�����ʧ��
	*/
	EXPORT_API bool MediapipeHolisticTrackiDetect(
		int image_width, 
		int image_height,
		void* image_data, 
		std::map<std::string, std::vector<float>>& holist_result, 
		bool show_result_image = false);

	/*
	@brief Google Mediapipe�ͷ�
	@return ���ز����ɹ�����ʧ��
		0 ʧ��
		1 �ɹ�
	*/
	EXPORT_API bool MediapipeHolisticTrackRelease();


#ifdef __cplusplus
}
#endif 


#endif // !MEDIAPIPE_HOLIST_TRACK_API_H
