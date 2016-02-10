/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: repmat.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 10-Feb-2016 01:21:12
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
  static const double a[2] = { 0.87252832079529319, -0.43244236935151575 };

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
  static const double a[10] = { 1.6993724782791035, -1.5594405570577639,
    -0.029104334865207276, -0.83649470756775313, 0.5782569882164823,
    -0.42035315078538854, -1.7584096264616473, -3.3066365553641317,
    -1.3190566246894091, 1.7768811119498027 };

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
