/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: repmat.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 01-Mar-2016 23:54:39
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
  static const double a[3] = { 0.0062086285430117683, 0.616227257079201,
    -0.47945560382638786 };

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
  static const double a[10] = { 1.5717055083484806, -0.60994418067648193,
    -0.23935031870344742, 0.259870738679722, -0.20174450977834582,
    -0.36538336605058053, 0.75226175113390292, -1.4075691545321862,
    -1.2215713653855522, 1.5856356671327707 };

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
