#ifndef _GDEF_H
#define _GDEF_H

/*/////////////////////////////////////////////////
		Параметры конфигурации
/////////////////////////////////////////////////*/

/* Параметры регулятора токов */
#define KI_DQCUR 400
#define KP_DQCUR 1000

//#define KI_DQCUR 200
//#define KP_DQCUR 600

/* Параметры регулятора скорости */
#define KI_SPD 0
#define KP_SPD 9000
//#define KP_SPD 4000

/* Параметры регулятора положения */
#define KI_POS 0
#define KP_POS 10000
//#define KP_POS 6000

#define MAXQCURRENT 2000

/* ограничение для регулятора скорости об/мин */
#define MAXSPEED 10000

/* ограничение скважности для ШИМ [0-512] */
#define MAXPWM 512

#define TRAPEZ_PWM_CORR
#define PHASE_FORW_EN

/* включение отладки контура скорости */
//#define DEBUG_SPEED_REG

/* включение отладки регулятора тока */
//#define DEBUG_CURR_REG

/* включение отладки шим */
//#define DEBUG_PWM

/* включение отладки контура момента */
//#define DEBUG_TORQUE_LOOP

//#define	DEBUG_SIMPLE_CONTROL

#endif
