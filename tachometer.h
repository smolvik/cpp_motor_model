#ifndef __TACHOMETER_H__
#define __TACHOMETER_H__
/**
 * @brief
 * Измеритель скорости вращения ротора
 */
class Tachometer{
	
	int32_t enc1;
	int32_t enc2;
	int32_t spd;
	int32_t pos;
	
public:	
	Tachometer(){
		enc1 = 0;
		enc2 = 0;
		spd = 0;
		pos = 0;
	}
	/**
	 * @brief обновляет состояние измерителя
	 * @param code 12 разр. код с энкодера 
	 * @return скорость вращения ротора об/мин
	 */	
	int32_t operator ()(int32_t code){
			
		int32_t denc;
		int32_t rate = 60*25000/8;
		
		denc = (code-enc2);
		enc2 = enc1;
		enc1 = code;
		if(abs(denc) > 1000){			
			if(denc < 0) denc += 4096;
			else denc -= 4096;
		}		

		pos += denc;

		spd = (denc*rate)>>13;
		return spd;
	}
	/**@return текущее положение ротора в условных угловых еденицах */
	int32_t position()
	{
		return pos>>1;
	}
	
	/**@return скорость вращения ротора об/мин */
	int32_t speed()
	{
		return spd;
	}	
};

#endif
