/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net_classify.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 18-Mar-2016 18:15:07
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "sum.h"
#include "neural_net.h"

/* Function Definitions */

/*
 * NEURAL_NET_CLASSIFY Classifies an audio data using the neural net
 *        audioData: The audio data
 *        fs: The sample rate
 * Arguments    : const float MFCCs[1612]
 * Return Type  : double
 */
double neural_net_classify(const float MFCCs[1612])
{
  float b_MFCCs[1488];
  int ixstart;
  int itmp;
  float result[620];
  float b_result[620];
  float k[5];
  float mtmp;
  int ix;
  boolean_T exitg1;

  /* MFCCs = extract_mfcc(int16(audioData), fs); */
  /* Ignore the first MFCC value */
  for (ixstart = 0; ixstart < 124; ixstart++) {
    for (itmp = 0; itmp < 12; itmp++) {
      b_MFCCs[itmp + 12 * ixstart] = MFCCs[(itmp + 13 * ixstart) + 1];
    }
  }

  neural_net(b_MFCCs, result);
  for (ixstart = 0; ixstart < 5; ixstart++) {
    for (itmp = 0; itmp < 124; itmp++) {
      b_result[itmp + 124 * ixstart] = result[ixstart + 5 * itmp];
    }
  }

  sum(b_result, k);
  ixstart = 1;
  mtmp = k[0];
  itmp = 1;
  if (rtIsNaNF(k[0])) {
    ix = 2;
    exitg1 = false;
    while ((!exitg1) && (ix < 6)) {
      ixstart = ix;
      if (!rtIsNaNF(k[ix - 1])) {
        mtmp = k[ix - 1];
        itmp = ix;
        exitg1 = true;
      } else {
        ix++;
      }
    }
  }

  if (ixstart < 5) {
    while (ixstart + 1 < 6) {
      if (k[ixstart] > mtmp) {
        mtmp = k[ixstart];
        itmp = ixstart + 1;
      }

      ixstart++;
    }
  }

  return itmp;
}

/*
 * File trailer for neural_net_classify.c
 *
 * [EOF]
 */
