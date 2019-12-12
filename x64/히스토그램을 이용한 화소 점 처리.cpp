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

// params: image 
// ������׷��� Ȯ���� ���� 
// params: histogram 
// ���Ǿ� ��ȯ�� ������׷� 
// params : bins
//������׷��� x�� ��� �� 
// params : range_max
//������׷��� �� �ִ� ��� �� 
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

// params : histogram
//ó���� ������׷� 
// params : histogram Image
//��ȯ�� ������׷� ���� 
// params : size 
// ������׷� ������ ũ�� 
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

//�ǽ� 1 : ������׷� ����
void ShowHistogram() {
#pragma region �ߺ��ڵ�
	auto fileName = OpenFileDialog();
	//������ �׷��̽�����(8��Ʈ = 256 ��鿵��)�� �о�ɴϴ�.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "���� �б� ����" << endl;
		return;
	}
#pragma endregion
	Mat histogram, histogramImage;
	//������׷� ��� 
	CalcHistogram(image, histogram, 256);
	//���� ��� 
	cout << histogram.t() << endl;
	GetHistogramImage(histogram, histogramImage);
	imshow("�ǽ� 1 : ������׷� ���� - ���� ����", image);
	imshow("�ǽ� 1 : ������׷� ���� - ������׷�", histogramImage);
	waitKey();
}

int Clamping(int input) {
	if (input < 0)
		input = 0; 
	else if (input > 255)
		input = 255; 
	return input;
}

void HistogramTransform() {
#pragma region �ߺ��ڵ�
	auto fileName = OpenFileDialog();
	//������ �׷��� ������(8��Ʈ = 256 ��� ����)�� �о� �ɴϴ�.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "���� �б� ����" << endl;
		return;
	}
#pragma endregion
	//ȭ�Ұ� ���� 
	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = Clamping(image.at<uchar>(i, j) * 1.2);
		}
	}
	Mat histogram, histogramImage;
	//������ ������׷� ��� 
	CalcHistogram(image, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("�ǽ� 2 : �������� ������ ������ ������׷� ���� - ����", histogramImage);
	//ó���� ������ ������׷� ��� 
	CalcHistogram(tmp, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("�ǽ� 2 : �� ������ ������ ������ ������׷� ���� - 1.2������", histogramImage);
	waitKey();
}

void BasicContrastStretching(Mat image, Mat& stretchedImage, int low, int high) {
	//������ ������ ������ ���� 
	stretchedImage = Mat(image.size(), CV_8U, Scalar(0));


	// ������ ��ȸ�ϸ鼭 ȭ�Ҹ� ó��
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			uchar oldPixel = image.at<uchar>(i, j);
			uchar result;

			// �ص� �� Ž�� 
			if (oldPixel <= low) 
				result = 0;
			
			else if (oldPixel < high) {
				// �⺻ ��� ��� ��Ʈ��Ī ���� ����
				result = (uchar)((double)((int)oldPixel - low) / (high - low) * 255);
			}
			else
				result = 255;
			//ó���� ȭ�� 
			stretchedImage.at<uchar>(i, j) = result;
		}
	}
}

void HistogramStretching() {
#pragma region �ߺ��ڵ�
	auto fileName = OpenFileDialog();
	//������ �׷��̽����Ϸ� �о�ɴϴ�.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);

	if (image.empty()) {
		cout << "���� �б� ����" << endl;
		return;
	}
#pragma endregion

	Mat histogram, histogramImage;
	//������ ������׷� ��� 
	CalcHistogram(image, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("�ǽ�3: ������׷� ��Ʈ��Ī - ���� ", histogramImage);

	int low, high;
	cout << "������ : ";
	cin >> low;
	cout << endl << "�ִ� �� : ";
	cin >> high;
	cout << endl;

	Mat stretchedlmage;
	BasicContrastStretching(image, stretchedlmage, low, high);
	// ó���� ������ ������׷� ���
	CalcHistogram(stretchedlmage, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("�ǽ� 3: ������׷� ��Ʈ��Ī- �⺻��ϴ�� �� �ص��� Ž��", histogramImage);
	imshow("�ǽ� 3: ������׷� ��Ʈ��Ī- ���� ", stretchedlmage);
	waitKey();

}


void CalcNormalizedCumulativeFrequency(Mat histogram, Mat& histogramSum) {
	//������׷����� ������ Mat �ν��Ͻ� ����, ������ �ʱ�ȭ
	histogramSum = Mat(histogram.rows, 1, CV_32F, Scalar(0));

	// ������׷��� ���� �ո� ��� 
	float sum = 0;
	for (int i = 0; i < histogram.rows; i++) {
		sum += histogram.at<float>(i);
		histogramSum.at<float>(i) = sum;
	}

	// ����ȭ 
	for (int i = 0; i < histogramSum.rows; i++) {
		histogramSum.at<float>(i) = histogramSum.at<float>(i) / sum * 255.0;
	}
}
	


void HistogramEqualizing() {
#pragma region �ߺ��ڵ�
	auto fileName = OpenFileDialog();
	//������ �׷��̽�����(8��Ʈ = 256 ��鿵��)�� �о� �ɴϴ�.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "���� �б� ����" << endl;
		return;
	}
#pragma endregion
	// 1�ܰ� : ������׷� ���� 
	Mat histogram, histogramSum;

	//2�ܰ� : ���� �� �� ��� 
	CalcHistogram(image, histogram, 256);

	// 3�ܰ� : ���� �� �� ����ȭ 
	CalcNormalizedCumulativeFrequency(histogram, histogramSum);

	//����ȭ�� ���� �� ���� ������� ������ ȭ�Ҹ� ��ȯ
	   // ȭ�Ұ� ����
	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at <uchar>(i, j) = (uchar)histogramSum.at<float>(image.at<uchar>(i, j));
		}
	}
	// ó���� ������ ������׷� ��� 
	Mat histogramImage;
	CalcHistogram(tmp, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow(" �ǽ� 4 : ������׷� ��Ȱȭ - ������׷�", histogramImage);
	imshow(" �ǽ� 4 : ������׷� ��Ȱȭ - ����", tmp);
	waitKey();
}

void Run() {
	//ShowHistogram(); //������׷�����
	//HistogramTransform(); //����
//	HistogramStretching(); //��Ʈ��Ī
	HistogramEqualizing(); //��Ȱȭ
}

int main() 
{
	Run();
}