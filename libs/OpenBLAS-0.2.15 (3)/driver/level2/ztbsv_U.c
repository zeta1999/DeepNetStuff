/*********************************************************************/
/* Copyright 2009, 2010 The University of Texas at Austin.           */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "common.h"

const static FLOAT dp1 = 1.;

int CNAME(BLASLONG n, BLASLONG k, FLOAT *a, BLASLONG lda, FLOAT *b, BLASLONG incb, void *buffer){

  BLASLONG i;
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *B = b;
  BLASLONG length;
#if (TRANSA == 2) || (TRANSA == 4)
  OPENBLAS_COMPLEX_FLOAT temp;
#endif
#ifndef UNIT
  FLOAT ar, ai, br, bi, ratio, den;
#endif

  if (incb != 1) {
    B = buffer;
    gemvbuffer = (FLOAT *)(((BLASLONG)buffer + n * sizeof(FLOAT) * COMPSIZE+ 4095) & ~4095);
    COPY_K(n, b, incb, buffer, 1);
  }

  a += (n - 1) * lda * COMPSIZE;

  for (i = n - 1; i >= 0; i--) {

#if (TRANSA == 2) || (TRANSA == 4)
    length  = n - i - 1;
    if (length > k) length = k;

    if (length > 0) {
#if TRANSA == 2
      temp = DOTU_K(length, a + COMPSIZE, 1, B + (i + 1) * COMPSIZE, 1);
#else
      temp = DOTC_K(length, a + COMPSIZE, 1, B + (i + 1) * COMPSIZE, 1);
#endif

      B[i * 2 + 0] -= CREAL(temp);
      B[i * 2 + 1] -= CIMAG(temp);
    }
#endif

#ifndef UNIT
#if (TRANSA == 1) || (TRANSA == 3)
    ar = a[k * 2 + 0];
    ai = a[k * 2 + 1];
#else
    ar = a[0];
    ai = a[1];
#endif

    if (fabs(ar) >= fabs(ai)){
      ratio = ai / ar;
      den = 1./(ar * ( 1 + ratio * ratio));

      ar =  den;
#if TRANSA < 3
      ai = -ratio * den;
#else
      ai =  ratio * den;
#endif
    } else {
      ratio = ar / ai;
      den = 1./(ai * ( 1 + ratio * ratio));
      ar =  ratio * den;
#if TRANSA < 3
      ai = -den;
#else
      ai =  den;
#endif
    }

    br = B[i * 2 + 0];
    bi = B[i * 2 + 1];

    B[i * 2 + 0] = ar*br - ai*bi;
    B[i * 2 + 1] = ar*bi + ai*br;
#endif

#if (TRANSA == 1) || (TRANSA == 3)
    length  = i;
    if (length > k) length = k;

    if (length > 0) {
#if   TRANSA == 1
      AXPYU_K(length, 0, 0,
	      -B[i * 2 + 0],  -B[i * 2 + 1],
	      a + (k - length) * COMPSIZE, 1, B + (i - length) * COMPSIZE, 1, NULL, 0);
#else
      AXPYC_K(length, 0, 0,
	      -B[i * 2 + 0],  -B[i * 2 + 1],
	      a + (k - length) * COMPSIZE, 1, B + (i - length) * COMPSIZE, 1, NULL, 0);
#endif

    }
#endif

    a -= lda * COMPSIZE;
  }

  if (incb != 1) {
    COPY_K(n, buffer, 1, b, incb);
  }

  return 0;
}

