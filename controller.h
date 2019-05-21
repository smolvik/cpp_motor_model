#include <stdint.h>
#include <iostream>
#include "gdef.h"
#include "filter.h"
#include "geometry.h"
#include "tachometer.h"
#include "regulator.h"
#include "delayer.h"

/**
 * @brief
 * Векторный контроллер
 */
class VectorController{

	/**@private*/
	/* dot product for 3d vectors */
	int32_t dot3(int32_t *a, int32_t *b)
	{
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	}
	/**@private*/
	/* Clark's transform */
	void abc_to_dq(int32_t *abc, int32_t *dq, int32_t angle)
	{
		int32_t ct[3] = {	cos_tb[angle], 
							cos_tb[1023&(angle+(4*512)/3)], 
							cos_tb[1023&(angle+(2*512)/3)] };
		int32_t st[3] = {	cos_tb[1023&(3*512/2+angle)], 
							cos_tb[1023&(3*512/2+angle+(4*512)/3)], 
							cos_tb[1023&(3*512/2+angle+2*512/3)] };

		dq[0] = (dot3(abc, ct)) >> 10;	
		dq[1] = (-dot3(abc, st)) >> 10;
	}
	/**@private*/
	/* Park's transform */
	void dq_to_abc(int32_t *abc, int32_t *dq, int32_t angle)
	{
		abc[0] = (dq[0]*cos_tb[angle] 				- dq[1]*cos_tb[1023&(angle+3*512/2)]) >> 20;
		abc[1] = (dq[0]*cos_tb[1023&(angle+4*512/3)] - dq[1]*cos_tb[1023&(angle+4*512/3+3*512/2)]) >> 20;
		abc[2] = (dq[0]*cos_tb[1023&(angle+2*512/3)] - dq[1]*cos_tb[1023&(angle+2*512/3+3*512/2)]) >> 20;
	}
	/**@private*/
	/* calc magnitude and angle of 2Dvector using CORDIC algorithm */
	void cord_atan(int32_t *v, int32_t *ang, int32_t *mag)
	{
		const int32_t AngTable[] = {128, 76, 40, 20, 10, 5, 3, 1};	// angles for those tg =0.5,0.25 etc
		const int32_t kc[] = {724,  648, 628,  623,  623,  622,  622,  622}; // mag correction 1/cos(ang)
		int32_t SumAngle = 0; 
		int i = 0;
		int x, y, x1, y1;
		int ns = 0;

		x = abs(v[0]);
		y = v[1];

		for(i = 0; i < 8; i++)
		{		
			ns++;
			
			x1 = x;
			y1 = y;
				
			if(y > 0){
				x = x1 + (y1 >> i); 
				y = y1 - (x1 >> i); 
				SumAngle = SumAngle + AngTable[i]; 
			}else{
				x = x1 - (y1 >> i); 
				y = y1 + (x1 >> i); 
				SumAngle = SumAngle - AngTable[i]; 
			}
			if(y == 0) break;
		}
		
		if(v[0] < 0) SumAngle = MY_PI-SumAngle;		
		if(SumAngle < 0) SumAngle += 2*MY_PI;	
		
		*ang = SumAngle;
		*mag = (kc[ns-1]*x) >> 10;
	}
	/**@private*/
	int32_t sinpwm(int32_t *abc, int32_t *dq, int32_t phase)
	{
		int32_t fs = 0;
		int32_t mag;
		int32_t ang;
		cord_atan(dq, &ang, &mag);
		
		mag = mag >> 10;
		int32_t phi = 1023&(phase + ang);
		
		if(mag > MAXPWM) {
			mag = MAXPWM;
			fs = 1;
		}
		else fs = 0;

		abc[0] = mag*mycos( 1023&(phi) ) >> 10;
		abc[1] = -mag*mycos( 1023&(MY_PI/3+phi) ) >> 10;
		abc[2] = -mag*mycos( 1023&(2*MY_PI+MY_PI/3-phi) ) >> 10;

		return fs;	
	}
	/**@private*/
	int32_t svpwm(int32_t *abc, int32_t *dq, int32_t phase)
	{
		int32_t fs = 0;
		int32_t mag;
		int32_t ang;
		cord_atan(dq, &ang, &mag);	
		
		mag = mag >> 10;

		if(mag > MAXPWM) {
			mag = MAXPWM;
			fs = 1;
		}
		else fs = 0;

		int32_t phi = 1023&(phase + ang);			
		int32_t ns = (phi*6) >> 10;	 // get the sector number
		int32_t r1;
		int32_t r2;
		
		switch(ns){
			case 0:
			r1 = mag*mysin(7*MY_PI/3-phi) >> 10;
			r2 = mag*mysin(phi) >> 10;
			
			abc[0] = r1+r2;
			abc[1] = -r1+r2;
			abc[2] = -r1-r2;			
			break;
			
			case 1:
			phi -= MY_PI/3;
			r1 = mag*mysin(7*MY_PI/3-phi) >> 10;
			r2 = mag*mysin(phi) >> 10;
			
			abc[0] = r1-r2;
			abc[1] = r1+r2;
			abc[2] = -r1-r2;		
			break;
			
			case 2:
			phi -= 2*MY_PI/3;
			r1 = mag*mysin(7*MY_PI/3-phi) >> 10;
			r2 = mag*mysin(phi) >> 10;
			
			abc[0] = -r1-r2;
			abc[1] = r1+r2;
			abc[2] = -r1+r2;
			break;
			
			case 3:
			phi -= 3*MY_PI/3;
			r1 = mag*mysin(7*MY_PI/3-phi) >> 10;
			r2 = mag*mysin(phi) >> 10;
			
			abc[0] = -r1-r2;
			abc[1] = r1-r2;
			abc[2] = r1+r2;		
			break;
			
			case 4:
			phi -= 4*MY_PI/3;
			r1 = mag*mysin(7*MY_PI/3-phi) >> 10;
			r2 = mag*mysin(phi) >> 10;
			
			abc[0] = -r1+r2;
			abc[1] = -r1-r2;
			abc[2] = r1+r2;	
			break;
			
			case 5:
			phi -= 5*MY_PI/3;
			r1 = mag*mysin(7*MY_PI/3-phi) >> 10;
			r2 = mag*mysin(phi) >> 10;
			
			abc[0] = +r1+r2;
			abc[1] = -r1-r2;
			abc[2] = r1-r2;
			break;
		}
		
		return fs;
	}
	/**@private*/
	int32_t adc_curin(double x)
	{
		const double kis = 66e-3; 		// curr sensor sens В/A
		const double kd = 43.0/(43.0+10.0); 	// volt divider
		const double kadc = 4095/3.3;
		int32_t y = (int32_t)(x*kis*kd*kadc);
		
		if(y>2048) y = 2048;
		if(y<-2048) y = -2048;		
		
		return y;
	}
	/**@private*/
	int32_t adcposin(double u)
	{
		double kadc = 4095.0/3.3;
		double u1 = (-u*0.15+1.6);
		int32_t x = (int32_t)(u1*kadc);

		if(x < 0) x = 0;
		if(x > 4095) x = 4095;
		
		return x; //-(71015*x-34423*4096)/4096;
	}	
	/**@private*/
	double pwmout(int32_t x)
	{
		double y = x*UPWR/1024.0;
		
		if(y > UPWR/2) y = UPWR/2;
		if(y < -UPWR/2) y = -UPWR/2;
		
		return y;
	}	
	/**@private*/
	uint32_t encoder(double phi)
	{
		double rem = fmod(phi*360.0/(2*M_PI), 360.0);
		if(rem<0) rem += 360;
		return (uint32_t)4095*(rem/360);
	}	
	
