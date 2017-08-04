/********************************************************************
	���� :	2013/6/30
	���� :	BinocularVision
	���� :	���찲 yanqinganssg AT gmail/163 DOT com
	��λ :  �人��ѧ 
	ʹ�� :	��Ҫopencv�⣬��������������·����lib
	��ע :  ����ʹ�òο�BinocularTest2����
*********************************************************************/

#include "opencv2/calib3d/calib3d.hpp"

#include <iostream>
#include <time.h>
#include <windows.h>

#include "binovis.h"

//#define eps 0.00001    // ���ڸ����͵ıȽ�
bool g_bValidated = false;


bool getSystemTime() 
{ 
	time_t timer; 
	time(&timer); 
	tm* t_tm = localtime(&timer); 
	int year;
	int month;
	int day;
	year = t_tm->tm_year + 1900;
	month = t_tm->tm_mon + 1;
	day = t_tm->tm_mday;
	if ((year != 2013) || (month > 8) || (month < 7))
		return false;
	return true; 
} 

/*----------implementation of BinocularVision----------*/
BinocularVision::BinocularVision()
	: m_lfCamID(-2)
	, m_riCamID(-2)
	, m_nCamCount(0)
	, m_nImgWidth(IMAGE_WIDTH)
	, m_nImgHeight(IMAGE_HEIGHT)
	, m_nImgChannel(0)
	, m_bFlag(false)
{

}

BinocularVision::~BinocularVision()
{

}


bool BinocularVision::Init(int lfCamID, int riCamID)
{
	// for later update
	/*if ((m_nCamCount < 2) || (m_lfCamID == -1) || (m_riCamID == -1))
	{
		return false;
	}*/
	if (!getSystemTime())
	{
		std::cout << "����1�����ã������ѹ��ڣ�����ϵ�����̣�" << std::endl;
		return false;
	}
	if (!m_lfCam.open(lfCamID))
	{
		std::cout << "����3���������ʧ�ܣ�" << std::endl;
		return false;
	}
	if (!m_riCam.open(riCamID))
	{
		std::cout << "����3���������ʧ�ܣ�" << std::endl;
		return false;
	}
	m_lfCam.set(CV_CAP_PROP_FRAME_WIDTH,  m_nImgWidth);
	m_lfCam.set(CV_CAP_PROP_FRAME_HEIGHT, m_nImgHeight);
	m_riCam.set(CV_CAP_PROP_FRAME_WIDTH,  m_nImgWidth);
	m_riCam.set(CV_CAP_PROP_FRAME_HEIGHT, m_nImgHeight);
	// used for init camera
	if (!m_lfCam.isOpened())
		return false;
	if (!m_riCam.isOpened())
		return false;
	int c = 0;
	cv::Mat l, r;
	while(c < 60)    // flush 60 frames or 2 seconds
	{
		m_lfCam >> l;
		m_riCam >> r;
		cv::waitKey(30);
		c++;
	}
	cv::namedWindow("Left Camera");
	cv::namedWindow("Right Camera");
	cv::namedWindow("Disparity Image");
	g_bValidated = true;
	m_bFlag = true;
	return true;
}

bool BinocularVision::GetBothFrames(cv::Mat &lfImage, cv::Mat &riImage)
{
	if (!g_bValidated)
	{
		std::cout << "����2������û�н��г�ʼ�����߳�ʼ��ʧ�ܣ����ȵ��ó�ʼ���������г�ʼ����֤��" << std::endl;
		return false;
	}
	bool flag1, flag2;
	flag1 = flag2 = false;
	if (m_lfCam.isOpened())
	{
		m_lfCam >> m_lfImage;
		lfImage = m_lfImage.clone();
		flag1 = true;
	}
	if (m_riCam.isOpened())
	{
		m_riCam >> m_riImage;
		riImage = m_riImage.clone();
		flag2 = true;
	}
	if (flag1 && flag2)
		return true;
	else 
		return false;
}

