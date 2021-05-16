// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <math.h>
#include <vector>
#include <iostream>

using namespace std;

// variabile globale pe care o sa le folosim pana spre finalul proiectului pt TESTARE
char fname[MAX_PATH] = "Images/saturn.bmp";
const int SECTION_LENGTH = 250;
// EXPERIMENTAT CU VALORI DIFERITE
int TEST_K = 10; 
int PUZZLE_ROWS = 3; // nu modificati valorile astea doua
int PUZZLE_COLS = 3; // pana cand nu implementati shuffleSections. (pt ca moemntat e hardcodat pt 3x3)

// USE THIS FUNCTION TO PRINT SECTIONS (it resize them to be bigger)
void showImage(const char* name, Mat image) {
	namedWindow(name, WINDOW_NORMAL);
	resizeWindow(name, SECTION_LENGTH, SECTION_LENGTH);
	imshow(name, image);
}

void displayPuzzleInput(int puzzleRows, int puzzleCols, std::vector<Mat_<uchar>> sections) {
	int solutionIndex = 0;
	//if (sections.size() != PUZZLE_ROWS * PUZZLE_COLS) printf("WTF?");

	for (int i = 0; i < puzzleRows; i++) {
		for (int j = 0; j < puzzleCols; j++) {
			string stringSectionName = "Input[" + std::to_string(i) + "][" + std::to_string(j) + "]";
			const char* sectionName = stringSectionName.c_str();
			showImage(sectionName, sections[solutionIndex]);
			solutionIndex++;
		}
	}
}

void displayPuzzleSolution(int puzzleRows, int puzzleCols, std::vector<Mat_<uchar>> usedSections) {
	int solutionIndex = 0;
	for (int i = 0; i < puzzleRows; i++) {
		for (int j = 0; j < puzzleCols; j++) {
			string stringSectionName = "Solution[" + std::to_string(i) + "][" + std::to_string(j) + "]";
			const char* sectionName = stringSectionName.c_str();
			showImage(sectionName, usedSections[solutionIndex]);
			solutionIndex++;
		}
	}
}

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

	// se stabilesc dimensiunile noilor imagini
	int height, width;
	height = src.rows / PUZZLE_ROWS;
	width = src.cols / PUZZLE_COLS;

	//numarul total de imagini/sectiuni
	int nrImg;
	nrImg = PUZZLE_ROWS * PUZZLE_COLS;

	int auxRows, auxCols;
	auxRows = 0;
	auxCols = 0;

	for (int i = 0; i < PUZZLE_ROWS; i++) {
		for (int j = 0; j < PUZZLE_COLS; j++) {
			Mat_<uchar> section = getSection(src, auxRows, auxCols, height, width);
			sections.push_back(section);
			auxCols += width;
		}
		auxRows += height;
		auxCols = 0;
	}
	return sections;

}

void testSectionImage() {
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> sections = sectionImage(src);
	displayPuzzleInput(PUZZLE_ROWS, PUZZLE_COLS, sections);
	waitKey(0);
	/*
	for (int i = 0; i < PUZZLE_ROWS; i++) {
		for (int j = 0; j < PUZZLE_COLS; j++) {
			string stringSectionName = "Input " + std::to_string(i);
			const char* sectionName = stringSectionName.c_str();
			showImage(sectionName, images[i]);
		}	
	}
	waitKey(0); // asteapta apasarea unei taste
	*/
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
	Mat_<uchar> upBorder = computeUpBorder(src, TEST_K);

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
	Mat_<uchar> rightBorder = computeRightBorder(src, TEST_K);

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
	Mat_<uchar> downBorder = computeDownBorder(src, TEST_K);

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
	Mat_<uchar> leftBorder = computeLeftBorder(src, TEST_K);

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

// TODO:
std::vector<Mat_<uchar>> shuffleSections(std::vector<Mat_<uchar>> sections) {
	std::vector<Mat_<uchar>> randomSections;
	// TODO: o functie care sa modifice ordinea sectiunilor, dar sections[0] sa ramana pe aceeasi pozitie
	randomSections = { sections[0],
						sections[8], sections[7], sections[2], sections[3], sections[4], sections[6], sections[5], sections[1] };

	return randomSections;
}

double computeUpMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> upBorder = computeUpBorder(firstSection, TEST_K);
	Mat_ <uchar> downBorder = computeDownBorder(secondSection, TEST_K);

	/*
	imshow("UpFirst", upBorder);       // afiseaza sectiunea 1
	imshow("UpSecond", downBorder);       // afiseaza sectiunea 2
	//waitKey(0);
	*/

	return computeRMSE(upBorder, downBorder);
}

double computeRightMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> rightBorder = computeRightBorder(firstSection, TEST_K);
	Mat_ <uchar> leftBorder = computeLeftBorder(secondSection, TEST_K);

	/*
	imshow("RightFirst", rightBorder);       // afiseaza sectiunea 1
	imshow("RightSecond", leftBorder);       // afiseaza sectiunea 2
	//waitKey(0);
	*/

	return computeRMSE(rightBorder, leftBorder);
}

double computeDownMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> downBorder = computeDownBorder(firstSection, TEST_K);
	Mat_ <uchar> upBorder = computeUpBorder(secondSection, TEST_K);

	/*
	imshow("DownFirst", downBorder);       // afiseaza sectiunea 1
	imshow("DownSecond", upBorder);       // afiseaza sectiunea 2
	//waitKey(0);
	*/

	return computeRMSE(downBorder, upBorder);
}

double computeLeftMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> leftBorder = computeLeftBorder(firstSection, TEST_K);
	Mat_ <uchar> rightBorder = computeRightBorder(secondSection, TEST_K);

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


enum CheckCodes {CHECK_ONLY_LEFT, CHECK_ONLY_UP, CHECK_BOTH};
int computeCheckCode(int row, int col) {
	if (row == 0) {
		// check only left
		return CHECK_ONLY_LEFT;
	}
	else if (col == 0) {
		// check only up
		return CHECK_ONLY_UP;
	}
	else {
		// check both
		return CHECK_BOTH;
	}
}

int findBestMatchIndex(int row, int col, int puzzleCols, std::vector<Mat_<uchar>> usedSections, std::vector<Mat_<uchar>> unusedSections) {
	// precondition: (row, col) != (0, 0) [which is fixed]
	
	int leftIndex = usedSections.size() - 1;
	int upIndex = usedSections.size() - puzzleCols;
	// see what we have to check
	int checkCode = computeCheckCode(row, col);
	
	switch (checkCode) {
		case CHECK_ONLY_LEFT: {
			// compute left section (from puzzle)
			Mat_ <uchar> leftSection = usedSections[leftIndex];
			std::vector<double> rightMatches = {};
			
			// compute matches for all candidate sections
			for (int i = 0; i < unusedSections.size(); i++) {
				Mat_ <uchar> candidateSection = unusedSections[i];
				//todo ROTATE
				rightMatches.push_back(computeRightMatching(leftSection, candidateSection));
			}

			// select min match (the best one)
			int minElementIndex = std::min_element(rightMatches.begin(), rightMatches.end()) - rightMatches.begin();
			return minElementIndex;
		}
		case CHECK_ONLY_UP: {
			// compute up section (from puzzle)
			Mat_ <uchar> upSection = usedSections[upIndex];
			std::vector<double> downMatches = {};

			// compute matches for all candidate sections
			for (int i = 0; i < unusedSections.size(); i++) {
				Mat_ <uchar> candidateSection = unusedSections[i];
				//todo ROTATE
				downMatches.push_back(computeDownMatching(upSection, candidateSection));
			}

			// select min match (the best one)
			int minElementIndex = std::min_element(downMatches.begin(), downMatches.end()) - downMatches.begin();
			return minElementIndex;
		}
		case CHECK_BOTH: {
			// compute left section (from puzzle)
			Mat_ <uchar> leftSection = usedSections[leftIndex];
			std::vector<double> rightMatches = {};

			// compute up section (from puzzle)
			Mat_ <uchar> upSection = usedSections[upIndex];
			std::vector<double> downMatches = {};

			std::vector<double> matches = {}; // averages of the 2 matches
			// compute matches for all candidate sections
			for (int i = 0; i < unusedSections.size(); i++) {
				Mat_ <uchar> candidateSection = unusedSections[i];
				// TODO rotate
				rightMatches.push_back(computeRightMatching(leftSection, candidateSection));
				downMatches.push_back(computeDownMatching(upSection, candidateSection));
				// compute average betweens this 2 values
				double match = (rightMatches[i] + downMatches[i]) / 2.0;
				matches.push_back(match);
			}
			
			// select min match (the best one)
			int minElementIndex = std::min_element(matches.begin(), matches.end()) - matches.begin();
			return minElementIndex;
		}
	}
}

void solvePuzzle(int puzzleRows, int puzzleCols, std::vector<Mat_<uchar>> sections) {
	// solve the puzzle
	std::vector<Mat_<uchar>> usedSections = { sections[0] }; // pieces that are already fixed in the puzzle solution
	sections.erase(sections.begin());
	std::vector<Mat_<uchar>> unusedSections = sections; // pieces that will be tested

	for (int i = 0; i < PUZZLE_ROWS; i++) {
		for (int j = 0; j < PUZZLE_COLS; j++) {

			// top-left corner piece is fixed, so skip this one
			if (i == 0 && j == 0) {
				continue;
			}

			int bestMatchIndex = findBestMatchIndex(i, j, PUZZLE_COLS, usedSections, unusedSections);
			usedSections.push_back(unusedSections[bestMatchIndex]);
			unusedSections.erase(unusedSections.begin() + bestMatchIndex);
		}
	}

	// print solution
	displayPuzzleSolution(PUZZLE_ROWS, PUZZLE_COLS, usedSections);
	waitKey(0); // asteapta apasarea unei taste
}

