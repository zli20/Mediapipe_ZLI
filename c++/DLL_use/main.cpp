#include <iostream>
#include <opencv2/opencv.hpp>

#include "HolisticTrack.h"

#include <opencv2/core/utils/logger.hpp>
// opencv 不输出log
void OpencvConfigureLogging() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
	//cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);//只输出错误日志
}

void detect_img(HolisticTrack & holitic, cv::Mat img) {

	cv::Mat copyMat = img.clone();
	cv::flip(copyMat, copyMat, 1);
	uchar* pImageData = copyMat.data;


	char** keys = nullptr;
	int num_keys = 0;
	float** values = nullptr;
	int* num_values = nullptr;

	if (!holitic.m_MediapipeHolisticTrackDetect(copyMat.cols, copyMat.rows, (void*)pImageData, &keys, &num_keys, &values, &num_values, true, false)) {
		std::cout << "检测失败" << std::endl;
		return ;
	}
	std::cout << "Number of keys: " << num_keys << std::endl;
	for (int i = 0; i < num_keys; ++i) {
		std::cout << "Key " << i << ": " << keys[i] << std::endl;

		// 输出对应的值数组
		int num_values_for_key = num_values[i];
		std::cout << "Number of values for key " << i << ": " << num_values_for_key << std::endl;
		const float* values_for_key = values[i];
		for (int j = 0; j < num_values_for_key / 4; ++j) {

			float x = values_for_key[j * 4 + 0];
			float y = values_for_key[j * 4 + 1];
			float z = values_for_key[j * 4 + 2];
			float visual = values_for_key[j * 4 + 3];
			// std::cout << "visual " << j << ": " << visual << std::endl;
			if (visual > 0.5) {
				cv::circle(img, cv::Point(x, y), 3, cv::Scalar(255, 0, 0), 5);
			}

		}
		//delete[] values_for_key;
	}
	//for (int i = 0; i < num_keys; ++i) {
	//	delete[] keys[i];
	//}
	// 释放动态分配的内存
	//delete[] keys;
	//delete[] values;
	//delete[] num_values;

	cv::imshow("pose", img);;
}


int main() {
	OpencvConfigureLogging();


	std::string dll_path = "./data/hs_holistic_track.dll";
	HolisticTrack holitic;
	holitic.LoadMediapipeHolisticTrackingDll(dll_path);
	holitic.GetAllFunctions();

	std::string mediapipe_hand_tracking_model_path = "./data/holistic_tracking_cpu.pbtxt";
	//std::string mediapipe_hand_tracking_model_path = "./holistic_landmark_cpu.pbtxt";
	if (holitic.m_MediapipeHolisticTrackInit(mediapipe_hand_tracking_model_path.c_str(), false, true, false, false, false, false))
	{
		std::cout << "初始化模型成功" << std::endl;
	}
	else
	{
		std::cout << "初始化模型失败" << std::endl;
	}

#if 0
	std::string video_path = "./data/taiji.mp4";
	//cv::VideoCapture video_reader(0);
	cv::VideoCapture video_reader(video_path);
	int w = video_reader.get(cv::CAP_PROP_FRAME_WIDTH);   // frame width
	int h = video_reader.get(cv::CAP_PROP_FRAME_HEIGHT);  // frame height
	int fps = video_reader.get(cv::CAP_PROP_FPS);         // fps
	int ttf = video_reader.get(cv::CAP_PROP_FRAME_COUNT); // total frame

	while (video_reader.isOpened())
	{
		cv::Mat img;
		video_reader >> img;

		if (img.empty())
			break;
		//cv::imshow("aaaa", img);
		//cv::waitKey(1);
		detect_img(holitic, img);
		if (cv::waitKey(1) >= 0)
			break;
	}

	video_reader.release();
	cv::destroyAllWindows();
#endif

#if 1
	std::string path = "./data/R.jpg";
	cv::Mat img = cv::imread(path);
	detect_img(holitic, img);
	cv::waitKey(0);
#endif
	//system("pause");
	return 0;
}
