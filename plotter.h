#ifndef __PLOTTER_H_
#define __PLOTTER_H_

#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include <vector>
#include <string>
#include "geometry.h"

/**
 * @brief
 * Класс построителя 2D диаграмм
 */
class Plotter{
	Vec3d y0;
	double x0;
	
	const int width = 1024;
	const int height = 1024;

	Vec3d yscale;
	double xscale;
	std::string fname;

	TGAImage *image;

	public:
	~Plotter();
	Plotter(const std::string fn, const double xsc, const Vec3d ysc);
	void operator() (const Vec3d &y, const double x);
	void line(int x0, int y0, int x1, int y1, TGAColor color);
};

#endif
