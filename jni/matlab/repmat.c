/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: repmat.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 13-Mar-2016 23:47:35
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "repmat.h"

/* Function Definitions */

/*
 * Arguments    : double b[372]
 * Return Type  : void
 */
void b_repmat(double b[372])
{
  int jtilecol;
  int ibtile;
  int k;
  static const double a[3] = { -0.23384390939718427, -0.25361230828454945,
    -0.42715641638597396 };

  for (jtilecol = 0; jtilecol < 124; jtilecol++) {
    ibtile = jtilecol * 3;
    for (k = 0; k < 3; k++) {
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
  static const double a[10] = { -1.7579504430419952, 0.989825433521468,
    0.70572743282206873, 0.69874438695737873, 1.0253672517358017,
    -0.72682546936398129, -0.43159121500934167, 0.69272062431881554,
    -1.20462912761671, 1.6738346890545215 };

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
