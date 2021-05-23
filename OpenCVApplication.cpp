// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"


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

std::vector<Mat> gaussianPyramid(Mat src, int n) {
	std::vector<Mat> mat;
	Mat src2 = src.clone();
	mat.push_back(src);
	Mat dst;
	int x = 2;
	for (int i = 0; i < n; i++) {
		pyrDown(src2, dst, Size(src.rows / x, src.cols / x));
		mat.push_back(dst);
		src2 = dst.clone();
		x *= 2;
	}

	return mat;
}

std::vector<Mat> laplacianPyramid(std::vector<Mat> mat) {
	std::vector<Mat> mat_dst;
	Mat l, dst;
	for (int i = (mat.size() - 1); i > 0; i--) {
		pyrUp(mat[i], l, Size(mat[i].rows * 2, mat[i].cols * 2));
		subtract(mat[i - 1], l, dst);
		mat_dst.push_back(dst);
	}

	return mat_dst;
}

Mat reconstructFromLaplace(std::vector<Mat> gaussian, std::vector<Mat> laplacian) {
	Mat g, ls;
	pyrUp(gaussian[1], g, Size(gaussian[1].rows * 2, gaussian[1].cols * 2));
	add(g, laplacian[laplacian.size() - 1], ls);
	//Mat ls = mat[0], aux;
	/*for (int i = 1; i < mat.size(); i++) {
		pyrUp(ls, aux, Size(ls.rows * 2, ls.cols * 2));
		add(aux, mat[i], ls);
	}*/

	return ls;
}

void alipire() {
	char fname[MAX_PATH];
	openFileDlg(fname);
	Mat img1 = imread(fname, IMREAD_GRAYSCALE);
	openFileDlg(fname);
	Mat img2 = imread(fname, IMREAD_GRAYSCALE);

	Mat dst = img1.clone();

	for (int i = 0; i < img2.rows; i++) {
		for (int j = img2.cols / 2; j < img2.cols; j++) {
			dst.at<uchar>(i, j) = img2.at<uchar>(i, j);
		}
	}

	imshow("Imagini alipite", dst);

	waitKey();
}

void printLevels() {
	int n;
	printf("Levels: ");
	scanf("%d", &n);
	Mat src;
	char fname[MAX_PATH];
	openFileDlg(fname);
	src = imread(fname, IMREAD_GRAYSCALE);

	imshow("Original image", src);

	std::vector<Mat> gaussian = gaussianPyramid(src, n);

	for (int i = 0; i < gaussian.size(); i++) {
		imshow("Gaussian", gaussian[i]);
	}

	std::vector<Mat> laplacian = laplacianPyramid(gaussian);

	for (int i = 0; i < laplacian.size(); i++) {
		imshow("Laplace", laplacian[i]);
	}

	Mat img = reconstructFromLaplace(gaussian, laplacian);

	imshow("Reconstructed image", img);

	waitKey();
}


int main()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Deschidere imagine....\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Color to Gray\n");
		printf(" 4 - Arata nivelele de Gaussian & Laplacian\n");
		printf(" 5 - Alipirea a doua imagini\n");
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
			printLevels();
			break;
		case 5:
			alipire();
			break;
		}
	} while (op != 0);
	return 0;
}