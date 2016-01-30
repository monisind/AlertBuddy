/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net_classify.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 21-Jan-2016 07:26:47
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
  double type;
  float b_MFCCs[1488];
  int ixstart;
  int ix;
  float result[248];
  float b_result[248];
  float k[2];
  float mtmp;
  boolean_T exitg1;

  /* MFCCs = extract_mfcc(int16(audioData), fs); */
  /* Ignore the first MFCC value */
  for (ixstart = 0; ixstart < 124; ixstart++) {
    for (ix = 0; ix < 12; ix++) {
      b_MFCCs[ix + 12 * ixstart] = MFCCs[(ix + 13 * ixstart) + 1];
    }
  }

  neural_net(b_MFCCs, result);
  for (ixstart = 0; ixstart < 2; ixstart++) {
    for (ix = 0; ix < 124; ix++) {
      b_result[ix + 124 * ixstart] = result[ixstart + (ix << 1)];
    }
  }

  sum(b_result, k);
  ixstart = 1;
  mtmp = k[0];
  if (rtIsNaNF(k[0])) {
    ix = 2;
    exitg1 = false;
    while ((!exitg1) && (ix < 3)) {
      ixstart = 2;
      if (!rtIsNaNF(k[1])) {
        mtmp = k[1];
        exitg1 = true;
      } else {
        ix = 3;
      }
    }
  }

  if ((ixstart < 2) && (k[1] > mtmp)) {
    mtmp = k[1];
  }

  if (mtmp == k[0]) {
    type = 1.0;
  } else {
    type = 2.0;
  }

  return type;
}

/*
 * File trailer for neural_net_classify.c
 *
 * [EOF]
 */
