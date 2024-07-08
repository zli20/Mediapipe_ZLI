#ifndef MEDIAPIPE_HOLIST_TRACK_H
#define MEDIAPIPE_HOLIST_TRACK_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"

#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"

class  MediapipeHolistTrack
{
public:
	MediapipeHolistTrack();
	~MediapipeHolistTrack();

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
	bool InitModel(const char* model_path,
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
	@param[out] holist_result - �ؼ�������������map��ʽ����
	@param[in] is_debug �Ƿ��ӡdebug��Ϣ
	@return ���ز����ɹ�����ʧ��
	*/
	bool DetectImage(
		int image_width, 
		int image_height, 
		void* image_data, 
		std::map<std::string, std::vector<float>> &holist_result, 
		bool show_result_image = false, 
		bool is_debug = false);

	/*
	@brief Google Mediapipe�ͷ�
	@return ���ز����ɹ�����ʧ��
	*/
	bool Release();

private:
	absl::Status Mediapipe_InitGraph(
		const char* model_path,
		bool is_need_video_outputstream = true,
		bool is_need_pose_outputstream = true,
		bool is_need_pose3d_outputstream = true,
		bool is_need_hand_outputstream = true,
		bool is_need_face_outputstream = true,
		bool is_debug = false);

	absl::Status Mediapipe_RunMPPGrapht(
		int image_width, 
		int image_height, 
		void* image_data, 
		std::map<std::string, std::vector<float >> &holist_result, 
		bool show_result_image = false,
		bool is_debug = false);

	absl::Status Mediapipe_ReleaseGraph();

private:
	bool m_bIsInit;
	bool m_bIsRelease;

	mediapipe::CalculatorGraph m_Graph;

	const char* m_Video_InputStreamName;

	const char* m_Video_OutputStreamName;
	const char* m_PoseLandmarks_OutputStreamName;
	const char* m_PoseWorldLandmarks_OutputStreamName;
	const char* m_LeftHandLandmarks_OutputStreamName;
	const char* m_RightHandLandmarks_OutputStreamName;
	const char* m_FaceLandmarks_OutputStreamName;

	const char* m_IsTrack_InputSidePacketName;

	std::unique_ptr<mediapipe::OutputStreamPoller> m_pVideoPoller;
	std::unique_ptr<mediapipe::OutputStreamPoller> m_pPoseLandmarksPoller;
	std::unique_ptr<mediapipe::OutputStreamPoller> m_pPoseWorldLandmarksPoller;
	std::unique_ptr<mediapipe::OutputStreamPoller> m_pLeftHandLandmarksPoller;
	std::unique_ptr<mediapipe::OutputStreamPoller> m_pRightHandLandmarksPoller;
	std::unique_ptr<mediapipe::OutputStreamPoller> m_pFaceLandmarksPoller;
};
#endif // !MEDIAPIPE_HOLIST_TRACK_H
