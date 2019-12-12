/*
����ó�� ���� ������׷� ��ȭ
2019.10.03.����� 
�ѱ����������б� �İ� 2015154007 ������
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <string.h>

using namespace cv;
using namespace std;

string OpenFileDialog() {
	char name[MAX_PATH] = { 0, };
	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "�������(*.*)\0*.*\0";
	ofn.lpstrFile = name;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";

	string strName;
	if (GetOpenFileNameA(&ofn)) {
		strName = name;
	}
	return strName;
}

void CalcHistogram(Mat image, Mat& histogram, int bins, int range_max = 256) {
	histogram = Mat(bins, 1, CV_32F, Scalar(0));
	float gap = (float)range_max / bins;
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			auto workPixel = image.at<uchar>(i, j);
			int idx = (int)(workPixel / gap);
			histogram.at<float>(idx)++;
		}
	}
}

void GetHistogramImage(Mat histogram, Mat& histogramImage, Size size = Size(256, 200)) {
	histogramImage = Mat(size, CV_8U, Scalar(255));
	float gap = (float)(histogramImage.cols / histogram.rows);
	normalize(histogram, histogram, 0, histogramImage.rows, NORM_MINMAX);
	for (int i = 0; i < histogram.rows; i++) {
		float sx = i * gap;
		float ex = (i + 1) * gap;
		Point2f pt_Ib(sx, 0), pt_rt(ex, histogram.at<float>(i));
		if (pt_rt.y > 0)
			rectangle(histogramImage, pt_Ib, pt_rt, Scalar(0), -1);
	}
	flip(histogramImage, histogramImage, 0);
}

void CalcNormalizedCumulativeFrequency(Mat histogram, Mat& histogramSum) {
	histogramSum = Mat(histogram.rows, 1, CV_32F, Scalar(0));
	//���� �� 
	float sum = 0;
	for (int i = 0; i < histogram.rows; i++) {
		sum += histogram.at<float>(i);
		histogramSum.at<float>(i) = sum;
	}
	//����ȭ 
	for (int i = 0; i < histogramSum.rows; i++) {
		histogramSum.at<float>(i) = histogramSum.at<float>(i) / sum * 255.0;
	}
}


void jinyeob_histogram_specificaion() {


	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// ��ȭ�� ����(�Է¿���) �ҷ����� ���                       //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	auto fileName = OpenFileDialog();
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "���� �б� ����" << endl;
		return;
	}
	Mat histogram, histogramImage;
	CalcHistogram(image, histogram, 256);

	cout << histogram.t() << endl;
	GetHistogramImage(histogram, histogramImage);
	imshow("���� ����", image);
	imshow("������׷�", histogramImage);
	waitKey();

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// ���ϴ� ���� ���� �ҷ����� ���                              //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	auto fileName2 = OpenFileDialog();
	Mat image2 = imread(fileName2, IMREAD_GRAYSCALE);
	if (image2.empty()) {
		cout << "���� �б� ����" << endl;
		return;
	}
	Mat histogram2, histogramImage2;
	CalcHistogram(image2, histogram2, 256);

	cout << histogram2.t() << endl;
	GetHistogramImage(histogram2, histogramImage2);
	imshow("���� ����", image2);
	imshow("������׷�", histogramImage2);
	waitKey();

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// ��ȭ�� ����(�Է¿���)�� ��Ȱȭ                            //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	// 1�ܰ� : ������׷� ���� 
	Mat histogramSum;
	CalcHistogram(image, histogram, 256);

	// 2�ܰ� : ���� �� �� ��� 
	// 3�ܰ� : ���� �� �� ����ȭ 
	CalcNormalizedCumulativeFrequency(histogram, histogramSum);

	//����ȭ�� ���� �� ���� ���, ������ ȭ�� ��ȯ
	auto tmp = Mat(image.size(), image.type());

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at <uchar>(i, j) = (uchar)histogramSum.at<float>(image.at<uchar>(i, j));
		}
	}

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// ���� ���� ��Ȱȭ                                            //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	// 1�ܰ� : ������׷� ���� 
	Mat histogramSum2;
	CalcHistogram(image2, histogram2, 256);

	// 2�ܰ� : ���� �� �� ��� 
	// 3�ܰ� : ���� �� �� ����ȭ 
	CalcNormalizedCumulativeFrequency(histogram2, histogramSum2);

	//����ȭ�� ���� �� ���� ���, ������ ȭ�� ��ȯ
	auto tmp2 = Mat(image2.size(), image2.type());
	for (int i = 0; i < image2.rows; i++) {
		for (int j = 0; j < image2.cols; j++) {
			tmp2.at <uchar>(i, j) = (uchar)histogramSum2.at<float>(image2.at<uchar>(i, j));
		}
	}

	Mat histogram_result;
	CalcHistogram(tmp2, histogram_result, 256);

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// 	��Ȱȭ�� ���ؿ��� ����Ȱȭ                                 //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////
	
	/*
	����ȭ�� ������ -> ��
	�� -> ����Ȱȭ��
	*/
	int two = 255;
	int one = 254;
	while (1) {
		for (int i = histogramSum2.at<float>(one); i <= histogramSum2.at<float>(two); i++) {
			histogram_result.at<float>(i) = two;
		}
		two = one;
		--one;
		if (one < 0)
			break;
	}

	//������ ����� ������̺�(����Ȱȭ��)�� ����, ��Ȱȭ�� �Է¿��� ȭ�� ��ȯ
	auto result = Mat(tmp.size(), tmp.type());

	for (int i = 0; i < tmp.rows; i++) {
		for (int j = 0; j < tmp.cols; j++) {
			result.at <uchar>(i, j) = (uchar)histogram_result.at <float>(tmp.at<uchar>(i, j));
		}
	}

	// ���� ������׷� ��� 
	CalcHistogram(result, histogram_result, 256);
	GetHistogramImage(histogram_result, histogramImage);

	imshow("��ȭ - ����", result);
	imshow("��ȭ - ������׷�", histogramImage);
	waitKey();

}

int main()
{
	jinyeob_histogram_specificaion();
}