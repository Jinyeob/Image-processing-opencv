/*
����ó�� ���� �׷��� ���� ���� ���� ������ ó��
2019.11.19.�����
�ѱ����������б� �İ� 2015154007 ������
*/
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <string.h>
#include <queue>
#include <vector>
#include <algorithm>
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

//���� ħ/�ؿ�
bool CheckMatch(Mat img, Point start, Mat mask, int mode) {
	for (int u = 0; u < mask.rows; u++) {
		for (int v = 0; v < mask.cols; v++) {
			//��ȸ�� ��ǥ
			Point pt(v, u);
			
			//����ũ ���
			int m = mask.at<uchar>(pt);

			//��꿡 ���� ȭ��
			int p = img.at<uchar>(start+pt);

			//����� ȭ�Ұ� ��ġ�ϴ��� ��
			bool ch = (p == 255);

			//mode=0:ħ��, mode=1: ��â
			if (m == 1 && ch == mode) {
				return false;
			}
		}
		return true;
	}
}

//���� ħ��
void Erosion(Mat img, Mat& dst, Mat mask) {
	dst = Mat(img.size(), CV_8U, Scalar(0));
	if (mask.empty()) {
		//�⺻ ����ũ
		mask = Mat(3, 3, CV_8UC1, Scalar(1));
	}
	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

				//���¼� ��ġ ���� ��
				bool check = CheckMatch(img, start, mask, 0);
				dst.at <uchar>(i, j) = (check) ? 255 : 0;
		}
	}
}

//���� ��â
void Dilation(Mat img, Mat& dst, Mat mask)
{
	dst = Mat(img.size(), CV_8U, Scalar(0));

	if (mask.empty())
		mask = Mat(3, 3, CV_8UC1, Scalar(0));

	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

			//���¼� ��ġ ���� ��
			bool check = CheckMatch(img, start, mask, 1);
			dst.at<uchar>(i, j) = (check) ? 0:255;

		}
	}
}

//���� ����
void Opening(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	Erosion(img, dst1, mask);
	Dilation(dst1, dst, mask);
}

//���� ����
void Closing(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	Dilation(img, dst1, mask);
	Erosion(dst1, dst, mask);
}

//�׷��� ħ�Ŀ�
int mycheck(Mat img, Point start, Mat mask) {
	priority_queue<int, vector<int>, greater<int>> q; //��������, top���� ���� ������
	for (int u = 0; u < mask.rows; u++) {
		for (int v = 0; v < mask.cols; v++) {
			//��ȸ�� ��ǥ
			Point pt(v, u);

			//����ũ ���
			int m = mask.at<uchar>(pt);

			//��꿡 ���� ȭ��
			int p = img.at<uchar>(start + pt);
			q.push(p - m);
		}
	}
	return q.top();
}

//�׷��� ��â��
int mycheck2(Mat img, Point start, Mat mask) {
	priority_queue<int> q; //��������, top���� ���� ū��
	for (int u = 0; u < mask.rows; u++) {
		for (int v = 0; v < mask.cols; v++) {
			//��ȸ�� ��ǥ
			Point pt(v, u);

			//����ũ ���
			int m = mask.at<uchar>(pt);

			//��꿡 ���� ȭ��
			int p = img.at<uchar>(start + pt);
			q.push(m + p);
		}
	}
	return q.top();
}

//�׷��� ħ��
void ErosionGray(Mat img, Mat& dst, Mat mask) {
	dst = Mat(img.size(), CV_8U, Scalar(0));
	if (mask.empty()) {
		//�⺻ ����ũ
		mask = Mat(3, 3, CV_8UC1, Scalar(1));
	}
	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

			int a = mycheck(img, start, mask);
			dst.at <uchar>(i, j) = a;
		}
	}
}

//�׷��� ��â
void DilationGray(Mat img, Mat& dst, Mat mask)
{
	dst = Mat(img.size(), CV_8U, Scalar(0));

	if (mask.empty())
		mask = Mat(3, 3, CV_8UC1, Scalar(0));

	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

			//���¼� ��ġ ���� ��
			int a = mycheck2(img, start, mask);
			dst.at <uchar>(i, j) = a;
		}
	}
}

//�׷��� ����
void GrayOpening(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	ErosionGray(img, dst1, mask);
	DilationGray(dst1, dst, mask);
}

//�׷��� ����
void GrayClosing(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	DilationGray(img, dst1, mask);
	ErosionGray(dst1, dst, mask);
}

void Run()
{
	Mat img = OpenImageDialog();
	Mat bImg, dst1, dst2, dst3, dst4, dst5, dst6, dst7;

	//// ���� ����ȭ
	//threshold(img, bImg, 128, 255, THRESH_BINARY);

	//uchar data1[] = {
	//	0, 1, 0,
	//	1, 1, 1,
	//	0, 1, 0
	//};
	//uchar data2[] = {
	//	1, 0, 1,
	//	0, 1, 0,
	//	1, 0, 1
	//};

	//Mat mask1(3, 3, CV_8UC1, data1);
	//Mat mask2(3, 3, CV_8UC1, data2);


	//// ��������� ħ��
	//Erosion(bImg, dst1, (Mat)mask1);
	//// OPEN_CV ħ��
	//morphologyEx(bImg, dst2, MORPH_ERODE, mask1);

	//// ��������� ��â
	//Dilation(bImg, dst3, (Mat)mask2);
	//// OPEN_CV ��â
	//morphologyEx(bImg, dst4, MORPH_DILATE, mask2);

	//// ����
	//Opening(bImg, dst5, (Mat)mask1);

	//// ����
	//Closing(bImg, dst6, (Mat)mask1);

	//imshow("����", img);
	//imshow("���� ����", bImg);
	//imshow("ħ�� (�����)", dst1);
	//imshow("ħ�� (���̺귯��)", dst2);
	//imshow("��â (�����)", dst3);
	//imshow("��â (���̺귯��)", dst4);
	//imshow("����", dst5);
	//imshow("����", dst6);

	//waitKey();

	///////////////////////////////////////////
	// ���⼭���ʹ� ������ �׽�Ʈ �ڵ��Դϴ�.//
	///////////////////////////////////////////

	// ������ ����ũ
	uchar data3[] = {
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	};
	Mat mask3(3, 3, CV_8UC1, data3);


	ErosionGray(img, dst1, (Mat)mask3);
	DilationGray(img, dst2, (Mat)mask3);

	GrayOpening(img, dst5, (Mat)mask3);
	GrayClosing(img, dst6, (Mat)mask3);

	imshow("���� (������)", img);
	imshow("ħ�� (������)", dst1);
	imshow("��â (������)", dst2);
	imshow("���� (������)", dst5);
	imshow("���� (������)", dst6);
	waitKey();
}
int main() {
	Run();
}