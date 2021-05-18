// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <math.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

const int SECTION_LENGTH = 250;

char fname[MAX_PATH] = "Images/cameraman.bmp";
const int PUZZLE_ROWS = 2;
const int PUZZLE_COLS = 2;
int DEPTH = 10; 

// USE THIS FUNCTION TO PRINT SECTIONS (it resize them to be bigger)
void showImage(const char* name, Mat image) {
	namedWindow(name, WINDOW_NORMAL);
	resizeWindow(name, SECTION_LENGTH, SECTION_LENGTH);
	imshow(name, image);
}

void displayPuzzleInput(std::vector<Mat_<uchar>> sections) {
	for (int i = 0; i < sections.size(); i++) {
		string stringSectionName = "Input[" + std::to_string(i) + "]";
		const char* sectionName = stringSectionName.c_str();
		showImage(sectionName, sections[i]);
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

std::vector<Mat_<uchar>> sectionImage(Mat_<uchar> src, int puzzleRows, int puzzleCols) {
	// se creeaza std::vector-ul de cadrane extrase din imaginea sursa
	std::vector<Mat_<uchar>> sections = {};

	// se stabilesc dimensiunile noilor imagini
	int height, width;
	height = src.rows / puzzleRows;
	width = src.cols / puzzleCols;

	//numarul total de imagini/sectiuni
	int nrImg;
	nrImg = puzzleRows * puzzleCols;

	int auxRows, auxCols;
	auxRows = 0;
	auxCols = 0;

	for (int i = 0; i < puzzleRows; i++) {
		for (int j = 0; j < puzzleCols; j++) {
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

	std::vector<Mat_<uchar>> sections = sectionImage(src, PUZZLE_ROWS, PUZZLE_COLS);
	displayPuzzleInput(sections);
	waitKey(0);
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
	Mat_<uchar> border = Mat_<uchar>(k, src.rows);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < k; j++) {
			border(k - 1 - j, i) = src(i, k - j - 1);
		}
	}

	return border;
}

Mat_<uchar> computeRightBorder(Mat_<uchar> src, int k) {
	//transpun bordura verticala dreapta pe orizontala
	Mat_<uchar> border = Mat_<uchar>(k, src.rows);
	//bordura dreapta se va "rasturna" spre stanga
	for (int i = 0; i < src.rows; i++) {
		for (int j = src.cols - 1; j >= src.cols - k; j--) {	
			border(src.cols - 1 - j, i) = src(i, j);
		}
	}

	return border;
}

void testComputeUpBorder() {
	char fname[MAX_PATH];
	openFileDlg(fname);

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> upBorder = computeUpBorder(src, DEPTH);

	imshow("original", src);
	imshow("up border", upBorder);
	waitKey(0);
}

void testComputeRightBorder() {
	char fname[MAX_PATH];
	openFileDlg(fname);

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> rightBorder = computeRightBorder(src, DEPTH);

	imshow("original", src);
	imshow("right border", rightBorder);
	waitKey(0);
}

void testComputeDownBorder() {
	char fname[MAX_PATH];
	openFileDlg(fname);

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> downBorder = computeDownBorder(src, DEPTH);

	imshow("original", src);
	imshow("down border", downBorder);
	waitKey(0);
}

void testComputeLeftBorder() {
	char fname[MAX_PATH];
	openFileDlg(fname);
	
	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	int k = 10;
	Mat_<uchar> leftBorder = computeLeftBorder(src, DEPTH);

	imshow("original", src);
	imshow("left border", leftBorder);
	waitKey(0);
}

std::vector<Mat_<uchar>> shuffleSections(std::vector<Mat_<uchar>> sections) {
	std::vector<Mat_<uchar>> randomSections(sections);

	randomSections[0] = sections[0];

	vector<int> visited;
	visited.push_back(0);

	for (int i = 1; i < sections.size(); i++)
	{
		int position;
		bool bad = true;

		while (bad) {
			bool found = false;

			position = rand() % sections.size();

			for (int j = 0; j < visited.size(); j++) {
				if (visited[j] == position) {
					found = true;
				}
			}
			
			if (found == false) {
				bad = false;
			}
				
		}

		randomSections[position] = sections[i];
		visited.push_back(position);
	}

	return randomSections;
}

double computeUpMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> upBorder = computeUpBorder(firstSection, DEPTH);
	Mat_ <uchar> downBorder = computeDownBorder(secondSection, DEPTH);

	return computeRMSE(upBorder, downBorder);
}

double computeRightMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> rightBorder = computeRightBorder(firstSection, DEPTH);
	Mat_ <uchar> leftBorder = computeLeftBorder(secondSection, DEPTH);

	return computeRMSE(rightBorder, leftBorder);
}

double computeDownMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> downBorder = computeDownBorder(firstSection, DEPTH);
	Mat_ <uchar> upBorder = computeUpBorder(secondSection, DEPTH);

	return computeRMSE(downBorder, upBorder);
}

