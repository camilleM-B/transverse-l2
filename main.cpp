//affiche
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include <iostream>
#include <opencv2/core.hpp>

//resize
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<opencv2/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"

#include <math.h>
#include "opencv2/imgproc.hpp"
using namespace cv;
using namespace std;
/*
void affichelena() {
	//Read image
	Mat img = imread("../images/lena.jpg ");
	//namedWindow("lena");
	imshow("lena", img);
	waitKey(5000);

}

void afficheplateau() {
	//Read image
	//Mat img = imread("images/testplateau.jpg ");
	//namedWindow("lena");
	//imshow("testplateau", img);
	//waitKey(5000);
	/*
	Mat image = imread("../images/testplateau.jpg ");
	namedWindow("Display frame", WINDOW_NORMAL);
	resizeWindow("Display frame", 1000, 700);
	imshow("Display frame", image);
	waitKey(0);*/
/*

	img = cv.imread("../images/testplateau.jpg "s)
	gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
	ret, thresh = cv.threshold(gray, 0, 255, cv.THRESH_BINARY_INV + cv.THRESH_OTSU)

}

int main() {
	afficheplateau();
	

	
	return 0;
}*/









int thresh = 50, N = 5;
const char* wndname = "Square Detection Demo";

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}

void listpoint(Point pt1, Point pt2, Point pt0 , vector<double> & listx,  vector<double> & listy, vector<double> & listx2, vector<double> & listy2)
{
	listx.push_back(pt1.x - pt0.x);

	listy.push_back(pt1.y - pt0.y);

	listx2.push_back(pt2.x - pt0.x);
	listy2.push_back(pt2.y - pt0.y);

}



// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
static void findSquares(const Mat& image, vector<vector<Point> >& squares)
{
	squares.clear();

	/*Mat pyr, timg, gray0(image.size(), CV_8U), gray;

	down-scale and upscale the image to filter out the noise
	pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
	pyrUp(pyr, timg, image.size());*/

	vector<double> listx, listy, listx2, listy2;


		// blur will enhance edge detection
	Mat timg(image);
	medianBlur(image, timg, 7);//can only be odd
	Mat gray0(timg.size(), CV_8U), gray;

	vector<vector<Point> > contours;

	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		for (int l = 0; l < N; l++)
		{
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				Canny(gray0, gray, 5, thresh, 5);
				// dilate canny output to remove potential
				// holes between edge segments
				dilate(gray, gray, Mat(), Point(-1, -1));
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				gray = gray0 >= (l + 1) * 255 / N;
			}
			
			// find contours and store them all as a list
			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			vector<Point> approx;

			// test each contour
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.024, true); //ICI POUR LE NOMBRE DE CARREE SA MERE opti 0.024

				//faire une fonction qui compare les coordonnées et "fusionnes" celles qui sont proches



				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(contourArea(Mat(approx))) > 1000 && isContourConvex(Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						// find the maximum cosine of the angle between joint edges
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);

						listpoint(approx[j % 4], approx[j - 2], approx[j - 1], listx,listy,listx2,listy2);
					}

					for (int i = 0;i < listx.size();i++) {
						for (int j = 0;j < listx.size();j++) {
							if (i != j) {
								if ((j < i - 20) || (j > i + 20))  {
									double jj = listx[j];
									listx.erase(listx.begin() + j);
									listy.erase(listy.begin() + j);
								}
								/*else {
									for (int m = 0;m < listx2.size();m++) {
										for (int n = 0;j < listx2.size();j++) {
											if (m != n) {
												if ((n < m - 20) || (n > m + 20)) {
													double jj = listx2[n];
													listx.erase(listx.begin() + n);
													listy.erase(listy.begin() + n);
												}
											}
										}
									}
								}*/ //affiche même nbr
							}
						}
					}

					/*for (int i = 0;i < MAX(listx.size(),listy.size());i++) {
						cout << "x==" << listx[i] << " y=" << listy[i] << endl;
					}*/



					// if cosines of all angles are small
					// (all angles are ~90 degree) then write quandrange
					// vertices to resultant sequence
					if (maxCosine < 0.2)
						squares.push_back(approx);
				}
			}
			
			
		}
	}
	cout << listx.size() << endl << listy.size();
}


// the function draws all the squares in the image
static void drawSquares(Mat& image, const vector<vector<Point> >& squares)
{
	
	for (size_t i = 0; i < squares.size(); i++)
	{
		const Point* p = &squares[i][0];

		int n = (int)squares[i].size();
		//dont detect the border
		if (p->x > 3 && p->y > 3)
			polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
	}
	imshow(wndname, image);

}


int main(int /*argc*/, char** /*argv*/)
{
	static const char* names[] = {"../images/tinytestplateau.jpg" ,0 };
//	help();
	namedWindow(wndname, 1);
	vector<vector<Point> > squares;

	for (int i = 0; names[i] != 0; i++)
	{
		Mat image = imread(names[i], 1);
		if (image.empty())
		{
			cout << "Couldn't load " << names[i] << endl;
			continue;
		}

		findSquares(image, squares);
		drawSquares(image, squares);
		//imwrite( "out", image );
		int c = waitKey();
		if ((char)c == 27)
			break;
	}


	return 0;
}