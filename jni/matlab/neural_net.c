/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 05-Mar-2016 19:32:56
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "neural_net.h"
#include "repmat.h"

/* Function Declarations */
static void mapminmax_apply(const float x[1488], const double settings_gain[12],
  const double settings_xoffset[12], float y[1488]);
static void mapminmax_reverse(const float y[620], float x[620]);
static void softmax_apply(float n[620], float a[620]);
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
 * Arguments    : const float y[620]
 *                float x[620]
 * Return Type  : void
 */
static void mapminmax_reverse(const float y[620], float x[620])
{
  int ak;
  int ck;
  float cv[5];
  int k;
  float a[620];

  /*  Map Minimum and Maximum Output Reverse-Processing Function */
  ak = 0;
  for (ck = 0; ck < 617; ck += 5) {
    for (k = 0; k < 5; k++) {
      cv[k] = y[ak + k] - -1.0F;
    }

    for (k = 0; k < 5; k++) {
      x[ck + k] = cv[k];
    }

    ak += 5;
  }

  memcpy(&a[0], &x[0], 620U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 617; ck += 5) {
    for (k = 0; k < 5; k++) {
      cv[k] = a[ak + k] / 2.0F;
    }

    for (k = 0; k < 5; k++) {
      x[ck + k] = cv[k];
    }

    ak += 5;
  }

  memcpy(&a[0], &x[0], 620U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 617; ck += 5) {
    for (k = 0; k < 5; k++) {
      cv[k] = a[ak + k];
    }

    for (k = 0; k < 5; k++) {
      x[ck + k] = cv[k];
    }

    ak += 5;
  }
}

/*
 * Arguments    : float n[620]
 *                float a[620]
 * Return Type  : void
 */
