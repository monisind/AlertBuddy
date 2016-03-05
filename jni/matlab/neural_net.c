/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 01-Mar-2016 23:54:39
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "neural_net_classify.h"
#include "neural_net.h"
#include "repmat.h"

/* Function Declarations */
static void mapminmax_apply(const float x[1488], const double settings_gain[12],
  const double settings_xoffset[12], float y[1488]);
static void mapminmax_reverse(const float y[372], float x[372]);
static void softmax_apply(float n[372], float a[372]);
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
 * Arguments    : const float y[372]
 *                float x[372]
 * Return Type  : void
 */
static void mapminmax_reverse(const float y[372], float x[372])
{
  int ak;
  int ck;
  float cv[3];
  int k;
  float a[372];

  /*  Map Minimum and Maximum Output Reverse-Processing Function */
  ak = 0;
  for (ck = 0; ck < 371; ck += 3) {
    for (k = 0; k < 3; k++) {
      cv[k] = y[ak + k] - -1.0F;
    }

    for (k = 0; k < 3; k++) {
      x[ck + k] = cv[k];
    }

    ak += 3;
  }

  memcpy(&a[0], &x[0], 372U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 371; ck += 3) {
    for (k = 0; k < 3; k++) {
      cv[k] = a[ak + k] / 2.0F;
    }

    for (k = 0; k < 3; k++) {
      x[ck + k] = cv[k];
    }

    ak += 3;
  }

  memcpy(&a[0], &x[0], 372U * sizeof(float));
  ak = 0;
  for (ck = 0; ck < 371; ck += 3) {
    for (k = 0; k < 3; k++) {
      cv[k] = a[ak + k];
    }

    for (k = 0; k < 3; k++) {
      x[ck + k] = cv[k];
    }

    ak += 3;
  }
}

/*
 * Arguments    : float n[372]
 *                float a[372]
 * Return Type  : void
 */
