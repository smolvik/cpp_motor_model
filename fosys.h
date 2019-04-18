#ifndef __FOSYS_H__
#define __FOSYS_H__

#include "geometry.h"

/**
 * @brief
 * Линейная система первого порядка
 */
class fosys {
	double tau;
	double y;
	quadrature_trapez *quad;
	
public:
	/**
	 * @param t постоянная времени системы
	 * @param dt шаг интегрирования
	 */
	fosys(double t, double dt)
	{
		tau = t;
		y = 0.0;
		quad = new quadrature_trapez {dt};	
	}
	
	~fosys()
	{
		delete quad;
	}
	
	/**
	 * @brief обновляет состояние
	 * @param x входной сигнал
	 * @return выходной сигнал
	 */
	double operator ()(double x)
	{
		return y = (1/tau)*(*quad)(x-y);
	}	
};


#endif
