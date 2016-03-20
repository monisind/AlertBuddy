/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 18-Mar-2016 18:15:07
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
 *  Generated by Neural Network Toolbox function genFunction, 18-Mar-2016 18:06:53.
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
  static const double dv0[12] = { 16.132376, -41.92286, -11.763175, 0.26042256,
    -48.499973, -10.59572, -11.150261, -50.75682, -17.053158, -20.999031,
    -31.260366, -16.086512 };

  static const double dv1[12] = { 0.0872760507447396, 0.0583581858719483,
    0.0496593431136582, 0.0621090054882946, 0.0398973259870547,
    0.0429406458505015, 0.0418338504206415, 0.0292683416538594,
    0.0406196264042357, 0.0324026540430679, 0.0488586616635397,
    0.0444680174345756 };

  double dv2[620];
  double dv3[1240];
  float fv0[1240];
  int i0;
  int i1;
  float f0;
  int i2;
  static const float fv1[120] = { -0.482139856F, 0.268292874F, -0.398598433F,
    -0.492702723F, -5.28534F, -1.3607614F, -1.31324863F, -2.90275884F,
    0.57084167F, 0.643586755F, 0.54507637F, 0.229064658F, 0.454308599F,
    -1.42380536F, -1.41992974F, 3.64180279F, 0.14793165F, 1.11103225F,
    -0.886625707F, -1.1550293F, 0.21188508F, 2.01851439F, -0.85992837F,
    -1.96682489F, -0.0088577373F, 3.95469904F, 0.961833417F, 2.36891127F,
    -0.709682882F, -2.43880248F, 0.912824154F, 1.19552827F, -0.754117072F,
    -0.0282568727F, -1.58362544F, -0.450968713F, -1.03895199F, 0.997594237F,
    -0.53181994F, -0.204281747F, -0.679035425F, -0.652160525F, -1.03224313F,
    0.574654937F, -0.419399559F, -1.09406209F, -1.95429432F, -1.64093769F,
    0.616113186F, 1.53872609F, 0.877005696F, -0.772606075F, 0.965004742F,
    -0.708744109F, -0.699137F, 0.178787217F, 0.661658704F, 1.50458348F,
    -1.11914206F, 2.82313919F, 1.04887187F, 1.11462772F, 3.57028317F,
    1.90066338F, 1.40841186F, 0.145877808F, -0.734749F, 1.91204357F,
    -0.942125857F, 0.125358075F, 0.803959668F, -2.41720366F, -0.863904297F,
    0.142200008F, -0.480167896F, -3.39202547F, -2.81302428F, -2.2363379F,
    0.470471919F, -1.23218513F, -0.235131681F, -3.10341072F, -2.91667509F,
    -0.866841495F, 0.148358867F, -0.961966038F, -0.191483766F, -2.43185115F,
    0.39682892F, -1.77321279F, -0.733736455F, 2.46021F, 1.28283262F, 1.75353122F,
    0.896416F, 2.57278728F, 1.11794055F, 3.0823822F, -0.0531957038F,
    -1.96753991F, 0.432154804F, -0.0155567462F, -2.25758934F, 1.58146751F,
    -0.0868694782F, -2.00317669F, 0.564771533F, -2.29165697F, 0.0372338742F,
    1.49811876F, -0.943119347F, -1.67992508F, 0.932955503F, -2.28386927F,
    2.41840959F, 0.486109674F, 2.88024926F, -0.583868921F, 1.52614343F,
    1.9853704F };

  float fv2[1240];
  float fv3[620];
  static const float fv4[50] = { 1.92884886F, 0.0791850165F, 0.606354833F,
    0.8203246F, -0.852406919F, -2.10267973F, -0.585249543F, -1.96586227F,
    5.016222F, -0.959461629F, -3.65689325F, 0.844967902F, 4.3082037F,
    1.28906775F, -3.90302444F, 0.817251503F, -3.68290186F, -0.165898442F,
    3.55818534F, -1.68369067F, -5.09043407F, 0.936949968F, 2.58750296F,
    2.75450921F, 0.50600791F, 5.61685228F, 4.00498772F, -4.83895683F,
    1.23465705F, -3.98150063F, -4.74256086F, 4.84532976F, -3.24344039F,
    0.342350513F, 4.4398303F, -4.56724262F, 3.84799695F, -1.66734827F,
    2.45293832F, -0.163840726F, -1.40159512F, 0.639144599F, 1.05117142F,
    -0.097580716F, 1.9411335F, 3.98067117F, -2.86589026F, -3.9957943F,
    -1.48145068F, 2.93353248F };

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
