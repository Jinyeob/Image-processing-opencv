#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <string.h>
#include <iostream>
using namespace std;
using namespace cv;

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

typedef struct {
	string title;
	Mat image;

}TrackBar;

int Warping(int input) {
	return input %= 256;
}
int Clamping(int input) {
	if (input < 0)
		input = 0;
	else if (input > 255)
		input = 255;
	return input;
}

void BrightnessControl(string file, bool isWarpping) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "����" << endl;
		return;
	}
	string title;
	if (!isWarpping) {
		title = "Ŭ����";
	}
	else {
		title = "����";
	}
	namedWindow(title, WINDOW_AUTOSIZE);
	int value = 128;
	TrackBar args{ title = title, image = image };
	createTrackbar(title + "���", title, &value, 255, [](int value, void* data) {
		auto args = (TrackBar*)data;
		string title = args->title;
		Mat image = args->image;
		auto tmp = image.clone();

		int change = value - 128;
		if (title.compare("Ŭ����")) {
			for (int i = 0; i < image.rows; i++) {
				for (int j = 0; j < image.cols; j++) {
					tmp.at<uchar>(i, j) = Clamping(image.at<uchar>(i, j) + change);
				}

			}
		}
		else {
			for (int i = 0; i < image.rows; i++) {
				for (int j = 0; j < image.cols; j++) {
					tmp.at<uchar>(i, j) = Warping(image.at<uchar>(i, j) + change);
				}

			}
		}
		imshow(title, tmp);
		}, &args);
	imshow(title, image);
	waitKey();
}
void DifferentialComputation(std::string file, std::string file2) {
	auto image1 = imread(file, IMREAD_GRAYSCALE); auto image2 = imread(file2, IMREAD_GRAYSCALE);
	if (image1.empty() || image2.empty()) {
		cout << "Different ia ComputationO:�����б����" << endl;
		return;
	}
	if (image1.size() != image2.size()) {
		cout << "Different ia Computation():�� ������ ũ�Ⱑ �ٸ�" << endl;
		return;
	}

	auto image = Mat(image1.size(), image1.type());
	for (int i = 0; i < image1.rows; i++) {
		for (int j = 0; j < image1.cols; j++) {
			image.at<uchar>(i, j) = Clamping(image1.at<uchar>(i, j) - image2.at<uchar>(i, j));
		}
	}
	imshow("������", image);
	waitKey();
}
void MultiplicationComputation(std::string file, double mul) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "MultiplicationComput ����" << endl;
			return;
	}

	auto tmp = Mat(image.size(), image.type());
		for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {
				tmp.at<uchar>(i, j) = Clamping(image.at<uchar>(i, j) * mul);
			}
		}
	imshow("������������", tmp);
	waitKey();
}
void AndComputation(std::string file1, std::string file2) {
	auto image1 = imread(file1, IMREAD_GRAYSCALE);
	auto image2 = imread(file2, IMREAD_GRAYSCALE);
	if (image1.empty() || image2.empty()) {
		cout << "AndComputation() : ����" << endl;
		return;
	}
	if (image1.size() != image2.size()) {
		cout << "AndComputation(): �ΰ� �ٸ�" << endl;
		return;
	}
	auto image = Mat(image1.size(), image1.type());
	for (int i = 0; i < image1.rows; i++) {
		for (int j = 0; j < image1.cols; j++) {
			image.at<uchar>(i, j) = image1.at<uchar>(i, j) & image2.at <uchar>(i, j);
		}
	}
	imshow("AND ����", image);
	waitKey();

}
void OrComputation(std::string file1, std::string file2) {
	auto image1 = imread(file1, IMREAD_GRAYSCALE);
	auto image2 = imread(file2, IMREAD_GRAYSCALE);
	if (image1.empty() || image2.empty()) {
		cout << "OrComputation() : ����" << endl;
		return;
	}
	if (image1.size() != image2.size()) {
		cout << "OrComputation(): �ΰ� �ٸ�" << endl;
		return;
	}
	auto image = Mat(image1.size(), image1.type());
	for (int i = 0; i < image1.rows; i++) {
		for (int j = 0; j < image1.cols; j++) {
			image.at<uchar>(i, j) = image1.at<uchar>(i, j) | image2.at<uchar>(i, j);
		}
	}
	imshow("OR ����", image);
	waitKey();
}
void XorComputation(std::string file, int con) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "XorComputation() : ����" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = image.at<uchar>(i, j) ^ (uchar)con;
		}
	}
	imshow("XOR ����", tmp);
	waitKey();
}
void NotComputation(std::string file) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "NotComputation() : ����" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = ~image.at<uchar>(i, j);
		}
	}
	imshow("NOT ����", tmp);
	waitKey();
}
void InvertedConversion(std::string file) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "InvertedComputation() : ����" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = 255-image.at<uchar>(i, j);
		}
	}
	imshow("������ȯ", tmp);
	waitKey();
}
void CalibrateGamma(std::string file,double gamma) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "CalibrateGamma() : ����" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = Clamping(pow(image.at<uchar>(i, j), 1.0 / gamma));

		}
	}
	imshow("��������", tmp);
	waitKey();
}
void Posterizing(std::string file,  int n) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "Posterizing() : ����" << endl;
		return;
	}
	if (n<2) {
		cout << "Posterizing() : ��谪 �ּ�(2)���� ����" << endl;
		return;
	}
	double step = 256.0 / n;
	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = ((uchar)(image.at<uchar> (i, j)/ step)) * step;
		}
	}
	imshow("��������", tmp);
	waitKey();
}

void Run() {
	auto fileName = OpenFileDialog();
	//Ŭ���� 
	BrightnessControl(fileName, 0);
	BrightnessControl(fileName, 1);
	auto differentialComputationFileName1 = OpenFileDialog();
	auto differentialComputationFileName2 = OpenFileDialog();
	//�� ���� 
	DifferentialComputation(differentialComputationFileName1, differentialComputationFileName2);
	//�� ����
	MultiplicationComputation(fileName, 1.5);
	//������ ���� 
	MultiplicationComputation(fileName, 0.66);
	auto andComputationImageFileName = OpenFileDialog();
	auto andComputationMaskFileName = OpenFileDialog();
	// AND
	AndComputation(andComputationImageFileName, andComputationMaskFileName);
	// OR
	OrComputation(andComputationImageFileName, andComputationMaskFileName);
	// XOR
	XorComputation(andComputationImageFileName, 128);
	//NOT ���� 
	NotComputation(andComputationImageFileName);
	//���� ��ȯ 
	InvertedConversion(andComputationImageFileName);
	//���� ����
	CalibrateGamma(andComputationImageFileName, 0.8);
	CalibrateGamma(andComputationImageFileName, 1.2);
	//������ ����¡ 
	Posterizing(andComputationImageFileName, 2);
	Posterizing(andComputationImageFileName, 8);
}

int main() {
	Run();
}