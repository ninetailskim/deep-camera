// openNI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "OpenNI.h"
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#define SAMPLE_READ_WAIT_TIMEOUT 2000

using namespace openni;
using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	Status rc = OpenNI::initialize();

	if (rc != STATUS_OK) {
		cout << "init" << OpenNI::getExtendedError() << endl;
		return 1;
	}

	Device device;

	if (argc < 2)
		rc = device.open(ANY_DEVICE);
	else
	{
		rc = device.open(argv[1]);
	}

	if (rc != STATUS_OK) {
		cout << "devices open" << OpenNI::getExtendedError() << endl;
		return 1;
	}
	else {
		cout << "get~" << endl;
	}

	VideoStream depth;

	if (device.getSensorInfo(SENSOR_DEPTH) != NULL) {
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK) {
			cout << "depth stream" << OpenNI::getExtendedError() << endl;
			return 1;
		}
	}

	rc = depth.start();

	if (rc != STATUS_OK){
		cout << "depth stream" << OpenNI::getExtendedError() << endl;
		return 1;
	}

	VideoFrameRef frame;

	while (true) {
		int changedStreamDummy;
		VideoStream* pStream = &depth;


		rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK) {
			cout << "wait" << OpenNI::getExtendedError() << endl;
			return 1;
		}

		rc = depth.readFrame(&frame);
		if(rc != STATUS_OK) {
			cout << "read" << OpenNI::getExtendedError() << endl;
			return 1;
		}

		if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM) {
			cout << "unexpected" << endl;
			continue;
		}
		Mat cv_frame;
		Mat depth_image;
		DepthPixel* pDepth = (DepthPixel*)frame.getData();
			
		cv_frame = Mat(frame.getHeight(), frame.getWidth(), CV_16UC1, pDepth).clone();
		//cv_frame.convertTo(depth_image, CV_8U, 255.0 / 8000);
		imshow("1", cv_frame);
		waitKey(1);

		int middleIndex = (frame.getHeight() + 1)* frame.getWidth() / 2;

		cout << (long long)frame.getTimestamp() << "    " << pDepth[middleIndex] << endl;
	}

	depth.stop();
	depth.destroy();
	device.close();
	OpenNI::shutdown();

    std::cout << "Hello World!\n"; 
	
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
