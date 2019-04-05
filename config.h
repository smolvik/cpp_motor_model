#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <cmath>

/* Параметры двигателя */
const double PSIMAX=0.01121;
const double LDRV = 152e-6;
const double RDRV = 0.9;
const double NPOL = 4;
const double JDRV = 14e-6;

/* Параметры редуктора */
const double J_REDPRIV = 1.33e-6;
const double K_STRDV = 1.778e-6;
const double M_STR0 = 0.005;
const double K_VTRDV = 4.52e-5;
const double I_RED = 2e-3/2/M_PI;
const double ETHA_T = 0.86;
const double ETHA_TORM = 0.84;
const double DELTA_RED = 30e-6;
const double C_RED = 1.064e8;
const double B_RED = 150e3;
const double K_N = 295e3;
const double M_MEX = 9.887;
const double F_STRN = 480.0;
const double K_VTRN = 640.0;

const double X_MAX = 23;

#endif
