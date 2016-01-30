/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 21-Jan-2016 07:26:47
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "neural_net.h"
#include "repmat.h"

/* Function Declarations */
static void mapminmax_apply(const float x[1488], const double settings_gain[12],
  const double settings_xoffset[12], float y[1488]);
static void mapminmax_reverse(const float y[248], float x[248]);
static void softmax_apply(float n[248], float a[248]);
static void tansig_apply(const float n[1240], float a[1240]);

/* Function Definitions */

/*
 * Arguments    : const float x[1488]
 *                const double settings_gain[12]
 *                const double settings_xoffset[12]
 *                float y[1488]
 * Return Type  : void
 */
static void mapminmax_apply(const float x[1488], const double settings_gain[12],
  const double settings_xoffset[12], float y[1488])
{
  int ak;
  int ck;
  float cv[12];
  int k;
  float a[1488];

  /*  ===== MODULE FUNCTIONS ======== */
  /*  Map Minimum and Maximum Input Processing Function */
  ak = 0;
  for (ck = 0; ck < 1478; ck += 12) {
    for (k = 0; k < 12; k++) {
      cv[k] = x[ak + k] - (float)settings_xoffset[k];
    }

    for (k = 0; k < 12; k++) {
      y[ck + k] = cv[k];
    }

    ak += 12;
  }

  memcpy(&a[0], &y[0], 1488U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 1478; ck += 12) {
    for (k = 0; k < 12; k++) {
      cv[k] = a[ak + k] * (float)settings_gain[k];
    }

    for (k = 0; k < 12; k++) {
      y[ck + k] = cv[k];
    }

    ak += 12;
  }

  memcpy(&a[0], &y[0], 1488U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 1478; ck += 12) {
    for (k = 0; k < 12; k++) {
      cv[k] = a[ak + k] + -1.0F;
    }

    for (k = 0; k < 12; k++) {
      y[ck + k] = cv[k];
    }

    ak += 12;
  }
}

/*
 * Arguments    : const float y[248]
 *                float x[248]
 * Return Type  : void
 */
static void mapminmax_reverse(const float y[248], float x[248])
{
  int ak;
  int ck;
  float cv[2];
  int k;
  float a[248];

  /*  Map Minimum and Maximum Output Reverse-Processing Function */
  ak = 0;
  for (ck = 0; ck < 248; ck += 2) {
    for (k = 0; k < 2; k++) {
      cv[k] = y[ak + k] - -1.0F;
    }

    for (k = 0; k < 2; k++) {
      x[ck + k] = cv[k];
    }

    ak += 2;
  }

  memcpy(&a[0], &x[0], 248U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 248; ck += 2) {
    for (k = 0; k < 2; k++) {
      cv[k] = a[ak + k] / 2.0F;
    }

    for (k = 0; k < 2; k++) {
      x[ck + k] = cv[k];
    }

    ak += 2;
  }

  memcpy(&a[0], &x[0], 248U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 248; ck += 2) {
    for (k = 0; k < 2; k++) {
      cv[k] = a[ak + k];
    }

    for (k = 0; k < 2; k++) {
      x[ck + k] = cv[k];
    }

    ak += 2;
  }
}

/*
 * Arguments    : float n[248]
 *                float a[248]
 * Return Type  : void
 */