	/**@private*/
	Regulator *dreg;
	/**@private*/
	Regulator *qreg;
	/**@private*/
	Regulator *sreg;
	/**@private*/
	Regulator *preg;
	/**@private*/
	RejFilter *rflt1;
	/**@private*/
	double curr_mag;
	/**@private*/
	double curr_ang;
	/**@private*/
	int32_t fCurSat;	
	/**@private*/
	int32_t fSpdSat;
	/**@private*/
	int32_t fPosSat;
	/**@private*/
	int32_t phase;
	/**@private*/
	int32_t tcnt;
	/**@private*/
	int32_t qref;
	/**@private*/
	int32_t maxqcurrent;
	/**@private*/
	Tachometer tacho;
	
	DigFilter2 *flt1;
	DigFilter2 *flt2;
	DigFilter1 *flt3_1;
	DigFilter1 *flt3_2;
	DigFilter1 *flt4;
	DigFilter1 *flt5;	

	int32_t antcor(int32_t x, int32_t wdv)
	{
		int32_t num = (*flt3_1)(abs((*flt1)(x))); 
		int32_t den = (*flt3_2)(abs((*flt2)(x))); 

		//cout << num << ":" << den << endl;

		if(den == 0) den = 1;

		int32_t zn = (*flt4)(num/den);	
		int32_t pf = abs(100*(*flt5)(x));

		int32_t kw = 0;

		if(pf == 0) {
			kw = wdv<<1;
		}
		else {
			if(zn < 8) kw = wdv>>2;
			else if(zn<12) kw = wdv;
			else if(zn<18) kw = wdv + (wdv>>1);
			else if(zn<25) kw = wdv + (wdv>>1) + (wdv>>2);
			else kw = wdv<<1;
		}

		//cout << zn << endl;
		return zn;

		//return kw;
	}	

public:

