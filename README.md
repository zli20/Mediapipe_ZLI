# Mediapipe 
记录mediapie 在winows下编译c++动态库DLL过程中环的问题
## python
安装：
```
pip install mediapipe
``` 
运行 `poseLandmark.py` 使用

## windows-VS-c++编译
### 环境安装和编译
参考以下链接：  
https://stubbornhuang.blog.csdn.net/article/details/119546019  
https://blog.csdn.net/HW140701/article/details/119675282  
官方文档：  
https://github.com/google-ai-edge/mediapipe/blob/master/docs/getting_started/install.md  
https://github.com/google-ai-edge/mediapipe/blob/master/docs/getting_started/cpp.md  
编译遇到过以下问题：  
1. bazel版本最好使用6.5及以上（目前使用的是6.5）;  
2. bazel环境配置时要注意：  
BAZEL_WINSDK_FULL_VERSION WINSDK的版本号在"程序和功能"里边可能有多个，可以通过下面两个方式确认：  
在C:\Program Files (x86)\Windows Kits\10\bin 有当前安装的所有winsdk版本号。  
![winsdk版本](./sources/1.png")
在Visual Stdio Installer ->　vs2019 -> 修改 -> 使用c++的桌面开发中可以查询当前使用的版本号。  
![vswinsdk版本](./sources/2.png")
3. 如果环境都配置好了，安装了多个版本vs时候，编译时指向的是其他版本，可能就是bazel环境配置有问题（我自己在编译时，编译时一直指向vs2022的编译器），建议把这几个环境变量直接加到系统环境变量中。  
在编译过程出现找不到 windows.h 或其他头文件的问题，可能是bazel没有找到winsdk的路径，需要设置 BAZEL_WINSDK变量，同样添加到系统变量中。  
![环境变量](./sources/3.png")
4. 编译helloworld可以正常运行，但是其他demo时，出现 `tensorflow/lite/core/c/operator.cc(28): error C7555: 使用指定的初始值设定项至少需要“/std:c++20”`,是库依赖版本的问题。  
解决方法是使用其他的版本的mediapipe。（我试了git clone 和0.10.13都有这个问题，最终使用0.10.10运行成功）。  
### 封装dll并使用
主要参考的是这位大佬的代码，https://github.com/HW140701/GoogleMediapipePackageDll  
对holistic_track关键点封装成动态库，供c++ 调用，使用过程如下：  
1. 接口设计：详见 MediapipeHolistTrack.h MediapipeHolistTrack.cpp MediapipeHolistTrackApi.h MediapipeHolistTrackApi.cpp
2. 编译：在Mediapipe仓库目录mediapipe\mediapipe\examples\desktop下新建一个名为hs_pose的文件夹，把上述代码，和Build文件拷贝过去，通过编译命令进行编译DLL动态库。（注意python环境变量和程序路径）
 ```
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 --action_env PYTHON_BIN_PATH="D:\\anconda\\envs\\mediapipe\\python.exe" mediapipe/examples/desktop/hS_pose:hs_holistic_track --verbose_failures
```
3. 根据DLL_use中的代码调用即可。程序运行时要把meidiapipe仓库和openccv依赖拷贝到到程序的运行目录下。  

封装和使用过程遇到的问题：
1. 下载的库中没有模型。本人没有找到下载链接，所以是从从python版本的库中拷贝过来的，位于 `D:\anconda\envs\mediapipe\Lib\site-packages\mediapipe\modules`
2. 在设计接口的时候，MediapipeHolistTrackApi.h的时候，原本是想要直接把关键点输出传入map中得到输出，如下：
```
	EXPORT_API bool MediapipeHolisticTrackDetect(
		int image_width, 
		int image_height,
		void* image_data, 
        std::map<std::string, std::vector<float>> result,
		bool show_result_image = false,
		bool is_debug = false);
```
但是在调用时发现，动态库中无法访问result，并且传入和传出的result地址也不一样。  
原因是：动态库使用bazel编译，而在调用的时候用vs的编译器，不同编译器之间的内存管理分配上存在不一致，导致ABI兼容性问题，因此使用c类型指针传出结果。最终如下：
```
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
```

