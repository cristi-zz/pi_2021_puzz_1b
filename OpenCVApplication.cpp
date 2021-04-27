// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

// variabile globale pe care o sa le folosim pana spre finalul proiectului pt TESTARE
char fname[MAX_PATH] = "Images/cameraman.bmp";
const int testK = 10;


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

	if (first.cols != second.cols || first.rows != second.rows) {
		printf("\nERROR: the borders do not have the same size\n");
		exit(1);
	}

	double rmse = 0.0;
	int nrOfElements = first.rows * first.cols;

	for (int i = 0; i < first.rows; i++)
	{
		for (int j = 0; j < first.cols; j++)
		{
			rmse += (first(i, j) - second(i, j)) * (first(i, j) - second(i, j));
		}
	}

	rmse = sqrt(rmse / nrOfElements);
	return rmse;
}


void testRMSE() {

	const int r = 2, c = 4;

	uchar m1[r][c] = {
	   {3,2,3,4},
	   {1,2,3,4}
	};
	uchar m2[r][c] = {
	   {1,2,3,4},
	   {1,2,3,4}
	};

	Mat_<uchar> b1 = Mat(r, c, CV_8UC1, &m1);
	Mat_<uchar> b2 = Mat(r, c, CV_8UC1, &m2);

	double rmse = computeRMSE(b1, b2);
	printf("RMSE = %f", rmse);
	
	imshow("just for waitKey", NULL);
	waitKey(0);
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

	return sections;
}

void testSectionImage()
{
	// partea asta o lasam comentata pana spre finalul proiectului. LUCRAM DOAR PE CAMERAMAN
	/*
	char fname[MAX_PATH];
	openFileDlg(fname);
	*/

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> images = sectionImage(src);
	imshow("section1", images[0]);       // afiseaza sectiunea 1
	imshow("section2", images[1]);       // afiseaza sectiunea 2
	imshow("section3", images[2]);       // afiseaza sectiunea 3
	imshow("section4", images[3]);       // afiseaza sectiunea 4
	waitKey(0); // asteapta apasarea unei taste
}


Mat_<uchar> computeUpBorder(Mat_<uchar> src, int k) {
	Mat_<uchar> border = Mat_<uchar>(k, src.cols);

	for (int i = 0; i < k; i++) {
		for (int j = 0; j < src.cols; j++) {
			border(i, j) = src(i, j);
		}
	}

	return border;
}

Mat_<uchar> computeDownBorder(Mat_<uchar> src, int k) {
	Mat_<uchar> border = Mat_<uchar>(k, src.cols);

	for (int i = src.rows - 1; i >= src.rows - k + 1; i--) {
		for (int j = 0; j < src.cols; j++) {
			border(src.rows - i, j) = src(i, j);
		}
	}

	return border;
}

Mat_<uchar> computeLeftBorder(Mat_<uchar> src, int k) {
	Mat_<uchar> border = Mat_<uchar>(k, src.cols);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < k; j++) {
			border(k - 1 - j, i) = src(i, k - j - 1);
		}
	}

	return border;
}

Mat_<uchar> computeRightBorder(Mat_<uchar> src, int k) {
	//transpun bordura verticala dreapta pe orizontala
	Mat_<uchar> border = Mat_<uchar>(k, src.cols);
	//bordura dreapta se va "rasturna" spre stanga
	for (int i = 0; i < src.rows; i++) {
		for (int j = src.cols - 1; j >= src.cols - k; j--) {
			border(src.cols - 1 - j, i) = src(i, j);
		}
	}

	return border;
}


void testComputeUpBorder() {
	// partea asta o lasam comentata pana spre finalul proiectului. LUCRAM DOAR PE CAMERAMAN
	/*
	char fname[MAX_PATH];
	openFileDlg(fname);
	*/

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> upBorder = computeUpBorder(src, testK);

	imshow("original", src);
	imshow("up border", upBorder);
	waitKey(0);
}

void testComputeRightBorder() {
	// partea asta o lasam comentata pana spre finalul proiectului. LUCRAM DOAR PE CAMERAMAN
	/*
	char fname[MAX_PATH];
	openFileDlg(fname);
	*/

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> rightBorder = computeRightBorder(src, testK);

	imshow("original", src);
	imshow("right border", rightBorder);
	waitKey(0);
}

void testComputeDownBorder() {
	// partea asta o lasam comentata pana spre finalul proiectului. LUCRAM DOAR PE CAMERAMAN
	/*
	char fname[MAX_PATH];
	openFileDlg(fname);
	*/

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> downBorder = computeDownBorder(src, testK);

	imshow("original", src);
	imshow("down border", downBorder);
	waitKey(0);
}

void testComputeLeftBorder() {
	// partea asta o lasam comentata pana spre finalul proiectului. LUCRAM DOAR PE CAMERAMAN
	/*
	char fname[MAX_PATH];
	openFileDlg(fname);
	*/

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> leftBorder = computeLeftBorder(src, testK);

	imshow("original", src);
	imshow("left border", leftBorder);
	waitKey(0);
}


void show(Mat_<uchar> img) {
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			printf("%d ", img(i, j));
		}
		printf("  --  \n");
	}
}
/*
void testLeftBorder() {
	Mat_<uchar> img = imread(fname, IMREAD_GRAYSCALE);
	Mat_<uchar> left = computeLeftBorder(img, 2);

	show(img);
	show(left);
}

void testRightBorder() {
	Mat_<uchar> img = imread(fname, IMREAD_GRAYSCALE);
	Mat_<uchar> left = computeRightBorder(img, 2);

	show(img);
	show(left);
}
*/

