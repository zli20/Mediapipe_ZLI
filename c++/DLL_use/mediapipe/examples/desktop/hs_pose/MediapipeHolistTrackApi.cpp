#include "MediapipeHolistTrackApi.h"
#include "MediapipeHolistTrack.h"

MediapipeHolistTrack m_MediapipeHolistTrack;

EXPORT_API bool MediapipeHolisticTrackInit(
	const char* model_path, 
	bool is_need_video_outputstream, 
	bool is_need_pose_outputstream, 
	bool is_need_pose3d_outputstream, 
	bool is_need_hand_outputstream, 
	bool is_need_face_outputstream)
{
	return m_MediapipeHolistTrack.InitModel(model_path, is_need_video_outputstream, is_need_pose_outputstream, is_need_pose3d_outputstream, is_need_hand_outputstream,is_need_face_outputstream);
}

EXPORT_API bool MediapipeHolisticTrackiDetect(
	int image_width, 
	int image_height, 
	void* image_data, 
	std::map<std::string, std::vector<float>>& holist_result, 
	bool show_result_image)
{
	return m_MediapipeHolistTrack.DetectImage(image_width, image_height, image_data, holist_result, show_result_image);
}


bool MediapipeHolisticTrackRelease()
{
	return m_MediapipeHolistTrack.Release();
}
