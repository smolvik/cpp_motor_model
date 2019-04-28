#include <iostream>
#include <plotter.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include "driver.h"

using namespace std;

class Oscilloscope
{
	XPlotter *plotter;
	
public:	
	Oscilloscope(double tmax, double xmin, double xmax)
	{
		PlotterParams params;	
		params.setplparam("BITMAPSIZE", (char*)"400x300");
		params.setplparam("USE_DOUBLE_BUFFERING", (char*)"yes");		
		
		plotter = new XPlotter {cin, cout, cerr, params};
		plotter->openpl();
		
		plotter->fspace(0.0, xmin, tmax, xmax);
		plotter->flinewidth(0.01);    
		plotter->pencolorname("green");
		plotter->bgcolorname("black");
		plotter->erase();
	}
	
	~Oscilloscope()
	{
		delete plotter;
	}


};

double get_ticks()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1.0 + tv.tv_usec*1e-6;
}

int wndsz = 0;
double *wndbuf;
double *wndbuf2;

void update_screen(int j, XPlotter &plotter, double dt)
{
	plotter.erase();

	int k = j;
	double x = 0.0;
	double dx = dt;
	plotter.pencolorname("green");
	for(int i = 0; i < wndsz-1; i++){		
		plotter.fmove(x, wndbuf[j]);
		plotter.fline(x, wndbuf[j], x+dx, wndbuf[(j+1)%wndsz]);
		
		j = (j+1)%wndsz;
		x += dx;
	}
	
	/*
	j = k;
	x = 0.0;
	plotter.pencolorname("red");
	for(int i = 0; i < wndsz-1; i++){		
		plotter.fmove(x, wndbuf2[j]);
		plotter.fline(x, wndbuf2[j], x+dx, wndbuf2[(j+1)%wndsz]);
		
		j = (j+1)%wndsz;
		x += dx;
	}
	*/	

	plotter.flushpl();	
}

int main ()
{
	double dtsim = 4e-6;
	double tmax = 1;
	double freq = 1;
	int nskip = 100;
	wndsz = tmax/dtsim/nskip;
	
	cout << wndsz << endl;
	
	Driver driver(dtsim);
	
	wndbuf = new double[wndsz]();
	wndbuf2 = new double[wndsz]();
	
	// set a Plotter parameter
	PlotterParams params;	
	
	params.setplparam("BITMAPSIZE", (char*)"400x300");
	params.setplparam("USE_DOUBLE_BUFFERING", (char*)"yes");
	
	XPlotter plotter(cin, cout, cerr, params);
	if (plotter.openpl () < 0)                  // open Plotter
	{
		cerr << "Couldn't open Plotter\n";
		return 1;
	}

	plotter.fspace(0.0, -10.0, tmax, 10.0);
	plotter.flinewidth(0.01);    
	plotter.pencolorname("green");
	plotter.bgcolorname("black");
	plotter.erase();
	
	double tsim = get_ticks();
	int wi = 0;
	int loop = 0;
	double vex = 0.0;
	int fscr = 0;

	while(1)
	{
		while(get_ticks() > tsim){
			vex = 3*sin(2*M_PI*freq*tsim);
			//double vex = sign(cos(2*M_PI*freq*tsim));
			Vec3d vs = driver(vex);

			tsim += dtsim;
			
			if(++loop == nskip){
				wndbuf2[wi] = 1.57*vex; // volts to mm
				wndbuf[wi] = 1000*vs.x;	// get mm		
				wi = (wi+1)%wndsz;			
				loop = 0;
				fscr = 1;
			}
			
		}
		
		if(fscr){
			fscr = 0;
			update_screen(wi, plotter, dtsim*nskip);
		 }

	}	
	
	plotter.closepl();


   return 0;
}     
