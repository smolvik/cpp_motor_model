#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "geometry.h"
#include "tgaplotter.h"
#include <stdint.h>
#include "driver.h"

/** @file 
 * Пример использования класса Driver.
 * Симуляция отработки приводом перекладки
 * 
 * */

/**
 * @param напряжение уставки В
 * @return максимальная скорость мм/с
 */
int main(int argc, char **argv)
{
	double dt = 4e-6;
	double tmax = 1;
	double klin = 1.57/1000;
	double aex = 0.0;
	
	if(argc < 2) return 0;	
	aex = atof(argv[1]);

	Driver driver(dt);

	double spdmax = 0.0;
	
	TGAPlotter plot5("step.tga", tmax,Vec3d(0.05,0.05,0.05));

	for( double t=0.0 ; t<tmax ; t+= dt ){		
		double n1 = 0.1*sin(2*M_PI*75*t);
		double v1 = ((t<0.2)?0:aex) + n1;
		//double v1 = aex;
		Vec3d vs = driver(v1);

		//cout << vs  << endl;
		plot5(Vec3d(v1*klin,vs.x,0), t);
		
		double linspd = vs.y;
		if(abs(linspd) > spdmax) spdmax = abs(linspd);				
	}

	std::cout << spdmax*1000 << " mm/s" << std::endl;

	return 0;
}