static void softmax_apply(float n[248], float a[248])
{
  float nmax[124];
  int ix;
  int iy;
  int i;
  int bk;
  float mtmp;
  int ak;
  boolean_T exitg1;
  float cv[2];
  float numer[248];
  float b_nmax;

  /*  Competitive Soft Transfer Function */
  ix = 0;
  iy = -1;
  for (i = 0; i < 124; i++) {
    ix += 2;
    bk = ix - 1;
    mtmp = n[ix - 2];
    if (rtIsNaNF(n[ix - 2])) {
      ak = ix;
      exitg1 = false;
      while ((!exitg1) && (ak <= ix)) {
        bk = ak;
        if (!rtIsNaNF(n[ak - 1])) {
          mtmp = n[ak - 1];
          exitg1 = true;
        } else {
          ak++;
        }
      }
    }

    if (bk < ix) {
      while (bk + 1 <= ix) {
        if (n[bk] > mtmp) {
          mtmp = n[bk];
        }

        bk++;
      }
    }

    iy++;
    nmax[iy] = mtmp;
  }

  memcpy(&a[0], &n[0], 248U * sizeof(float));
  ak = 0;
  bk = 0;
  for (ix = 0; ix < 248; ix += 2) {
    for (iy = 0; iy < 2; iy++) {
      cv[iy] = a[ak + iy] - nmax[bk];
    }

    for (iy = 0; iy < 2; iy++) {
      n[ix + iy] = cv[iy];
    }

    ak += 2;
    bk++;
  }

  for (ak = 0; ak < 248; ak++) {
    numer[ak] = (real32_T)exp(n[ak]);
  }

  ix = 1;
  iy = -1;
  for (i = 0; i < 124; i++) {
    ak = ix;
    ix += 2;
    iy++;
    nmax[iy] = numer[ak - 1] + numer[ak];
  }

  for (i = 0; i < 124; i++) {
    b_nmax = nmax[i];
    if (nmax[i] == 0.0F) {
      b_nmax = 1.0F;
    }

    nmax[i] = b_nmax;
  }

  ak = 0;
  bk = 0;
  for (ix = 0; ix < 248; ix += 2) {
    for (iy = 0; iy < 2; iy++) {
      cv[iy] = numer[ak + iy] / nmax[bk];
    }

    for (iy = 0; iy < 2; iy++) {
      a[ix + iy] = cv[iy];
    }

    ak += 2;
    bk++;
  }
}

/*
 * Arguments    : const float n[1240]
 *                float a[1240]
 * Return Type  : void
 */
static void tansig_apply(const float n[1240], float a[1240])
{
  int i3;

  /*  Sigmoid Symmetric Transfer Function */
  for (i3 = 0; i3 < 1240; i3++) {
    a[i3] = 2.0F / (1.0F + (real32_T)exp(-2.0F * n[i3])) - 1.0F;
  }
}

/*
 * NEURAL_NET neural network simulation function.
 *
 *  Generated by Neural Network Toolbox function genFunction, 21-Jan-2016 06:49:40.
 *
 *  [y1] = neural_net(x1) takes these arguments:
 *    x = 12xQ matrix, input #1
 *  and returns:
 *    y = 2xQ matrix, output #1
 *  where Q is the number of samples.
 * Arguments    : const float x1[1488]
 *                float b_y1[248]
 * Return Type  : void
 */
