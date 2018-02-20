#include <Kinect.h>
#include <iostream>
#include <opencv2\opencv.hpp>
#include "KinectSensor.h"
#include "Timer.h"

void ExampleCodeUsingNativeCppAPI()
{
	IKinectSensor *pKinectSensor = nullptr;
	IColorFrameSource *pColorFrameSource = nullptr;
	IColorFrameReader *pColorReader = nullptr;
	IColorFrame* pColorFrame = nullptr;

	RGBQUAD *pData = new RGBQUAD[1920 * 1080];

	// Ű��Ʈ�� ���´�.
	GetDefaultKinectSensor(&pKinectSensor);

	// Ű��Ʈ ������ ����Ϸ��� ���� ������ ����!
	pKinectSensor->Open();
	// Color Frame�� �ڱ�ġ�� ��õ (�ҽ�)�� ��´�.
	pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
	// �� �ҽ��� ����(����)�� �Ŵ´�.
	pColorFrameSource->OpenReader(&pColorReader);

	// �ҽ��� �ʿ����. ����, ���븸 �̿��ϸ� �ȴ�.
	pColorFrameSource->Release();

	// IColorFrameReader���� Latest Frame�� ��´�.
	while (true)
	{
		HRESULT hr = pColorReader->AcquireLatestFrame(&pColorFrame);

		// ȹ���� �����ߴٸ�
		if (SUCCEEDED(hr))
		{
			std::cout << "Success!\n";
			IFrameDescription *desc;
			ColorImageFormat format;
			int nWidth, nHeight;

			pColorFrame->get_FrameDescription(&desc);

			desc->get_Width(&nWidth);
			desc->get_Height(&nHeight);

			// 1920 x 1080
			//std::cout << nWidth << " " << nHeight << " " << std::endl;
			pColorFrame->get_RawColorImageFormat(&format);

			if (format == ColorImageFormat_Bgra)
			{
			}
			else if (format == ColorImageFormat_Yuy2)
			{
				int bufferSize = 1920 * 1080 * sizeof(RGBQUAD);
				if (SUCCEEDED(pColorFrame->CopyConvertedFrameDataToArray(bufferSize, reinterpret_cast<BYTE*>(pData), ColorImageFormat_Bgra)))
				{
					std::cout << "����!\n";
					cv::Mat data(cv::Size(1920, 1080), CV_8UC4, pData);
				}
			}

			// ���� �������� �����⿡ �ռ� ���� �������� Release����߸� �Ѵ�.
			// �ƴϸ�, AcquireLatestFrame()�� ȣ���ϸ� PENDING�ȴٰ� ���´�.
			pColorFrame->Release();
			// ���� �������� nullptr�� �缳��
			pColorFrame = nullptr;
		}
	}

	pColorReader->Release();
	pKinectSensor->Close();
	pKinectSensor->Release();
}

int main(void)
{
	//ExampleCodeUsingNativeCppAPI();
	KinectSensor sensor;

	if (!sensor.Initialize())
	{
		std::cout << "Failed.. \n";
	}
	cv::namedWindow("result", 1);

	while (true)
	{
		cv::Mat lastFrame = sensor.GetLatestColorFrame();

		if (lastFrame.rows > 0 && lastFrame.cols > 0)
		{
			cv::imshow("result", lastFrame);
			if (cv::waitKey(1) == 27)
			{
				return 0;
			}
		}
	}


	return 0;
}