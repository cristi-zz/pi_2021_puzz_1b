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

Mat_<uchar> getSection(Mat_<uchar> src, int startRow, int startCol, int height, int width)
{
	// se creeaza imaginea cadran de dimensiunea ceruta
	Mat_<uchar> section = Mat_<uchar>(height, width);

	// se extrage din imaginea sursa cadranul cerut
	for (int i = startRow; i < startRow + height; ++i) {
		for (int j = startCol; j < startCol + width; ++j) {
			section(i - startRow, j - startCol) = src(i, j);
		}
	}

	// se returneaza sectiunea extrasa
	return section;
}

std::vector<Mat_<uchar>> sectionImage(Mat_<uchar> src) {
	// se creeaza std::vector-ul de cadrane extrase din imaginea sursa
	std::vector<Mat_<uchar>> sections = {};

	// se stabilesc dimensiunile
	int height, width;
	height = src.rows / 2;
	width = src.cols / 2;

	// colt stanga-sus
	Mat_<uchar> section1 = getSection(src, 0, 0, height, width);
	sections.push_back(section1); // se adauga sectiunea extrasa in std::vector-ul de sectiuni

	// colt dreapta-sus
	Mat_<uchar> section2 = getSection(src, 0, width, height, width);
	sections.push_back(section2);

	// colt stanga-jos
	Mat_<uchar> section3 = getSection(src, height, 0, height, width);
	sections.push_back(section3);

	// colt dreapta-jos
	Mat_<uchar> section4 = getSection(src, height, width, height, width);
	sections.push_back(section4);

	imshow("section1", section1);       // afiseaza sectiunea 1
	imshow("section2", section2);       // afiseaza sectiunea 2
	imshow("section3", section3);       // afiseaza sectiunea 3
	imshow("section4", section4);       // afiseaza sectiunea 4

	return sections;
}

void testSectionImage()
{
	char fname[MAX_PATH];
	openFileDlg(fname);

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> images = sectionImage(src);

	waitKey(0); // asteapta apasarea unei taste
}



Mat_<uchar> computeUpBorder(Mat_<uchar> src, int k) {
    Mat_<uchar> border = Mat_<uchar>(k, src.cols);

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < src.cols; j++) {
            border(i, j) = src(i, j);

        }
    }

    imshow("opened image", src);
    imshow("up border", border);
    waitKey(0);
    return border;
}

Mat_<uchar> computeDownBorder(Mat_<uchar> src, int k) {
    Mat_<uchar> border = Mat_<uchar>(k, src.cols);

    for (int i = src.rows - 1; i >= src.rows - k + 1; i--) {
        for (int j = 0; j < src.cols; j++) {

            border(src.rows - i, j) = src(i, j);

        }
    }

    imshow("opened image", src);
    imshow("down border", border);
    waitKey(0);
    return border;
}

void testComputeUpBorder() {
	char fname[MAX_PATH] = "Images/cameraman.bmp";
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> upBorder = computeUpBorder(src, k);

	imshow("opened image", upBorder);
	waitKey(0);
}


void testComputeRightBorder() {
	char fname[MAX_PATH] = "Images/cameraman.bmp";
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	//Mat_<uchar> rightBorder = computeUpBorder(src, k);

	//imshow("opened image", upBorder);
	waitKey(0);
}
	
void testComputeDownBorder() {
	char fname[MAX_PATH] = "Images/cameraman.bmp";
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> downBorder = computeDownBorder(src, k);

	imshow("opened image", downBorder);
	waitKey(0);
}

void testComputeLeftBorder() {
	char fname[MAX_PATH] = "Images/cameraman.bmp";
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	//Mat_<uchar> leftBorder = computeLeftBorder(src, k);

	//imshow("opened image", leftBorder);
	waitKey(0);
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
	printf(" 4 - RMSE\n");
	printf(" 5 - Section image\n");
	printf(" 6 - Up border \n");
    printf(" 7 - Right border /todo\n");
    printf(" 8 - Down border \n");
    printf(" 9 - Left border /todo\n");

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
	case 5:
		testSectionImage();
		break;
	case 6:
        testComputeUpBorder();
        break;
	case 7:
		testComputeRightBorder();
		break;
	case 8:
		testComputeDownBorder();
		break;
	case 9:
		testComputeLeftBorder();
		break;


	}
	//} 	while (op != 0);
	return 0;
}