double computeLeftMatching(Mat_ <uchar> firstSection, Mat_ <uchar> secondSection) {
	Mat_ <uchar> leftBorder = computeLeftBorder(firstSection, DEPTH);
	Mat_ <uchar> rightBorder = computeRightBorder(secondSection, DEPTH);

	return computeRMSE(leftBorder, rightBorder);
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

void displayPuzzleMatching(int puzzleRows, int puzzleCols, std::vector<Mat_<uchar>> sections) {
	
	printf("RIGHT MATCHING\n");
	std::vector<double> rightMatches = {};
	printf("First piece (TOP-LEFT CORNER) matches:\n");
	for (int i = 1; i < sections.size(); i++) {
		double rightMatch = computeRightMatching(sections[0], sections[i]);
		rightMatches.push_back(rightMatch);
		printf("With section %d = %f\n", i, rightMatch);
	}
	int minRightElementIndex = std::min_element(rightMatches.begin(), rightMatches.end()) - rightMatches.begin() + 1;
	printf("The picked one is Input[%d]\n\n", minRightElementIndex);
	
	sections.erase(sections.begin() + minRightElementIndex);

	printf("DOWN MATCHING\n");
	std::vector<double> downMatches = {};
	printf("First piece (TOP-LEFT CORNER) matches:\n");
	for (int i = 1; i < sections.size(); i++) {
		double downMatch = computeDownMatching(sections[0], sections[i]);
		downMatches.push_back(downMatch);
		int k = i;
		if (k >= minRightElementIndex) k++;
		printf("With section %d = %f\n", k, downMatch);
	}
	int minDownElementIndex = std::min_element(downMatches.begin(), downMatches.end()) - downMatches.begin() + 2;
	printf("The picked one is Input[%d]\n\n", minDownElementIndex);

}

void solvePuzzle(int puzzleRows, int puzzleCols, std::vector<Mat_<uchar>> sections) {
	displayPuzzleMatching(puzzleRows, puzzleCols, sections);

	// solve the puzzle
	std::vector<Mat_<uchar>> usedSections = { sections[0] }; // pieces that are already fixed in the puzzle solution
	sections.erase(sections.begin());
	std::vector<Mat_<uchar>> unusedSections = sections; // pieces that will be tested

	for (int i = 0; i < puzzleRows; i++) {
		for (int j = 0; j < puzzleCols; j++) {

			// top-left corner piece is fixed, so skip this one
			if (i == 0 && j == 0) {
				continue;
			}

			int bestMatchIndex = findBestMatchIndex(i, j, puzzleCols, usedSections, unusedSections);
			usedSections.push_back(unusedSections[bestMatchIndex]);
			unusedSections.erase(unusedSections.begin() + bestMatchIndex);
		}
	}

	// print solution
	displayPuzzleSolution(puzzleRows, puzzleCols, usedSections);
	waitKey(0); // asteapta apasarea unei taste
}

void testPuzzle(){
	char fname[MAX_PATH];
	openFileDlg(fname);

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	// computing the input
	std::vector<Mat_<uchar>> sections = sectionImage(src, PUZZLE_ROWS, PUZZLE_COLS); // get the pieces of puzzle
 	sections = shuffleSections(sections); // shuffle them
	displayPuzzleInput(sections); // and display them

	solvePuzzle(PUZZLE_ROWS, PUZZLE_COLS, sections);
}

void testPuzzleMatching() {
	char fname[MAX_PATH];
	openFileDlg(fname);

	Mat_<uchar> src; // matricea sursa
	src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> sections = sectionImage(src, PUZZLE_ROWS, PUZZLE_COLS);

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

	showImage("Input0", firstSection);       // afiseaza sectiunea 1
	showImage("Input2", secondSection);       // afiseaza sectiunea 2
	waitKey(0);
}

void multipleCases() {
	const int SOURCE_COUNT = 3; // 3 test images
	char* sources[SOURCE_COUNT] = { "Images/cameraman.bmp", "Images/kids.bmp", "Images/saturn.bmp" };
	
	// test on multiple puzzle dimensions
	const int PUZZLE_DIMENSIONS_COUNT = 3; // 3 cases: 1) ROWS = COLS [patrat]; 2) ROWS > COLS [portrait]; 3) ROWS < COLS [landscape]
	const int puzzleRowsValues[PUZZLE_DIMENSIONS_COUNT] = { 3, 4, 2 };
	const int puzzleColsValues[PUZZLE_DIMENSIONS_COUNT] = { 3, 2, 4 };

	// test on multiple depths
	const int DEPTH_COUNT = 3;
	const int depths[DEPTH_COUNT] = { 5, 10, 15 }; 
	
	for (int sourceIndex = 0; sourceIndex < SOURCE_COUNT; sourceIndex++) {
		// for each image source
		char* fname = sources[sourceIndex];
		Mat_<uchar> src = imread(fname, IMREAD_GRAYSCALE);

		for (int puzzleDimensionIndex = 0; puzzleDimensionIndex < PUZZLE_DIMENSIONS_COUNT; puzzleDimensionIndex++) {
			// for each puzzle dimension
			int puzzleRows = puzzleRowsValues[puzzleDimensionIndex];
			int puzzleCols = puzzleColsValues[puzzleDimensionIndex];

			// computing the input
			std::vector<Mat_<uchar>> sections = sectionImage(src, puzzleRows, puzzleCols); // get the pieces of puzzle
			sections = shuffleSections(sections); // shuffle them
			displayPuzzleInput(sections); // and display them

			for (int depthIndex = 0; depthIndex < DEPTH_COUNT; depthIndex++) {
				// for each depth
				DEPTH = depths[depthIndex];

				// show info
				printf("Image: %s, Puzzle: %dx%d, Depth: %d\n", fname, puzzleRows, puzzleCols, DEPTH);
				
				// and solve the puzzle for each specific configuration
				solvePuzzle(puzzleRows, puzzleCols, sections);
			}
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
		case 20:
			testPuzzle();
			break;
		case 21:
			testPuzzleMatching();
			break;
		case 22:
			multipleCases();
			break;
		}
	} while (op != 0);
	return 0;
}
