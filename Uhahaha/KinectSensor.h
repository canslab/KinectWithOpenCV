#pragma once
#include <Kinect.h>
#include <opencv2\opencv.hpp>

class KinectSensor
{
public:
	bool Initialize();
	cv::Mat GetLatestColorFrame();

public:
	KinectSensor();
	virtual ~KinectSensor();

private:
	IKinectSensor *m_pKinectSensor;
	IColorFrameReader *m_pColorFrameReader;
	IColorFrame* m_pColorFrame;
	RGBQUAD* m_pColorImageBuffer;

	int m_nWidth;
	int m_nHeight;
};

