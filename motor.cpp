#include "motor.h"

Motor::Motor(double t)
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

Motor::~Motor()
{
	delete[] quad_ele;
	delete quad_mex;
	delete quad_rot;
}

double Motor::operator ()(const Vec3d &vex, double load)
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

Vec3d Motor::getstate()
{
	return Vec3d(position, speed, acceleration);
}

Vec3d Motor::getcurr()
{
	return i_abc;
}

double MotorReducer::operator ()(const Vec3d &vex, double spd)
{
	Vec3d tmp = Vec3d(sin(position*NPOL), sin(position*NPOL-2*M_PI/3), sin(position*NPOL+2*M_PI/3))*NPOL*PSIMAX;
	didt_abc = (tmp*spd + vex - i_abc*RDRV)*(1/LDRV);
	i_abc = Vec3d (quad_ele[0](didt_abc.x), quad_ele[1](didt_abc.y), quad_ele[2](didt_abc.z));

	torque = i_abc*tmp*(-1);
	position = (*quad_rot)(spd);	

	speed = spd;

	return torque;
}
