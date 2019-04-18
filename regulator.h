#ifndef __REGULATOR_H__
#define __REGULATOR_H__

/**
 * @brief
 * ПИ регулятор
 */
class Regulator{
	int32_t ki;
	int32_t kp;
	int32_t a;
	int32_t y;		
	
public:
	/**
	 * @param _ki интегральный коэффициент
	 * @param _kp пропорциональный коэффициент
	 */
	Regulator(int32_t _ki, int32_t _kp)
	{
		ki = _ki;
		kp = _kp;
		a = 0;
		y = 0;
	}

	/**
	 * @brief обновляет состояние
	 * @param e ошибка
	 * @param fs флаг насыщения
	 * @return выходное значение регулятора
	 */
	int32_t operator ()(int32_t e, int32_t fs)
	{
		int32_t d = ki*e;
		
		// will accumulator grow up?
		if(fs) if( ((a>0)&&(d>0))||((a<0)&&(d<0)) ) d = 0;
		
		a += d;
		y = e*kp + a;
		a = a;		
		
		return y;
	}
};

#endif
