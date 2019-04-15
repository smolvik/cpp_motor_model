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

class Driver{
	
	Delayer<Vec3d> *mcu_delv;
	Delayer<Vec3d> *mcu_deli;	
	Delayer<double> *enc_delayer;
	Delayer<double> *pos_delayer;	
	
	MotorReducer *motor;	
	Reducer *reducer;
	VectorController *controller;
	
	int ccnt;
	double dt;
	double speed;
	Vec3d vabc;
	
public:
	Driver(double _dt)
	{
		ccnt = 0;		
		dt = _dt;
		speed = 0.0;
		vabc = Vec3d(0.0, 0.0, 0.0);
		
		reducer = new Reducer {dt};
		motor = new MotorReducer {dt};
		controller = new VectorController;
		
		mcu_delv = new Delayer<Vec3d> {5};
		mcu_deli = new Delayer<Vec3d> {3};
		enc_delayer = new Delayer<double> {4};
		pos_delayer = new Delayer<double> {3};
	}
	
	~Driver(){
		delete reducer;
		delete motor;
		delete mcu_deli;
		delete mcu_delv;
		delete pos_delayer;
		delete enc_delayer;
		delete controller;
	}
	
	Vec3d operator () (double ux)
	{
		speed = (*reducer)((*motor)((*mcu_delv)(vabc), speed));
		double del_phi = (*enc_delayer)((*motor).getpos());
		Vec3d del_iabc = (*mcu_deli)((*motor).getcurr());
		double del_pos = (*pos_delayer)(ux);

		/* 40 us sampler */
		if(0==(ccnt++%10)) (vabc = (*controller)(del_phi, del_pos, del_iabc));		
		
		return reducer->getstate();
	}

};

int main(int argc, char **argv)
{
	double dt = 4e-6;
	double tmax = 1;
	
	Driver driver(dt);
	
	double spdmax = 0.0;
	
	Plotter plot5("x_dxdt_f.tga", tmax,Vec3d(0.1,1,10000));
	
	for( double t=0.0 ; t<tmax ; t+= dt ){		
		Vec3d vs = driver(-10);
		//cout << vs  << endl;
		plot5(vs, t);
		
		double linspd = vs.y;
		if(abs(linspd) > spdmax) spdmax = abs(linspd);				
	}
	
	cout << spdmax*1000 << " mm/s" << endl;

/*
	Plotter plot("pos_spd_ang.tga", tmax,Vec3d(50,400,10000));
	Plotter plot2("curr.tga", tmax,Vec3d(100,100,100));
	Plotter plot3("volt.tga", tmax,Vec3d(100,100,100));
	Plotter plot4("torgue.tga", tmax,Vec3d(10,5000,5000));
	Plotter plot5("x_dxdt_f.tga", tmax,Vec3d(0.1,1,10000));
	Plotter plot6("ac.tga", tmax,Vec3d(0.03,0.03,10000));

	VectorController controller;
	Vec3d vabc(0, 0, 0);
	double Md = 0;
	int ccnt = 0;
	exciter vex(3, 10);

	MotorReducer motor(dt);	
	Reducer reducer(dt);
	double speed = 0.0;
	
	double spdmax = 0.0;
	double linspd = 0.0;

	Delayer<Vec3d> mcu_delv(5);
	Delayer<Vec3d> mcu_deli(3);	
	Delayer<double> enc_delayer(4);
	Delayer<double> pos_delayer(3);

    for( double t=0.0 ; t<tmax ; t+= dt ){

		Vec3d v1 = vex(t);

		speed = reducer(Md = motor(mcu_delv(vabc), speed));
		double del_phi = enc_delayer(motor.getpos());
		Vec3d del_iabc = mcu_deli(motor.getcurr());
		double del_pos = pos_delayer(v1.x);

		// 40 us sampler
		if(0==(ccnt++%10)) (vabc = controller(del_phi, del_pos, del_iabc));

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
		Vec3d ac = reducer.getstate();
		ac.y = v1.x*1.57/1000;
		plot6(ac, t);
	}	

	cout << spdmax*1000 << " mm/s" << endl;
*/

	return 0;
}
