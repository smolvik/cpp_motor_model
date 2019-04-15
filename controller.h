#include <stdint.h>
#include <iostream>
#include "gdef.h"
#include "filter.h"
#include "geometry.h"
#include "tachometer.h"
#include "regulator.h"
#include "delayer.h"

class VectorController{

	/* dot product for 3d vectors */
	int32_t dot3(int32_t *a, int32_t *b)
	{
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	}

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

	/* Park's transform */
	void dq_to_abc(int32_t *abc, int32_t *dq, int32_t angle)
	{
		abc[0] = (dq[0]*cos_tb[angle] 				- dq[1]*cos_tb[1023&(angle+3*512/2)]) >> 20;
		abc[1] = (dq[0]*cos_tb[1023&(angle+4*512/3)] - dq[1]*cos_tb[1023&(angle+4*512/3+3*512/2)]) >> 20;
		abc[2] = (dq[0]*cos_tb[1023&(angle+2*512/3)] - dq[1]*cos_tb[1023&(angle+2*512/3+3*512/2)]) >> 20;
	}

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
	
	int32_t adcposin(double u)
	{
		double kadc = 4095.0/3.3;
		double u1 = (-u*0.15+1.6);
		int32_t x = (int32_t)(u1*kadc);

		if(x < 0) x = 0;
		if(x > 4095) x = 4095;
		
		return -(71015*x-34423*4096)/4096;
	}	
	
	double pwmout(int32_t x)
	{
		const double Umax = 57.0;
		double y = x*Umax/1024.0;
		
		if(y > Umax/2) y = Umax/2;
		if(y < -Umax/2) y = -Umax/2;
		
		return y;
	}	
	
	uint32_t encoder(double phi)
	{
		double rem = fmod(phi*360.0/(2*M_PI), 360.0);
		if(rem<0) rem += 360;
		return (uint32_t)4095*(rem/360);
	}	

	Regulator *dreg;
	Regulator *qreg;
	Regulator *sreg;
	Regulator *preg;
	
	double curr_mag;
	double curr_ang;
	
	int32_t fCurSat;	
	int32_t fSpdSat;
	int32_t fPosSat;
	int32_t phase;
	int32_t tcnt;

	int32_t qref;

	int32_t maxqcurrent;

	Tachometer tacho;

public:

	VectorController()
	{
		// init the regulators
		dreg = new Regulator {KI_DQCUR, KP_DQCUR};
		qreg = new Regulator {KI_DQCUR, KP_DQCUR};	
		sreg = new Regulator {KI_SPD, KP_SPD};	
		preg = new Regulator {KI_POS, KP_POS};

		fCurSat = 0;
		fSpdSat = 0;
		fPosSat = 0;
		phase = 0;
		tcnt = 0;

		qref = 0;

		maxqcurrent = MAXQCURRENT;
		
	}
	~VectorController()
	{
		delete dreg;
		delete qreg;
		delete sreg;
		delete preg;		
	}

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

			/* position regulator */
			int32_t pe = adcposin(refpos) - tacho.position();
			(*preg)(pe, fPosSat);
			refspeed = preg->y>>12;
			//std::cout << tacho.position() << std::endl;

			fPosSat = 0;
			if(refspeed > MAXSPEED) {refspeed = MAXSPEED; fPosSat = 1;}
			if(refspeed < -MAXSPEED) {refspeed = -MAXSPEED; fPosSat = 1;}			
			//refspeed = refpos;
						
			int32_t se = refspeed - speed;			

			/* speed regulator */
			(*sreg)(se, fSpdSat);
			
			qref = sreg->y>>12;
			
			fSpdSat = 0;
			if(qref > maxqcurrent) {qref = maxqcurrent; fSpdSat = 1;}
			if(qref < -maxqcurrent) {qref = -maxqcurrent; fSpdSat = 1;}			
		}

		//qref = adcin(refpos);

		// get the errors
		int32_t ed = 0 - dq[0];
		int32_t eq = qref - dq[1];

		// currents regulators do its work
		(*dreg)(ed, fCurSat);
		(*qreg)(eq, fCurSat);

		dq[0] = dreg->y>>2;
		dq[1] = qreg->y>>2;

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

/*
int32_t lerr_filter(int32_t x)
{
	static int32_t j = 0;
	static int32_t a = 0;
	static int32_t b[256];
	
	j = (j+1)&(256-1);
	a = a-b[j]+x;
	b[j] = x;
	
	return a;
}

int32_t lpos_filter(int32_t x)
{
	static int32_t j = 0;
	static int32_t a = 0;
	static int32_t b[16];
	
	j = (j+1)&(16-1);
	a = a-b[j]+x;
	b[j] = x;
	
	return a>>4;
}

int32_t prefilter(int32_t xk)
{
	const int32_t a2 = 958;
	const int32_t b1 = 66;
	static int32_t yk1 = 0;
	int32_t yk = 0;
	
	yk = b1*xk + ((a2*yk1)>>10);
	yk1 = yk;
	
	return yk >> 10;
}

*/
