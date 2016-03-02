/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: repmat.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 20-Feb-2016 01:28:22
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
  static const double a[2] = { 0.677397987268582, -0.080123027903081373 };

  for (jtilecol = 0; jtilecol < 124; jtilecol++) {
    ibtile = jtilecol << 1;
    for (k = 0; k < 2; k++) {
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
  static const double a[10] = { -1.6606429429994278, 1.366817725909895,
    0.74734595022469807, -0.33910377047560908, -0.31143131969537607,
    -0.0933100139830533, -0.37826479787160039, 0.041580410583523741,
    -0.9135315634773461, 1.5901785405097577 };

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