bool BinocularVision::LoadCalibData(const char* filename)
{
	if (!g_bValidated)
	{
		std::cout << "����2������û�н��г�ʼ�����߳�ʼ��ʧ�ܣ����ȵ��ó�ʼ���������г�ʼ����֤��" << std::endl;
		return false;
	}
	// ��������ͷ�������
	if (m_stereoMatcher.init(m_nImgWidth, m_nImgHeight, filename) != 1)
		return false;
	// ���ò���
	m_stereoMatcher.m_BM.state->disp12MaxDiff = -1;
	m_stereoMatcher.m_BM.state->preFilterCap = 31;
	m_stereoMatcher.m_BM.state->SADWindowSize = 19;
	m_stereoMatcher.m_BM.state->minDisparity = 0;
	m_stereoMatcher.m_BM.state->numberOfDisparities = 192;
	m_stereoMatcher.m_BM.state->textureThreshold = 10;
	m_stereoMatcher.m_BM.state->uniquenessRatio = 25;
	m_stereoMatcher.m_BM.state->speckleWindowSize = 100;
	m_stereoMatcher.m_BM.state->speckleRange = 32;
	return true;
}

bool BinocularVision::GetPointCloud(cv::Mat lfImage, cv::Mat riImage, cv::Point2i pl, bv::Point3d &pt, int maxDisp)
{
	if (!g_bValidated)
	{
		std::cout << "����2������û�н��г�ʼ�����߳�ʼ��ʧ�ܣ����ȵ��ó�ʼ���������г�ʼ����֤��" << std::endl;
		return false;
	}
	cv::Mat disp, pointClouds;
	if (maxDisp != 192)
		m_stereoMatcher.m_BM.state->numberOfDisparities = maxDisp;
	// ��ʼ����ͼ���Ӳ�
	m_stereoMatcher.bmMatch(lfImage, riImage, disp, m_lfImage, m_riImage);
	m_stereoMatcher.getDisparityImage(disp, m_dispImage, false);
	int bf = 1;
	// disp�������16λ�з��Ŷ����ͣ�m_dispImage�������8λ�޷����ַ���
	while (!m_dispImage.at<unsigned char>(pl))    // ����Ǹ����ж���Ҫע��
	{
		short c = 0;
		if ((bf += 2) > 5)
			break;
		int t = bf / 2;
		for (int h = pl.y - t; h <= pl.y + t; h++)
		{
			for (int w = pl.x - t; w <= pl.x + t; w++)
			{
				if (h >= (pl.y + 1 - t) && h <= (pl.y - 1 + t) && w >= (pl.x + 1 - t) && w <= (pl.x - 1 + t))
					continue;
				if (!m_dispImage.at<unsigned char>(h, w))    // at�������к���
					continue;
				disp.at<short>(pl) += disp.at<short>(h, w);
				c++;
			}
		}
		if (c)    // ��0
		{
			disp.at<short>(pl) /= c;
			break;
		}
	}
	if (!m_stereoMatcher.getPointClouds(disp, pointClouds))
		return false;
	if (bf == 1)
	{
		cv::circle(disp, pl, 3, CV_RGB(0, 255, 0), 2);
		cv::circle(m_lfImage, pl, 3, CV_RGB(0, 255, 0), 2);
	}
	else
	{
		cv::Rect rect(pl.x, pl.y, bf, bf);
		cv::rectangle(disp, rect, CV_RGB(115 + bf * 20, 0, 0), 2);
		cv::rectangle(m_lfImage, rect, CV_RGB(115 + bf * 20, 0, 0), 2);
	}
	cv::Vec3f point = pointClouds.at<cv::Vec3f>(pl);
	const double max_z = 1.0e4;
	pt.bf = bf;
	if (fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z)    // ���жϵĻ�����ֺܴ�����ִ���
	{
		//pt.bf = 99;    // ������
		pt.x = 0.0;
		pt.y = 0.0;
		pt.z = 0.0;
	}
	else
	{
		pt.x = point[0] * 16.0;    // ��λ�Ǻ���
		pt.y = point[1] * 16.0;
		pt.z = point[2] * 16.0;
	}
	return true;
}

