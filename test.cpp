#include <iostream>
#include <math.h>
#include <stdint.h>
#include <complex>
#include "tgaimage.h"
#include "geometry.h"
#include "tgaplotter.h"
#include "driver.h"
#include "filter.h"

int32_t antcor(int32_t x, int32_t wdv)
{
	static DigFilter2 flt1(-1819, 821, 0, 81919, -81919, 10, 10+7);
	static DigFilter2 flt2(-2019, 996, 0, 10240, -10240, 10, 10+10);
	static DigFilter1 flt3_1(-1004, 5243, 0, 10, 10+8);
	static DigFilter1 flt3_2(-1004, 5243, 0, 10, 10+8);	
	static DigFilter1 flt4(-996, 7282, 0, 10, 10+8);
	static DigFilter1 flt5(-896, 131072, -131072, 10, 10+10);	

	int32_t num = flt3_1(abs(flt1(x))); //flt3(abs(flt1(x)));
	int32_t den = flt3_2(abs(flt2(x))); //flt3(abs(flt2(x)));

	//cout << num << ":" << den << endl;

	if(den == 0) den = 1;

	int32_t zn = flt4(num/den);	
	int32_t pf = abs(100*flt5(x));

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
	//return pf;
}
/*
int main(int argc, char **argv)
{
	double dt = 4e-6;
	double tmax = 10;
	double klin = 1.57/1000;
	double aex = 0.0;
	
	if(argc < 2) return 0;	
	aex = atof(argv[1]);

	Driver driver(dt);

	double spdmax = 0.0;
	
	TGAPlotter plot4("curr.tga", tmax,Vec3d(100,100,100));
	TGAPlotter plot5("step.tga", tmax,Vec3d(0.05,0.05,0.05));

	for( double t=0.0 ; t<tmax ; t+= dt ){		
		//double v1 = (t<0.2)?10:0;
		double v1 = aex;
		Vec3d vs = driver(v1);
		
		//cout << vs  << endl;
		plot5(Vec3d(v1*klin,vs.x,0), t);
		plot4(driver.motor->getcurr(), t);
		
		
		double linspd = vs.y;
		if(abs(linspd) > spdmax) spdmax = abs(linspd);				
	}

	std::cout << spdmax*1000 << " mm/s" << std::endl;

	return 0;
}
*/

#define NFFT 16

complex<int32_t> wtbl[NFFT];

complex<int32_t> fft(int k, int32_t *x, int n, int is, int d)
{
	complex<int32_t> c = 0;

	if (n == 1) {
		int32_t arg = (k<<(int32_t)log2(NFFT/2)) & (NFFT-1);
		c = 10000*x[is] + x[is+d]*wtbl[arg];
	}
	else {
		int32_t arg = (k<<((int32_t)log2(NFFT)-n)) & (NFFT-1);
		c = fft(k, x, n-1, is, d<<1) + wtbl[arg]*(fft(k, x, n-1, is+d, d<<1)/10000);
	}
	
	return c;	
}

complex<double> fft(int k, double *x, int n, int is, int d)
{
	complex<double> c = 0;	
	
	if (n == 2) {
		complex<double> arg = -M_PI*k*1i;
		c = x[is] + x[is+d]*exp(arg);		
	}
	else {
		complex<double> arg = -(2*M_PI*k/n)*1i;
		c = fft(k, x, n/2, is, d*2) + exp(arg)*fft(k, x, n/2, is+d, d*2);
	}
	
	return c;	
}


union UNI_UARTCWORD
{
  uint64_t d64;
  struct
  {
	uint64_t ref1 : 12;
	uint64_t ref2 : 12;
	uint64_t ref3 : 12;
	uint64_t cw_bof3 : 1;
	uint64_t cw_bof2 : 1;
	uint64_t cw_bof1 : 1;
	uint64_t cw_emul : 1;
	uint64_t cw_pwon : 1;
	uint64_t cw_cal3 : 1;
	uint64_t cw_cal2 : 1;
	uint64_t cw_cal1 : 1;
	uint64_t cw_gmod : 1;
	uint64_t cw_res1 : 1;
	uint64_t cw_res2 : 1;
	uint64_t cw_res3 : 1;
	uint64_t crc : 16;
  }
  b;
};

