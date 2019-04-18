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
#include "fosys.h" 

/**
 * @brief
 * Одногоканальный привод
 */
class Driver{

	Delayer<Vec3d> *mcu_delv;
	Delayer<Vec3d> *mcu_deli;	
	Delayer<double> *enc_delayer;
	Delayer<double> *pos_delayer;	
	
	fosys *uposrc;

	int ccnt;
	double dt;
	double speed;
	Vec3d vabc;

	int32_t ns;

	MotorReducer *motor;	
	Reducer *reducer;
	VectorController *controller;

public:

	
	/**
	 * @param _dt Шаг интегрирования 
	 * 
	 * */
	Driver(double _dt)
	{
		ccnt = 0;		
		dt = _dt;
		speed = 0.0;
		vabc = Vec3d(0.0, 0.0, 0.0);
		ns = (int32_t)(40e-6/dt);

		reducer = new Reducer {dt};
		motor = new MotorReducer {dt};
		controller = new VectorController;
		uposrc = new fosys {20e3*0.01e-6, dt};

		mcu_delv = new Delayer<Vec3d> {5}; //20us
		mcu_deli = new Delayer<Vec3d> {3}; //12us
		pos_delayer = new Delayer<double> {3}; //12us		
		enc_delayer = new Delayer<double> {4}; //16us
	}

	~Driver(){
		delete reducer;
		delete motor;
		delete mcu_deli;
		delete mcu_delv;
		delete pos_delayer;
		delete enc_delayer;
		delete controller;
		delete uposrc;
	}
	
	/**
	 * @brief обновляет состояние
	 * @param ux напряжение уставки регулятора положения -10В..+10В
	 * @return вектор состояния - (положение м, скорость м/с, усилие на штоке Н)
	 */
	Vec3d operator () (double ux)
	{
		
		speed = (*reducer)((*motor)((*mcu_delv)(vabc), speed));
		double del_phi = (*enc_delayer)((*motor).getpos());
		Vec3d del_iabc = (*mcu_deli)((*motor).getcurr());
		double del_pos = (*pos_delayer)((*uposrc)(ux));

		/*
		speed = (*reducer)((*motor)(vabc, speed));
		double del_phi = motor->getpos();
		Vec3d del_iabc = motor->getcurr();
		double del_pos = ux;
		*/

		/* 40 us sampler */
		if(0==(ccnt++%ns)) (vabc = (*controller)(del_phi, del_pos, del_iabc));		
		
		return reducer->getstate();
	}

};

#endif
