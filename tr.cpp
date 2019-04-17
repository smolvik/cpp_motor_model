#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "config.h"
#include "geometry.h"
#include "plotter.h"
#include "filter.h"
#include <stdint.h>
#include <cmath> 
#include <complex>
#include "driver.h"

using namespace std;    

int main(int argc, char **argv)
{
	double dt = 40e-6;
	double tmax = 1;
	double klin = 1.57/1000;
	double aex = 0.0;
	
	if(argc < 2) return 0;	
	aex = atof(argv[1]);

	Driver driver(dt);

	double spdmax = 0.0;
	
	Plotter plot5("step.tga", tmax,Vec3d(0.05,0.05,1000));

	for( double t=0.0 ; t<tmax ; t+= dt ){		
		//double v1 = (t<0.2)?10:0;
		double v1 = aex;
		Vec3d vs = driver(v1);
		
		//cout << vs  << endl;
		plot5(Vec3d(v1*klin,vs.x,0), t);
		
		double linspd = vs.y;
		if(abs(linspd) > spdmax) spdmax = abs(linspd);				
	}

	cout << spdmax*1000 << " mm/s" << endl;

	return 0;
}
