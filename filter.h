#ifndef _FILTER_H
#define _FILTER_H

#define RF_A11 1048576
#define RF_A12 -2040723
#define RF_A13 998244

#define RF_B11 265665200
#define RF_B12 -522425040
#define RF_B13 258320810

#define RF_A21 1048576
#define RF_A22 -1854522
#define RF_A23 818938

#define RF_B21 252268930
#define RF_B22 -474757680
#define RF_B23 225814620

#define RF_SHIFTA 20
#define RF_SHIFTB 28

/**
 * @brief
 * Цифровой усредняющий фильтр
 */
class MeanFilter{
	int32_t *wnd;
	int32_t order;
	int32_t idx;
	int32_t acc;	
	
public:	
	MeanFilter(int32_t ord){
		order = ord;
		wnd = new int32_t[order]();
		acc = 0;
		idx = 0;
	}
	~MeanFilter(){
		delete[] wnd;
	}
	
	int32_t operator ()(int32_t x){
		idx = (idx+1)%order;
		acc = acc-wnd[idx]+x;
		wnd[idx] = x;		
		return acc/order;
	}	
};

/**
 * @brief
 * Цифровой фильтр 2 порядка
 */
class DigFilter2{

protected:
	/**@private*/
	int32_t x1;
	/**@private*/	
	int32_t x2;
	/**@private*/
	int32_t y1;
	/**@private*/
	int32_t y2;	
	/**@private*/
	int64_t a2;
	/**@private*/
	int64_t a3;
	/**@private*/
	int64_t b1;
	/**@private*/
	int64_t b2;
	/**@private*/
	int64_t b3;
	/**@private*/
	int32_t shiftA;
	/**@private*/
	int32_t shiftB;

public:
	DigFilter2(int32_t _a2, int32_t _a3, int32_t _b1, int32_t _b2, int32_t _b3, int32_t sA, int32_t sB):x1(0),x2(0),y1(0),y2(0),
	a2(_a2), a3(_a3), b1(_b1), b2(_b2), b3(_b3), shiftA(sA), shiftB(sB){}

	int32_t operator ()(int32_t x)
	{
		int64_t y = (-a2*y1 - a3*y2 + b1*x + b2*x1 + b3*x2) >> shiftA;
		x2 = x1; x1 = x;
		y2 = y1; y1 = y;		
		
		return y >> (shiftB - shiftA);
	}		
};

class RejFilter{
	DigFilter2 *apflt;
	int32_t kpi;
	const int32_t shk = 10;

public:
	RejFilter(double fr, double k2, double kp, double fs)
	{
		double k1 = -cos(fr*2*M_PI/fs);

		int32_t sha = 20;
		int32_t shb = sha+8;

		int32_t a1 = round(1.0*pow(2,sha));
		int32_t a2 = round((k1*(1+k2))*pow(2,sha));
		int32_t a3 = round(k2*pow(2,sha));

		int32_t b1 = round(k2*pow(2,shb));
		int32_t b2 = round((k1*(1+k2))*pow(2,shb));
		int32_t b3 = round(1.0*pow(2,shb));

		apflt = new DigFilter2 {a2, a3, b1, b2, b3, sha, shb};

		kpi = round(kp*pow(2,shk));
		
		//std::cout << kpi << std::endl;
	}

	~RejFilter()
	{
		delete apflt;
	}

	int32_t operator ()(int32_t x)
	{
		int32_t s = (*apflt)(x);
		return (((1<<shk)+kpi)*x+((1<<shk)-kpi)*s)>>(shk+1);
	}
	
	int32_t operator ()(int32_t x, int32_t kpin)
	{
		kpi = kpin;
		int32_t s = (*apflt)(x);
		return (((1<<shk)+kpi)*x+((1<<shk)-kpi)*s)>>(shk+1);
	}	
};

/**
 * @brief
 * Цифровой фильтр 1 порядка
 */
class DigFilter1{
	int32_t x1;
	int32_t y1;

	int32_t a2;

	int32_t b1;
	int32_t b2;

	int32_t shiftA;
	int32_t shiftB;

public:
	DigFilter1(int32_t _a2, int32_t _b1, int32_t _b2, int32_t sA, int32_t sB):x1(0), y1(0),
	a2(_a2), b1(_b1), b2(_b2), shiftA(sA), shiftB(sB){}

	int32_t operator ()(int32_t x)
	{
		int32_t y = (-a2*y1 + b1*x + b2*x1) >> shiftA;
		x1 = x;		
		y1 = y;		

		return y >> (shiftB - shiftA);
	}		
};

#endif
