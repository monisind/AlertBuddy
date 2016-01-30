/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: repmat.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 21-Jan-2016 07:26:47
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "repmat.h"

/* Function Definitions */

/*
 * Arguments    : double b[248]
 * Return Type  : void
 */
void b_repmat(double b[248])
{
  int jtilecol;
  int ibtile;
  int k;
  for (jtilecol = 0; jtilecol < 124; jtilecol++) {
    ibtile = jtilecol << 1;
    for (k = 0; k < 2; k++) {
      b[ibtile + k] = 0.85026694242598178 + -1.2604479334081822 * (double)k;
    }
  }
}

/*
 * Arguments    : double b[1240]
 * Return Type  : void
 */
void repmat(double b[1240])
{
  int jtilecol;
  int ibtile;
  static const double a[10] = { 1.6968721883962998, -1.5378802035945132,
    -0.29348023865528022, -0.97297232998478356, 0.56576992864084119,
    -0.577958968639903, -1.7885005260600033, -3.1661077118127063,
    -1.3103626971319142, 1.7714697776399095 };

  for (jtilecol = 0; jtilecol < 124; jtilecol++) {
    ibtile = jtilecol * 10;
    memcpy(&b[ibtile], &a[0], 10U * sizeof(double));
  }
}

/*
 * File trailer for repmat.c
 *
 * [EOF]
 */
