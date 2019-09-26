// openNI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "OpenNI.h"
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <ctime>
#include <windows.h>

#define SAMPLE_READ_WAIT_TIMEOUT 2000

using namespace openni;
using namespace cv;
using namespace std;

string lltoString(long long t) {
	std::string result;
	std::stringstream ss;
	ss << t;
	ss >> result;
	return result;
}

string mknowfolder() {
	time_t now = time(0);
	tm *ltm = localtime(&now);

	string folderPath = to_string(ltm->tm_mon) + "-" + to_string(ltm->tm_mday) + "-" + to_string(ltm->tm_hour) + "-" + to_string(ltm->tm_min) + "-" + to_string(ltm->tm_sec);
	string command ;
	command = "mkdir " + folderPath;
	system(command.c_str());

	return folderPath;
}


int main(int argc, char* argv[])
{	
	
	/*if (!GetFileAttributesA(folderPath.c_str()) & FILE_ATTRIBUTE_DIRECTORY) {
		bool flag = CreateDirectory(folderPath.c_str(), NULL);
	}
	else {
		cout << "Directory already exists." << endl;
	}*/

	string folderpath = mknowfolder();

	long long frameNum = 0;
	Status rc = OpenNI::initialize(), rcc;

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

	device.setDepthColorSyncEnabled(true);
	VideoStream depth;
	VideoStream color;
	if (device.getSensorInfo(SENSOR_DEPTH) != NULL) {
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK) {
			cout << "depth stream" << OpenNI::getExtendedError() << endl;
			return 1;
		}
		else {
			VideoMode mModeDepth;
			
			mModeDepth.setResolution(640, 480);
			mModeDepth.setFps(30);
			mModeDepth.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
			depth.setVideoMode(mModeDepth);
			rc = depth.start();
			if (rc != STATUS_OK) {
				cout << "depth stream" << OpenNI::getExtendedError() << endl;
				return 1;
			}
		}
	}

	if (device.getSensorInfo(SENSOR_COLOR) != NULL) {
		rc = color.create(device, SENSOR_COLOR);
		if (rc != STATUS_OK) {
			cout << "color stream" << OpenNI::getExtendedError() << endl;
			return 1;
		}
		else {
			VideoMode mModeColor;
			mModeColor.setResolution(640, 480);
			mModeColor.setFps(30);
			mModeColor.setPixelFormat(PIXEL_FORMAT_RGB888);
			color.setVideoMode(mModeColor);
			rc = color.start();
			if (rc != STATUS_OK) {
				cout << "color stream" << OpenNI::getExtendedError() << endl;
				return 1;
			}
		}
	}	

	if (!color.isValid() || !depth.isValid()) {
		cout << "valid" << endl;
		OpenNI::shutdown();
		return 1;
	}

	if (device.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR)) {
		device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	}

	namedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
	namedWindow("Color Image", CV_WINDOW_AUTOSIZE);

	int iMaxDepth = depth.getMaxPixelValue();
	cout << iMaxDepth << endl;
	VideoFrameRef frameDepth, frameColor;

	VideoStream* streams[] = { &depth ,&color};

	Mat mScaledDepth, hScaledDepth;
	Mat cImageBGR, bImageBGR, hImageBGR;
	while (true) {

		char cframeNum[10];

		sprintf(cframeNum, "%08I64d", frameNum);

		string sframeNum = cframeNum;

		/*int changedStreamDummy = -1;

		rc = OpenNI::waitForAnyStream(streams, 2, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK) {
			cout << "wait" << OpenNI::getExtendedError() << endl;
			return 1;
		}*/

		rc = depth.readFrame(&frameDepth);
		rcc = color.readFrame(&frameColor);

		clock_t start, ends;;

		if (rc != STATUS_OK) {
			cout << "read" << OpenNI::getExtendedError() << endl;
			return 1;
		}
		else {
			start = clock();
			const Mat mImageDepth(frameDepth.getHeight(), frameDepth.getWidth(), CV_16UC1, (void*)frameDepth.getData());

			
			mImageDepth.convertTo(mScaledDepth, CV_8U, 255.0 / iMaxDepth);

			cv::flip(mScaledDepth, mScaledDepth, 1);
			imshow("Depth Image", mScaledDepth);

			imwrite(folderpath + "/" + sframeNum + "-depth.jpg", mScaledDepth);
			
			ends = clock();

			std::cout << ends - start <<std::endl;
		}

		/*if (frameDepth.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frameDepth.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM) {
			cout << "unexpected" << endl;
			continue;
		}
		Mat cv_frame;
		Mat depth_image;
		DepthPixel* pDepth = (DepthPixel*)frameDepth.getData();

		cv_frame = Mat(frameDepth.getHeight(), frameDepth.getWidth(), CV_16UC1, pDepth).clone();
		cv_frame.convertTo(depth_image, CV_8U, 255.0 / 5000);
		imshow("Depth Image", depth_image);*/
		

		
		if (rcc != STATUS_OK) {
			cout << "read" << OpenNI::getExtendedError() << endl;
			return 1;
		}
		else {
			const Mat mImageRGB(frameColor.getHeight(), frameColor.getWidth(), CV_8UC3, (void*)frameColor.getData());
			
			cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);
			cv::flip(cImageBGR, cImageBGR, 1);
			imshow("Color Image", cImageBGR);
			imwrite(folderpath + "/" + sframeNum + "-color.jpg", cImageBGR);
		}

		Mat cvFusionImg;
		cvtColor(mScaledDepth, cvFusionImg, CV_GRAY2BGR);
		addWeighted(cImageBGR, 0.5, cvFusionImg, 0.5, 0, cvFusionImg);
		imshow("fusion Image", cvFusionImg);

		frameNum++;
		if (waitKey(1) == 27)
			break;
	}

	depth.stop();
	color.stop();
	depth.destroy();
	color.destroy();
	device.close();
	OpenNI::shutdown();

	return 0;
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
