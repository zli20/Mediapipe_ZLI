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

bool MediapipeHolistTrack::InitModel(const char* model_path, bool is_need_video_outputstream, bool is_need_pose_outputstream, bool is_need_pose3d_outputstream, bool is_need_hand_outputstream, bool is_need_face_outputstream)
{
	absl::Status run_status = Mediapipe_InitGraph(model_path, is_need_video_outputstream, is_need_pose_outputstream, is_need_pose3d_outputstream, is_need_hand_outputstream, is_need_face_outputstream);
	if (!run_status.ok()) {
		std::cout << "Failed to Init Mediapipe Graph!!" << std::endl;
		return false;
	}
	m_bIsInit = true;
	return  true;
}

bool MediapipeHolistTrack::DetectImage(int image_width, int image_height, void* image_data, std::map<std::string, std::vector<float>> & holist_result, bool show_result_image)
{
	if (!m_bIsInit) {
		std::cout << "Mediapipe Graph is Not Init!!" << std::endl;
		return false;
	}
	absl::Status run_status = Mediapipe_RunMPPGrapht(image_width, image_height, image_data, holist_result, show_result_image);
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

absl::Status MediapipeHolistTrack::Mediapipe_InitGraph(const char* model_path, bool is_need_video_outputstream, bool is_need_pose_outputstream, bool is_need_pose3d_outputstream, bool is_need_hand_outputstream, bool is_need_face_outputstream)
{
	std::string calculator_graph_config_contents;
	MP_RETURN_IF_ERROR(mediapipe::file::GetContents(model_path, &calculator_graph_config_contents));
	std::cout << "mediapipe::file::GetContents success" << std::endl;

	mediapipe::CalculatorGraphConfig config =
		mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
			calculator_graph_config_contents);

	MP_RETURN_IF_ERROR(m_Graph.Initialize(config));
	std::cout << "m_Graph.Initialize(config) success" << std::endl;

	// 视频输出
	if (is_need_video_outputstream)
	{
		auto videoOutputStream = m_Graph.AddOutputStreamPoller(m_Video_OutputStreamName);
		assert(videoOutputStream.ok());
		m_pVideoPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(videoOutputStream.value()));
	}

	// PoseLandmarks输出
	if (is_need_pose_outputstream)
	{
		mediapipe::StatusOrPoller poseLandmarks = m_Graph.AddOutputStreamPoller(m_PoseLandmarks_OutputStreamName);
		assert(poseLandmarks.ok());
		m_pPoseLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(poseLandmarks.value()));
	}

	// PoseLandmarks 世界坐标输出
	if (is_need_pose3d_outputstream)
	{
		mediapipe::StatusOrPoller poseWorldLandmarks = m_Graph.AddOutputStreamPoller(m_PoseWorldLandmarks_OutputStreamName);
		assert(poseWorldLandmarks.ok());
		m_pPoseWorldLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(poseWorldLandmarks.value()));
	}

	// LeftHandLandmarks输出
	if (is_need_hand_outputstream)
	{
		// 左手
		mediapipe::StatusOrPoller leftHandLandmarks = m_Graph.AddOutputStreamPoller(m_LeftHandLandmarks_OutputStreamName);
		assert(leftHandLandmarks.ok());
		m_pLeftHandLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(leftHandLandmarks.value()));
		// 右手
		mediapipe::StatusOrPoller rightHandLandmarks = m_Graph.AddOutputStreamPoller(m_RightHandLandmarks_OutputStreamName);
		assert(rightHandLandmarks.ok());
		m_pRightHandLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(rightHandLandmarks.value()));
	}

	// FaceLandmarks输出
	if (is_need_face_outputstream)
	{
		mediapipe::StatusOrPoller faceLandmarks = m_Graph.AddOutputStreamPoller(m_FaceLandmarks_OutputStreamName);
		assert(faceLandmarks.ok());
		m_pFaceLandmarksPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(faceLandmarks.value()));
	}

	// 设置图的输入流队列中最大可排队Packet数
	m_Graph.SetInputStreamMaxQueueSize(m_Video_InputStreamName, 1);
	// 设置图的输入流队列添加新包的参数
	m_Graph.SetGraphInputStreamAddMode(mediapipe::CalculatorGraph::GraphInputStreamAddMode::WAIT_TILL_NOT_FULL);

	MP_RETURN_IF_ERROR(m_Graph.StartRun({}));
	std::cout << "----------------Graph StartRun Success---------------------" << std::endl;
	return absl::OkStatus();
}

