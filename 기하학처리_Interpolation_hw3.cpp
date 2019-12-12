/*
2015154007_�İ�_������
����ó�� ����
19.11.07
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <string.h>

using namespace cv;
using namespace std;

Mat OpenImageDialog()
{
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

	Mat image = Mat();
	if (GetOpenFileNameA(&ofn)) {
		image = imread(name, IMREAD_GRAYSCALE);
		if (image.empty()) {
			cout << "���� �б� ����" << endl;
			exit(1);
		}
	}
	else {
		cout << "���� ���� ����" << endl;
		exit(1);
	}
	return image;
}

//���� �̵�
void Translation(Mat img, Mat& dst, Point pt) {
	Rect rect(Point(0, 0), img.size());
	dst = Mat(img.size(), img.type(), Scalar(0));

	//���� ���� ��ȸ - ������ ���
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			Point dstPt(j, i);
			Point imgPt = dstPt - pt;

			//���� ���� ��Ī�Ǵ� ȭ�� ����
			if (rect.contains(imgPt))
				dst.at<uchar>(dstPt) = img.at<uchar>(imgPt);
		}
	}
}
//�����ϸ�
void Scaling(Mat img, Mat& dst, Size size) {
	dst = Mat(size, CV_8U, Scalar(0));
	double ratioY = (double)size.height / img.rows;
	double ratioX = (double)size.width / img.cols;

	//���� ���� ��ȸ - ������ ���
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			int x = (int)(j * ratioX);
			int y = (int)(i * ratioY);
			dst.at<uchar>(y, x) = img.at<uchar>(i, j);
		}
	}
}

//�ֱ��� �̿� ������
void ScalingNearest(Mat img, Mat& dst, Size size) {
	dst = Mat(size, CV_8U, Scalar(0));
	double ratioY = (double)size.height / (img.rows - 1);
	double ratioX = (double)size.width / (img.cols - 1);

	//���� ���� ��ȸ - ������ ���
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			int x = (int)cvRound(j / ratioX);
			int y = (int)cvRound(i / ratioY);
			dst.at<uchar>(i, j) = img.at<uchar>(y, x);
		}
	}
}

uchar BilinearValue(Mat img, double x, double y) {
	if (x >= img.cols - 1)x--;
	if (y >= img.rows - 1)y--;

	//4�� ȭ��
	Point pt((int)x, (int)y);
	int lt = img.at<uchar>(pt),
		lb = img.at<uchar>(pt + Point(0, 1)),
		rt = img.at<uchar>(pt + Point(1, 0)),
		rb = img.at<uchar>(pt + Point(1, 1));

	//�Ÿ� ����
	double alpha = y - pt.y, //y��
		beta = x - pt.x;//x��
	
	int M1 = lt + (int)cvRound(alpha * (lb - lt));
	int M2 = rt + (int)cvRound(alpha * (rb - rt));
	int P = M1 + (int)cvRound(beta * (M2 - M1));

	//�Ǽ����� uchar ������ ��ȯ
	return saturate_cast<uchar>(P);
}

void ScalingBilinear(Mat img, Mat& dst, Size size) {
	dst = Mat(size, img.type(), Scalar(0));
	double ratioY = (double)size.height / img.rows;
	double ratioX = (double)size.width / img.cols;

	//���� ���� ��ȸ - ������ ���
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			double x = j / ratioX;
			double y = i / ratioY;

			dst.at<uchar>(i, j) = BilinearValue(img, x, y);
		}
	}
}

void Rotation(Mat img, Mat& dst, double dgree) {
	double radian = dgree / 180 * CV_PI;
	double sinVal = sin(radian);
	double cosVal = cos(radian);

	Rect rect(Point(0, 0), img.size());
	dst = Mat(img.size(), img.type(), Scalar(0));

	//���� ���� ��ȸ - ������ ���
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			//ȸ�� ��ȯ
			double x = j * cosVal + i * sinVal;
			double y = -j * sinVal + i * cosVal;

			if(rect.contains(Point2d(x,y)))
			dst.at<uchar>(i, j) = BilinearValue(img, x, y);
		}
	}
}
void CalcHistogram(Mat image, Mat& histogram, int bins, int range_max = 256) {
	//������׷� ���� ������ Mat �ν��Ͻ� ����, 0���� �ʱ�ȭ 
	histogram = Mat(bins, 1, CV_32F, Scalar(0));
	//x�� �� ���� 
	float gap = (float)range_max / bins;
	//������ ��ȸ�ϸ鼭 ȭ�Ҹ� ó�� 
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			// var : workPixel 
			//ó���� ȭ�� 
			//auto : ������ �������� �����Ϸ��� ������ ���� �ڵ����� ���� Ÿ���� ����, ���ø� ���� ������ ��� �� �� 
			auto workPixel = image.at<uchar>(i, j);
			//�ش� ȭ�Ұ� �� ������ ��� 
			int idx = (int)(workPixel / gap);
			//������׷��� ������ ���� ���� 
			histogram.at<float>(idx)++;
		}
	}
}
void GetHistogramImage(Mat histogram, Mat& histogramImage, Size size = Size(256, 200)) {
	//255�� �� ������׷� ������ ���� 
	histogramImage = Mat(size, CV_8U, Scalar(255));
	//������׷� �� ������ ǥ���� �ʺ� 
	float gap = (float)(histogramImage.cols / histogram.rows);
	// ������׷��� �󵵸� �ּҰ� 0, �ִ밡 ��� ������ ���̰��� ������ ���� 
	normalize(histogram, histogram, 0, histogramImage.rows, NORM_MINMAX);
	for (int i = 0; i < histogram.rows; i++) {
		//������׷� ���� �簢���� ���۰� ���� ��ǥ 
		float sx = i * gap;
		float ex = (i + 1) * gap;
		//�簢���� ���� �ϴ���ǥ�� ���� ��� ��ǥ 
		Point2f pt_Ib(sx, 0), pt_rt(ex, histogram.at<float>(i));
		//�簢���� ���� ���� ������ �簢���� �׸� 
		if (pt_rt.y > 0)
			rectangle(histogramImage, pt_Ib, pt_rt, Scalar(0), -1);
	}
	// X�� �������� ������ ������ (���󿡼� ��ǥ��� �Ʒ��� ���� ���� �����Ƿ�, ���� ���� ���� ���� ��ǥ��� ��ȯ) 
	flip(histogramImage, histogramImage, 0);
}
void run() {
	auto img = OpenImageDialog();

	Mat rotImg, transImg, scalingImg1, scalingImg2, scalingImg3, scalingImg4, scalingImg5;

//	Translation(img, transImg, Point(20, 20));
//	Scaling(img, scalingImg1, Size(1024, 1024));
//	Rotation(img, rotImg, 25);

	//�����ϸ�, �������� -> 4096x4096
	ScalingNearest(img, scalingImg2, Size(4096, 4096)); //�ֱ��� 
	ScalingBilinear(img, scalingImg3, Size(4096, 4096)); //�缱��
	resize(img, scalingImg4, Size(4096, 4096), INTER_CUBIC); //����ť��
	resize(img, scalingImg5, Size(4096, 4096), INTER_LANCZOS4); //LANCZOS

	//�Ϻκ� �ڸ���
	Rect rect(1024, 1024, 512, 512);
	Mat subImage2 = scalingImg2(rect);
	Mat subImage3 = scalingImg3(rect);
	Mat subImage4 = scalingImg4(rect);
	Mat subImage5 = scalingImg5(rect);

	//������׷� ���
	Mat histogram2, histogram3, histogram4, histogram5,
		histogramImage2, histogramImage3, histogramImage4, histogramImage5;
	CalcHistogram(subImage2, histogram2, 256);
	CalcHistogram(subImage3, histogram3, 256);
	CalcHistogram(subImage4, histogram4, 256);
	CalcHistogram(subImage5, histogram5, 256);

	GetHistogramImage(histogram2, histogramImage2);
	GetHistogramImage(histogram3, histogramImage3);
	GetHistogramImage(histogram4, histogramImage4);
	GetHistogramImage(histogram5, histogramImage5);

	//���
//	imshow("�̵�", transImg);
//	imshow("ȸ��", rotImg);
	imshow("�ֱ���", subImage2);
	imshow("�缱��", subImage3);
	imshow("����ť��", subImage4);
	imshow("LANCZOS", subImage5);

	imshow("�ֱ��� ������׷�", histogramImage2);
	imshow("�缱�� ������׷�", histogramImage3);
	imshow("����ť�� ������׷�", histogramImage4);
	imshow("LANCZOS ������׷�", histogramImage5);

	waitKey();

}

int main() {
	run();
}