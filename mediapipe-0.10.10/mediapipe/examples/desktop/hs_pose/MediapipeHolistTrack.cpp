#include "MediapipeHolistTrack.h"

MediapipeHolistTrack::MediapipeHolistTrack()
{
	m_bIsInit = false;
	m_bIsRelease = false;

	m_Video_InputStreamName = "input_video";

	m_Video_OutputStreamName = "output_video";
	m_PoseLandmarks_OutputStreamName = "pose_landmarks";
	m_PoseWorldLandmarks_OutputStreamName = "pose_world_landmarks";
	m_LeftHandLandmarks_OutputStreamName = "left_hand_landmarks";
	m_RightHandLandmarks_OutputStreamName = "right_hand_landmarks";
	m_FaceLandmarks_OutputStreamName = "face_landmarks";

	m_pVideoPoller = nullptr;
	m_pPoseLandmarksPoller = nullptr;
	m_pPoseWorldLandmarksPoller = nullptr;
	m_pLeftHandLandmarksPoller = nullptr;
	m_pRightHandLandmarksPoller = nullptr;
	m_pFaceLandmarksPoller = nullptr;

	m_IsTrack_InputSidePacketName = "use_prev_landmarks";
}

MediapipeHolistTrack::~MediapipeHolistTrack()
{
	if (m_bIsInit && !m_bIsRelease)
	{
		Release();
	}
}

bool MediapipeHolistTrack::InitModel(
	const char* model_path, 
	bool is_need_video_outputstream, 
	bool is_need_pose_outputstream,
	bool is_need_pose3d_outputstream, 
	bool is_need_hand_outputstream, 
	bool is_need_face_outputstream,
	bool is_debug)
{
	absl::Status run_status = Mediapipe_InitGraph(model_path, is_need_video_outputstream, is_need_pose_outputstream, is_need_pose3d_outputstream, is_need_hand_outputstream, is_need_face_outputstream, is_debug);
	if (!run_status.ok()) {
		std::cout << "Failed to Init Mediapipe Graph!!" << std::endl;
		return false;
	}
	m_bIsInit = true;
	return  true;
}

bool MediapipeHolistTrack::DetectImage(
	int image_width, 
	int image_height, 
	void* image_data, 
	std::map<std::string, std::vector<float>> & holist_result,
	bool show_result_image,
	bool is_debug)
{
	if (!m_bIsInit) {
		std::cout << "Mediapipe Graph is Not Init!!" << std::endl;
		return false;
	}
	if (is_debug) {
		std::cout << "Mediapipe_RunMPPGrapht " << std::endl;
	}
	absl::Status run_status = Mediapipe_RunMPPGrapht(image_width, image_height, image_data, holist_result, show_result_image, is_debug);
	if (!run_status.ok()) {
		std::cout << "Failed to Detect !!" << std::endl;
		return false;
	}
	return true;
}

bool MediapipeHolistTrack::Release()
{
	absl::Status run_status = Mediapipe_ReleaseGraph();
	if (!run_status.ok()) {
		return false;
	}
	m_bIsRelease = true;
	return true;
}

