#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "config.h"
#include "geometry.h"
#include "motor.h"
#include "reducer.h"

using namespace std;    

const int width  = 1024;
const int height = 1024;

TGAImage image(width, height, TGAImage::RGB);		

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor blue   = TGAColor(0, 0,   255,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAColor color) 
{
	if(abs(y1-y0)<=abs(x1-x0)){
	
		int y = y0;
		int err = 0;	
		int de1 = abs(2*(y1-y0));
		int de2 = abs(2*(x1-x0));
		int dx = (x1>x0)?1:-1;
		int dy = (y1>y0)?1:-1;

		for(int x=x0; x!=x1; x+=dx){
			err += de1;
			if(abs(err) >= abs(x1-x0)) {
				y += dy;
				err -= de2;
			}
			image.set(x, y, color);			
		}
	}
	else{

		int x = x0;
		int err = 0;	
		int de1 = abs(2*(x1-x0));
		int de2 = abs(2*(y1-y0));
		int dy = (y1>y0)?1:-1;
		int dx = (x1>x0)?1:-1;

		for(int y=y0; y!=y1; y+=dy){
			err += de1;
			if(abs(err) >= abs(y1-y0)) {
				x += dx;
				err -= de2;
			}
			image.set(x, y, color);			
		}		

	}		
}

class Plotter{
	Vec3d x0;
	double t0;
	
	public:
	Plotter(const Vec3d &x, const double &t) : x0(x), t0(t) {}
	void operator() (const Vec3d &x, const double t){
		//cout << t << '\t' << x.x << '\t' << x.y <<'\t' << x.z << endl;

		double sc[3] = {50000,1,0.2};
		double st = 1000;
		
		line(t0*st, width/2 + x0.x*sc[0], t*st, width/2 + x.x*sc[0], red);
		line(t0*st, width/2 + x0.y*sc[1], t*st, width/2 + x.y*sc[1], green);
		line(t0*st, width/2 + x0.z*sc[2], t*st, width/2 + x.z*sc[2], blue);
		
		x0 = x;
		t0 = t;
	}
};

class exciter{
	double amp;
	double freq;

	public:
	
	Vec3d v;
	
	exciter(double a, double f): amp(a), freq(f) {
		v.x = 0;
		v.y = 0;
		v.z = 0;
		}
	Vec3d operator() (double t){
		v.x = amp*cos(2*M_PI*freq*t);
		v.y = amp*cos(2*M_PI*freq*t-2*M_PI/3);
		v.z = amp*cos(2*M_PI*freq*t+2*M_PI/3);
		
		return v;
	}
};

int main(int argc, char **argv)
{
	double dt = 1e-5;
	Vec3d st(0.0, 0.0, 0.0);
	Plotter plot(st, 0.0);
	
	MotorReducer motor(dt);
	Reducer reducer(dt);	
	exciter vex(10, 50);
	double speed = 0.0;

    for( double t=0.0 ; t<10.0 ; t+= dt ){
		vex(t);
		speed = reducer(motor(vex.v, speed));
		
		//plot(motor.getstate(), t);
		plot(reducer.getstate(), t);
	}	

	/* 
	Reducer reducer(dt);
	double Mdv = 10;	
	for( double t=0.0 ; t<0.1 ; t+= dt ){
		reducer.update(Mdv);
		plot(reducer.getstate(), t);
	}
	*/
	
	/*
	Motor motor(dt);
	exciter vex(27, 30);
	double Mn = 0.6;

    for( double t=0.0 ; t<1.0 ; t+= dt ){
		vex(t);
		motor(vex.v, Mn);
		
		plot(motor.getstate(), t);
	}
	*/

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");		

	return 0;
}
