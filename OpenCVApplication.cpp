// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <math.h> 
#include <vector>
#include <iostream>

using namespace std;

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i, j) = (r + g + b) / 3;
			}
		}

		imshow("original image", src);
		imshow("gray image", dst);
		waitKey();
	}
}


double computeRMSE(Mat_<uchar> first, Mat_<uchar> second) {

	if (first.cols != second.cols) return -1.0;

	double rmse = 0.0;
	int n = first.cols;

	for (int i = 0; i < n; i++)
	{
		rmse += (double)((first(0, i) - second(0, i)) * (first(0, i) - second(0, i))) / n;
	}

	rmse = sqrt(rmse);
	return rmse;
}

// verifici k si iti dai seama daca e oriz sau vert
// first second
vector<double> computeRMSEK(Mat_<uchar> first, Mat_<uchar> second, int k) {

	if (first.cols != second.cols || first.rows != second.rows) {
		printf("\nERROR: the matrices do not have the same size\n");
		exit(1);
	}

	if (k > first.rows || k < 0) {
		printf("\nERROR: k out of range\n");
		exit(1);
	}

	vector<double> rmse;

	for (int i = 0; i < k; i++)
	{
		rmse.push_back(computeRMSE(first.row(i), second.row(i)));
	}

	return rmse;
}


void testRMSE() {

	
	// test RMSE
	/*
	Mat_<uchar> a(1, 4);
	Mat_<uchar> b(1, 4);

	a(0, 0) = 1;
	a(0, 1) = 2;
	a(0, 2) = 3;
	a(0, 3) = 4;

	b(0, 0) = 1;
	b(0, 1) = 2;
	b(0, 2) = 3;
	b(0, 3) = 5;*/

	//printf("%f ", computeRMSE(a, b));

	// test rmseK
	
	const int r = 2, c = 4;

	uchar m1[r][c] = {
	   { 1, 2, 3, 4},
	   { 1, 2, 3, 4}
	};
	uchar m2[r][c] = {
	   { 2, 2, 3, 4},
	   { 1, 2, 3, 4}
	};

	Mat_<uchar> b1 = Mat(r, c, CV_8UC1, &m1);
	Mat_<uchar> b2 = Mat(r, c, CV_8UC1, &m2);

	vector<double> rmse = computeRMSEK(b1, b2, 2);

	// print rmse
	for (int i = 0; i < rmse.size(); i++) {
	std::cout << rmse.at(i) << ' ';
	}
}

int main()
{
	int op;
	//do
	//{
	system("cls");
	destroyAllWindows();
	printf("Menu:\n");
	printf(" 1 - Basic image opening...\n");
	printf(" 2 - Open BMP images from folder\n");
	printf(" 3 - Color to Gray\n");
	printf(" 4 - rmse\n");
	printf(" 0 - Exit\n\n");
	printf("Option: ");
	scanf("%d", &op);
	switch (op)
	{
	case 1:
		testOpenImage();
		break;
	case 2:
		testOpenImagesFld();
		break;
	case 3:
		testColor2Gray();
		break;

	case 4:
		testRMSE();
		break;

	}
	//} 	while (op != 0);
	return 0;
}
