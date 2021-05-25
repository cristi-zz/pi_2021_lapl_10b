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

std::vector<Mat> reconstructFromLaplace(std::vector<Mat> gaussian, std::vector<Mat> laplacian) {
	Mat g, aux;
	std::vector<Mat> ls;
	int i = 1;
	while( i <= laplacian.size()) {
		pyrUp(gaussian[i], g, Size(gaussian[i].rows * 2, gaussian[i].cols * 2));
		add(g, laplacian[laplacian.size() - i], aux);
		ls.push_back(aux);
		i++;
	}
	//Mat ls = mat[0], aux;
	/*for (int i = 1; i < mat.size(); i++) {
		pyrUp(ls, aux, Size(ls.rows * 2, ls.cols * 2));
		add(aux, mat[i], ls);
	}*/

	return ls;
}

void diferenta(Mat reconstructed, Mat original) {
	Mat diferenta = original.clone();
	for (int i = 0; i < reconstructed.rows; i++)
		for (int j = 0; j < reconstructed.cols; j++)
			diferenta.at<uchar>(i,j) = abs(original.at<uchar>(i, j) - reconstructed.at<uchar>(i, j));
	float medie = 0.0;
	for (int i = 0; i < reconstructed.rows; i++)
		for (int j = 0; j < reconstructed.cols; j++)
			medie += diferenta.at<uchar>(i, j);
	
	medie = medie / (float)(reconstructed.rows * reconstructed.cols);
	printf("Media: %f\n", medie);

	waitKey();
}

void alipire() {
	char fname[MAX_PATH];
	openFileDlg(fname);
	Mat img1 = imread(fname, IMREAD_GRAYSCALE);
	openFileDlg(fname);
	Mat img2 = imread(fname, IMREAD_GRAYSCALE);

	int n;
	printf("Levels: ");
	scanf("%d", &n);

	imshow("Original image1", img1);
	imshow("Original image2", img2);

	std::vector<Mat> gaussian1 = gaussianPyramid(img1, n);
	std::vector<Mat> gaussian2 = gaussianPyramid(img2, n);


	std::vector<Mat> laplacian1 = laplacianPyramid(gaussian1);
	std::vector<Mat> laplacian2 = laplacianPyramid(gaussian2);

	std::vector<Mat> combinatGaussian = gaussianPyramid(img1, n);
	std::vector<Mat> combinatLaplacian = laplacianPyramid(gaussian1);


	for (int i = 0; i < gaussian1.size(); i++)
		for (int j = 0; j < gaussian1[i].rows; j++)
			for (int k = gaussian1[i].cols / 2; k < gaussian1[i].cols; k++)
				combinatGaussian[i].at<uchar>(j, k) = gaussian2[i].at<uchar>(j, k);

	for (int i = 0; i < laplacian1.size(); i++)
		for (int j = 0; j < laplacian1[i].rows; j++)
			for (int k = laplacian1[i].cols / 2; k < laplacian1[i].cols; k++)
				combinatLaplacian[i].at<uchar>(j, k) = laplacian2[i].at<uchar>(j, k);


	std::vector<Mat> img = reconstructFromLaplace(combinatGaussian, combinatLaplacian);
	imshow("Reconstructed image", img[0]);
	waitKey();



	/*Mat dst = img1.clone();



	for (int i = 0; i < img2.rows; i++) {
		for (int j = img2.cols / 2; j < img2.cols; j++) {
			dst.at<uchar>(i, j) = img2.at<uchar>(i, j);
		}
	}

	//imshow("Imagini alipite", dst);
	return dst;
	waitKey();*/
}

Mat citire() {
	Mat src;
	char fname[MAX_PATH];
	openFileDlg(fname);
	src = imread(fname, IMREAD_GRAYSCALE);
	return src;
}

Mat constructLevels(Mat src) {
	int n;
	printf("Levels: ");
	scanf("%d", &n);

	imshow("Original image", src);

	std::vector<Mat> gaussian = gaussianPyramid(src, n);

	for (int i = 0; i < gaussian.size(); i++) {

		imshow("Gaussian" + std::to_string(i), gaussian[i]);
	}

	std::vector<Mat> laplacian = laplacianPyramid(gaussian);

	for (int i = 0; i < laplacian.size(); i++) {
		imshow("Laplace" + std::to_string(i), laplacian[i]);
	}

	std::vector<Mat> img = reconstructFromLaplace(gaussian, laplacian);
	int x = laplacian.size();
	imshow("Reconstructed image", img[0]);

	return img[0];
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
		printf(" 6 - Diferenta\n");
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
		case 4: {
			Mat src1 = citire();
			constructLevels(src1);
			break; }
		case 5: 
			alipire();
			break; 
		case 6: {
			Mat src1 = citire();
			diferenta(src1, constructLevels(src1));
			break; }
		}

	} while (op != 0);
	return 0;
}