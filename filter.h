#ifndef _FILTER_H
#define _FILTER_H

#define RF_A11 1024
#define RF_A12 -1990
#define RF_A13 973

#define RF_B11 258212
#define RF_B12 -509394
#define RF_B13 252969

#define RF_A21 1024
#define RF_A22 -1930
#define RF_A23 922

#define RF_B21 254935
#define RF_B22 -493957
#define RF_B23 243139

#define RF_SHIFTA 10
#define RF_SHIFTB 18

/**
 * @brief
 * Класс цифрового усредняющего фильтра
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
 * Класс цифрового режекторнорго фильтра
 */
class RejFilter{
	int32_t x1;
	int32_t x2;
	int32_t y1;
	int32_t y2;	

	int32_t a2;
	int32_t a3;
	
	int32_t b1;
	int32_t b2;
	int32_t b3;
	
public:
	RejFilter(int32_t _a2, int32_t _a3, int32_t _b1, int32_t _b2, int32_t _b3):x1(0),x2(0),y1(0),y2(0),
	a2(_a2), a3(_a3), b1(_b1), b2(_b2), b3(_b3){}
	
	int32_t operator ()(int32_t x)
	{
		int32_t y = (-a2*y1 - a3*y2 + b1*x + b2*x1 + b3*x2) >> RF_SHIFTA;
		x2 = x1; x1 = x;
		y2 = y1; y1 = y;		
		
		return y >> (RF_SHIFTB - RF_SHIFTA);
	}		
};


#endif
