#include "reducer.h"

Reducer::Reducer(double t)
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

Reducer::~Reducer()
{
	delete quad_w_dv;
	delete quad_dx;
	delete quad_x;
	delete quad_h1;
	delete quad_h12;
}

double Reducer::get_Mct_dv_m(double M)
{
	double ret = 0.0; 
	
	if(isdzero(w_dv)){
		if(abs(M) <= Mct_dv) ret = M;
		else ret = Mct_dv*sign(M);
	}else ret = Mct_dv*sign(w_dv);
	
	return ret;	
}

double Reducer::get_Fct_n_m(double F, double dx)
{
	double ret = 0.0; 
	
	if(isdzero(dx)){
		if(abs(F) <= F_STRN) ret = F;
		else ret = F_STRN*sign(F);
	}else ret = F_STRN*sign(dx);
	
	return ret;
}

double Reducer::get_Mct_dv()
{
	return (M_STR0 + pow(K_STRDV*(abs(Fupr)), 1.33));
}

double Reducer::get_Dh12()
{
	double ret = 0.0;
	if(abs(h12)<(DELTA_RED/2)) ret = DH12;
	return ret;
}

double Reducer::get_etha_r()
{
	double ret = 0.0;
	
	if(isdzero(w_dv*Fupr)) ret = 1.0;
	else 
	if(w_dv*Fupr>0) ret = ETHA_T;
	else ret = 1/ETHA_TORM;
	
	return ret;
}	

double Reducer::operator ()(double Mdv)
{
	Mupr = Fupr*I_RED/get_etha_r();
	Mct_dv = get_Mct_dv();
	Mct_dv_m = get_Mct_dv_m(Mdv-Mupr);
	w_dv = (*quad_w_dv)(Mdv-Mupr-Mct_dv_m-K_VTRDV*w_dv)/(JDRV+J_REDPRIV);
	
	Fct_n_m = get_Fct_n_m(Fupr-Fn, dxdt);
	
	dxdt = (*quad_dx)((Fupr-Fn-Fct_n_m-dxdt*K_VTRN)/M_MEX);
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

Vec3d Reducer::getstate()
{
	return Vec3d(xpos, w_dv, Fn);
}

int32_t Reducer::linsensor()
{
	int32_t x = 2048*(uint32_t)xpos/X_MAX;
	if(x>2048) x = 2048; 
	else if(x<-2048) x = -2048;
	return x;
}
