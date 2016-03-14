/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: neural_net.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 13-Mar-2016 23:47:35
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
 *  Generated by Neural Network Toolbox function genFunction, 13-Mar-2016 23:46:11.
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
  static const double dv0[12] = { 18.537577, -38.786446, -13.968928, -6.6917577,
    -40.67978, -9.433435, -4.4070444, -38.46941, -16.094198, -12.057183,
    -25.863316, -11.065968 };

  static const double dv1[12] = { 0.11519437409108, 0.0677425011337554,
    0.0627540401207915, 0.0637778272346482, 0.0405506592999906,
    0.042423699015225, 0.0513985356516074, 0.0423095980296251,
    0.0424621210810355, 0.04619840380357, 0.0465058557732032, 0.0518639205861723
  };

  double dv2[372];
  double dv3[1240];
  float fv0[1240];
  int i0;
  int i1;
  float f0;
  int i2;
  static const float fv1[120] = { 0.636366367F, -0.69084996F, -0.390597343F,
    1.25392222F, 0.88723892F, -0.643431544F, -0.302463442F, 0.370987773F,
    -0.847608626F, 0.884094298F, -0.492839664F, 0.130992055F, -0.814654171F,
    -0.824977398F, 0.0386752449F, -0.589994371F, 0.851025045F, 0.545353115F,
    0.620249152F, -1.23877168F, 0.451379657F, 0.154061F, 0.89034766F,
    -2.03998756F, -0.109904155F, 1.51268923F, -0.47141546F, -0.169045806F,
    -0.598067045F, -0.0670133606F, 0.596230388F, 0.385361373F, -0.261092514F,
    -2.04856396F, -1.03145874F, 0.964684427F, -0.201066196F, -0.623581171F,
    -0.327561378F, -0.389108121F, 0.489738256F, 1.14247608F, 0.183025852F,
    0.763038039F, -0.0633701161F, -0.89478153F, -0.0853185579F, 1.13841498F,
    0.692545593F, 0.157136098F, 0.0108917225F, 1.02161682F, -0.521262944F,
    0.0698080957F, 0.310053229F, -0.434984356F, 0.256250083F, 0.19219251F,
    -0.576410234F, 0.469885856F, -0.387662351F, -0.577110291F, -0.251866221F,
    0.772809148F, 0.746499777F, 0.785251439F, -0.254600108F, -0.317601681F,
    -0.41413191F, -0.175391316F, 0.0419155695F, -0.992714226F, 0.415922076F,
    0.312999427F, -0.471641362F, -0.568666935F, 1.70928741F, 0.95675391F,
    0.0207238402F, 0.317391217F, 0.789402306F, -0.229193628F, 0.843726099F,
    -0.570605397F, -0.766774178F, -0.0230074357F, 0.29784444F, -0.919070065F,
    -0.130439103F, -0.28430593F, -0.180022672F, -0.548359871F, -0.0728577226F,
    -0.269168437F, 0.406512856F, 0.654129922F, -0.205552205F, 0.412385434F,
    -0.0215045754F, 0.443771243F, 0.617640138F, -0.183995232F, 0.546377F,
    1.2253561F, 0.519226849F, -1.08715069F, -0.0655918494F, 1.01071024F,
    -0.951005518F, -0.732700944F, 0.24734953F, 0.402251512F, 0.0484237559F,
    -0.711802959F, -0.306844473F, -0.0442575179F, -0.768894672F, 0.0743289888F,
    -0.0925418958F, 0.187485054F };

  float fv2[1240];
  float fv3[372];
  static const float fv4[30] = { 0.449740738F, -0.245610029F, 1.01805377F,
    0.863953233F, -0.35267517F, 1.04208231F, 0.759321928F, -0.163300857F,
    0.425618649F, -1.23947561F, 3.29334426F, -1.91088843F, 0.409054846F,
    1.41298902F, -1.12847722F, 2.87986F, -0.514584899F, -1.19113863F,
    -0.837483525F, 0.0936673135F, 1.251894F, -1.06599641F, 0.85941571F,
    1.36410594F, -0.974582732F, -0.97832787F, 0.286582798F, -0.563718379F,
    1.22956872F, -1.39485812F };

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