int main(int argc, char **argv)
{
	int32_t x[NFFT];
	double xd[NFFT];
	double ftst = 32;
	double fs = 5.0*NFFT;
	
	//union UNI_UARTCWORD tu;
	//printf("%d\n", sizeof(tu));	
	//return 0;
	
	for(int i = 0; i < NFFT; i++){
		complex<double> arg = -(2*M_PI/NFFT)*i*1i;
		wtbl[i] = 10000.0*exp(arg);
		//wtbl[i] = complex<int32_t>( round(1000.0*cos(-(2*M_PI/NFFT)*i)), round(1000.0*sin(-(2*M_PI/NFFT)*i)) );
		x[i] = round(1000*cos(2*M_PI*ftst*i/fs));
		xd[i] = 1000*cos(2*M_PI*ftst*i/fs);
	}

	for(int k = 0; k < NFFT; k++){
		complex<int32_t> c = (fft(k, x, log2(NFFT), 0, 1)+500)/1000;
		//complex<double> c = fft(k, xd, NFFT, 0, 1);
		cout << abs(c) << endl;
		//cout << wtbl[k] << endl;
	}

	return 0;
}

/*
int main(int argc, char **argv)
{
	double dt = 4e-6;
	double tmax = 1;
	double aex = 0;
	double fex = 10;
	int cnt = 0;
	double v1 = 0;
	double v2 = 0;
	
	aex = atof(argv[1]);
	fex = atof(argv[2]);	
	
	//RejFilter flt1(38.4, 0.952, 0.57, 1/320e-6);
	//RejFilter flt2(58.7, 0.781, 0.45, 1/320e-6);
	RejFilter flt1(35, 0.7, 0.85, 1/320e-6);
	RejFilter flt2(35, 0.7, 0.4, 1/320e-6);
	
	TGAPlotter plot5("step.tga", tmax,Vec3d(5000,5000,5000));

	for( double t=0.0 ; t<tmax ; t+= dt ){		
		//v1 = aex;
		v1 = (t<0.2)?0:aex;
		//v1 = aex*sin(2*M_PI*fex*t);
		//v1 = aex*sign(sin(2*M_PI*5*t));

		// 3000 Hz
		//if(0==(cnt++%80)) v2 = flt2(v1);
		if(0==(cnt++%80)) v2 = flt2(flt1(v1));
		//if(0==(cnt++%80)) v2 = antcor(v1,1000);

		//cout << vs  << endl;
		plot5(Vec3d(v1, v2, 0), t);
	}

	return 0;
}
*/

/*
int main(int argc, char **argv)
{
	double dt = 4e-6;
	double tmax = 1;
	int fex = 10;
	double aex = 1000;

	if(argc < 3) return 0;

	aex = atof(argv[1]);
	fex = atof(argv[2]);

	DigFilter2 flt1(-1819, 821, 0, 81919, -81919, 10, 10+10);
	DigFilter2 flt2(-2019, 996, 0, 10240, -10240, 10, 10+13);
	DigFilter1 flt3(-1004, 5243, 0, 10, 10+8);
	DigFilter1 flt4(-996, 7282, 0, 10, 10+8);
	DigFilter1 flt5(-896, 131072, -131072, 10, 10+10);
	RejFilter rjflt1(38.4, 0.952, 0.57, 1/320e-6);
	RejFilter rjflt2(58.7, 0.781, 0.45, 1/320e-6);

	TGAPlotter plot5("sin.tga", tmax,Vec3d(5000,50,5000));

	complex<double> arg = 0;
	complex<double> c1(0.0, 0.0);
	complex<double> c2(0.0, 0.0);

	double vs = 0.0;
	int cnt = 0;

	for( double t=0.0 ; t<tmax ; t+= dt ){		
		//double v1 = aex*sin(2*M_PI*fex*t) + 100*sin(2*M_PI*20*t);
		double v1 = aex*sin(2*M_PI*fex*t);

		//if(0==(cnt++%80)) cout << flt3(flt1(v1)) <<":" << flt3(flt2(v1)) << endl;
		if(0==(cnt++%80)) vs = antcor(v1,1000);
		//if(0==(cnt++%80)) vs = rjflt2(v1);
		
		c1 += v1*exp(arg);
		c2 += vs*exp(arg);		

		complex<double> darg = -(2*M_PI*dt/tmax)*1i*fex;
		arg = arg + darg;		
		
		//cout << vs  << endl;
		plot5(Vec3d(v1,vs,0), t);
		
	}

	std::cout << fex << " " << 20*log10(std::abs(c2/c1)) << " " << (180/M_PI)*std::arg(c2/c1) << std::endl;

	return 0;
}
*/
