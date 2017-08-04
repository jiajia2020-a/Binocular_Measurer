/********************************************************************
	���� :	2013/6/30
	���� :	BinocularVision
	���� :	���찲 yanqinganssg AT gmail/163 DOT com
	��λ :  �人��ѧ 
	ʹ�� :	��Ҫopencv�⣬��������������·����lib
	��ע :  ����ʹ�òο�BinocularTest2����
*********************************************************************/


#pragma once

#include "stereomatch.h"

// Ĭ�Ϸֱ��ʴ�С��һ�㲻Ҫ��
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

/*----------structures----------*/
namespace bv
{
struct Point3d
{
    double x;
    double y;
    double z;
    unsigned int bf;    // bf �ǿ��ŶȲ��������ҵ��Ӳ�������С���ڴ�СΪ׼��ȡֵΪ��1��3��5��7
};
}

/*----------class of BinocularVision----------*/
class BinocularVision
{
public:
	BinocularVision();
	~BinocularVision();

	// ����Ϊ�ӿں���

/*----------------------------
* ���� : �������ͷ�ķֱ��ʴ�С
*----------------------------
* ���� : BinocularVision::GetImageSize
* ���� : public 
* ���� : void
*
* ���� : width	    [out]	�������ͼ�������
* ���� : height		[out]	�������ͼ�������
*/
	inline void GetImageSize(int &width, int &height)
	{
		width = m_nImgWidth;
		height = m_nImgHeight;
	}

/*----------------------------
* ���� : ��������ͷ�ķֱ��ʴ�С
*----------------------------
* ���� : BinocularVision::SetImageSize
* ���� : public 
* ���� : void
*
* ���� : width	    [in]	Ҫ�޸ĳɵ�ͼ������
* ���� : height		[in]	Ҫ�޸ĳɵ�ͼ������
*/
	// �����޸ķֱ��ʴ�С������Init����֮ǰ����
	inline void SetImageSize(int width, int height)
	{
		m_nImgWidth = width;
		m_nImgHeight = height;
	}

/*----------------------------
* ���� : ��������ͷ�������ʼ��
*----------------------------
* ���� : BinocularVision::Init
* ���� : public 
* ���� : bool
*
* ���� : lfCamID	    [in]	������ͷID
* ���� : riCamID		[in]	������ͷID
*/
	// �����������ͼ������ߣ���ô����֮��������2�����ϵ�����ͷʱ���Լ�����Ҫ������������ͷ���
	bool Init(int lfCamID = 0 /* 1 2 ... */, int riCamID = 1 /* 0 2 ... */); 

/*----------------------------
* ���� : ���ر궨����
*----------------------------
* ���� : BinocularVision::LoadCalibData
* ���� : public 
* ���� : bool
*
* ���� : filename	    [in]	�ļ�·��
*/
	// ��ȡMFC�汾���ɵ�xml�궨�ļ�����ѭ�������
	bool LoadCalibData(const char *filename); 

/*----------------------------
* ���� : ��ȡ���������ͼ��֡
*----------------------------
* ���� : BinocularVision::GetBothFrames
* ���� : public 
* ���� : bool
*
* ���� : lfImage	    [out]	���������ͷͼ��֡
* ���� : riImage		[out]	���������ͷͼ��֡
*/
	// ��ȡ˫Ŀͼ����������Ƶ��ʾ����Ҫ��while(1)ѭ���е���
	bool GetBothFrames(cv::Mat &lfImage, cv::Mat &riImage);   

/*----------------------------
* ���� : ����һ����ά������
*----------------------------
* ���� : BinocularVision::GetPointCloud
* ���� : public 
* ���� : bool
*
* ���� : lfImage	    [in]	������ͷͼ��֡
* ���� : riImage		[in]	������ͷͼ��֡
* ���� : pl	            [in]	Ҫ����ͼ�����ص�����
* ���� : pt		        [out]	��ż���õ�����ά������
* ���� : maxDisp	    [in]	��Ҫ�������Ӳ����
*/
	// bfΪ7�������������û���Ӳ�޷��õ��ռ������
	bool GetPointCloud(cv::Mat lfImage, cv::Mat riImage, cv::Point2i pl, bv::Point3d &pt, int maxDisp = 192);

/*----------------------------
* ���� : ��������ά������
*----------------------------
* ���� : BinocularVision::GetPointCloud
* ���� : public 
* ���� : bool
*
* ���� : lfImage	    [in]	������ͷͼ��֡
* ���� : riImage		[in]	������ͷͼ��֡
* ���� : pls	        [in]	Ҫ����ͼ�����ص����꣨������
* ���� : pts		    [out]	��ż���õ�����ά�����꣨������
* ���� : maxDisp	    [in]	��Ҫ�������Ӳ����
*/
	bool GetPointCloud(cv::Mat lfImage, cv::Mat riImage, std::vector<cv::Point2i> pls, std::vector<bv::Point3d> &pts, int maxDisp = 192);

/*----------------------------
* ���� : ��ʾ���
*----------------------------
* ���� : BinocularVision::Show
* ���� : public 
* ���� : int
*
* ���� : isShowFrames	    [in]	�Ƿ���ʾ����ͼ������У����ģ�
* ���� : isShowDisp		    [in]	�Ƿ���ʾ�Ӳ�ͼ
* ���� : delay	            [in]	��ʾ��һ֮֡ǰ������ӳ�ʱ�䣨��λ�����룩
*/
	// 0����Ҫ�������������ʾ��һ֡������Esc����ΪEsc�ǳ����˳������Ҫ���Լ��ĳ�����������жϣ����������ִ����ӳ�delay������ʾ��һ֡
	int Show(bool isShowFrames = true, bool isShowDisp = true, int delay = 0);    

/*----------------------------
* ���� : �ͷų���̬���ɵ��ڴ�
*----------------------------
* ���� : BinocularVision::Close
* ���� : public 
* ���� : void
* 
* ���� : ��
*/
	void Close();

private:
	cv::VideoCapture m_lfCam;
	cv::VideoCapture m_riCam;
	cv::Mat m_lfImage;
	cv::Mat m_riImage;
	cv::Mat m_dispImage;
	int m_lfCamID;
	int m_riCamID;
	int m_nCamCount;
	int m_nImgWidth;
	int m_nImgHeight;
	int m_nImgChannel;
	bool m_bFlag;
	StereoMatch m_stereoMatcher;
};






