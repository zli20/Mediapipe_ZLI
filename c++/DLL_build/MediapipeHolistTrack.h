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
	@brief 初始化Google Mediapipe
	@param[in] model_path 需要加载的模型路径,pbtx文件
	@param[in] is_need_video_outputstream 是否在Graph中添加视频输出流
	@param[in] is_need_pose_outputstream 是否在Graph中添加pose输出流
	@param[in] is_need_pose3d_outputstream 是否在Graph中添加pose3d输出流
	@param[in] is_need_hand_outputstream 是否在Graph中添加hand输出流
	@param[in] is_need_face_outputstream 是否在Graph中添加face输出流
	@param[in] is_debug 是否打印debug消息
	@return 返回操作成功或者失败
	@note: pose输出流也包含了3d坐标，是头顶为参考坐标
	*/
	bool InitModel(const char* model_path,
		bool is_need_video_outputstream = true,
		bool is_need_pose_outputstream = true,
		bool is_need_pose3d_outputstream = true,
		bool is_need_hand_outputstream = true,
		bool is_need_face_outputstream = true,
		bool is_debug = false);

	/*
	@brief 检测图像
	@param[in] image_width 图像帧宽度
	@param[in] image_height 图像帧高度
	@param[in] image_data 图像帧数据
	@param[in] show_result_image 是否显示结果图片
	@param[out] holist_result - 关键点输出结果，以map形式给出
	@param[in] is_debug 是否打印debug消息
	@return 返回操作成功或者失败
	*/
	bool DetectImage(
		int image_width, 
		int image_height, 
		void* image_data, 
		std::map<std::string, std::vector<float>> &holist_result, 
		bool show_result_image = false, 
		bool is_debug = false);

	/*
	@brief Google Mediapipe释放
	@return 返回操作成功或者失败
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