void testPuzzle(){
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	// computing the input
	std::vector<Mat_<uchar>> sections = sectionImage(src); // get the pieces of puzzle
 	sections = shuffleSections(sections); // shuffle them
	displayPuzzleInput(PUZZLE_ROWS, PUZZLE_COLS, sections); // and display them

	solvePuzzle(PUZZLE_ROWS, PUZZLE_COLS, sections);
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

void multipleCases() {
	// TODO: TEST ON
	//			multiple images [change src]
	//			different puzzle dimensions [change puzzleRows/puzzleCols]
	//			different depths [change test_k]
	
	// test on multiple images // NU MERG IMAGINILE PORTRAIT! FOLOSITI NUMA IMAGINI LANDSCAPE (SAU PATRATE)
	const int SOURCE_COUNT = 3; // 3 test images
	char* sources[SOURCE_COUNT] = { "Images/cameraman.bmp", "Images/eight.bmp", "Images/saturn.bmp" };
	
	// test on multiple puzzle dimensions
	const int PUZZLE_ROWS_COUNT = 3; // 3 cases: 1) ROWS = COLS [patrat]; 2) ROWS > COLS [portrait]; 3) ROWS < COLS [landscape]
	const int PUZZLE_COLS_COUNT = 3; // <same here>
	const int puzzleRowsValues[PUZZLE_ROWS_COUNT] = { 3, 4, 2 };
	const int puzzleColsValues[PUZZLE_ROWS_COUNT] = { 3, 2, 4 };

	// test on multiple depths
	const int DEPTH_COUNT = 3;
	const int depths[DEPTH_COUNT] = { 5, 10, 15 }; 
	
	for (int sourceIndex = 0; sourceIndex < SOURCE_COUNT; sourceIndex++) {
		// for each image source
		char* fname = sources[sourceIndex];
		Mat_<uchar> src = imread(fname, IMREAD_GRAYSCALE);

		for (int puzzleDimensionIndex = 0; puzzleDimensionIndex < PUZZLE_ROWS_COUNT; puzzleDimensionIndex++) {
			// for each puzzle dimension
			int puzzleRows = puzzleRowsValues[puzzleDimensionIndex];
			int puzzleCols = puzzleColsValues[puzzleDimensionIndex];

			// computing the input
			std::vector<Mat_<uchar>> sections = sectionImage(src); // get the pieces of puzzle
			sections = shuffleSections(sections); // shuffle them
			displayPuzzleInput(puzzleRows, puzzleRows, sections); // and display them

			for (int depthIndex = 0; depthIndex < DEPTH_COUNT; depthIndex++) {
				// for each depth
				TEST_K = depths[depthIndex];

				printf("Image: %s, Puzzle: %dx%d, Depth: %d\n", fname, puzzleRows, puzzleCols, TEST_K);
				
				solvePuzzle(PUZZLE_ROWS, PUZZLE_COLS, sections);
			}
		}
	}
}

void testMultipleCases() {
	const int SOURCE_COUNT = 3; // 3 test images
	char* sources[SOURCE_COUNT] = { "Images/cameraman.bmp", "Images/eight.bmp", "Images/saturn.bmp" };

	// test on multiple puzzle dimensions
	const int PUZZLE_ROWS_COUNT = 3; // 3 cases: 1) ROWS = COLS [patrat]; 2) ROWS > COLS [portrait]; 3) ROWS < COLS [landscape]
	const int PUZZLE_COLS_COUNT = 3; // <same here>
	const int puzzleRowsValues[PUZZLE_ROWS_COUNT] = { 3, 4, 2 };
	const int puzzleColsValues[PUZZLE_ROWS_COUNT] = { 3, 2, 4 };

	// test on multiple depths
	const int DEPTH_COUNT = 3;
	const int depths[DEPTH_COUNT] = { 5, 10, 15 };

	for (int sourceIndex = 0; sourceIndex < SOURCE_COUNT; sourceIndex++) {
		// for each image source
		char* fname = sources[sourceIndex];
		Mat_<uchar> src = imread(fname, IMREAD_GRAYSCALE);

		int puzzleRows = 3;
		int puzzleCols = 3;

		// computing the input
		std::vector<Mat_<uchar>> sections = sectionImage(src); // get the pieces of puzzle
		sections = shuffleSections(sections); // shuffle them
		displayPuzzleInput(puzzleRows, puzzleRows, sections); // and display them

		for (int depthIndex = 0; depthIndex < DEPTH_COUNT; depthIndex++) {
			// for each depth
			TEST_K = depths[depthIndex];

			// show info
			printf("Image: %s, Puzzle: %dx%d, Depth: %d\n", fname, puzzleRows, puzzleCols, TEST_K);

			solvePuzzle(PUZZLE_ROWS, PUZZLE_COLS, sections);
		}
	}
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
		printf(" 7 - Right border \n");
		printf(" 8 - Down border \n");
		printf(" 9 - Left border \n");
		printf(" 20 - Solve Puzzle\n");
		printf(" 21 - Test Puzzle Matching\n");
		printf(" 22 - Puzzle Comparisons\n");
		printf(" 23 - Test Puzzle Comparisons\n");
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
		case 22:
			multipleCases();
			break;
		case 23:
			testMultipleCases();
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