static void softmax_apply(float n[620], float a[620])
{
  float nmax[124];
  int ix;
  int iy;
  int i;
  int bk;
  float mtmp;
  int ak;
  boolean_T exitg1;
  float cv[5];
  int k;
  float numer[620];
  float s;
  float b_nmax;

  /*  Competitive Soft Transfer Function */
  ix = 0;
  iy = -1;
  for (i = 0; i < 124; i++) {
    ix += 5;
    bk = ix - 4;
    mtmp = n[ix - 5];
    if (rtIsNaNF(n[ix - 5])) {
      ak = ix - 3;
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

  memcpy(&a[0], &n[0], 620U * sizeof(float));
  ak = 0;
  bk = 0;
  for (ix = 0; ix < 617; ix += 5) {
    for (k = 0; k < 5; k++) {
      cv[k] = a[ak + k] - nmax[bk];
    }

    for (k = 0; k < 5; k++) {
      n[ix + k] = cv[k];
    }

    ak += 5;
    bk++;
  }

  for (ak = 0; ak < 620; ak++) {
    numer[ak] = (real32_T)exp(n[ak]);
  }

  ix = 0;
  iy = -1;
  for (i = 0; i < 124; i++) {
    ak = ix;
    ix++;
    s = numer[ak];
    for (k = 0; k < 4; k++) {
      ix++;
      s += numer[ix - 1];
    }

    iy++;
    nmax[iy] = s;
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
  for (ix = 0; ix < 617; ix += 5) {
    for (k = 0; k < 5; k++) {
      cv[k] = numer[ak + k] / nmax[bk];
    }

    for (k = 0; k < 5; k++) {
      a[ix + k] = cv[k];
    }

    ak += 5;
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
 *  Generated by Neural Network Toolbox function genFunction, 05-Mar-2016 19:32:19.
 *
 *  [y1] = neural_net(x1) takes these arguments:
 *    x = 12xQ matrix, input #1
 *  and returns:
 *    y = 5xQ matrix, output #1
 *  where Q is the number of samples.
 * Arguments    : const float x1[1488]
 *                float b_y1[620]
 * Return Type  : void
 */
void neural_net(const float x1[1488], float b_y1[620])
{
  float xp1[1488];
  static const double dv0[12] = { 14.111271, -42.97188, -16.657677, -2.814637,
    -36.676994, -9.870411, -7.642342, -35.384453, -10.23617, -11.976542,
    -27.353817, -7.3301578 };

  static const double dv1[12] = { 0.091468884640769, 0.0517735583811872,
    0.0541434771282116, 0.0552589470175558, 0.0558719745329282,
    0.0562722815374274, 0.0631358271038002, 0.0494858648748772,
    0.0595506392135838, 0.071275542838098, 0.0631963027887802,
    0.0714428503028166 };

  double dv2[620];
  double dv3[1240];
  float fv0[1240];
  int i0;
  int i1;
  float f0;
  int i2;
  static const float fv1[120] = { 0.666778922F, -0.980718374F, 0.184729397F,
    1.34195733F, 0.699296F, -0.418313831F, 0.22999759F, -0.608896375F,
    -0.370886743F, 2.70673442F, -0.177178204F, 1.09860659F, 1.69606066F,
    2.53745174F, 0.910957098F, -0.976814747F, 2.46346688F, 2.81364107F,
    -1.17538202F, -0.737531602F, -0.576749623F, 2.28943634F, 0.464389056F,
    -0.823904455F, -1.0956558F, 1.10062087F, -0.978261352F, 2.96629715F,
    0.604097724F, -3.14696097F, 1.68788421F, -1.87901568F, 0.853260398F,
    -1.68916154F, -1.42242503F, 1.62980568F, -2.85555434F, 0.387279332F,
    1.65428138F, 1.91124654F, 0.0180834364F, -0.022400815F, 0.160549507F,
    1.61837316F, 1.40326893F, -0.168698698F, -0.500568211F, -1.04526663F,
    1.50169587F, 1.48088241F, -0.259083599F, 1.8400979F, -1.7104162F,
    1.20157957F, -1.80021107F, -1.95415807F, -0.486198902F, 0.521026731F,
    -0.329529494F, -1.88614559F, -0.0454440676F, -0.712228775F, 0.551333547F,
    1.52022F, 0.00191100396F, -2.07701468F, -0.663344383F, -1.75013137F,
    0.654039502F, -0.664242089F, 0.936776578F, -1.60442698F, 1.19335508F,
    0.157180414F, 1.39097536F, -2.12733459F, -0.173176706F, -1.79214048F,
    0.567551911F, 0.21541734F, -0.422236294F, 1.38767934F, -3.05270624F,
    0.417826951F, 1.17571676F, -1.35075808F, -0.0618141778F, -1.43035138F,
    0.372418761F, -0.516048253F, -1.2955091F, 2.11038399F, -2.3002913F,
    1.06761038F, 1.1062187F, -0.651717067F, -0.384775341F, 0.433648974F,
    0.352144778F, -0.708835363F, 0.907008171F, -1.15783334F, 1.20916688F,
    -0.891478419F, -0.180258632F, 0.569383F, -0.222125113F, 1.27400839F,
    -1.13737619F, -0.167179078F, -0.0026821543F, -1.53719187F, 0.398619F,
    1.06828201F, -1.48788726F, -2.15402222F, 0.26432544F, 0.297755778F,
    0.464246213F, 0.0383403413F };

  float fv2[1240];
  float fv3[620];
  static const float fv4[50] = { 1.22975338F, 1.08313131F, -1.47278368F,
    1.10758376F, 0.63462168F, 2.50977087F, -4.09911442F, 0.96659112F,
    2.56163907F, -2.53591776F, -1.81185019F, 1.24156082F, -3.08782482F,
    2.84464955F, -0.304214239F, -2.46588826F, -2.97071934F, 0.120386161F,
    2.66901088F, 1.49015653F, -0.441408187F, -0.601479769F, -0.334828943F,
    2.89584875F, 0.176404551F, 2.45381355F, 2.95522571F, -1.16863036F,
    0.5661695F, -2.77053881F, -3.59130955F, 0.274709731F, 1.31427181F,
    0.88601923F, 2.75781798F, -2.20349479F, 0.306654364F, 0.635394812F,
    3.69286394F, -2.52891493F, 1.81788886F, -0.0403945297F, -1.09275675F,
    1.0687238F, 0.378812373F, -1.2073698F, 1.57390416F, -4.26139212F,
    -1.61218679F, 4.0781126F };

  float fv5[620];

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
  for (i0 = 0; i0 < 5; i0++) {
    for (i1 = 0; i1 < 124; i1++) {
      f0 = 0.0F;
      for (i2 = 0; i2 < 10; i2++) {
        f0 += fv4[i0 + 5 * i2] * fv2[i2 + 10 * i1];
      }

      fv3[i0 + 5 * i1] = (float)dv2[i0 + 5 * i1] + f0;
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
