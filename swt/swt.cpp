#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <numeric>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "LineSegment.hpp"
#include "Rotation.hpp"
#include "Line.hpp"

#include "mex.h"

using namespace cv;

enum CharColor{
    BRIGHT, DARK, BOTH
};
double angle_thres = M_PI/6.0;
double lowThres = 172.0, highThres = 344.0;
int ksize = 3;
bool L2 = true;
bool showFlag = false; //true;
bool multiRay = true;
bool crossFlag = true;
bool kernelFlag = true;
bool exclude_edge = true;
double max_width = 70.0;
CharColor charcolor = DARK;

std::vector<LineSegment> makePairs(Mat edges, Mat dx, Mat dy, Mat col) ;
void showPoints(Point p0, Point p1, double dx, double dy) ;
void fillMap(Mat map, std::vector<LineSegment> lines) ;
std::vector<LineSegment> makePairs(Mat edges, Mat dx, Mat dy, Mat col) ;
void addRays(Point p0, double dx, double dy, std::vector<Point> dirs) ;
Point* findPair(Mat edges, Point p0, Point p1, Mat gx, Mat gy) ;
Point calcEndPoint(Mat edges, Point p0, double dx, double dy) ;
void medianLimit(Mat map, std::vector<LineSegment> lines) ;


Mat execswt(Mat gray, Mat map){
	//Mat gray = Mat::zeros(image.rows, image.cols, CV_8U);
	//cvtColor(image, gray, COLOR_BGR2GRAY);

	Mat edges = Mat::ones(gray.rows, gray.cols, CV_8U);
	Canny(gray, edges, lowThres, highThres, ksize, L2);

	int nrows = edges.rows, ncols = edges.cols;
	Mat dx = Mat::zeros(nrows, ncols, CV_16S);
	Mat dy = Mat::zeros(nrows, ncols, CV_16S);

	Sobel(gray, dx, CV_16S, 1, 0); // xorder means vertical
	Sobel(gray, dy, CV_16S, 0, 1); // yorder means horizontal

	// for showing lines.
	Mat edge_col = Mat::zeros(edges.rows, edges.cols, CV_8U);
	cvtColor(edges, edge_col, COLOR_GRAY2BGR);

	std::vector<LineSegment> lines = makePairs(edges, dx, dy, edge_col);

	fillMap(map, lines);
	
    std::sort(lines.begin(), lines.end());
	
    for (int i = 0; i < (int)lines.size(); i++){
        LineSegment line = lines.at(i);
        //printf("line%d : %d %d,%d->%d,%d\n",i, line.w,line.s.x,line.s.y,line.e.x,line.e.y);
    }
    medianLimit(map, lines);
    ///std::cout << "map= " << std::endl << " " << map << std::endl << std::endl;

	return map;
}


