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
	Motor(double t)
	{
		i_abc = Vec3d(0.0, 0.0, 0.0);
		didt_abc = Vec3d(0.0, 0.0, 0.0);
		
		acceleration = 0.0;
		speed = 0.0;
		position = 0.0;
		torque = 0.0;
		dt = t;
		
		quad_ele = new quadrature_trapez[3] {{dt}, {dt}, {dt}};
		quad_mex = new quadrature_trapez {dt};
		quad_rot = new quadrature_trapez {dt};	
	}
	
	~Motor()
	{
		delete[] quad_ele;
		delete quad_mex;
		delete quad_rot;
	}

	double operator ()(const Vec3d &vex, double load)
	{
		Vec3d tmp = Vec3d(sin(position*NPOL), sin(position*NPOL-2*M_PI/3), sin(position*NPOL+2*M_PI/3))*NPOL*PSIMAX;
		didt_abc = (tmp*speed + vex - i_abc*RDRV)*(1/LDRV);
		i_abc = Vec3d (quad_ele[0](didt_abc.x), quad_ele[1](didt_abc.y), quad_ele[2](didt_abc.z));

		torque = i_abc*tmp*(-1);
		
		acceleration = (torque-load)/JDRV;
		speed = (*quad_mex)(acceleration);
		position = (*quad_rot)(speed);
		
		return torque;
	}

	Vec3d getstate() {return Vec3d(position, speed, acceleration);}
	Vec3d getcurr() {return i_abc;}

	friend std::ostream& operator<<(std::ostream& s, Motor& m)
	{
		s << m.position << ":" << m.speed << ":" << m.acceleration << std::endl;
		return s;
	}
	
	double getspd() {return speed;}
	double getpos() {return position;}
	double gettorque() {return torque;}
};

class MotorReducer : public Motor{
public:
	MotorReducer(double dt) : Motor(dt) {};
	double operator ()(const Vec3d &vex, double spd)
	{
		Vec3d tmp = Vec3d(sin(position*NPOL), sin(position*NPOL-2*M_PI/3), sin(position*NPOL+2*M_PI/3))*NPOL*PSIMAX;
		didt_abc = (tmp*spd + vex - i_abc*RDRV)*(1/LDRV);
		i_abc = Vec3d (quad_ele[0](didt_abc.x), quad_ele[1](didt_abc.y), quad_ele[2](didt_abc.z));

		torque = i_abc*tmp*(-1);
		position = (*quad_rot)(spd);	

		speed = spd;

		return torque;
	}	
};

#endif
