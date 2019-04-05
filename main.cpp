#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "config.h"
#include "geometry.h"
#include "motor.h"
#include "reducer.h"
#include "plotter.h"
#include <stdint.h>
#include <cmath> 

using namespace std;    

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
	double dt = 4e-6;
	double tmax = 0.1;

/*	
	Plotter plot("output.tga", tmax,Vec3d(0.01,1000,5000));
	Plotter plot2("curr.tga", tmax,Vec3d(100,100,100));

	MotorReducer motor(dt);
	Reducer reducer(dt);	
	exciter vex(10, 50);
	double speed = 0.0;

    for( double t=0.0 ; t<tmax ; t+= dt ){
		vex(t);
		speed = reducer(motor(vex.v, speed));
		plot(reducer.getstate(), t);
		plot2(motor.getcurr(), t);
	}
	*/	

	 
	Reducer reducer(dt);
	double Mdv = 1;	
	Plotter plot("output.tga", tmax,Vec3d(0.1,5000,10000));
	for( double t=0.0 ; t<tmax ; t+= dt ){
		reducer(Mdv);
		plot(reducer.getstate(), t);
	}


/*
	Motor motor(dt);
	exciter vex(27, 50);
	double Mn = 0.5;
	
	Plotter plot("output.tga", tmax,Vec3d(100,1000,5000));
	Plotter plot2("curr.tga", tmax,Vec3d(100,100,100));
	Plotter plot3("enc.tga", tmax,Vec3d(10000,10000,10000));	

    for( double t=0.0 ; t<tmax ; t+= dt ){
		vex(t);
		motor(vex.v, Mn);
		
		plot(motor.getstate(), t);
		
		plot3(Vec3d(motor.encoder(),0,0), t);
	}
	*/

	return 0;
}