std::vector<LineSegment> makePairs(Mat edges, Mat dx, Mat dy, Mat col) {
	int nrows = edges.rows, ncols = edges.cols;
	const double min_val = 0.00001;

	int ncount = 0;
	std::vector<LineSegment> lines = std::vector<LineSegment>();
    ///imshow("image",col);

	for (int r = 0; r < nrows; r++) {
		for (int c = 0; c < ncols; c++) {
			double edge_val = (double)edges.at<uchar>(r, c);

			if (edge_val > 0.0) {
				double dx_val = (double)dx.at<short>(r, c);
				double dy_val = (double)dy.at<short>(r, c);

				if (charcolor == DARK) {
					dx_val = dx_val * -1.0;
					dy_val = dy_val * -1.0;
				}

				//double mag0 = 0.0;
				double sqr_sum = dx_val * dx_val + dy_val
						* dy_val;
				if (sqr_sum > min_val) {
					//mag0 = sqrt(sqr_sum);
					Point p0 = Point((double) c, (double) r);
					std::vector<Point> dirs = std::vector<Point>();
					Point dir0 = Point(dx_val, dy_val);
					dirs.push_back(dir0);

					if (multiRay) {
						// Three directions
						addRays(p0, dx_val, dy_val, dirs);
					}

                    std::vector<Point>::iterator pdir;
					for (pdir=dirs.begin();pdir!=dirs.end();++pdir) {
                        Point dir = *pdir;
						Point p1 = calcEndPoint(edges, p0, dir.x, dir.y);
						Point *ppe = findPair(edges, p0, p1, dx, dy);
						if (ppe != NULL) {
                            Point pe = *ppe;
							LineSegment ls = LineSegment(p0, pe);
							lines.push_back(ls);
							//if (col != NULL)
							//	Line::draw(col, p0, pe, Scalar(0, 255, 0));
							if (showFlag) {
								showPoints(p0, p1, dx_val, dy_val);
								printf("Pe(%d, %d)",(int)pe.x,(int)pe.y);
							}
						}
					}
					ncount++;
				}
			}
		}
	}

	return lines;
}

	/**
	 * 
	 * @param p0
	 * @param dx
	 * @param dy
	 */
	void addRays(Point p0, double dx, double dy, std::vector<Point> dirs) {
		double angle = angle_thres;

		Rotation rot = Rotation();
		rot.exec(p0, dx, dy, angle);
		Point p = Point(rot.getDx(), rot.getDy());

		dirs.push_back(p);

		rot.exec(p0, dx, dy, -angle);
		p = Point(rot.getDx(), rot.getDy());
		dirs.push_back(p);
        ///printf("%d  %d\n",p.x,p.y);
	}

/*
 * Find a paring edge pixel and se
 */
Point* findPair(Mat edges, Point p0, Point p1, Mat gx, Mat gy) {

	int dx = (int)abs(p1.x - p0.x);
	int dy = (int)abs(p1.y - p0.y);
	int sx, sy;
	int err = dx - dy;

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

	int x0 = (int) p0.x;
	int y0 = (int) p0.y;
	int x = x0;
	int y = y0;

	double g_x = (double)gx.at<short>(y, x);
	double g_y = (double)gy.at<short>(y, x);
	double mag = sqrt(g_x * g_x + g_y * g_y);
	g_x /= mag;
	g_y /= mag;

	int dx5[] = { -1, 0, 1, 0, 0 };
	int dy5[] = { 0, 0, 0, -1, 1 };

	int kx = x0;
	int ky = y0;
	bool flag = false;
	while (count <= max_step) {
		int idx = abs(x - x0);
		int idy = abs(y - y0);
		int dx2 = (x - x0) * (x - x0);
		int dy2 = (y - y0) * (y - y0);
		double length = sqrt((double) (dx2 + dy2));
		if (length > max_width) {
			break;
		}

		if (x < 1 || y < 1 || x >= edges.cols || y >= edges.rows - 1)
			break;

		if (idx >= 2 || idy >= 2) {
			for (int k = 0; k < 5; k++) {
				kx = x + dx5[k];
				ky = y + dy5[k];
				double edge_val = edges.at<uchar>(ky, kx);
				if (edge_val > 0.0) {
					flag = true;
					break;
				}
			}
			if (flag) {
				break;
			}
		}

		if (x == p1.x && y == p1.y) {
			break;
		}
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x += sx;
		}

		if (x < 1 || y < 1 || x >= edges.cols - 1
				|| y >= edges.rows - 1)
			break;

		if (x == p1.x && y == p1.y) {
			if (idx >= 2 || idy >= 2) {
				for (int k = 0; k < 5; k++) {
					kx = x + dx5[k];
					ky = y + dy5[k];
					double edge_val = edges.at<uchar>(ky, kx);
					if (edge_val > 0.0) {
						flag = true;
						break;
					}
				}
			}
			break;
		}
		if (e2 < dx) {
			err += dx;
			y += sy;
		}
		count++;
	}

	int dx9[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	int dy9[] = { 0, 0, 0, -1, -1, -1, 1, 1, 1 };

	// check the angle in 3x3 pixels if an edge is found.
	Point* pe = NULL;// = NULL;
	if (flag && kx < edges.cols - 1 && kx > 0 && ky < edges.rows - 1
			&& ky > 0) {
		for (int k = 0; k < 9; k++) {
			double g_xt = gx.at<short>(ky + dy9[k], kx + dx9[k]);
			double g_yt = gy.at<short>(ky + dy9[k], kx + dx9[k]);
			double magt = sqrt(g_xt * g_xt + g_yt * g_yt);
			g_xt /= magt;
			g_yt /= magt;
			double angle = acos(g_x * (-g_xt) + g_y * (-g_yt));
			if (angle < angle_thres) {
                Point *p = (Point *)malloc(sizeof(Point));
                *p = Point(kx,ky);
                return p;
				//pe = &Point(kx, ky);
				break;
			}
		}
	}

	return pe;
}

