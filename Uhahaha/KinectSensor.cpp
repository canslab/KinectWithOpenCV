#include "KinectSensor.h"

bool KinectSensor::Initialize()
{
	HRESULT hr = E_FAIL;

	if (m_pKinectSensor == nullptr)
	{
		hr = GetDefaultKinectSensor(&m_pKinectSensor);
		IColorFrameSource* pColorFrameSource = nullptr;

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->Open();
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}

		if (SUCCEEDED(hr))
		{
			m_pColorImageBuffer = new RGBQUAD[m_nWidth * m_nHeight];
			pColorFrameSource->Release();
			pColorFrameSource = nullptr;
		}
	}

	return (SUCCEEDED(hr)) ? true : false;
}

cv::Mat KinectSensor::GetLatestColorFrame()
{
	// reeset m_pColorFrame before use.
	if (m_pColorFrame)
	{
		m_pColorFrame->Release();
		m_pColorFrame = nullptr;
	}

	HRESULT hr;
	ColorImageFormat imageFormat;
	UINT nBufferSize = 0;
	RGBQUAD* pBuffer = nullptr;
	cv::Mat retMat;

	// Acquire the latest image from the Kinect sensor
	hr = m_pColorFrameReader->AcquireLatestFrame(&m_pColorFrame);

	if (SUCCEEDED(hr))
	{
		m_pColorFrame->get_RawColorImageFormat(&imageFormat);

		if (imageFormat == ColorImageFormat_Bgra)
		{
			hr = m_pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
		}
		else if (m_pColorImageBuffer)
		{
			pBuffer = m_pColorImageBuffer;
			nBufferSize = m_nWidth * m_nHeight * sizeof(RGBQUAD);
			hr = m_pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);
		}
		else
		{
			hr = E_FAIL;
		}
	}

	if (SUCCEEDED(hr))
	{
		cv::Mat tempMat(cv::Size(m_nWidth, m_nHeight), CV_8UC4, pBuffer);

		if (imageFormat == ColorImageFormat_Bgra)
		{
			tempMat.copyTo(retMat);
		}
		else
		{
			retMat = tempMat;
		}
	}

	return retMat;
}

KinectSensor::KinectSensor()
	:m_nWidth(1920),
	m_nHeight(1080),
	m_pKinectSensor(nullptr),
	m_pColorFrameReader(nullptr),
	m_pColorFrame(nullptr),
	m_pColorImageBuffer(nullptr)
{

}

KinectSensor::~KinectSensor()
{
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
		m_pKinectSensor->Release();
		m_pKinectSensor = nullptr;
	}

	if (m_pColorFrameReader)
	{
		m_pColorFrameReader->Release();
		m_pColorFrameReader = nullptr;
	}

	if (m_pColorFrame)
	{
		m_pColorFrame->Release();
		m_pColorFrame = nullptr;
	}

	if (m_pColorImageBuffer)
	{
		delete[]m_pColorImageBuffer;
		m_pColorImageBuffer = nullptr;
	}
}
