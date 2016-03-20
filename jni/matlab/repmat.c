/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: repmat.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 18-Mar-2016 18:15:07
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "repmat.h"

/* Function Definitions */

/*
 * Arguments    : double b[620]
 * Return Type  : void
 */
void b_repmat(double b[620])
{
  int jtilecol;
  int ibtile;
  int k;
  static const double a[5] = { 0.08676610104910816, -0.317661514906772,
    -0.39057331517752408, 0.075859163569468313, -0.66581822940243851 };

  for (jtilecol = 0; jtilecol < 124; jtilecol++) {
    ibtile = jtilecol * 5;
    for (k = 0; k < 5; k++) {
      b[ibtile + k] = a[k];
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
  static const double a[10] = { 1.3842496921056755, 0.4777578188887287,
    0.2639651459664763, 0.35020783100722241, 0.25792600214461525,
    0.75358030886139882, -0.98583490285861541, -0.896673473333501,
    -1.6455855815504972, 0.44534481876932897 };

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
