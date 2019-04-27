#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "config.h"
#include "geometry.h"
#include "tgaplotter.h"
#include "filter.h"
#include <stdint.h>
#include <cmath> 
#include <complex>
#include "driver.h"

/** @file 
 * Пример использования класса Driver.
 * Симуляция отработки гармонического воздействия
 * и частотный анализ первой гармоники
 * */

using namespace std;    

/**
 * @param амплитуда В
 * @param частота Гц
 * @return частота Гц, АЧХ дБ, ФЧХ градус
 */
int main(int argc, char **argv)
{
	double dt = 4e-6;
	double tmax = 1;
	int fex = 10;
	double aex = 0.1;
	double klin = 1.57/1000;

	if(argc < 3) return 0;

	aex = atof(argv[1]);
	fex = atof(argv[2]);

	Driver driver(dt);

	double spdmax = 0.0;
	
	TGAPlotter plot5("sin.tga", tmax,Vec3d(0.01,0.01,10000));

	complex<double> arg = 0;
	complex<double> c1(0.0, 0.0);
	complex<double> c2(0.0, 0.0);
	
	for( double t=0.0 ; t<tmax ; t+= dt ){		
		double v1 = aex*sin(2*M_PI*fex*t);
		Vec3d vs = driver(v1);
		
		c1 += (v1*klin)*exp(arg);
		c2 += vs.x*exp(arg);		

		complex<double> darg = -(2*M_PI*dt/tmax)*1i*fex;
		arg = arg + darg;		
		
		//cout << vs  << endl;
		plot5(Vec3d(v1*klin,vs.x,0), t);
		
		double linspd = vs.y;
		if(abs(linspd) > spdmax) spdmax = abs(linspd);				
	}

	cout << fex << " " << 20*log10(std::abs(c2/c1)) << " " << (180/M_PI)*std::arg(c2/c1) << endl;
	//cout << spdmax*1000 << " mm/s" << endl;

	return 0;
}