absl::Status MediapipeHolistTrack::Mediapipe_InitGraph(
	const char* model_path, 
	bool is_need_video_outputstream, 
	bool is_need_pose_outputstream, 
	bool is_need_pose3d_outputstream, 
	bool is_need_hand_outputstream, 
	bool is_need_face_outputstream,
	bool is_debug)
{
	std::string calculator_graph_config_contents;
	MP_RETURN_IF_ERROR(mediapipe::file::GetContents(model_path, &calculator_graph_config_contents));

	if (is_debug) {
		std::cout << "mediapipe::file::GetContents Success" << std::endl;
	}

	mediapipe::CalculatorGraphConfig config =
		mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
			calculator_graph_config_contents);

	MP_RETURN_IF_ERROR(m_Graph.Initialize(config));

	if (is_debug) {
		std::cout << "m_Graph.Initialize(config) Success" << std::endl;
	}


	// 视频输出
	if (is_need_video_outputstream)
	{
		auto videoOutputStream = m_Graph.AddOutputStreamPoller(m_Video_OutputStreamName);
		assert(videoOutputStream.ok());
		m_pVideoPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(videoOutputStream.value()));

		if (is_debug) {
			std::cout << "Video_outputstream Add Success" << std::endl;
		}
	}

	// PoseLandmarks输出
	if (is_need_pose_outputstream)
	{
		mediapipe::StatusOrPoller poseLandmarks = m_Graph.AddOutputStreamPoller(m_PoseLandmarks_OutputStreamName);
		assert(poseLandmarks.ok());
		m_pPoseLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(poseLandmarks.value()));
		if (is_debug) {
			std::cout << "Pose_outputstream Add Success" << std::endl;
		}
	}

	// PoseLandmarks 世界坐标输出
	if (is_need_pose3d_outputstream)
	{
		mediapipe::StatusOrPoller poseWorldLandmarks = m_Graph.AddOutputStreamPoller(m_PoseWorldLandmarks_OutputStreamName);
		assert(poseWorldLandmarks.ok());
		m_pPoseWorldLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(poseWorldLandmarks.value()));
		if (is_debug) {
			std::cout << "Pose3d_outputstream Add Success" << std::endl;
		}
	}

	// LeftHandLandmarks输出
	if (is_need_hand_outputstream)
	{
		// 左手
		mediapipe::StatusOrPoller leftHandLandmarks = m_Graph.AddOutputStreamPoller(m_LeftHandLandmarks_OutputStreamName);
		assert(leftHandLandmarks.ok());
		m_pLeftHandLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(leftHandLandmarks.value()));
		if (is_debug) {
			std::cout << "Left Hand_outputstream Add Success" << std::endl;
		}
		// 右手
		mediapipe::StatusOrPoller rightHandLandmarks = m_Graph.AddOutputStreamPoller(m_RightHandLandmarks_OutputStreamName);
		assert(rightHandLandmarks.ok());
		m_pRightHandLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(rightHandLandmarks.value()));
		if (is_debug) {
			std::cout << "Right Hand_outputstream Add Success" << std::endl;
		}
	}

	// FaceLandmarks输出
	if (is_need_face_outputstream)
	{
		mediapipe::StatusOrPoller faceLandmarks = m_Graph.AddOutputStreamPoller(m_FaceLandmarks_OutputStreamName);
		assert(faceLandmarks.ok());
		m_pFaceLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(faceLandmarks.value()));
		if (is_debug) {
			std::cout << "Face_outputstream Add Success" << std::endl;
		}
	}

	// 设置图的输入流队列中最大可排队Packet数
	m_Graph.SetInputStreamMaxQueueSize(m_Video_InputStreamName, 1);
	// 设置图的输入流队列添加新包的参数
	m_Graph.SetGraphInputStreamAddMode(mediapipe::CalculatorGraph::GraphInputStreamAddMode::WAIT_TILL_NOT_FULL);

	if (is_debug) {
		std::cout << "SetInputStream success" << std::endl;
	}

	MP_RETURN_IF_ERROR(m_Graph.StartRun({}));
	if (is_debug) {
		std::cout << "----------------Graph StartRun Success---------------------" << std::endl;
	}

	return absl::OkStatus();
}