void neural_net(const float x1[1488], float b_y1[248])
{
  float xp1[1488];
  static const double dv0[12] = { 21.2111263275146, -42.8025703430176,
    -6.57227420806885, 4.53922557830811, -44.7316207885742, -2.46110773086548,
    -2.23018765449524, -40.6490058898926, 1.08411467075348, -8.78223896026611,
    -29.0861682891846, 1.02367472648621 };

  static const double dv1[12] = { 0.180726365643349, 0.0827783663900457,
    0.078953260309308, 0.0755625098612377, 0.0563504231752465,
    0.0478333381934899, 0.0571842769987757, 0.053745634556866, 0.055305575689199,
    0.0571722984114423, 0.0716452503918659, 0.064378255157245 };

  double dv2[248];
  double dv3[1240];
  float fv0[1240];
  int i0;
  int i1;
  float f0;
  int i2;
  static const float fv1[120] = { -0.111021213F, 0.351592928F, -1.03312111F,
    -1.42289793F, 2.42040944F, 2.64441466F, -0.962474227F, -2.55125833F,
    -0.639099538F, 0.391904414F, 0.0738699F, -0.378460169F, -1.62177563F,
    0.636001825F, -3.74884033F, 0.695934117F, 1.73193908F, 1.80349696F,
    -0.329469413F, 0.782757223F, -0.378341943F, 1.09949732F, -0.744687796F,
    -0.381215751F, -3.16749573F, 2.97005606F, -0.230892569F, 0.325515479F,
    0.229375243F, -0.837802231F, 0.430357575F, -0.420946062F, 1.0830332F,
    -0.421850353F, -0.199849501F, 2.09592247F, -1.91031682F, 0.152014107F,
    0.205876142F, 0.713088214F, -0.881339788F, 0.149274141F, 0.633878112F,
    1.15224218F, 1.06767714F, -0.28742367F, -1.2306906F, 1.06300282F,
    0.661102593F, -0.33205694F, 0.462622315F, 0.336516708F, 0.759072304F,
    -0.503683269F, 0.413545F, -0.291831732F, -0.793779314F, -0.411691278F,
    -0.639915407F, -0.492901981F, 0.343803674F, 0.417473555F, 1.89108634F,
    -0.436316639F, 2.33247781F, 1.74629712F, -0.398449123F, -1.63586414F,
    0.292765915F, 0.494064242F, 0.524524212F, 0.134841338F, 1.27811885F,
    -0.736721277F, 0.319268465F, -0.363583624F, 0.372041017F, 0.689448118F,
    0.457749248F, -0.378218681F, -0.0957961082F, -0.22439371F, -0.797130227F,
    -0.201822F, 1.21591473F, -0.788171589F, 0.961189806F, -0.478256822F,
    0.0996801257F, -0.317924529F, -0.810083389F, 0.173050165F, -1.41140425F,
    1.08003271F, 2.17183971F, 0.550035596F, 0.740171731F, 0.552487791F,
    0.579668641F, -0.171350449F, 0.675021827F, 0.755992413F, -0.618094623F,
    1.17066109F, 1.20498323F, 1.43473697F, 0.524795413F, 0.110123128F,
    -0.802000046F, -0.155156761F, -0.123716921F, -0.225690201F, -0.293204546F,
    -0.277464241F, -0.208557978F, -0.454307586F, 0.539626241F, -1.07890666F,
    0.353061587F, -0.0504630953F };

  float fv2[1240];
  float fv3[248];
  static const float fv4[20] = { 0.904034436F, 0.501971066F, -0.893109202F,
    1.48865211F, 2.59431291F, -1.21369922F, 0.746158123F, -2.01608944F,
    4.66697216F, -4.79392767F, -3.45944047F, 2.85210395F, 1.66565096F,
    -1.46530032F, 2.11429381F, -3.44093442F, -0.659549713F, -0.066114977F,
    1.09721065F, -0.912652373F };

  float fv5[248];

  /*  ===== NEURAL NETWORK CONSTANTS ===== */
  /*  Input 1 */
  /*  Layer 1 */
  /*  Layer 2 */
  /*  Output 1 */
  /*  ===== SIMULATION ======== */
  /*  Dimensions */
  /*  samples */
  /*  Input 1 */
  mapminmax_apply(x1, dv1, dv0, xp1);

  /*  Layer 1 */
  /*  Layer 2 */
  b_repmat(dv2);
  repmat(dv3);
  for (i0 = 0; i0 < 10; i0++) {
    for (i1 = 0; i1 < 124; i1++) {
      f0 = 0.0F;
      for (i2 = 0; i2 < 12; i2++) {
        f0 += fv1[i0 + 10 * i2] * xp1[i2 + 12 * i1];
      }

      fv0[i0 + 10 * i1] = (float)dv3[i0 + 10 * i1] + f0;
    }
  }

  tansig_apply(fv0, fv2);
  for (i0 = 0; i0 < 2; i0++) {
    for (i1 = 0; i1 < 124; i1++) {
      f0 = 0.0F;
      for (i2 = 0; i2 < 10; i2++) {
        f0 += fv4[i0 + (i2 << 1)] * fv2[i2 + 10 * i1];
      }

      fv3[i0 + (i1 << 1)] = (float)dv2[i0 + (i1 << 1)] + f0;
    }
  }

  /*  Output 1 */
  softmax_apply(fv3, fv5);
  mapminmax_reverse(fv5, b_y1);
}

/*
 * File trailer for neural_net.c
 *
 * [EOF]
 */
