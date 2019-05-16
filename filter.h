#ifndef _FILTER_H
#define _FILTER_H

#define RF_A11 1024
#define RF_A12 -1811
#define RF_A13 800

#define RF_B11 246356
#define RF_B12 -463631
#define RF_B13 220522

#define RF_A21 1024
#define RF_A22 -1993
#define RF_A23 975

#define RF_B21 259439
#define RF_B22 -510181
#define RF_B23 252266


#define RF_SHIFTA 10
#define RF_SHIFTB 18

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
	int32_t x1;
	int32_t x2;
	int32_t y1;
	int32_t y2;	

	int32_t a2;
	int32_t a3;
	
	int32_t b1;
	int32_t b2;
	int32_t b3;
	
	int32_t shiftA;
	int32_t shiftB;

public:
	DigFilter2(int32_t _a2, int32_t _a3, int32_t _b1, int32_t _b2, int32_t _b3, int32_t sA, int32_t sB):x1(0),x2(0),y1(0),y2(0),
	a2(_a2), a3(_a3), b1(_b1), b2(_b2), b3(_b3), shiftA(sA), shiftB(sB){}

	int32_t operator ()(int32_t x)
	{
		int32_t y = (-a2*y1 - a3*y2 + b1*x + b2*x1 + b3*x2) >> shiftA;
		x2 = x1; x1 = x;
		y2 = y1; y1 = y;		
		
		return y >> (shiftB - shiftA);
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
