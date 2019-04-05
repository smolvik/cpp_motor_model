#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "config.h"
#include "geometry.h"
#include <stdint.h>


class Motor{
	
protected:
	quadrature_trapez *quad_ele;
	quadrature_trapez *quad_mex;
	quadrature_trapez *quad_rot;	
	
	double acceleration;
	double speed;
	double position;
	double torque;
	double dt;
		
	Vec3d i_abc;
	Vec3d didt_abc;	
	
public:
	Motor(double dt);
	~Motor();
	double operator ()(const Vec3d &vex, double load);
	Vec3d getstate();
	Vec3d getcurr();
	uint32_t encoder();
	
};

class MotorReducer : public Motor{
public:
	MotorReducer(double dt) : Motor(dt) {};
	double operator ()(const Vec3d &vex, double speed);
};

#endif
