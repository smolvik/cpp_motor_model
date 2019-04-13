#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "config.h"
#include "geometry.h"
#include <stdint.h>
#include "delayer.h"
#include <iostream>

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
	
	Delayer<double> *encdel;
	
public:
	Motor(double dt);
	~Motor();
	double operator ()(const Vec3d &vex, double load);
	Vec3d getstate();
	Vec3d getcurr();
	uint32_t encoder();
	friend std::ostream& operator<<(std::ostream& s, Motor& m)
	{
		s << m.position << ":" << m.speed << ":" << m.acceleration << std::endl;
		return s;
	}
	
	double getspd() {return speed;}
	double getpos() {return position;}
};

class MotorReducer : public Motor{
public:
	MotorReducer(double dt) : Motor(dt) {};
	double operator ()(const Vec3d &vex, double speed);
};

#endif
