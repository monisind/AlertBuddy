/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: repmat.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 05-Mar-2016 19:32:56
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
  static const double a[5] = { -0.33556913576082842, -0.42759181966372672,
    -0.68505148118922221, -0.68944135564891251, 0.92622599739482236 };

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
  static const double a[10] = { 1.169339354075815, -0.2508937812000846,
    -0.25575833578900353, -0.29881274057374385, -0.66315863474369574,
    -1.0046802645061315, 0.66905495894511169, -1.212623176813642,
    -0.076237432475232514, 1.9287549778918 };

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
