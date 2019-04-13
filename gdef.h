#ifndef _GDEF_H
#define _GDEF_H

/*/////////////////////////////////////////////////
		Параметры конфигурации
/////////////////////////////////////////////////*/

/* Номер канала [1-3] */
//#define CHAN_ID	1

/* Параметры регулятора токов */
#define KI_DQCUR 200
#define KP_DQCUR 600

/* Параметры регулятора скорости */
//#define KI_SPD 10
//#define KP_SPD 1500

#define KI_SPD 0
#define KP_SPD 4000

/* Параметры регулятора положения */
#define KI_POS 0
#define KP_POS 6000

#define MAXQCURRENT 2000

/* ограничение для регулятора скорости об/мин */
#define MAXSPEED 6000

/* ограничение скважности для ШИМ [0-512] */
#define MAXPWM 500

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
