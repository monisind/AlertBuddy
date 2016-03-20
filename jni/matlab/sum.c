/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: sum.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 18-Mar-2016 18:15:07
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "sum.h"

/* Function Definitions */

/*
 * Arguments    : const float x[620]
 *                float y[5]
 * Return Type  : void
 */
void sum(const float x[620], float y[5])
{
  int ix;
  int iy;
  int i;
  int ixstart;
  float s;
  ix = -1;
  iy = -1;
  for (i = 0; i < 5; i++) {
    ixstart = ix + 1;
    ix++;
    s = x[ixstart];
    for (ixstart = 0; ixstart < 123; ixstart++) {
      ix++;
      s += x[ix];
    }

    iy++;
    y[iy] = s;
  }
}

/*
 * File trailer for sum.c
 *
 * [EOF]
 */
