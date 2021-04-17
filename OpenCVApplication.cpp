// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"


void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				Vec3b v3 = src(i,j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i,j) = (r+g+b)/3;
			}
		}
		
		imshow("original image",src);
		imshow("gray image",dst);
		waitKey();
	}
}

void gaussianPyramid(Mat src, Mat *dst1, Mat* dst2, Mat* dst3) {
	pyrDown(src, *dst1, Size(src.rows/2, src.cols/2));
	pyrDown(*dst1, *dst2, Size(src.rows/4, src.cols/4));
	pyrDown(*dst2, *dst3, Size(src.rows/8, src.cols/8));
}

void laplacianPyramid(Mat src1, Mat src2, Mat src3, Mat* dst1, Mat* dst2) {
	Mat l1, l2, l3;
	pyrUp(src1, l1, Size(src1.rows*2, src1.cols*2));
	pyrUp(src2, l2, Size(src2.rows*2, src2.cols*2));
	pyrUp(src3, l3, Size(src3.rows*2, src3.cols*2));

	subtract(src1, l2, *dst1);
	subtract(src2, l3, *dst2);
}

void printPyramidLevels() {
	Mat src, dst1, dst2, dst3;
	char fname[MAX_PATH];
	openFileDlg(fname);
	src = imread(fname, IMREAD_GRAYSCALE);
	gaussianPyramid(src, &dst1, &dst2, &dst3);

	imshow("Original image", src);
	imshow("Gaussian 1", dst1);
	imshow("Gaussian 2", dst2);
	imshow("Gaussian 3", dst3);
	
	Mat dst4, dst5, dst6;

	laplacianPyramid(dst1, dst2, dst3, &dst4, &dst5);

	imshow("Laplace 1", dst4);
	imshow("Laplace 2", dst5);

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
		printf(" 1 - Deschidere imagine...!\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Color to Gray\n");
		printf(" 4 - Show Gaussian & Laplacian levels\n");
		printf(" 0 - Exit\n\n");

		printf("Option: ");
		scanf("%d",&op);
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
				printPyramidLevels();
		}
	}
	while (op!=0);
	return 0;
}