	VectorController()
	{
		// init the regulators
		dreg = new Regulator {KI_DQCUR, KP_DQCUR};
		qreg = new Regulator {KI_DQCUR, KP_DQCUR};	
		sreg = new Regulator {KI_SPD, KP_SPD};	
		preg = new Regulator {KI_POS, KP_POS};

		rflt1 = new RejFilter {35, 0.7, 0.85, 1/320e-6};

		fCurSat = 0;
		fSpdSat = 0;
		fPosSat = 0;
		phase = 0;
		tcnt = 0;

		qref = 0;

		maxqcurrent = MAXQCURRENT;
		
		flt1 = new DigFilter2 {-1819, 821, 0, 81919, -81919, 10, 10+7};
		flt2 = new DigFilter2 {-2019, 996, 0, 10240, -10240, 10, 10+10};
		flt3_1 = new DigFilter1 {-1004, 5243, 0, 10, 10+8};
		flt3_2 = new DigFilter1 {-1004, 5243, 0, 10, 10+8};
		flt4 = new DigFilter1 {-996, 7282, 0, 10, 10+8};
		flt5 = new DigFilter1 {-896, 131072, -131072, 10, 10+10};		
		
	}
	~VectorController()
	{
		delete dreg;
		delete qreg;
		delete sreg;
		delete preg;	
		
		delete rflt1;
		
		delete flt1;
		delete flt2;
		delete flt3_1;
		delete flt3_2;
		delete flt4;		
		delete flt5;		
	}

	/**
	 * @brief обновляет состояние
	 * @param phi угловое положение ротора рад
	 * @param refpos напряжение уставки контура положения В
	 * @param iabc фазные токи статора А
	 * @return вектор фазных напряжений статора В
	 */
	Vec3d operator ()(double phi, double refpos, Vec3d iabc)
	{		
		int32_t code = encoder(phi);
		phase = (0+code) & (1024-1);
		
		// convert abc currents to dq
		int32_t abc[3];
		int32_t dq[2];
		int32_t refspeed = 0;

		abc[0] = adc_curin(iabc.x);
		abc[1] = adc_curin(iabc.y);
		abc[2] = adc_curin(iabc.z);
		abc_to_dq(abc, dq, phase);

		tcnt++;
		if( (0x0007 & tcnt) == 0){		
			/* 300 us */

			//phase = (phase+3) & 1023;
			int32_t speed = tacho(code);
			const int32_t xrp0 = 1986;
			int32_t xrp = adcposin(refpos)-xrp0;

			/* prefilter */
			if(antcor(xrp, speed)>7) xrp = (*rflt1)(xrp,410);
			else xrp = (*rflt1)(xrp,870);			

			xrp = -(71015*(xrp+xrp0)-34423*4096)/4096;

			//std::cout << xrp << std::endl;
			//std::cout << adcposin(refpos) << ":" << xrp << std::endl;

			/* position regulator */	
			int32_t pe = xrp - tacho.position();
			refspeed = (*preg)(pe, fPosSat) >> 12;

			//std::cout << tacho.position() << std::endl;

			fPosSat = 0;
			if(refspeed > MAXSPEED) {refspeed = MAXSPEED; fPosSat = 1;}
			if(refspeed < -MAXSPEED) {refspeed = -MAXSPEED; fPosSat = 1;}			
			//refspeed = refpos;
			//refspeed = 1000;
			
			int32_t se = refspeed - speed;
			
/*
#ifdef CFG_ANTONCORREN
			int32_t se = refspeed - speed_kw;
#else
			int32_t se = refspeed - speed;
#endif
*/
			/* speed regulator */
			qref = (*sreg)(se, fSpdSat) >> 12;

			fSpdSat = 0;
			if(qref > maxqcurrent) {qref = maxqcurrent; fSpdSat = 1;}
			if(qref < -maxqcurrent) {qref = -maxqcurrent; fSpdSat = 1;}			
		}

		//qref = adcin(refpos);
		//qref = 1000;

		// get the errors
		int32_t ed = 0 - dq[0];
		int32_t eq = qref - dq[1];

		// currents regulators do its work
		dq[0] = (*dreg)(ed, fCurSat) >> 2;
		dq[1] = (*qreg)(eq, fCurSat) >> 2;

		//std::cout << dq[0] << ":" << dq[1] << std::endl;

		//dq[0] = 0;
		//dq[1] = 1000<<8;
		
		fCurSat = svpwm(abc, dq, phase);
		//fCurSat = sinpwm(abc, dq, phase);
		
		//std::cout << phase << std::endl;
		//std::cout << abc[0] << ":" << abc[1] << ":" << abc[2] << std::endl;

		return Vec3d(pwmout(abc[0]), pwmout(abc[1]), pwmout(abc[2]));
	}	
};