/**
 * Calcurate the endpoint of the ray
 * 
 * @param dx
 * @return
 */
Point calcEndPoint(Mat edges, Point p0, double dx, double dy) {
	// crosspoint at x = xmax (nrows-1)

	double xmax = -1.0, ymax = -1.0;

	if (crossFlag) {
		xmax = edges.cols - 2.0;
		ymax = edges.rows - 2.0;
	} else {
		xmax = edges.cols - 1.0;
		ymax = edges.rows - 1.0;
	}
	const double min_grad = 0.000001;

	Point p1 = Point(); // crossing point at x = xmax or x = 0
	if (abs(dx) < min_grad) {
		p1.x = p0.x;
		if (dy > 0.0) {
			p1.y = ymax;
		} else {
			p1.y = 0.0;
		}
	} else {
		double m = dy / dx;
		double b = p0.y - m * p0.x;

		// try x = 0 or xmax = 0
		if (dx > 0.0) {
			p1.x = xmax;
		} else {
			p1.x = 0.0;
		}
		p1.y = m * p1.x + b;

		// try y = ymax or y = 0 next
		if (p1.y > ymax || p1.y < 0.0) {
			if (dy > 0) {
				p1.y = ymax;
			} else {
				p1.y = 0.0;
			}
			p1.x = (p1.y - b) / m;
		}
	}

	return p1;
}



/**
 * Fill the Stroke Width Map with detected stroke widths.
 * 
 * @param map
 * @param lines
 */
