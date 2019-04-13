#ifndef __REDUCER_H__
#define __REDUCER_H__

#include "config.h"
#include "geometry.h"
#include <stdint.h>

class Reducer{
	
	double dt;
	double xpos;
	double Mupr;
	double w_dv;
	double Fupr;
	double h12;
	double DH12;
	double Mct_dv_m;
	double Fct_n_m;
	double Mct_dv;
	double dh12dt;
	double Fn;
	double dxdt;
	
	quadrature_trapez *quad_w_dv;
	quadrature_trapez *quad_dx;
	quadrature_trapez *quad_x;
	quadrature_trapez *quad_h1;
	quadrature_trapez *quad_h12;
		
public:
	Reducer(double dt);
	~Reducer();
	double operator ()(double Mdv);
	Vec3d getstate();
	double get_Mct_dv_m(double M);
	double get_Fct_n_m(double F, double dx);
	double get_Mct_dv();
	double get_Dh12();
	double get_etha_r();
	
	friend std::ostream& operator<<(std::ostream& s, Reducer& r)
	{
		s << r.xpos << ":" << r.dxdt << ":" << r.Fn << std::endl;
		return s;
	}	
	
	int32_t linsensor();
	
};

#endif
