package jp.co.rakuten.rit.edge;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import jp.co.rakuten.rit.drawing.Line;
import jp.co.rakuten.rit.drawing.LineSegment;
import jp.co.rakuten.rit.drawing.Rotation;
import jp.co.rakuten.rit.textdetection.ShowImage;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

/**
 * Stroke Width Transform
 * 
 * @author naoki
 * 
 */
public class SWT {
	private double lowThres = 172.0, highThres = 344.0;
	private int ksize = 3;
	private boolean L2 = true;
	private boolean showFlag = false;
	private double angle_thres = Math.PI / 6.0;
	private boolean multiRay = true;
	private boolean crossFlag = true; // use a cross shape (4-neighbor) when
										// finding an edge pixel.
	private boolean kernelFlag = true; // use 3x3 kernel when finding a same
										// orientation pixel.
	private CharColor charcolor;
	private double max_width = 70.0;
	private boolean exclude_edge = true; // do not put width on edge pixels.

	Mat gray = null; // grayscale image of the input image
	Mat edges = null; // Edge image of the gray image
	Mat dx = null; // horizontal first derivative by Sobel
	Mat dy = null; // vertical first derivative by Sobel.

	public enum CharColor {
		bright, dark, both
	}

	public SWT() {
		showFlag = false;
		charcolor = SWT.CharColor.bright;
	}

	public ArrayList<Mat> exec(Mat image, ArrayList<Mat> maps) {
		int nmaps = maps.size();

		if (nmaps == 2 || nmaps == 1) {
			gray = new Mat(image.rows(), image.cols(), CvType.CV_8U);
			Imgproc.cvtColor(image, gray, Imgproc.COLOR_BGR2GRAY);

			edges = new Mat(image.rows(), image.cols(), CvType.CV_8U);
			Imgproc.Canny(gray, edges, lowThres, highThres, ksize, L2);

			int nrows = edges.rows(), ncols = edges.cols();
			dx = new Mat(nrows, ncols, CvType.CV_16S);
			dy = new Mat(nrows, ncols, CvType.CV_16S);

			Imgproc.Sobel(gray, dx, CvType.CV_16S, 1, 0); // xorder means
															// vertical
			Imgproc.Sobel(gray, dy, CvType.CV_16S, 0, 1); // yorder means
															// horizontal
			// edges = closeOutline(edges); // fatten slanted edges

			int i = 0;
			for (Mat map : maps) {
				if (nmaps == 2) {
					charcolor = getBrightnessPattern(i);
				}
				ArrayList<LineSegment> lines = makePairs(edges, dx, dy, null);

				fillMap(map, lines);
				Collections.sort(lines, new Comparator<LineSegment>() {
					public int compare(LineSegment o1, LineSegment o2) {
						return o1.w - o2.w;
					}
				});
				medianLimit(map, lines);
				i++;
			}

			if (nmaps == 2)
				charcolor = CharColor.both;
		}

		return maps;
	}

	/**
	 * Make Stroke Width Map
	 * 
	 * @param image
	 *            : 3 channel color
	 */
	public Mat exec(Mat image, Mat map) {
		gray = new Mat(image.rows(), image.cols(), CvType.CV_8U);
		Imgproc.cvtColor(image, gray, Imgproc.COLOR_BGR2GRAY);

		edges = new Mat(image.rows(), image.cols(), CvType.CV_8U);
		Imgproc.Canny(gray, edges, lowThres, highThres, ksize, L2);

		int nrows = edges.rows(), ncols = edges.cols();
		dx = new Mat(nrows, ncols, CvType.CV_16S);
		dy = new Mat(nrows, ncols, CvType.CV_16S);

		Imgproc.Sobel(gray, dx, CvType.CV_16S, 1, 0); // xorder means vertical
		Imgproc.Sobel(gray, dy, CvType.CV_16S, 0, 1); // yorder means horizontal

		// for showing lines.
		Mat edge_col = new Mat(edges.rows(), edges.cols(), CvType.CV_8U);
		Imgproc.cvtColor(edges, edge_col, Imgproc.COLOR_GRAY2BGR);

		ArrayList<LineSegment> lines = makePairs(edges, dx, dy, edge_col);

		fillMap(map, lines);
		Collections.sort(lines, new Comparator<LineSegment>() {
			public int compare(LineSegment o1, LineSegment o2) {
				return o1.w - o2.w;
			}
		});
		medianLimit(map, lines);

		return map;
	}

