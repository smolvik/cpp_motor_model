#ifndef __REDUCER_H__
#define __REDUCER_H__

#include "config.h"
#include "geometry.h"
#include <stdint.h>

/**
 * @brief
 * Класс линейного редуктора
 */

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

	double get_Mct_dv_m(double M)
	{
		double ret = 0.0; 
		
		if(isdzero(w_dv)){
			if(abs(M) <= Mct_dv) ret = M;
			else ret = Mct_dv*sign(M);
		}else ret = Mct_dv*sign(w_dv);
		
		return ret;	
	}	
	
	double get_Fct_n_m(double F, double dx)
	{
		double ret = 0.0; 
		
		if(isdzero(dx)){
			if(abs(F) <= F_STRN) ret = F;
			else ret = F_STRN*sign(F);
		}else ret = F_STRN*sign(dx);
		
		return ret;
	}	

	double get_Mct_dv()
	{
		return (M_STR0 + pow(K_STRDV*(abs(Fupr)), 1.33));
	}	
	
	double get_Dh12()
	{
		double ret = 0.0;
		if(abs(h12)<(DELTA_RED/2)) ret = DH12;
		return ret;
	}	
	
	double get_etha_r()
	{
		double ret = 0.0;
		
		if(isdzero(w_dv*Fupr)) ret = 1.0;
		else 
		if(w_dv*Fupr>0) ret = ETHA_T;
		else ret = 1/ETHA_TORM;
		
		return ret;
	}	
		
public:
	/**@param t шаг интегрирования */
	Reducer(double t)
	{
		dt = t;

		xpos = 0.0;
		Mupr = 0.0;
		w_dv = 0.0;
		Fupr = 0.0;
		h12 = 0.0;
		DH12 = 0.0;
		Mct_dv_m = 0.0;
		Fct_n_m = 0.0;
		Mct_dv = 0.0;
		dh12dt = 0.0;
		Mupr = 0.0;		
		Fn = 0.0;
		dxdt = 0.0;
		
		quad_w_dv = new quadrature_trapez {dt};	
		quad_dx = new quadrature_trapez {dt};	
		quad_x = new quadrature_trapez {dt};	
		quad_h1 = new quadrature_trapez {dt};	
		quad_h12 = new quadrature_trapez {dt};	
	}	

	~Reducer()
	{
		delete quad_w_dv;
		delete quad_dx;
		delete quad_x;
		delete quad_h1;
		delete quad_h12;
	}	
	/**
	 * @brief обновляет состояние
	 * @param Mdv момент выдаваемый двигателем Н*м
	 * @return угловая скорость вращения ротора рад/c
	 * 
	 */
	double operator ()(double Mdv)
	{
		Mupr = Fupr*I_RED/get_etha_r();
		Mct_dv = get_Mct_dv();
		Mct_dv_m = get_Mct_dv_m(Mdv-Mupr);
		w_dv = (*quad_w_dv)(Mdv-Mupr-Mct_dv_m-K_VTRDV*w_dv)/(JDRV+J_REDPRIV);
		
		Fct_n_m = get_Fct_n_m(Fupr-Fn, dxdt);
		
		dxdt = (*quad_dx)((Fupr-Fn-Fct_n_m-dxdt*K_VTRN))/M_MEX;
		xpos = (*quad_x)(dxdt);
		Fn = K_N*xpos;

		double dx1dt = w_dv*I_RED;	
		double dh1dt = dx1dt-dxdt;
		double h1 = (*quad_h1)(dh1dt);	
		double Fupr1 = h1*C_RED+dh1dt*B_RED;

		DH12 = dh1dt + (h1-h12)*C_RED/B_RED;

		h12 = (*quad_h12)(DH12,(abs(h12)>=(DELTA_RED/2)));

		dh12dt = get_Dh12();
		double Fupr12 = h12*C_RED + dh12dt*B_RED;
		
		Fupr = Fupr1 - Fupr12;	
		
		return w_dv;
	}	
	/**
	 * @return вектор состояния редуктора (положение м, скорость м/с, усилие на штоке Н)
	 */
	Vec3d getstate() {return Vec3d(xpos, dxdt, Fn);}
	
	friend std::ostream& operator<<(std::ostream& s, Reducer& r)
	{
		s << r.xpos << ":" << r.dxdt << ":" << r.Fn << std::endl;
		return s;
	}	
};

#endif
