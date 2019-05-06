#ifndef __OSCILLOSCOPE_H__
#define __OSCILLOSCOPE_H__

#include <plotter.h>

/**
 * @brief
 * Визуализация сигнала в реальном времени
 */
  
class Oscilloscope
{
	XPlotter *plotter;
	double *wndbuf;
	int wndsz;
	int wi;
	double tmax;
	double dt;
	
public:	
	Oscilloscope(double _tmax, double _dt, double xmin, double xmax)
	{	
		tmax = _tmax;
		dt = _dt;

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
		
		wndsz = tmax/dt;
		wndbuf = new double[wndsz]();
		wi = 0;
	}
	
	~Oscilloscope()
	{
		plotter->closepl();
		delete plotter;
		delete [] wndbuf;
	}
	
	void update_buffer(double x)
	{
		wndbuf[wi] = x;
		wi = (wi+1)%wndsz;					
	}
	
	void update_screen()
	{
		plotter->erase();
		
		int j = wi;
		double x = 0.0;
		double dx = dt;
		for(int i = 0; i < wndsz-1; i++){		
			plotter->fmove(x, wndbuf[j]);
			plotter->fline(x, wndbuf[j], x+dx, wndbuf[(j+1)%wndsz]);
			
			j = (j+1)%wndsz;
			x += dx;
		}
		
		plotter->flushpl();			
	}

};

#endif