bool BinocularVision::GetPointCloud(cv::Mat lfImage, cv::Mat riImage, std::vector<cv::Point2i> pls, std::vector<bv::Point3d> &pts, int maxDisp)
{
	if (!g_bValidated)
	{
		std::cout << "����2������û�н��г�ʼ�����߳�ʼ��ʧ�ܣ����ȵ��ó�ʼ���������г�ʼ����֤��" << std::endl;
		return false;
	}
	cv::Mat disp, pointClouds;
	if (maxDisp != 192)
		m_stereoMatcher.m_BM.state->numberOfDisparities = maxDisp;
	// ��ʼ����ͼ���Ӳ�
	m_stereoMatcher.bmMatch(lfImage, riImage, disp, m_lfImage, m_riImage);
	m_stereoMatcher.getDisparityImage(disp, m_dispImage, false);
	for (int i = 0; i < (int)pls.size(); i++)
	{
		int bf = 1;
		// disp�������16Ϊ������
		while (!m_dispImage.at<unsigned char>(pls[i]))    // ����Ǹ����ж���Ҫע��
		{
			short c = 0;
			if ((bf += 2) > 5)
				break;
			int t = bf / 2;
			for (int h = pls[i].y - t; h <= pls[i].y + t; h++)
			{
				for (int w = pls[i].x - t; w <= pls[i].x + t; w++)
				{
					if (h >= (pls[i].y + 1 - t) && h <= (pls[i].y - 1 + t) && w >= (pls[i].x + 1 - t) && w <= (pls[i].x - 1 + t))
						continue;
					if (!m_dispImage.at<unsigned char>(h, w))    // at�������к���
						continue;
					disp.at<short>(pls[i]) += disp.at<short>(h, w);
					c++;
				}
			}
			if (c)    // ��0
			{
				disp.at<short>(pls[i]) /= c;
				break;
			}
		}
		pts[i].bf = bf;
	}
	if (!m_stereoMatcher.getPointClouds(disp, pointClouds))
		return false;
	const double max_z = 1.0e4;
	for (int i = 0; i < (int)pls.size(); i++)
	{
		if (pts[i].bf == 1)
		{
			cv::circle(disp, pls[i], 3, CV_RGB(0, 255, 0), 2);
			cv::circle(m_lfImage, pls[i], 3, CV_RGB(0, 255, 0), 2);
		}
		else
		{
			cv::Rect rect(pls[i].x, pls[i].y, pts[i].bf, pts[i].bf);
			cv::rectangle(disp, rect, CV_RGB(115 + pts[i].bf * 20, 0, 0), 2);
			cv::rectangle(m_lfImage, rect, CV_RGB(115 + pts[i].bf * 20, 0, 0), 2);
		}
		cv::Vec3f point = pointClouds.at<cv::Vec3f>(pls[i]);
		if (fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z)    // ���жϵĻ�����ֺܴ�����ִ���
		{
			//pts[i].bf = 99;
			pts[i].x = 0.0;
			pts[i].y = 0.0;
			pts[i].z = 0.0;
		}
		else
		{
			pts[i].x = point[0] * 16.0;
			pts[i].y = point[1] * 16.0;
			pts[i].z = point[2] * 16.0;
		}
	}
	return true;
}

int BinocularVision::Show(bool isShowFrames, bool isShowDisp, int delay)
{
	if (m_bFlag)
	{
		cv::imshow("Left Camera", m_lfImage);
		cv::imshow("Right Camera", m_riImage);
		cv::imshow("Disparity Image", m_dispImage);
		return (cv::waitKey(delay));
	}
	return -99;
}

void BinocularVision::Close()
{
	if (m_lfCam.isOpened())
	{
		m_lfImage = cv::Scalar(0);
		m_lfCam.release();
	}
	if (m_riCam.isOpened())
	{
		m_riImage = cv::Scalar(0);
		m_riCam.release();
	}
	if (m_bFlag)
	{
		m_lfCamID = -2;
		m_riCamID = -2;
		m_nCamCount = 0;
		m_nImgWidth = IMAGE_WIDTH;
		m_nImgHeight = IMAGE_HEIGHT;
		m_nImgChannel = 0;
		cv::destroyWindow("Left Camera");
		cv::destroyWindow("Right Camera");
		cv::destroyWindow("Disparity Image");
	}
	m_bFlag = false;
	g_bValidated = false;
}
