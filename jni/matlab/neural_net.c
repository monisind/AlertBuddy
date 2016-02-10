/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 10-Feb-2016 01:21:12
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
 *  Generated by Neural Network Toolbox function genFunction, 10-Feb-2016 00:56:09.
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
  static const double dv0[12] = { 33.1217651367188, -57.9367294311523,
    -9.22938442230225, 4.13434839248657, -52.0801773071289, -3.87744426727295,
    -3.57925415039063, -45.8114280700684, 0.0920843333005905, -10.6806783676147,
    -32.8212738037109, 0.0258490778505802 };

  static const double dv1[12] = { 0.110280511832762, 0.0625838755632663,
    0.0647774219756596, 0.0646855916656327, 0.0494813248906741,
    0.042697283238683, 0.0516073858818726, 0.0488623111989787,
    0.0505190503432872, 0.0523630699473718, 0.0656748144952099,
    0.0589628719581747 };

  double dv2[248];
  double dv3[1240];
  float fv0[1240];
  int i0;
  int i1;
  float f0;
  int i2;
  static const float fv1[120] = { -0.11019358F, 0.353617907F, -1.03034091F,
    -1.52316892F, 2.62231445F, 2.48986387F, -0.92879796F, -2.68389821F,
    -0.629687F, 0.375163645F, 0.0726878718F, -0.381505F, -1.5665381F, 0.642327F,
    -3.59890747F, 0.792559505F, 1.77906024F, 1.76988757F, -0.327397287F,
    0.786748707F, -0.379304647F, 1.10773551F, -0.803115964F, -0.395182967F,
    -3.17808795F, 2.9580822F, -0.235601097F, 0.320790231F, 0.23124291F,
    -0.840326965F, 0.431924671F, -0.425277442F, 1.17354381F, -0.465680122F,
    -0.210323244F, 2.17285F, -2.04650807F, 0.0384130627F, 0.205124334F,
    0.713097334F, -0.877718866F, 0.129441842F, 0.818868637F, 1.17110074F,
    1.03924143F, -0.136454374F, -1.3806864F, 1.04253769F, 0.65965569F,
    -0.322334617F, 0.464556754F, 0.324422538F, 0.796900272F, -0.473178804F,
    0.279755563F, -0.394698262F, -0.862525523F, -0.327212363F, -0.639557242F,
    -0.490611553F, 0.338751733F, 0.417324334F, 1.89138305F, -0.466857791F,
    2.30198121F, 1.57073843F, -0.360180676F, -1.61062908F, 0.290326834F,
    0.493579715F, 0.517733514F, 0.148501232F, 1.27300084F, -0.771182477F,
    0.353106827F, -0.327302068F, 0.460389167F, 0.68549329F, 0.453909457F,
    -0.378462F, -0.0985195488F, -0.206431329F, -0.965057969F, -0.250209689F,
    1.13507164F, -0.772459507F, 0.954256713F, -0.531792581F, 0.103379428F,
    -0.326746881F, -0.811861038F, 0.171166062F, -1.57008541F, 0.974830329F,
    2.20185018F, 0.603572369F, 0.584255338F, 0.347585112F, 0.588902771F,
    -0.174917772F, 0.675572574F, 0.730382323F, -0.554826379F, 1.19485497F,
    1.31812453F, 1.59313333F, 0.380175382F, 0.000625401037F, -0.798348188F,
    -0.146159127F, -0.121262707F, -0.23522909F, -0.271712333F, -0.190319672F,
    -0.23512578F, -0.550875843F, 0.530418098F, -1.01209939F, 0.353056282F,
    -0.049062226F };

  float fv2[1240];
  float fv3[248];
  static const float fv4[20] = { 0.920917094F, 0.485088438F, -0.910088181F,
    1.50563109F, 2.80413747F, -1.42352366F, 0.797785819F, -2.06771708F,
    4.65926266F, -4.78621817F, -3.40747523F, 2.80013871F, 1.8366791F,
    -1.63632846F, 2.1747694F, -3.50140977F, -0.672621071F, -0.0530436411F,
    1.11808205F, -0.933523834F };

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
