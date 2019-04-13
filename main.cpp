#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "config.h"
#include "geometry.h"
#include "motor.h"
#include "reducer.h"
#include "plotter.h"
#include "controller.h"
#include "filter.h"
#include <stdint.h>
#include <cmath> 
#include "delayer.h" 
#include "encoder.h" 

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
	double tmax = 1;

	VectorController controller;
	Vec3d vabc(0, 0, 0);
	double Md = 0;
	int ccnt = 0;
	exciter vex(27, 50);

	MotorReducer motor(dt);	
	Reducer reducer(dt);
	double speed = 0.0;

	Plotter plot("pos_spd_ang.tga", tmax,Vec3d(50,400,10000));
	Plotter plot2("curr.tga", tmax,Vec3d(100,100,100));
	Plotter plot3("volt.tga", tmax,Vec3d(100,100,100));
	Plotter plot4("torgue.tga", tmax,Vec3d(10,5000,5000));
	Plotter plot5("x_dxdt_f.tga", tmax,Vec3d(0.1,1,10000));

	double spdmax = 0.0;
	double linspd = 0.0;

	Encoder encoder;

	Delayer<Vec3d> mcu_delv(5);
	Delayer<Vec3d> mcu_deli(3);	
	Delayer<double> posdelayer(4);

    for( double t=0.0 ; t<tmax ; t+= dt ){

		vex(t);

		speed = reducer(Md = motor(mcu_delv(vabc), speed));
		double del_pos = posdelayer(motor.getpos());
		Vec3d del_iabc = mcu_deli(motor.getcurr());

		/* 40 us sampler */
		if(0==(ccnt++%10)) (vabc = controller(encoder(del_pos), 37000, del_iabc));

		linspd = reducer.getstate().y;
		if(abs(linspd) > spdmax) spdmax = abs(linspd);		

		//cout << Md << endl;
		//Vec3d vo = motor.getstate();
		//Vec3d vo = reducer.getstate();
		//cout << vo << endl;		
		//cout << motor.getpos()*4096/2/M_PI << endl;
		//cout << motor.getspd()*60/2/M_PI << endl;
		//cout << motor << endl;
		//cout << reducer << endl;		

		plot(motor.getstate(), t);
		plot2(motor.getcurr(), t);
		plot3(vabc, t);
		plot4(Vec3d(Md,0,0), t);
		plot5(reducer.getstate(), t);
	}	

	cout << spdmax*1000 << " mm/s" << endl;

	/*
	Plotter plot("output.tga", tmax,Vec3d(100,100,100));
	VectorController cont;
	int ccnt = 0;
	Vec3d iabc(0,0,0);
	Vec3d vabc(0,0,0);
	for( double t=0.0 ; t<tmax ; t+= dt ){

		iabc = vabc*(0.5);
		
		if(ccnt == 9){
			// 40 us
			ccnt = 0;
			vabc = cont(0, 10, iabc);
			
		} else ccnt++;
		
		plot(iabc, t);
	}
	*/
	
	/*
	VectorController controller;
	Motor motor(dt);
	//exciter vex(27, 50);
	Vec3d vex(0,0,0);
	double Mn = 0;
	
	Plotter plot("output.tga", tmax,Vec3d(100,1000,5000));
	Plotter plot2("curr.tga", tmax,Vec3d(100,100,100));
	Plotter plot3("enc.tga", tmax,Vec3d(10000,10000,10000));	

    for( double t=0.0 ; t<tmax ; t+= dt ){
		motor(vex, Mn);
		
		if(concnt == 10){
			concnt = 0;
			vex = controller(motor.encoder(), 0, 0, motor.getcurr());
		}else concnt++;
		
		plot(motor.getstate(), t);		
		plot2(motor.getcurr(), t);
		plot3(Vec3d(motor.encoder(),0,0), t);
	}
*/	
	

/*
	exciter vex(1000, 20);
	RejFilter flt(RF_A12, RF_A13, RF_B11, RF_B12, RF_B13);
	//MeanFilter flt(1000);	
	Plotter plot("output.tga", tmax,Vec3d(4000,4000,4000));	
    for( double t=0.0 ; t<tmax ; t+= dt ){
		vex(t);
		plot(Vec3d(vex.v.x,flt(vex.v.x),0), t);
	}
*/

/*	
	MeanFilter flt(10);	
	while(1){
		cout << flt(23) << endl;
	}
*/	

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

	/* 
	Reducer reducer(dt);
	double Mdv = 1;	
	Plotter plot("output.tga", tmax,Vec3d(0.1,5000,10000));
	for( double t=0.0 ; t<tmax ; t+= dt ){
		reducer(Mdv);
		plot(reducer.getstate(), t);
	}
*/

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
