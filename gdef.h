#ifndef _GDEF_H
#define _GDEF_H

struct pi_reg_state{
	int32_t ki;
	int32_t kp;
	int32_t a;
	int32_t y;	
};

/*/////////////////////////////////////////////////
		Параметры конфигурации
/////////////////////////////////////////////////*/

/* Номер канала [1-3] */
//#define CHAN_ID	1

/* Параметры регулятора токов 30A */
//#define KI_DQCUR 100
//#define KP_DQCUR 1500

/* Параметры регулятора токов 70A */
#define KI_DQCUR 50
#define KP_DQCUR 1500

/* Параметры регулятора скорости */
//#define KI_SPD 10
//#define KP_SPD 1500

#define KI_SPD 0
#define KP_SPD 5000
//#define KP_SPD 6500

/* Параметры регулятора положения (режим позиционирования в ноль) */
#define KI_LIN 20
#define KP_LIN 220000

/* Параметры регулятора положения (рабочий режим) */
#define KI_POS 0
#define KP_POS 4500

/* MAXQCURR_START - режим позиционирования в ноль
 * MAXQCURR_WORK - рабочий режим
 */
#define MAXQCURR_START 660
//#define MAXQCURR_WORK 2500
#define MAXQCURR_WORK 660

/* ограничение для регулятора скорости об/мин */
#define MAXSPEED 4000

/* ограничение скважности для ШИМ [0-512] */
#define MAXPWM 500

/* скважность для муфты [-512-512] */
//#define BRAKEPWM (-330)
//#define BRAKEPWM (-82)

/* температурная коррекция для муфты */
#define BRK_T0		(1802)
#define BRK_K		(-341)
#define BRK_U0		(-251658)

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

/* включение фильтрации входного сигнала положения */
//#define PREFILTER_ENABLE

/* разрешение позиционирования в начальное положение при включении  */
#define ZERO_POS_ENABLE

#endif
