/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 20-Feb-2016 01:28:22
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
 *  Generated by Neural Network Toolbox function genFunction, 20-Feb-2016 01:25:55.
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
  static const double dv0[12] = { 14.111271, -39.717815, -16.657677, -2.814637,
    -36.676994, -6.7565117, -7.642342, -34.000916, -4.9420257, -11.446178,
    -22.226938, -4.165919 };

  static const double dv1[12] = { 0.091468884640769, 0.05653599588626,
    0.0671334049512497, 0.0846798200960286, 0.0558719745329282,
    0.0616758987829054, 0.0631358271038002, 0.0629430640960552,
    0.0766982163373436, 0.0757176920369142, 0.0754132400461499,
    0.080547189275625 };

  double dv2[248];
  double dv3[1240];
  float fv0[1240];
  int i0;
  int i1;
  float f0;
  int i2;
  static const float fv1[120] = { 0.713202894F, -0.362154841F, -0.581430793F,
    -0.216478258F, 0.286884874F, 0.768223345F, -0.499602556F, -0.539050877F,
    -0.957081378F, 0.287090629F, -0.565853655F, 0.266809F, -0.578647852F,
    -0.484611034F, 0.726410687F, -1.24342692F, 0.40962863F, 0.642041266F,
    0.859779954F, -0.712403893F, 0.2704826F, -0.38566035F, 0.961770058F,
    1.02601898F, 1.04608858F, -1.97489715F, 0.917895555F, 1.60375881F,
    -0.0968340859F, 0.487594634F, 0.540444911F, -0.193772301F, -0.0786820576F,
    0.0597725175F, -0.0234222524F, -0.849808216F, 0.391757101F, 0.412250936F,
    -0.227604985F, 0.378362238F, 0.555342F, 0.933567584F, 0.274220228F,
    0.257665366F, 0.272812545F, 0.070877783F, -0.709205568F, 0.475355625F,
    0.694785953F, 0.460190207F, 0.0543987378F, 0.764302611F, -0.720768034F,
    -0.892860234F, -0.287210673F, -0.0244856048F, 0.650601625F, 0.16604811F,
    -0.540184736F, -0.137822807F, -0.430493116F, -0.53710407F, -0.496334434F,
    0.290359885F, 0.387836F, 0.974101305F, -0.253433257F, -0.207358673F,
    -0.50688982F, -0.730764031F, -0.0221340843F, -0.531541407F, 0.330196649F,
    0.102373004F, -0.215923443F, 0.68673265F, 0.634165704F, 0.156132311F,
    -0.232185289F, 0.59114486F, 0.756264448F, 0.111451991F, 0.785712659F,
    -0.514131069F, -0.802895844F, 0.469011396F, 0.147054598F, -1.17701292F,
    -0.32780233F, -0.109452657F, -0.0911378115F, -0.394967318F, -0.0500132702F,
    -0.426770687F, 0.388392419F, 0.48159191F, -0.125701442F, 0.218446076F,
    0.144604683F, 0.47814244F, 0.752865374F, -0.121387862F, 0.539377332F,
    0.498943716F, 0.485757828F, -0.343366653F, -0.39683494F, 0.467565417F,
    -0.9276613F, -0.78406322F, 0.277833194F, 0.242055431F, -0.133986235F,
    -0.785119176F, -0.599404037F, 0.535097837F, -0.445583016F, -0.219496384F,
    -0.246990517F, -0.109094709F };

  float fv2[1240];
  float fv3[248];
  static const float fv4[20] = { 0.275915205F, 0.248670712F, 0.688507557F,
    0.548051596F, 0.0433741957F, 0.971025646F, -0.261375308F, 1.08464587F,
    -0.547310412F, 0.735978127F, 2.26683283F, -2.11415124F, 0.245297164F,
    1.25323176F, -1.23922074F, 1.43146336F, -0.681668282F, 0.858599663F,
    -0.179385841F, 0.379479527F };

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
