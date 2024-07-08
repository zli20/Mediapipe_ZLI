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
	@param[in] is_debug �Ƿ��ӡdebug��Ϣ
	@return ���ز����ɹ�����ʧ��
	@note: pose�����Ҳ������3d���꣬��ͷ��Ϊ�ο�����
	*/
	EXPORT_API bool MediapipeHolisticTrackInit(
		const char* model_path,
		bool is_need_video_outputstream = true,
		bool is_need_pose_outputstream = true,
		bool is_need_pose3d_outputstream = true,
		bool is_need_hand_outputstream = true,
		bool is_need_face_outputstream = true,
		bool is_debug = false);

	/*
	@brief ���ͼ��
	@param[in] image_width ͼ��֡���
	@param[in] image_height ͼ��֡�߶�
	@param[in] image_data ͼ��֡����
	@param[in] show_result_image �Ƿ���ʾ���ͼƬ
	@param[out] keys_out �ؼ������� pose��face��hand
	@param[out] num_keys ����ؼ���������Ŀ�������ֲ�һ��ȫ�����
	@param[out] points_out �ؼ������꣬��x y z visual ����
	@param[out] num_points ÿ��ؼ���������һ������� pose->33*4  face->468*4  hand->21*4 (���Ҹ�һֻ) 
	@return ���ز����ɹ�����ʧ��
	ע��ֱ��ʹ��std::map���ݻᱨ����Ϊ��������ͬ�ģ���̬���������֮�����ڴ��������ϴ��ڲ�һ�£�����ABI���������⣬��˽��������c����ָ��
	*/
	EXPORT_API bool MediapipeHolisticTrackDetect(
		int image_width, 
		int image_height,
		void* image_data, 
		char*** keys_out,
		int* num_keys,
		float*** points_out,
		int** num_points,
		bool show_result_image = false,
		bool is_debug = false);

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
