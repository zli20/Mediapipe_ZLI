#include "MediapipeHolistTrackApi.h"
#include "MediapipeHolistTrack.h"
#include <map>
#include <string>
#include <vector>
#include <cstring>

MediapipeHolistTrack m_MediapipeHolistTrack;

EXPORT_API bool MediapipeHolisticTrackInit(
	const char* model_path, 
	bool is_need_video_outputstream, 
	bool is_need_pose_outputstream, 
	bool is_need_pose3d_outputstream, 
	bool is_need_hand_outputstream, 
	bool is_need_face_outputstream,
	bool is_debug)
{
	return m_MediapipeHolistTrack.InitModel(model_path, is_need_video_outputstream, is_need_pose_outputstream, is_need_pose3d_outputstream, is_need_hand_outputstream,is_need_face_outputstream, is_debug);
}

EXPORT_API bool MediapipeHolisticTrackDetect(
	int image_width, 
	int image_height, 
	void* image_data,
	char*** keys_out,
	int* num_keys,
	float*** points_out,
	int** num_points,
	bool show_result_image,
	bool is_debug)
{
	std::map<std::string, std::vector<float>>holist_result;
	if(m_MediapipeHolistTrack.DetectImage(image_width, image_height, image_data, holist_result, show_result_image, is_debug)){

		if (is_debug) {
			std::cout << "Num  of Keys: "  << holist_result.size() << std::endl;
		}

		// 分配并传递键的数组
		*num_keys = holist_result.size();
		char** keys = new char* [*num_keys];
		float** values = new  float* [*num_keys];
		int* num_values_arr = new int[*num_keys];

		int i = 0;
		for (const auto& kv : holist_result) {
			if (is_debug) {
				std::cout << "Keys Name: " << kv.first << std::endl;
				std::cout << "Keys Nums: " << kv.second.size() << std::endl;
			}
			// 复制键字符串
			char* key = new char[kv.first.size() + 1];
			std::strcpy(key, kv.first.c_str());
			keys[i] = key;

			num_values_arr[i] = kv.second.size();
			//for (int j = 0; j < kv.second.size(); j++) {
			//	std::cout << kv.second[j] << std::endl;
			//}
			// 复制值数组
			float* value_array = new float[kv.second.size()];
			std::memcpy(value_array, kv.second.data(), kv.second.size() * sizeof(float));
			values[i] = value_array;

			++i;
		}

		// 设置输出指针
		*keys_out = keys;
		*points_out = values;
		*num_points = num_values_arr;

		return true;
	}
	return false;
}


bool MediapipeHolisticTrackRelease()
{
	return m_MediapipeHolistTrack.Release();
}