absl::Status MediapipeHolistTrack::Mediapipe_RunMPPGrapht(int image_width, int image_height, void* image_data, std::map<std::string, std::vector<float>>& holist_result, bool show_result_image)
{
	holist_result.clear();
	// 图像预处理
	cv::Mat camera_frame(cv::Size(image_width, image_height), CV_8UC3, (uchar*)image_data);
	cv::Mat camera_frame_RGB;
	cv::cvtColor(camera_frame, camera_frame_RGB, cv::COLOR_BGR2RGB);
	cv::flip(camera_frame_RGB, camera_frame_RGB, 1);

	// 转mediapipe::ImageFrame
	auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
		mediapipe::ImageFormat::SRGB, camera_frame_RGB.cols, camera_frame_RGB.rows,
		mediapipe::ImageFrame::kDefaultAlignmentBoundary);
	//camera_frame_RGB.copyTo(mediapipe::formats::MatView(input_frame.get()));
	cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
	camera_frame_RGB.copyTo(input_frame_mat);

	// 推理
	size_t frame_timestamp_us =
		(double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
	MP_RETURN_IF_ERROR(m_Graph.AddPacketToInputStream(
		m_Video_InputStreamName, mediapipe::Adopt(input_frame.release())
		.At(mediapipe::Timestamp(frame_timestamp_us))));

	// 导出结果
	//----视频流------
	if (m_pVideoPoller != nullptr)
	{
		mediapipe::Packet videoPacket;
		if (m_pVideoPoller->QueueSize() != 0)
		{
			if (!m_pVideoPoller->Next(&videoPacket))
			{
				return absl::InvalidArgumentError("no next packet");
			}

			if (show_result_image)
			{
				// 从视频输出获取mediapipe::ImageFrame结果
				auto& output_frame = videoPacket.Get<mediapipe::ImageFrame>();

				// 转换mediapipe::ImageFrame为cv::Mat
				cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);

				// 显示cv::Mat结果
				cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
				cv::imshow("MediapipeHolistic", output_frame_mat);
			}
		}
	}
	//----关键点------
	if (m_pPoseLandmarksPoller != nullptr)
	{
		mediapipe::Packet poseeLandmarksPacket;

		if (m_pPoseLandmarksPoller->QueueSize() != 0)
		{
			if (m_pPoseLandmarksPoller->Next(&poseeLandmarksPacket))
			{
				auto& output_landmarks = poseeLandmarksPacket.Get<mediapipe::NormalizedLandmarkList>();
				//std::cout << "PoseLandmarks size:" << output_landmarks.landmark_size() << std::endl;

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
				holist_result["PoseLandmarks"] = posePoints;
			}
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
				//std::cout << "PoseLandmarks size:" << output_landmarks.landmark_size() << std::endl;

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
				//std::cout << "LeftHandLandmarks size:" << output_landmarks.landmark_size() << std::endl;

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
				//std::cout << "RightHandLandmarks size:" << output_landmarks.landmark_size() << std::endl;

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
	}

	return absl::OkStatus();
}

absl::Status MediapipeHolistTrack::Mediapipe_ReleaseGraph()
{
	MP_RETURN_IF_ERROR(m_Graph.CloseInputStream(m_Video_InputStreamName));
	return m_Graph.WaitUntilDone();
}
