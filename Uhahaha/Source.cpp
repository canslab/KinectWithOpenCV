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

	// 키넥트를 얻어온다.
	GetDefaultKinectSensor(&pKinectSensor);

	// 키넥트 센서를 사용하려면 파일 열듯이 열자!
	pKinectSensor->Open();
	// Color Frame이 솟구치는 원천 (소스)를 얻는다.
	pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
	// 그 소스에 빨대(리더)를 꼽는다.
	pColorFrameSource->OpenReader(&pColorReader);

	// 소스는 필요없다. 이제, 빨대만 이용하면 된다.
	pColorFrameSource->Release();

	// IColorFrameReader에서 Latest Frame을 얻는다.
	while (true)
	{
		HRESULT hr = pColorReader->AcquireLatestFrame(&pColorFrame);

		// 획득이 성공했다면
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
					std::cout << "성공!\n";
					cv::Mat data(cv::Size(1920, 1080), CV_8UC4, pData);
				}
			}

			// 다음 프레임을 얻어오기에 앞서 이전 프레임을 Release해줘야만 한다.
			// 아니면, AcquireLatestFrame()을 호출하면 PENDING된다고 나온다.
			pColorFrame->Release();
			// 좋은 습관으로 nullptr로 재설정
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