#ifndef __ENCODER_H__
#define __ENCODER_H__

class Encoder{
	
public:
	uint32_t operator ()(double position)
	{
		double rem = fmod(position*360.0/(2*M_PI), 360.0);
		if(rem<0) rem += 360;
		return (uint32_t)4095*(rem/360);
	}	
};

#endif
