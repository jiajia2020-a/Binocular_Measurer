/********************************************************************
	���� :	2013/6/30
	���� :	BinocularVision
	���� :	���찲 yanqinganssg AT gmail/163 DOT com
	��λ :  �人��ѧ 
	ʹ�� :	��Ҫopencv�⣬��������������·����lib
	��ע :  ����ʹ�òο�BinocularTest2����
*********************************************************************/


#pragma once

#include <vector>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"


class StereoMatch
{
public:
	StereoMatch();
	virtual ~StereoMatch();

	int init(int imgWidth, int imgHeight, const char *xmlFilePath);
	int bmMatch(cv::Mat &frameLeft, cv::Mat &frameRight, cv::Mat &disparity, cv::Mat &imageLeft, cv::Mat &imageRight);
	int getPointClouds(cv::Mat &disparity, cv::Mat &pointClouds);
	int getDisparityImage(cv::Mat &disparity, cv::Mat &disparityImage, bool isColor = false);
	void savePointClouds(cv::Mat &pointClouds, const char *filename);
    inline void setViewField(int viewWidth, int viewHeight, int viewDepth)
    {
        m_nViewWidth = viewWidth;
        m_nViewHeight = viewHeight;
        m_nViewDepth = viewDepth;
    }

	cv::StereoBM m_BM;     // ����ƥ�� BM ����
	double m_FL;    // �������У����Ľ���ֵ

private:

	int loadCalibData(const char *xmlFilePath);

	bool m_Calib_Data_Loaded;    // �Ƿ�ɹ����붨�����
    cv::Mat m_Calib_Mat_Q;    // Q ����
	cv::Mat m_Calib_Mat_Remap_X_L;    // ����ͼ����У����������ӳ����� X
	cv::Mat m_Calib_Mat_Remap_Y_L;    // ����ͼ����У����������ӳ����� Y
	cv::Mat m_Calib_Mat_Remap_X_R;    // ����ͼ����У����������ӳ����� X
	cv::Mat m_Calib_Mat_Remap_Y_R;    // ����ͼ����У����������ӳ����� Y
	cv::Mat m_Calib_Mat_Mask_Roi;    // ����ͼУ�������Ч����
	cv::Rect m_Calib_Roi_L;    // ����ͼУ�������Ч�������
	cv::Rect m_Calib_Roi_R;    // ����ͼУ�������Ч�������

	int m_frameWidth;    // ֡��
	int m_frameHeight;    // ֡��
	int m_numberOfDisparies;    // �Ӳ�仯��Χ

    int m_nViewWidth;    // �ӳ����
    int m_nViewHeight;    // �ӳ��߶�
    int m_nViewDepth;    // �ӳ����

};