	/**
	 * Make Stroke Width Map from an edge image and x/y derivatives.
	 * 
	 * @param edges
	 */
	private ArrayList<LineSegment> makePairs(Mat edges, Mat dx, Mat dy, Mat col) {
		int nrows = edges.rows(), ncols = edges.cols();
		final double min_val = 0.00001;

		int ncount = 0;
		ArrayList<LineSegment> lines = new ArrayList<LineSegment>();

		for (int r = 0; r < nrows; r++) {
			for (int c = 0; c < ncols; c++) {
				double edge_val[] = edges.get(r, c);

				if (edge_val[0] > 0.0) {
					double dx_val[] = dx.get(r, c);
					double dy_val[] = dy.get(r, c);

					if (charcolor == CharColor.dark) {
						dx_val[0] = dx_val[0] * -1.0;
						dy_val[0] = dy_val[0] * -1.0;
					}

					double mag0 = 0.0;
					double sqr_sum = dx_val[0] * dx_val[0] + dy_val[0]
							* dy_val[0];
					if (sqr_sum > min_val) {
						mag0 = Math.sqrt(sqr_sum);
						Point p0 = new Point((double) c, (double) r);
						ArrayList<Point> dirs = new ArrayList<Point>();
						Point dir0 = new Point(dx_val[0], dy_val[0]);
						dirs.add(dir0);

						if (multiRay) {
							addRays(p0, dx_val[0], dy_val[0], dirs);
						}

						for (Point dir : dirs) {
							Point p1 = calcEndPoint(edges, p0, dir.x, dir.y);
							Point pe = findPair(edges, p0, p1, dx, dy);
							if (pe != null) {
								LineSegment ls = new LineSegment(p0, pe);
								lines.add(ls);
								if (col != null)
									Line.draw(col, p0, pe,
											new Scalar(0, 255, 0));
								if (showFlag) {
									showPoints(p0, p1, dx_val[0], dy_val[0]);
									System.out.println("Pe(" + (int) pe.x
											+ ", " + (int) pe.y + ")");
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
	private void addRays(Point p0, double dx, double dy, ArrayList<Point> dirs) {
		double angle = angle_thres;

		Rotation rot = new Rotation();
		rot.exec(p0, dx, dy, angle);
		Point p = new Point(rot.getDx(), rot.getDy());

		dirs.add(p);

		rot.exec(p0, dx, dy, -angle);
		p = new Point(rot.getDx(), rot.getDy());
		dirs.add(p);

	}

	/*
	 * Find a paring edge pixel and se
	 */
	private Point findPair(Mat edges, Point p0, Point p1, Mat gx, Mat gy) {

		int dx = (int) Math.abs(p1.x - p0.x);
		int dy = (int) Math.abs(p1.y - p0.y);
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

		double g_x = gx.get(y, x)[0];
		double g_y = gy.get(y, x)[0];
		double mag = Math.sqrt(g_x * g_x + g_y * g_y);
		g_x /= mag;
		g_y /= mag;

		int dx5[] = { -1, 0, 1, 0, 0 };
		int dy5[] = { 0, 0, 0, -1, 1 };

		int kx = x0;
		int ky = y0;
		boolean flag = false;
		while (count <= max_step) {
			int idx = Math.abs(x - x0);
			int idy = Math.abs(y - y0);
			int dx2 = (x - x0) * (x - x0);
			int dy2 = (y - y0) * (y - y0);
			double length = Math.sqrt((double) (dx2 + dy2));
			if (length > max_width) {
				break;
			}

			if (x < 1 || y < 1 || x >= edges.cols() || y >= edges.rows() - 1)
				break;

			if (idx >= 2 || idy >= 2) {
				for (int k = 0; k < 5; k++) {
					kx = x + dx5[k];
					ky = y + dy5[k];
					double edge_val = edges.get(ky, kx)[0];
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

			if (x < 1 || y < 1 || x >= edges.cols() - 1
					|| y >= edges.rows() - 1)
				break;

			if (x == p1.x && y == p1.y) {
				if (idx >= 2 || idy >= 2) {
					for (int k = 0; k < 5; k++) {
						kx = x + dx5[k];
						ky = y + dy5[k];
						double edge_val = edges.get(ky, kx)[0];
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
		Point pe = null;
		if (flag && kx < edges.cols() - 1 && kx > 0 && ky < edges.rows() - 1
				&& ky > 0) {
			for (int k = 0; k < 9; k++) {
				double g_xt = gx.get(ky + dy9[k], kx + dx9[k])[0];
				double g_yt = gy.get(ky + dy9[k], kx + dx9[k])[0];
				double magt = Math.sqrt(g_xt * g_xt + g_yt * g_yt);
				g_xt /= magt;
				g_yt /= magt;
				double angle = Math.acos(g_x * (-g_xt) + g_y * (-g_yt));
				if (angle < angle_thres) {
					pe = new Point(kx, ky);
					break;
				}
			}
		}

		return pe;
	}

	/**
	 * Fill the Stroke Width Map with detected stroke widths.
	 * 
	 * @param map
	 * @param lines
	 */
	private void fillMap(Mat map, ArrayList<LineSegment> lines) {
		for (LineSegment line : lines) {
			Point p0 = line.s;
			Point p1 = line.e;
			double ddx = p1.x - p0.x;
			double ddy = p1.y - p0.y;
			double width = Math.sqrt(ddx * ddx + ddy * ddy);
			int dx = (int) Math.abs(ddx);
			int dy = (int) Math.abs(ddy);
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

			while (count <= max_step) {
				double old_width = map.get(y, x)[0];

				// replace the stroke width if the new value is smaller.
				if (old_width == -1.0 || width < old_width) {
					if (exclude_edge) {
						if (x == (int) p0.x && y == (int) p0.y) {
							// do nothing
						} else if (x == (int) p1.x && y == (int) p1.y) {
							// do nothing
						} else {
							map.put(y, x, width);
						}
					}else{
						map.put(y, x, width);
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
						old_width = map.get(y, x)[0];
						if (old_width == -1.0 || width < old_width) {
							map.put(y, x, width);
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
	 * Calcurate the endpoint of the ray
	 * 
	 * @param dx
	 * @return
	 */
	private Point calcEndPoint(Mat edges, Point p0, double dx, double dy) {
		// crosspoint at x = xmax (nrows-1)

		double xmax = -1.0, ymax = -1.0;

		if (crossFlag) {
			xmax = edges.cols() - 2.0;
			ymax = edges.rows() - 2.0;
		} else {
			xmax = edges.cols() - 1.0;
			ymax = edges.rows() - 1.0;
		}
		final double min_grad = 0.000001;

		Point p1 = new Point(); // crossing point at x = xmax or x = 0
		if (Math.abs(dx) < min_grad) {
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
	 * set the number of ray to be only one.
	 */
	public void setSingleRay() {
		multiRay = false;
	}

	/**
	 * Set the low threshold for Canny edge detection
	 * 
	 * @param value
	 */
	public void setLowThreshold(double value) {
		if (value > 0.0) {
			lowThres = value;
		}
	}

	/**
	 * Set the hight thresold for Canny edge detection
	 * 
	 * @param value
	 */
	public void setHighThreshold(double value) {
		if (value > 0.0) {
			highThres = value;
		}
	}

	public void setAngle(double angle) {
		if (angle > 0.0) {
			angle_thres = angle;
		}
	}
	
	/**
	 * Set the flag if edge pixels are excluded or not
	 * @param vale
	 */
	public void setEdgeExclusion(boolean value){
		exclude_edge = value;
	}

	/**
	 * Limit the stroke widths along a ray with the median value for corners.
	 * 
	 * @param mat
	 * @param lines
	 */
	public void medianLimit(Mat map, ArrayList<LineSegment> lines) {
		for (LineSegment line : lines) {
			Point p0 = line.s;
			Point p1 = line.e;
			double ddx = p1.x - p0.x;
			double ddy = p1.y - p0.y;
			double width = Math.sqrt(ddx * ddx + ddy * ddy);
			int dx = (int) Math.abs(ddx);
			int dy = (int) Math.abs(ddy);
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

			ArrayList<Double> widthList = new ArrayList<Double>();
			while (count <= max_step) {
				double st_wd = map.get(y, x)[0];
				if (st_wd > 0.0) {
					widthList.add(st_wd);
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
					st_wd = map.get(y, x)[0];
					if (st_wd > 0.0) {
						widthList.add(st_wd);
					}
					break;
				}
				if (e2 < dx) {
					err += dx;
					y += sy;
				}
				count++;
			}

			if(widthList == null || widthList.size() == 0) continue;
			Collections.sort(widthList);
			int med_i = widthList.size() / 2;
			double median = widthList.get(med_i);

			x = (int) p0.x;
			y = (int) p0.y;
			count = 0;

			while (count <= max_step) {
				double st_wd = map.get(y, x)[0];
				if (st_wd > median) {
					map.put(y, x, median);
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
					st_wd = map.get(y, x)[0];
					if (st_wd > median) {
						map.put(y, x, median);
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
	 * Add pixels for slanted edges by 2x2 filtering to be a 2-pixel width line.
	 * 
	 * @param image
	 * @return
	 */
	static private Mat closeOutline(Mat src) {
		int nrows = src.rows(), ncols = src.cols();
		Mat dst = new Mat(nrows, ncols, CvType.CV_8U);
		dst.zeros(nrows, ncols, CvType.CV_8U);
		int r_end = nrows - 1, c_end = ncols - 1;

		for (int r = 0; r < r_end; r++) {
			for (int c = 0; c < c_end; c++) {
				int val_d = (int) dst.get(r, c)[0];
				int val_s = (int) src.get(r, c)[0];
				if (val_d == 0) {
					dst.put(r, c, (double) val_s);
				}
				int val_s11 = (int) src.get(r + 1, c + 1)[0];
				if (val_s > 0 && val_s11 > 0) {
					dst.put(r, c + 1, (double) val_s);
					dst.put(r + 1, c, (double) val_s);
				}
				int val_01 = (int) src.get(r, c + 1)[0];
				int val_10 = (int) src.get(r + 1, c)[0];
				if (val_01 > 0 && val_10 > 0) {
					dst.put(r, c, (double) val_01);
					dst.put(r + 1, c + 1, (double) val_01);
				}
			}
			// last column
			int val_d = (int) dst.get(r, c_end)[0];
			if (val_d == 0) {
				int val_s = (int) src.get(r, c_end)[0];
				dst.put(r, c_end, (double) val_s);
			}
		}
		// last row
		for (int c = 0; c < ncols; c++) {
			int val_d = (int) dst.get(r_end, c)[0];
			if (val_d == 0) {
				int val_s = (int) src.get(r_end, c)[0];
				dst.put(r_end, c, (double) val_s);
			}
		}

		return dst;
	}

	static public CharColor getBrightnessPattern(int i) {
		CharColor color;

		if (i == 0) {
			color = CharColor.bright;
		} else {
			color = CharColor.dark;
		}

		return color;
	}

	public void setCharBright() {
		charcolor = CharColor.bright;
	}

	public void setCharDark() {
		charcolor = CharColor.dark;
	}

	public void setCharBoth() {
		charcolor = CharColor.both;
	}

	public Mat getEdges() {
		return edges;
	}

	public Mat getGrayImage() {
		return gray;
	}

	public Mat getDxImage() {
		return dx;

	}

	public Mat getDyImage() {
		return dy;
	}

	private void showPoints(Point p0, Point p1, double dx, double dy) {
		System.out.println("p0.x = " + Double.toString(p0.x));
		System.out.println("p0.y = " + Double.toString(p0.y));
		System.out.println("dx = " + Double.toString(dx));
		System.out.println("dy = " + Double.toString(dy));
		System.out.println("p1.x = " + Double.toString(p1.x));
		System.out.println("p1.y = " + Double.toString(p1.y));
	}
}