/*****************************************************************************
  Copyright (c) 2011, Intel Corp.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************
* Contents: Native middle-level C interface to LAPACK function cheevr
* Author: Intel Corporation
* Generated November, 2011
*****************************************************************************/

#include "lapacke_utils.h"

lapack_int LAPACKE_cheevr_work( int matrix_order, char jobz, char range,
                                char uplo, lapack_int n,
                                lapack_complex_float* a, lapack_int lda,
                                float vl, float vu, lapack_int il,
                                lapack_int iu, float abstol, lapack_int* m,
                                float* w, lapack_complex_float* z,
                                lapack_int ldz, lapack_int* isuppz,
                                lapack_complex_float* work, lapack_int lwork,
                                float* rwork, lapack_int lrwork,
                                lapack_int* iwork, lapack_int liwork )
{
    lapack_int info = 0;
    if( matrix_order == LAPACK_COL_MAJOR ) {
        /* Call LAPACK function and adjust info */
        LAPACK_cheevr( &jobz, &range, &uplo, &n, a, &lda, &vl, &vu, &il, &iu,
                       &abstol, m, w, z, &ldz, isuppz, work, &lwork, rwork,
                       &lrwork, iwork, &liwork, &info );
        if( info < 0 ) {
            info = info - 1;
        }
    } else if( matrix_order == LAPACK_ROW_MAJOR ) {
        lapack_int ncols_z = ( LAPACKE_lsame( range, 'a' ) ||
                             LAPACKE_lsame( range, 'v' ) ) ? n :
                             ( LAPACKE_lsame( range, 'i' ) ? (iu-il+1) : 1);
        lapack_int lda_t = MAX(1,n);
        lapack_int ldz_t = MAX(1,n);
        lapack_complex_float* a_t = NULL;
        lapack_complex_float* z_t = NULL;
        /* Check leading dimension(s) */
        if( lda < n ) {
            info = -7;
            LAPACKE_xerbla( "LAPACKE_cheevr_work", info );
            return info;
        }
        if( ldz < ncols_z ) {
            info = -16;
            LAPACKE_xerbla( "LAPACKE_cheevr_work", info );
            return info;
        }
        /* Query optimal working array(s) size if requested */
        if( liwork == -1 || lrwork == -1 || lwork == -1 ) {
            LAPACK_cheevr( &jobz, &range, &uplo, &n, a, &lda_t, &vl, &vu, &il,
                           &iu, &abstol, m, w, z, &ldz_t, isuppz, work, &lwork,
                           rwork, &lrwork, iwork, &liwork, &info );
            return (info < 0) ? (info - 1) : info;
        }
        /* Allocate memory for temporary array(s) */
        a_t = (lapack_complex_float*)
            LAPACKE_malloc( sizeof(lapack_complex_float) * lda_t * MAX(1,n) );
        if( a_t == NULL ) {
            info = LAPACK_TRANSPOSE_MEMORY_ERROR;
            goto exit_level_0;
        }
        if( LAPACKE_lsame( jobz, 'v' ) ) {
            z_t = (lapack_complex_float*)
                LAPACKE_malloc( sizeof(lapack_complex_float) *
                                ldz_t * MAX(1,ncols_z) );
            if( z_t == NULL ) {
                info = LAPACK_TRANSPOSE_MEMORY_ERROR;
                goto exit_level_1;
            }
        }
        /* Transpose input matrices */
        LAPACKE_che_trans( matrix_order, uplo, n, a, lda, a_t, lda_t );
        /* Call LAPACK function and adjust info */
        LAPACK_cheevr( &jobz, &range, &uplo, &n, a_t, &lda_t, &vl, &vu, &il,
                       &iu, &abstol, m, w, z_t, &ldz_t, isuppz, work, &lwork,
                       rwork, &lrwork, iwork, &liwork, &info );
        if( info < 0 ) {
            info = info - 1;
        }
        /* Transpose output matrices */
        LAPACKE_che_trans( LAPACK_COL_MAJOR, uplo, n, a_t, lda_t, a, lda );
        if( LAPACKE_lsame( jobz, 'v' ) ) {
            LAPACKE_cge_trans( LAPACK_COL_MAJOR, n, ncols_z, z_t, ldz_t, z,
                               ldz );
        }
        /* Release memory and exit */
        if( LAPACKE_lsame( jobz, 'v' ) ) {
            LAPACKE_free( z_t );
        }
exit_level_1:
        LAPACKE_free( a_t );
exit_level_0:
        if( info == LAPACK_TRANSPOSE_MEMORY_ERROR ) {
            LAPACKE_xerbla( "LAPACKE_cheevr_work", info );
        }
    } else {
        info = -1;
        LAPACKE_xerbla( "LAPACKE_cheevr_work", info );
    }
    return info;
}