void testMatchingBorders()
{
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> sections = sectionImage(src);

	Mat_<uchar> img1 = computeRightBorder(sections[0], 40);
	Mat_<uchar> img2 = computeLeftBorder(sections[1], 40);

	imshow("colt-stanga-sus", img1);
	imshow("colt-dreapta-sus", img2);

	waitKey(0);
}

std::vector<Mat_<uchar>> shuffleSections(std::vector<Mat_<uchar>> sections) {
	std::vector<Mat_<uchar>> randomSections;
	randomSections = sections;
	srand(time(NULL));
	int size = randomSections.size();
	for (int i = 0; i < size; i++) {
		int k = rand() * rand() % size;
		swap(randomSections[i], randomSections[k]);
	}

	return randomSections;
}

double computeUpMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> upBorder = computeUpBorder(firstSection, testK);
	Mat_ <uchar> downBorder = computeDownBorder(secondSection, testK);

	/*
	imshow("UpFirst", upBorder);       // afiseaza sectiunea 1
	imshow("UpSecond", downBorder);       // afiseaza sectiunea 2
	//waitKey(0);
	*/

	return computeRMSE(upBorder, downBorder);
}

double computeRightMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> rightBorder = computeRightBorder(firstSection, testK);
	Mat_ <uchar> leftBorder = computeLeftBorder(secondSection, testK);

	/*
	imshow("RightFirst", rightBorder);       // afiseaza sectiunea 1
	imshow("RightSecond", leftBorder);       // afiseaza sectiunea 2
	//waitKey(0);
	*/

	return computeRMSE(rightBorder, leftBorder);
}

double computeDownMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> downBorder = computeDownBorder(firstSection, testK);
	Mat_ <uchar> upBorder = computeUpBorder(secondSection, testK);

	/*
	imshow("DownFirst", downBorder);       // afiseaza sectiunea 1
	imshow("DownSecond", upBorder);       // afiseaza sectiunea 2
	//waitKey(0);
	*/

	return computeRMSE(downBorder, upBorder);
}

double computeLeftMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> leftBorder = computeLeftBorder(firstSection, testK);
	Mat_ <uchar> rightBorder = computeRightBorder(secondSection, testK);

	/*
	imshow("LeftFirst", leftBorder);       // afiseaza sectiunea 1
	imshow("LeftSecond", rightBorder);       // afiseaza sectiunea 2
	//waitKey(0);
	*/

	return computeRMSE(leftBorder, rightBorder);
}

double computeScore(std::vector<Mat_<uchar>> sections) {
	double totalScore = 0;
	int size = sections.size();

	// sections[0] - FIXAT

	for (int i = 0; i < size - 1; i++) {
		Mat_<uchar> firstSection = sections[i];

		for (int j = i + 1; j < size; j++) {

			//for each rotation...

			// for each pair of sections
			Mat_<uchar> secondSection = sections[j];
			
			// todo: check Up, Right, Down, Left matching

			double upMatching = computeUpMatching(firstSection, secondSection);
			double rightMatching = computeRightMatching(firstSection, secondSection);
			double downMatching = computeDownMatching(firstSection, secondSection);
			double leftMatching = computeLeftMatching(firstSection, secondSection);

			printf("Between %d and %d:\n", i, j);
			printf("UP = %f\n", upMatching);
			printf("RIGHT = %f\n", rightMatching);
			printf("DOWN = %f\n", downMatching);
			printf("LEFT = %f\n", leftMatching);
				
			printf("\n");
			//printf("(i = %d, j = %d)\n", i, j);

		}
	}
	return totalScore;
}

void testPuzzle(){

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> sections = sectionImage(src);
	double inputScore = computeScore(sections);

	
	//std::vector<Mat_<uchar>> randomSections = shuffleSections(sections);
	//double outputScore = computeScore(randomSections);
	
	//namedWindow() // google
	
	imshow("Input0", sections[0]);       // afiseaza sectiunea 1
	imshow("Input1", sections[1]);       // afiseaza sectiunea 2
	imshow("Input2", sections[2]);       // afiseaza sectiunea 3
	imshow("Input3", sections[3]);       // afiseaza sectiunea 4
	/*
	imshow("Output0", randomSections[0]);       // afiseaza sectiunea 1
	imshow("Output1", randomSections[1]);       // afiseaza sectiunea 2
	imshow("Output2", randomSections[2]);       // afiseaza sectiunea 3
	imshow("Output3", randomSections[3]);       // afiseaza sectiunea 4
	*/
	waitKey(0); // asteapta apasarea unei taste
	
}

void testPuzzleMatching() {
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> sections = sectionImage(src);

	Mat_ <uchar> firstSection = sections[0];
	Mat_ <uchar> secondSection = sections[2];



	double upMatching = computeUpMatching(firstSection, secondSection);
	double rightMatching = computeRightMatching(firstSection, secondSection);
	double downMatching = computeDownMatching(firstSection, secondSection);
	double leftMatching = computeLeftMatching(firstSection, secondSection);
	printf("UP = %f\n", upMatching);
	printf("RIGHT = %f\n", rightMatching);
	printf("DOWN = %f\n", downMatching);
	printf("LEFT = %f\n", leftMatching);

	imshow("Input0", firstSection);       // afiseaza sectiunea 1
	imshow("Input1", secondSection);       // afiseaza sectiunea 2
	waitKey(0);
}

int main()
{
	int op;
	do
	{
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
		printf(" 20 - Test Puzzle\n");
		printf(" 21 - Test Puzzle Matching\n");
		printf(" 31 - Test Matching Borders\n");

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
		// ANDREI
		// SASA
		case 20:
			testPuzzle();
			break;
		case 21:
			testPuzzleMatching();
			break;
		// IULIA
		case 31:
			testMatchingBorders();
			break;
		// DIANA
		}
	} while (op != 0);
	return 0;
}
