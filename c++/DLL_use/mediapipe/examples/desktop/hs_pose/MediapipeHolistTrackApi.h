#ifndef MEDIAPIPE_HOLIST_TRACK_API_H
#define MEDIAPIPE_HOLIST_TRACK_API_H

#define EXPORT

/* 定义动态链接库dll的导出 */
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
	@brief 初始化Google Mediapipe
	@param[in] model_path 需要加载的模型路径,pbtx文件
	@param[in] is_need_video_outputstream 是否在Graph中添加视频输出流
	@param[in] is_need_pose_outputstream 是否在Graph中添加pose输出流
	@param[in] is_need_pose3d_outputstream 是否在Graph中添加pose3d输出流
	@param[in] is_need_hand_outputstream 是否在Graph中添加hand输出流
	@param[in] is_need_face_outputstream 是否在Graph中添加face输出流
	@return 返回操作成功或者失败
	@note: pose输出流也包含了3d坐标，是头顶为参考坐标
	*/
	EXPORT_API bool MediapipeHolisticTrackInit(
		const char* model_path,
		bool is_need_video_outputstream = true,
		bool is_need_pose_outputstream = true,
		bool is_need_pose3d_outputstream = true,
		bool is_need_hand_outputstream = true,
		bool is_need_face_outputstream = true);

	/*
	@brief 检测图像
	@param[in] image_width 图像帧宽度
	@param[in] image_height 图像帧高度
	@param[in] image_data 图像帧数据
	@param[in] show_result_image 是否显示结果图片
	@param[out] holist_result - 关键点输出结果，以map形式给出
	@return 返回操作成功或者失败
	*/
	EXPORT_API bool MediapipeHolisticTrackiDetect(
		int image_width, 
		int image_height,
		void* image_data, 
		std::map<std::string, std::vector<float>>& holist_result, 
		bool show_result_image = false);

	/*
	@brief Google Mediapipe释放
	@return 返回操作成功或者失败
		0 失败
		1 成功
	*/
	EXPORT_API bool MediapipeHolisticTrackRelease();


#ifdef __cplusplus
}
#endif 


#endif // !MEDIAPIPE_HOLIST_TRACK_API_H