static void softmax_apply(float n[372], float a[372])
{
  float nmax[124];
  int ix;
  int iy;
  int i;
  int bk;
  float mtmp;
  int ak;
  boolean_T exitg1;
  float cv[3];
  int k;
  float numer[372];
  float s;
  float b_nmax;

  /*  Competitive Soft Transfer Function */
  ix = 0;
  iy = -1;
  for (i = 0; i < 124; i++) {
    ix += 3;
    bk = ix - 2;
    mtmp = n[ix - 3];
    if (rtIsNaNF(n[ix - 3])) {
      ak = ix - 1;
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

  memcpy(&a[0], &n[0], 372U * sizeof(float));
  ak = 0;
  bk = 0;
  for (ix = 0; ix < 371; ix += 3) {
    for (k = 0; k < 3; k++) {
      cv[k] = a[ak + k] - nmax[bk];
    }

    for (k = 0; k < 3; k++) {
      n[ix + k] = cv[k];
    }

    ak += 3;
    bk++;
  }

  for (ak = 0; ak < 372; ak++) {
    numer[ak] = (real32_T)exp(n[ak]);
  }

  ix = 0;
  iy = -1;
  for (i = 0; i < 124; i++) {
    ak = ix;
    ix++;
    s = numer[ak];
    for (k = 0; k < 2; k++) {
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
  for (ix = 0; ix < 371; ix += 3) {
    for (k = 0; k < 3; k++) {
      cv[k] = numer[ak + k] / nmax[bk];
    }

    for (k = 0; k < 3; k++) {
      a[ix + k] = cv[k];
    }

    ak += 3;
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
 *  Generated by Neural Network Toolbox function genFunction, 01-Mar-2016 23:37:19.
 *
 *  [y1] = neural_net(x1) takes these arguments:
 *    x = 12xQ matrix, input #1
 *  and returns:
 *    y = 3xQ matrix, output #1
 *  where Q is the number of samples.
 * Arguments    : const float x1[1488]
 *                float b_y1[372]
 * Return Type  : void
 */
void neural_net(const float x1[1488], float b_y1[372])
{
  float xp1[1488];
  static const double dv0[12] = { 14.111271, -39.717815, -16.657677, -2.814637,
    -36.676994, -6.7565117, -7.642342, -34.000916, -4.9420257, -11.446178,
    -22.604774, -4.165919 };

  static const double dv1[12] = { 0.091468884640769, 0.05653599588626,
    0.0581666686200973, 0.0846798200960286, 0.0558719745329282,
    0.0616758987829054, 0.0631358271038002, 0.0629430640960552,
    0.070694565659605, 0.0726486747864638, 0.0743539257274175, 0.080547189275625
  };

  double dv2[372];
  double dv3[1240];
  float fv0[1240];
  int i0;
  int i1;
  float f0;
  int i2;
  static const float fv1[120] = { -0.362357706F, 0.745791F, 0.767863572F,
    0.490121305F, -2.04002881F, 0.368799597F, -1.24477863F, 0.238086626F,
    -0.744406879F, 0.373874098F, 0.24882412F, -1.1599772F, -1.34146798F,
    0.0385768712F, -0.0926403925F, 0.438618422F, -0.0272059347F, 1.21003819F,
    -0.205970109F, 0.920833111F, -0.247386634F, 0.133594319F, -1.59321141F,
    -1.40062976F, 1.32958615F, 0.655596F, 0.476490796F, -0.503650308F,
    0.280557543F, -0.700125635F, 0.551994205F, -0.894925416F, -0.163589925F,
    -1.03137302F, -0.126215562F, 0.984663963F, -1.41546524F, 0.787463486F,
    0.34560892F, 0.643950522F, -0.742952824F, 0.259231091F, -0.533150554F,
    0.498813033F, 0.804226339F, 1.01362777F, -0.918435037F, 0.722627342F,
    0.830468774F, -0.448332131F, 0.420550644F, 0.922970116F, -0.599106431F,
    -0.607566714F, 0.285149068F, -1.09692371F, 2.71649671F, -1.21400046F,
    -0.728972495F, -0.536751747F, 0.332200915F, 0.910227835F, 0.535661459F,
    0.406887591F, 0.33696267F, -0.14196302F, 0.736564398F, -1.34101772F,
    0.264815807F, 0.538505375F, 0.5967592F, 0.0954845399F, 0.6535905F,
    -0.146637008F, -0.842939794F, -0.155199021F, -1.62375569F, 0.533017337F,
    0.595444143F, -0.290933073F, -0.175878391F, -0.650224626F, -0.112338595F,
    -0.337275386F, 0.747257888F, -0.0498512425F, 1.33269453F, -1.33465791F,
    -0.00398254441F, -0.240206093F, -0.8595891F, -0.226114392F, -0.770279765F,
    -0.0165398344F, 0.632731199F, -0.221124813F, 0.786900759F, -0.0331174396F,
    0.536301851F, -0.218702763F, 0.674284F, 0.687142372F, 0.172376469F,
    0.667069316F, -0.265155494F, -0.364616454F, -0.680472732F, 0.324544072F,
    -0.726400733F, -0.229688913F, -0.287720442F, 0.214617446F, 0.10598094F,
    0.143715009F, -0.861471832F, 0.392431259F, 0.279063404F, -0.234370485F,
    0.370342761F, -0.0309862923F };

  float fv2[1240];
  float fv3[372];
  static const float fv4[30] = { 0.481801301F, 0.685507953F, 0.124857344F,
    0.379319638F, 0.0365195386F, 1.67415643F, -1.55563426F, -0.81993258F,
    1.81493306F, -1.38386023F, -0.836855352F, 0.777125955F, 2.15268373F,
    -0.625501454F, -2.03752446F, -0.906109512F, 0.98907721F, -1.42458F,
    2.98732591F, -2.97926021F, 0.109883629F, -0.238430902F, 3.10797191F,
    -1.64735651F, 0.457611471F, 1.14384401F, -0.0480951369F, 0.0299650598F,
    0.476629972F, 0.51504463F };

  float fv5[372];

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
  for (i0 = 0; i0 < 3; i0++) {
    for (i1 = 0; i1 < 124; i1++) {
      f0 = 0.0F;
      for (i2 = 0; i2 < 10; i2++) {
        f0 += fv4[i0 + 3 * i2] * fv2[i2 + 10 * i1];
      }

      fv3[i0 + 3 * i1] = (float)dv2[i0 + 3 * i1] + f0;
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
