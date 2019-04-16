#ifndef __DRIVER_H__
#define __DRIVER_H__

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

class Driver{
	
	Delayer<Vec3d> *mcu_delv;
	Delayer<Vec3d> *mcu_deli;	
	Delayer<double> *enc_delayer;
	Delayer<double> *pos_delayer;	
	
	int ccnt;
	double dt;
	double speed;
	Vec3d vabc;
	
public:

	MotorReducer *motor;	
	Reducer *reducer;
	VectorController *controller;

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

#endif
