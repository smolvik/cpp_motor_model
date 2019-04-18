#ifndef _GDEF_H
#define _GDEF_H

/** @file 
 * Параметры векторного контроллера */

/**@param Интегральная составляющая регулятора токов */
#define KI_DQCUR 400
//#define KI_DQCUR 200
/**@param Пропорциональная составляющая регулятора токов */
#define KP_DQCUR 1000
//#define KP_DQCUR 600

/**@param Интегральная составляющая регулятора скорости */
#define KI_SPD 0
/**@param Пропорциональная составляющая регулятора скорости */
#define KP_SPD 9000
//#define KP_SPD 4000

/**@param Интегральная составляющая регулятора положения */
#define KI_POS 50
/**@param Пропорциональная составляющая регулятора положения */
#define KP_POS 13000
//#define KP_POS 6000

/**@param ограничение для регулятора тока */
#define MAXQCURRENT 2000

/**@param ограничение для регулятора скорости об/мин */
#define MAXSPEED 10000

/**@param ограничение скважности для ШИМ [0-512] */
#define MAXPWM 512

#endif
