#ifndef __EXCITER_H__
#define __EXCITER_H__

/**
 * @brief
 * Трехфазный источник напряжения
 */
class Exciter{
	double amp;
	double freq;

	public:
	
	Vec3d v;
	/**
	 * @param a амплитуда В
	 * @param f частота Гц
	 */
	Exciter(double a, double f): amp(a), freq(f) {
		v.x = 0;
		v.y = 0;
		v.z = 0;
		}
		/**
 * @brief обновляет состояние
 * @param t текущее время
 * @return вектор фазных напряжений 
 * 
 */
	Vec3d operator() (double t){
		v.x = amp*cos(2*M_PI*freq*t);
		v.y = amp*cos(2*M_PI*freq*t-2*M_PI/3);
		v.z = amp*cos(2*M_PI*freq*t+2*M_PI/3);
		
		return v;
	}
};

#endif
