#include <math.h>
#include <opencv2/core/core.hpp>
#include "Line.hpp"

/**
 * Line drawing based on Breseham's algorithm
 * 
 * @author naoki
 * 
 */
Line::Line() {

}

/**
 * Line drawing using the Simplification code in Wiki.
 * 
 * point.x: vertical direction (row) point.y: horizontal direction (column)
 * 
 * @param image
 * @param p0
 * @param y0
 */
void Line::draw(cv::Mat image, cv::Point p0, cv::Point p1, cv::Scalar value) {
	int dx = (int) abs(p1.x - p0.x);
	int dy = (int) abs(p1.y - p0.y);
	int sx, sy;
	int err = dx - dy;

	unsigned int data[3];// = new byte[3];
	
	if(image.channels() == 3){
		for(int i = 0;i < 3;i++){
			data[i] = (unsigned int) value.val[i];
		}
	}
		
	if (p0.x < p1.x) {
		sx = 1;
	} else {
		sx = -1;
	}

	if (p0.y < p1.y) {
		sy = 1;
	} else {
		sy = -1;
	}

	int max_step;
	if (dx > dy) {
		max_step = dx;
	} else {
		max_step = dy;
	}

	int count = 0;

	int x = (int) p0.x;
	int y = (int) p0.y;
	while (count <= max_step) {
        for(int i = 0;i < 3;i++){
            image.at<uchar>(y,x,i) = data[i];
        }
		if (x == p1.x && y == p1.y) {
			break;
		}
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x += sx;
		}
		if (x == p1.x && y == p1.y) {
            for(int i = 0;i < 3;i++){
                image.at<uchar>(y,x,i) = data[i];
            }
			break;
		}
		if (e2 < dx) {
			err += dx;
			y += sy;
		}
		count++;
	}
}

/**
 * draw a line
 * @param image
 * @param p0
 * @param dx
 * @param dy
 * @param value
 */
void Line::draw(cv::Mat image, cv::Point p0, double n_dx, double n_dy, cv::Scalar value) {
	int dx = (int) (n_dx * 100.0);
	int dy = (int) (n_dy * 100.0);
	int sx, sy;
	int err = dx - dy;

	cv::Point p1 = cv::Point(p0.x + dx, p0.y + dy);
	unsigned int data[3]; // = new byte[3];
	
	if(image.channels() == 3){
		for(int i = 0;i < 3;i++){
			data[i] = (unsigned int) value.val[i];
		}
	}
		
	if (p0.x < p1.x) {
		sx = 1;
	} else {
		sx = -1;
	}

	if (p0.y < p1.y) {
		sy = 1;
	} else {
		sy = -1;
	}

	int max_step;
	if (dx > dy) {
		max_step = dx;
	} else {
		max_step = dy;
	}

	int count = 0;

	int x = (int) p0.x;
	int y = (int) p0.y;
	while (count <= max_step) {
		for(int i = 0;i < 3;i++){
            image.at<uchar>(y,x,i) = data[i];
		}
		if (x == p1.x && y == p1.y) {
			break;
		}
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x += sx;
		}
		if (x == p1.x && y == p1.y) {
            for(int i = 0;i < 3;i++){
                image.at<uchar>(y,x,i) = data[i];
            }
			break;
		}
		if (e2 < dx) {
			err += dx;
			y += sy;
		}
		count++;
	}

}