absl::Status MediapipeHolistTrack::Mediapipe_RunMPPGrapht(
	int image_width, 
	int image_height, 
	void* image_data, 
	std::map<std::string, std::vector<float>>& holist_result, 
	bool show_result_image,
	bool is_debug)
{
	if (is_debug) {
		std::cout << "Image PreProcess Start" << std::endl;
		//std::cout << "Map size before clear: " << holist_result.size() << std::endl;
		//std::cout << "enter Address of map before clear: " << &holist_result << std::endl;
		//std::cout << "enter Address of image before clear: " << &image_data << std::endl;
	}
	holist_result.clear();
	

	// 图像预处理
	cv::Mat camera_frame(cv::Size(image_width, image_height), CV_8UC3, (uchar*)image_data);
	cv::Mat camera_frame_RGB;
	cv::cvtColor(camera_frame, camera_frame_RGB, cv::COLOR_BGR2RGB);
	cv::flip(camera_frame_RGB, camera_frame_RGB, 1);
	if (is_debug) {
		cv::imshow("MediapipeHolistic", camera_frame_RGB);
		cv::waitKey(0);
		std::cout << "Image PreProcess finished" << std::endl;
	}

	// 转mediapipe::ImageFrame
	auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
		mediapipe::ImageFormat::SRGB, camera_frame_RGB.cols, camera_frame_RGB.rows,
		mediapipe::ImageFrame::kDefaultAlignmentBoundary);
	//camera_frame_RGB.copyTo(mediapipe::formats::MatView(input_frame.get()));
	cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
	camera_frame_RGB.copyTo(input_frame_mat);
	if (is_debug) {
		std::cout << "Trans to mediapipe::ImageFrame Success" << std::endl;
	}

	// 推理
	size_t frame_timestamp_us =
		(double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
	MP_RETURN_IF_ERROR(m_Graph.AddPacketToInputStream(
		m_Video_InputStreamName, mediapipe::Adopt(input_frame.release())
		.At(mediapipe::Timestamp(frame_timestamp_us))));
	if (is_debug) {
		std::cout << "Mediapipe Inference Success" << std::endl;
	}

	// 导出结果
	//----视频流------
	if (m_pVideoPoller != nullptr)
	{
		mediapipe::Packet videoPacket;
		//if (m_pVideoPoller->QueueSize() != 0)
		//{
			//if (!m_pVideoPoller->Next(&videoPacket))
			//{
			//	return absl::InvalidArgumentError("No VideoPacket Next");
			//}

			if (show_result_image)
			{
				// 从视频输出获取mediapipe::ImageFrame结果
				auto& output_frame = videoPacket.Get<mediapipe::ImageFrame>();
				if (is_debug) {
					std::cout << "Get Output Frame Success" << std::endl;
				}

				// 转换mediapipe::ImageFrame为cv::Mat
				cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
				if (is_debug) {
					std::cout << "Output Frame Mat Size:" << output_frame_mat.rows << " " <<output_frame_mat.cols<<std::endl;
				}				
				
				// 显示cv::Mat结果
				cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
				cv::imshow("MediapipeHolistic", output_frame_mat);
				cv::waitKey(0);
			}
		// }
		if (is_debug) {
			std::cout << "Get Output Video Packet Success" << std::endl;
		}
	}
	//----关键点------
	if (m_pPoseLandmarksPoller != nullptr)
	{
		mediapipe::Packet poseeLandmarksPacket;
		//if (m_pPoseLandmarksPoller->QueueSize() != 0)
		//{
			if (m_pPoseLandmarksPoller->Next(&poseeLandmarksPacket))
			{
				auto& output_landmarks = poseeLandmarksPacket.Get<mediapipe::NormalizedLandmarkList>();
				if (is_debug) {
					std::cout << "Get PoseLandmarks Size:" << output_landmarks.landmark_size() << std::endl;
				}

				std::vector<float> posePoints;
				posePoints.clear();

				for (int i = 0; i < output_landmarks.landmark_size(); ++i)
				{
					const mediapipe::NormalizedLandmark landmark = output_landmarks.landmark(i);
					float x = landmark.x() * camera_frame.cols;
					float y = landmark.y() * camera_frame.rows;
					float z = landmark.z();
					float visibility = landmark.visibility();
		/*			if (is_debug) 
					{
						std::cout << "Point:"<<  "x:"<< x << "y:"<< y << "z:"<< z <<"visibility:"<< visibility << std::endl;
					}*/
					posePoints.push_back(x);
					posePoints.push_back(y);
					posePoints.push_back(z);
					posePoints.push_back(visibility);
				}
				holist_result["PoseLandmarks"] = posePoints;
			}
		//}
		if (is_debug) {
			std::cout << "Get PoseLandmarks Packet Success" << std::endl;
		}
	}

	//----World关键点------
	if (m_pPoseWorldLandmarksPoller != nullptr)
	{
		mediapipe::Packet poseeWorldLandmarksPacket;

		if (m_pPoseWorldLandmarksPoller->QueueSize() != 0)
		{
			if (m_pPoseWorldLandmarksPoller->Next(&poseeWorldLandmarksPacket))
			{
				auto& output_landmarks = poseeWorldLandmarksPacket.Get<mediapipe::NormalizedLandmarkList>();	
				if (is_debug) {
					std::cout << "Get PoseWorldLandmarks Size:" << output_landmarks.landmark_size() << std::endl;
				}

				std::vector<float> posePoints;
				posePoints.clear();

				for (int i = 0; i < output_landmarks.landmark_size(); ++i)
				{
					const mediapipe::NormalizedLandmark landmark = output_landmarks.landmark(i);
					float x = landmark.x() * camera_frame.cols;
					float y = landmark.y() * camera_frame.rows;
					float z = landmark.z();
					float visibility = landmark.visibility();
					posePoints.push_back(x);
					posePoints.push_back(y);
					posePoints.push_back(z);
					posePoints.push_back(visibility);
				}
				holist_result["PoseWorldLandmarks"] = posePoints;
			}
		}
		if (is_debug) {
			std::cout << "Get PoseWorldLandmarks Packet Success" << std::endl;
		}
	}

	//----左手关键点------
	if (m_pLeftHandLandmarksPoller != nullptr)
	{
		mediapipe::Packet leftHandLandmarksPacket;
		if (m_pLeftHandLandmarksPoller->QueueSize() > 0)
		{
			if (m_pLeftHandLandmarksPoller->Next(&leftHandLandmarksPacket))
			{
				auto& output_landmarks = leftHandLandmarksPacket.Get<mediapipe::NormalizedLandmarkList>();
				if (is_debug) {
					std::cout << "Get LeftHandLandmarks Size:" << output_landmarks.landmark_size() << std::endl;
				}

				std::vector<float> posePoints;
				posePoints.clear();

				for (int i = 0; i < output_landmarks.landmark_size(); ++i)
				{
					const mediapipe::NormalizedLandmark landmark = output_landmarks.landmark(i);
					float x = landmark.x() * camera_frame.cols;
					float y = landmark.y() * camera_frame.rows;
					float z = landmark.z();
					float visibility = landmark.visibility();
					posePoints.emplace_back(x);
					posePoints.emplace_back(y);
					posePoints.emplace_back(z);
					posePoints.emplace_back(visibility);
				}
				holist_result["LeftHandLandmarks"] = posePoints;
			}
		}
		if (is_debug) {
			std::cout << "Get LeftHandLandmarks Packet Success" << std::endl;
		}
	}

	//----右手关键点------
	if (m_pRightHandLandmarksPoller != nullptr)
	{
		mediapipe::Packet rightHandLandmarksPacket;

		if (m_pRightHandLandmarksPoller->QueueSize() > 0)
		{
			if (m_pRightHandLandmarksPoller->Next(&rightHandLandmarksPacket))
			{
				auto& output_landmarks = rightHandLandmarksPacket.Get<mediapipe::NormalizedLandmarkList>();
				if (is_debug) {
					std::cout << "Get RightHandLandmarks Size:" << output_landmarks.landmark_size() << std::endl;
				}

				std::vector<float> posePoints;
				posePoints.clear();

				for (int i = 0; i < output_landmarks.landmark_size(); ++i)
				{
					const mediapipe::NormalizedLandmark landmark = output_landmarks.landmark(i);
					float x = landmark.x() * camera_frame.cols;
					float y = landmark.y() * camera_frame.rows;
					float z = landmark.z();
					float visibility = landmark.visibility();
					posePoints.emplace_back(x);
					posePoints.emplace_back(y);
					posePoints.emplace_back(z);
					posePoints.emplace_back(visibility);
				}
				holist_result["RightHandLandmarks"] = posePoints;
			}
		}
		if (is_debug) {
			std::cout << "Get RightHandLandmarks Packet Success" << std::endl;
		}
	}

	//----脸部关键点------
	if (m_pFaceLandmarksPoller != nullptr)
	{
		mediapipe::Packet faceLandmarksPacket;
		if (m_pFaceLandmarksPoller->QueueSize() > 0)
		{
			if (m_pFaceLandmarksPoller->Next(&faceLandmarksPacket))
			{
				auto& output_landmarks = faceLandmarksPacket.Get<mediapipe::NormalizedLandmarkList>();
				if (is_debug) {
					std::cout << "Get FaceLandmarks Size:" << output_landmarks.landmark_size() << std::endl;
				}
				std::vector<float> posePoints;
				posePoints.clear();

				for (int i = 0; i < output_landmarks.landmark_size(); ++i)
				{
					const mediapipe::NormalizedLandmark landmark = output_landmarks.landmark(i);
					float x = landmark.x() * camera_frame.cols;
					float y = landmark.y() * camera_frame.rows;
					float z = landmark.z();
					float visibility = landmark.visibility();
					posePoints.emplace_back(x);
					posePoints.emplace_back(y);
					posePoints.emplace_back(z);
					posePoints.emplace_back(visibility);
				}
				holist_result["FaceLandmarks"] = posePoints;
			}
		}
		if (is_debug) {
			std::cout << "Get FaceLandmarks Packet Success" << std::endl;
		}
	}

	return absl::OkStatus();
}

absl::Status MediapipeHolistTrack::Mediapipe_ReleaseGraph()
{
	MP_RETURN_IF_ERROR(m_Graph.CloseInputStream(m_Video_InputStreamName));
	return m_Graph.WaitUntilDone();
}