void fillMap(Mat map, std::vector<LineSegment> lines) {
    vector<LineSegment>::iterator pline;
    for (pline=lines.begin();pline!=lines.end();++pline) {
        LineSegment line = *pline;
		Point p0 = line.s;
		Point p1 = line.e;
		double ddx = p1.x - p0.x;
		double ddy = p1.y - p0.y;
		double width = sqrt(ddx * ddx + ddy * ddy);
		int dx = (int) abs(ddx);
		int dy = (int) abs(ddy);
		int sx, sy;
		int err = dx - dy;
        //printf("Line: %f,%f width%f\n",ddx,ddy,width);

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
			double old_width = map.at<float>(y, x);

			// replace the stroke width if the new value is smaller.
			if (old_width == -1.0 || width < old_width) {
				if (exclude_edge) {
					if (x == (int) p0.x && y == (int) p0.y) {
						// do nothing
					} else if (x == (int) p1.x && y == (int) p1.y) {
						// do nothing
					} else {
						map.at<float>(y, x) = width;
					}
				}else{
                    map.at<float>(y, x) = width;
				}
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
				if (exclude_edge == false) {
					old_width = map.at<float>(y, x);
					if (old_width == -1.0 || width < old_width) {
                        map.at<float>(y, x) = width;
					}
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

}

/**
 * Limit the stroke widths along a ray with the median value for corners.
 * 
 * @param mat
 * @param lines
 */
void medianLimit(Mat map, std::vector<LineSegment> lines) {
    vector<LineSegment>::iterator pline;
    for (pline=lines.begin();pline!=lines.end();++pline) {
        LineSegment line = *pline;
        Point p0 = line.s;
        Point p1 = line.e;
        double ddx = p1.x - p0.x;
        double ddy = p1.y - p0.y;
        // double width = sqrt(ddx * ddx + ddy * ddy);
        int dx = (int) abs(ddx);
        int dy = (int) abs(ddy);
        int sx, sy;
        int err = dx - dy;

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

        std::vector<double> widthList = std::vector<double>();
        while (count <= max_step) {
            double st_wd = map.at<float>(y, x, 0);
            if (st_wd > 0.0) {
                widthList.push_back(st_wd);
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
                st_wd = map.at<float>(y, x, 0);
                if (st_wd > 0.0) {
                    widthList.push_back(st_wd);
                }
                break;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
            count++;
        }

        if(widthList.size() == 0) continue;
        std::sort(widthList.begin(),widthList.end());
        //Collections.sort(widthList);
        int med_i = widthList.size() / 2;
        double median = widthList.at(med_i);

        x = (int) p0.x;
        y = (int) p0.y;
        count = 0;

        while (count <= max_step) {
            double st_wd = map.at<float>(y, x, 0);
            if (st_wd > median) {
                map.at<float>(y, x) = median;
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
                st_wd = map.at<float>(y, x, 0);
                if (st_wd > median) {
                    map.at<float>(y, x) = median;
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
}

void showPoints(Point p0, Point p1, double dx, double dy) {
	printf("p0.x = %d" , p0.x);
	printf("p0.y = %d" , p0.y);
	printf("dx = %f" , dx);
	printf("dy = %f" , dy);
	printf("p1.x = %d" , p1.x);
	printf("p1.y = %d" , p1.y);
}

double calcMedian(vector<double> scores) {
    double median;
    size_t size = scores.size();

    sort(scores.begin(), scores.end());

    if(size % 2 == 0){
        median = (scores[size / 2 - 1] + scores[size / 2]) / 2;
    }else{
        median = scores[size / 2];
    }
    return median;
}

double calcVariance(vector<double> scores) {
    double var = 0.0;
    double avg = std::accumulate(scores.begin(),scores.end(),0) / (double)scores.size();
    for(vector<double>::iterator cur=scores.begin();cur!=scores.end();cur++){
        var += (avg - *cur)*(avg - *cur);
    }
    return var / (double)scores.size();
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){

    if (nlhs>2)  mexErrMsgTxt("Too many output arguments");
    if (nrhs==0) mexErrMsgTxt("No Image");
    if (nrhs==1) mexErrMsgTxt("Charcolor is not spcified");
    if (mxGetClassID(prhs[0])!=6) mexErrMsgTxt("Matrix is not of type double");

    int color = mxGetScalar(prhs[1]);
    if (color == 1){
        charcolor = DARK;
    }else{
        charcolor = BRIGHT;
    }
    double *pixels = mxGetPr(prhs[0]);

    int img_size[2];
    img_size[0] = mxGetM(prhs[0]);
    img_size[1]  = mxGetN(prhs[0]);

    Mat img = Mat::zeros(img_size[0], img_size[1], CV_8U);
    for (int i = 0; i < img_size[0]; i++){
        for (int j = 0; j < img_size[1]; j++){
            img.at<uchar>(i,j) = (uchar)pixels[j*img_size[0] + i];
        }
    }

    //printf("SWT: img %dx%d\n",img_size[0],img_size[1]);

    Mat SWTImage = Mat::ones(img.size(),CV_32F);
    SWTImage = SWTImage.mul(SWTImage, -1.0f);

    execswt(img,SWTImage);

    int nregion = 0;
    std::vector<double> wlist = std::vector<double>();
    for (int i = 0; i < SWTImage.rows; i++){
        for (int j = 0; j < SWTImage.cols; j++){
            double w = SWTImage.at<float>(i,j);
            if (w != -1){
                wlist.push_back(w);
                nregion++;
            }
        }
    }
    double avg = 0.0;
    double median = 0.0;
    double var = 0.0;
    if (nregion != 0){
        avg = std::accumulate(wlist.begin(),wlist.end(),0) / (double)nregion;
        median = calcMedian(wlist);
        var = calcVariance(wlist);
    }

    //std::cout << "SWTImage= " << std::endl << " " << SWTImage << std::endl << std::endl;

    plhs[0] = mxCreateDoubleScalar(avg);
    plhs[1] = mxCreateDoubleScalar(var);
}
