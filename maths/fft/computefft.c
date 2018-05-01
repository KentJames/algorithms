/*

  Example of Cooley-Tukey FFT including ARM SVE Optimisations.
  Only works for powers of two and four (sorry(not sorry)).
  
  Four reference FFT implementations are presented:
  - Radix-2 DIT (Decimation in Time)
  - Radix-4 DIT
  - Radix-4 DIF (Decimation in Frequency)
  - Radix-4 DIF

  No optimisation is attempted on these, they are purely an easy 
  to follow piece of reference code.

  An attempt at optimising a Radix-4 DIF with SVE and other
  optimisations is included. 

  AUTHOR: James Kent <jck42@cam.ac.uk>

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <complex.h>
#ifdef FFTW_CHECK
#include "fftw3.h"
#endif



// DIT, depth first, In-Place 1D FFT. Radix-2
void fft_1d_DIT_radix2(double complex *input, int fft_size){

  assert(fft_size && !(fft_size & (fft_size-1))); // Power of two check. Keep it simple..
  if(fft_size < 2) {
    //Bottom of the recursion pile.
  } else {
    
    //Seperate
    double complex *temp_odd = calloc(fft_size/2, sizeof(double complex));
    for(int i = 0; i < fft_size/2; ++i){
      temp_odd[i] = input[i*2 + 1];
    }
    for(int i = 0; i < fft_size/2; ++i){
      input[i] = input[i*2];
    }
    for(int i = 0; i < fft_size/2; ++i){
      input[i + fft_size/2] = temp_odd[i];
      
    }
    free(temp_odd);

    //Recurse
    fft_1d_DIT_radix2(input, fft_size/2);
    fft_1d_DIT_radix2(input + fft_size/2, fft_size/2);

    //Butterfly
    for(int i = 0; i < fft_size/2; ++i){
      double complex even = input[i];
      double complex odd =  input[i + fft_size/2];
      double complex twiddle = cexp(0 + I * -2.0 * M_PI * i/fft_size);
      input[i] = even + twiddle * odd;
      input[i + fft_size/2] = even - twiddle * odd;
    }
  }	 
}

// DIT, depth first, In-Place 1D FFT. Radix -4.
void fft_1d_DIT_radix4(double complex *input, int fft_size){

  assert(fft_size & 0x55555555); //Check if any even bit is set(is power of four)

  if(fft_size < 4) {
    //Bottom of the recursion pile.
  } else {
    
    //Seperate
    double complex *temp_even_1 = calloc(fft_size/4, sizeof(double complex));
    double complex *temp_even_2 = calloc(fft_size/4, sizeof(double complex));
    double complex *temp_odd_1 = calloc(fft_size/4, sizeof(double complex));
    double complex *temp_odd_2 = calloc(fft_size/4, sizeof(double complex));
    
    
    for(int i = 0; i < fft_size/4; ++i){
      temp_even_1[i] = input[4*i];
      temp_odd_1[i] = input[4*i + 1];
      temp_even_2[i] = input[4*i + 2];
      temp_odd_2[i] = input[4*i + 3];
    }
    for(int i = 0; i < fft_size/4; ++i){
      input[i] = temp_even_1[i];
      input[i + fft_size/4] = temp_odd_1[i];
      input[i + fft_size/2] = temp_even_2[i];
      input[i + 3 * fft_size/4] = temp_odd_2[i];
    }


    //Recurse
    fft_1d_DIT_radix4(input, fft_size/4);
    fft_1d_DIT_radix4(input + fft_size/4, fft_size/4);
    fft_1d_DIT_radix4(input + fft_size/2, fft_size/4);
    fft_1d_DIT_radix4(input + 3 * fft_size/4, fft_size/4);

    //Butterfly
    for(int i = 0; i < fft_size/4; ++i){
            
      double complex twiddle_e1 = cexp(0 + I * (-2.0 * M_PI * 0 * i)/fft_size);
      double complex twiddle_e2 = cexp(0 + I * (-2.0 * M_PI * 2 * i)/fft_size); 
      double complex twiddle_o1 = cexp(0 + I * (-2.0 * M_PI * 1 * i)/fft_size);
      double complex twiddle_o2 = cexp(0 + I * (-2.0 * M_PI * 3 * i)/fft_size);

      double complex even_1 = input[i] * twiddle_e1;
      double complex odd_1 = input[i + fft_size/4] * twiddle_o1; 
      double complex even_2 = input[i + fft_size/2] * twiddle_e2;
      double complex odd_2 = input[i + 3 * fft_size/4] * twiddle_o2;
      
      
      input[i] = even_1 + odd_1 + even_2 + odd_2;
      input[i + fft_size/4] = (even_1 - even_2) - I * (odd_1 - odd_2);
      input[i + fft_size/2] = (even_1 + even_2) - (odd_1 + odd_2);
      input[i + 3 * fft_size/4] = (even_1 - even_2) + I * (odd_1 - odd_2);
      
    }

    free(temp_even_1);
    free(temp_even_2);
    free(temp_odd_1);
    free(temp_odd_2);
    
  }	 
}



// DIF, breadth first, out-of-place 1D FFT. Radix-2
void fft_1d_DIF_radix2(double complex *input, int fft_size){

  assert(fft_size && !(fft_size & (fft_size-1))); // Power of two check.

  if(fft_size < 2){
    // Do nothing.
  } else {

    double complex *even = malloc(fft_size/2 * sizeof(double complex));
    double complex *odd = malloc(fft_size/2 * sizeof(double complex));
    
    //Butterfly
    for(int i = 0; i <fft_size/2; ++i){
      double complex twiddle = cexp(0 + I * -2.0 * M_PI * i/fft_size);
      even[i] = input[i] + input[i + fft_size/2];
      odd[i] = (input[i] - input[i + fft_size/2]) * twiddle;
    }    

    //Recurse
    fft_1d_DIF_radix2(even, fft_size/2);
    fft_1d_DIF_radix2(odd, fft_size/2);

    //Stitch back together
    for(int i = 0; i < fft_size/2; ++i){
      input[2*i] = even[i];
      input[2*i + 1] = odd[i];
    }

    free(even);
    free(odd);
  }

}

// DIF, breadth first, in-place 1D FFT. Radix-4
void fft_1d_DIF_radix4(double complex *input, int fft_size){

  assert(fft_size & 0x55555555); //Check if any even bit is set(is power of four)

  if(fft_size < 4){
    // Have recursed to the bottom.
  } else {

    double complex *even_1 = malloc(fft_size/4 * sizeof(double complex));
    double complex *even_2 = malloc(fft_size/4 * sizeof(double complex));
    double complex *odd_1 = malloc(fft_size/4 * sizeof(double complex));
    double complex *odd_2 = malloc(fft_size/4 * sizeof(double complex));

    //Butterfly
    for(int i = 0; i < fft_size/4; ++i){
      
      double complex twiddle_e1 = cexp(0 + I * (-2.0 * M_PI * 0 * i)/fft_size);
      double complex twiddle_e2 = cexp(0 + I * (-2.0 * M_PI * 2 * i)/fft_size); 
      double complex twiddle_o1 = cexp(0 + I * (-2.0 * M_PI * 1 * i)/fft_size);
      double complex twiddle_o2 = cexp(0 + I * (-2.0 * M_PI * 3 * i)/fft_size);
      
      even_1[i] = (input[i] + input[i + fft_size/4] +
		   input[i + fft_size/2] + input[i + 3 * fft_size/4]) * twiddle_e1;
      even_2[i] = ((input[i] + input[i + fft_size/2]) -
		   (input[i + fft_size/4] + input[i + 3 * fft_size/4])) * twiddle_e2;
      odd_1[i] = ((input[i] - input[i + fft_size/2]) -
		  I * (input[i + fft_size/4] - input[i + 3*fft_size/4])) * twiddle_o1;
      odd_2[i] = ((input[i] - input[i + fft_size/2]) +
		  I * (input[i + fft_size/4] - input[i + 3*fft_size/4])) * twiddle_o2;
    }

    //Recurse
    fft_1d_DIF_radix4(even_1, fft_size/4);
    fft_1d_DIF_radix4(even_2, fft_size/4);
    fft_1d_DIF_radix4(odd_1, fft_size/4);
    fft_1d_DIF_radix4(odd_2, fft_size/4);

    //Stitch back together.
    for(int i = 0; i < fft_size/4; ++i){

      input[4*i] = even_1[i];
      input[4*i + 1] = odd_1[i];
      input[4*i + 2] = even_2[i];
      input[4*i + 3] = odd_2[i];
    }

    free(even_1);
    free(even_2);
    free(odd_1);
    free(odd_2);
  }
}



#if (defined(HAVE_SVE) || defined(HAVE_SVE2)) && defined(USE_SVE)

/*
  This attempts to optimise the above example using standard
  FFT optimisations as well as leveraging vectorisation provided
  by the ARM Scalable Vector Extensions (SVE) Instruction set.

  Standard Optimisations:
  - Pre-computed Twiddle Factors
  - Trivial twiddle factor (twiddle_e1) is removed.


*/

//Easier to think manually than dealing with libc..

struct __Complex_S
{
  double re;
  double im;
};
typedef struct __Complex_S __Complex;


// Dear ARM, please implement complex SVE maths functions. Love James xo
// Pretty sure they aren't optimal either #gradstudent
svfloat64x2_t complex_multiply_sve(svfloat64_t in1_real,
				   svfloat64_t in1_imag,
				   svfloat64_t in2_real,
				   svfloat64_t in2_imag,
				   svbool_t predicate){
				   

  // Work out real parts.
  svfloat64_t vec_re1 = svmul_m(predicate, in1_real, in2_real);
  svfloat64_t vec_re2 = svmul_m(predicate, in1_imag, in2_imag);
  // Work out imaginary parts.
  svfloat64_t vec_im1 = svmul_m(predicate, in1_real, in2_imag);
  svfloat64_t vec_im2 = svmul_m(predicate, in2_real, in1_imag);

  // Stitch together and return.
  svfloat64x2_t result;
  result.v0 = svsub_m(predicate, vec_re1, vec_re2);
  result.v1 = svadd_m(predicate, vec_im1, vec_im2);
  return result;
}


svfloat64x2_t complex_multiply_imm_sve(svfloat64_t in_real,
				       svfloat64_t in_imag,
				       svbool_t predicate,
				       __Complex mul){

  // Work out real parts.
  svbool_t pg_t = svptrue_b64();
  svfloat64_t vec_re1 = svmul_m(predicate, in_real, mul.re);
  svfloat64_t vec_re2 = svmul_m(predicate, in_imag, mul.im);

  // Work out imaginary parts.
  svfloat64_t vec_im1 = svmul_m(predicate, in_real, mul.im);
  svfloat64_t vec_im2 = svmul_m(predicate, in_imag, mul.re);

  // Stitch together and return.
  svfloat64x2_t result;
  result.v0 = svsub_m(predicate, vec_re1, vec_re2);
  result.v1 = svadd_m(predicate, vec_im2, vec_im1);
  return result;
}

// Standard FFT Optimisation.
// Array is strided by fft_size/radix.
void pre_compute_twiddle_factors(double complex *twiddle, int fft_size, int radix){

  int recursion_depth = log(fft_size)/log(4);
  printf("Computing Twiddle Factors for recursion depth: %d\n\n",recursion_depth);

  int fft_size_v = fft_size;
  
  for(int tbl = 0; tbl <= recursion_depth; ++tbl){
    int scale_factor = pow(radix, tbl); 
    for(int i = 0; i < (fft_size_v/radix); ++i){
      for(int r = 0; r < radix; ++r){
	twiddle[fft_size * tbl + r * (fft_size_v/radix) + i] =
	  cexp(0 + I * (-2.0 * M_PI * r * i)/fft_size_v);
      }
    }
  }
  fft_size_v /= fft_size;
}


// DIF, breadth first, in-place 1D FFT. Radix-4.

 void fft_1d_DIF_radix4_SVE(double complex *input, 
			   double complex *twiddles, //These are pre-computed. In practice use a LUT.
			   int twiddle_size, //This should be the same as fft_size for first call.
			   int fft_size,
			   svbool_t pg_t, // Why outlaw global sizeless variables?
			   svbool_t pg_f,
			   svbool_t pg_ft,
			   svbool_t pg_tf,
			   int vl){

  /*
    This checks to ensure we don't have a vector length larger than the smallest butterfly size.
    If the vector length is larger, you can get around this by masking off the bits not required,
    but that would waste computation.

    This attempt at optimising is purely for academic purposes..
  */
   __Complex cmp_i; cmp_i.re = 0; cmp_i.im=1;
  assert(vl * sizeof(double) == 4 * sizeof(double complex)/2);
  assert(fft_size & 0x55555555); //Check if any even bit is set(is power of four)
  int scale = log(twiddle_size)/log(4) - log(fft_size)/log(4); // For indexing twiddle factor LUT.
  
  if(fft_size < 4){
    // Have recursed to the bottom.
    
  } else if(fft_size = 4) { // At FFT Leaf
    double complex *even_1 = malloc(fft_size/4 * sizeof(double complex));;
    double complex *even_2 = malloc(fft_size/4 * sizeof(double complex));
    double complex *odd_1 = malloc(fft_size/4 * sizeof(double complex));
    double complex *odd_2 = malloc(fft_size/4 * sizeof(double complex));

    //Butterfly
    for(int i = 0; i < fft_size/4; ++i){

      //LLVM Bug means passing more than four svbool_t's in causes it to crash...
      svbool_t pg_0011 = svuzp2_b64(pg_tf, pg_ft);
      svbool_t pg_0110 = sveor_z(pg_t,pg_ft, pg_0011); // Exclusive or.

      
      // Setup Indexes for a gather load.
      // We cast double complex as double.
      uint64_t igm_re[4] = {0,  fft_size/2, fft_size,  3 * fft_size/2 };
      uint64_t igm_im[4] = {1, 1 + fft_size/2, 1 + fft_size, 1 + 3 * fft_size/2};
      svuint64_t ig_re = svld1_u64(pg_t, igm_re);
      svuint64_t ig_im = svld1_u64(pg_t, igm_im);

      // Pre-computed Twiddle Factors.
      double complex twiddle_e1 = 1.0;
      double complex twiddle_e2 = twiddles[twiddle_size * scale + fft_size/2 + i];
      double complex twiddle_o1 = twiddles[twiddle_size * scale + fft_size/4 + i];
      double complex twiddle_o2 = twiddles[twiddle_size * scale +  3 * fft_size/4 + i];
       
      /* double complex twiddle_e1 = 1.0; // Evaluates to 1, but demonstrates point. */
      /* double complex twiddle_e2 = cexp(0 + I * (-2.0 * M_PI * 2 * i)/fft_size); */
      /* double complex twiddle_o1 = cexp(0 + I * (-2.0 * M_PI * 1 * i)/fft_size); */
      /* double complex twiddle_o2 = cexp(0 + I * (-2.0 * M_PI * 3 * i)/fft_size); */
      
      // Radix-4 Butterfly.
      svfloat64_t vec_real = svld1_gather_index(pg_t, (double*)&input[0], ig_re);
      svfloat64_t vec_imag = svld1_gather_index(pg_t, (double*)&input[0], ig_im);
      even_1[0] = (svaddv(pg_t, vec_real) + I * svaddv(pg_t, vec_imag));
      
      //even_1[i+1] = svadda(pg_t, 0.0, vec_imag);

      svfloat64_t vec_e2_real = vec_real;
      svfloat64_t vec_e2_imag = vec_imag;
      vec_e2_real = svneg_m(vec_e2_real, pg_ft, vec_e2_real);
      vec_e2_imag = svneg_m(vec_e2_imag, pg_ft, vec_e2_imag);
      even_2[0] = (svaddv(pg_t, vec_e2_real) + I * svaddv(pg_t, vec_e2_imag)) * twiddle_e2;
      
      //even_2[i+1] = svadda(pg_t, 0.0, vec_e2_imag);
      //printf("\n%le %le",creal(svadda(pg_t, 0.0, vec_e2_real)),cimag(svadda(pg_t, 0.0, vec_e2_imag)));
      svfloat64x2_t vec_o1;
      
      vec_o1.v0 = vec_real; // v0 = real
      vec_o1.v1 = vec_imag; // v1 = imag
      vec_o1 = complex_multiply_imm_sve(vec_o1.v0, vec_o1.v1, pg_ft, cmp_i); // Multiply by I.
      vec_o1.v0 = svneg_m(vec_o1.v0, pg_0110, vec_o1.v0);
      vec_o1.v1 = svneg_m(vec_o1.v1, pg_0110, vec_o1.v1);
      odd_1[i] = (svaddv(pg_t, vec_o1.v0) + I * svaddv(pg_t, vec_o1.v1)) * twiddle_o1;
      //odd_1[i+1] = svadda(pg_t, 0.0, vec_o1.v1);
      
      svfloat64x2_t vec_o2;
      vec_o2.v0 = vec_real;
      vec_o2.v1 = vec_imag;
      vec_o2 = complex_multiply_imm_sve(vec_o2.v0, vec_o2.v1, pg_ft, cmp_i);
      vec_o2.v0 = svneg_m(vec_o2.v0, pg_0011, vec_o2.v0);
      vec_o2.v1 = svneg_m(vec_o2.v1, pg_0011, vec_o2.v1);
      //minus_i.im = 1;
      odd_2[i] = (svaddv(pg_t, vec_o2.v0) + I * svaddv(pg_t, vec_o2.v1)) * twiddle_o2;
      //odd_2[i] = svadda(pg_t, 0.0, vec_o2.v1);
      
      
      /* even_1[i] = (input[i] + input[i + fft_size/4] + */
      /* 		   input[i + fft_size/2] + input[i + 3 * fft_size/4]) * twiddle_e1;  */
      /* even_2[i] = (input[i] - input[i + fft_size/4] + */
      /* 		   input[i + fft_size/2] - input[i + 3 * fft_size/4])*twiddle_e2; */
      /* odd_1[i] = (input[i] - I * input[i + fft_size/4] - */
      /* 		  input[i + fft_size/2] + I * input[i + 3 * fft_size/4])*twiddle_o1; */
      /* odd_2[i] = (input[i] + I * input[i + fft_size/4] - */
      /* 		  input[i + fft_size/2] - I * input[i + 3 * fft_size/4]) * twiddle_o2; */
      
    } else { // Node

      svfloat64x2_t in_1, in_2, in_3, in_4; //Input Vectors
      svfloat64x2_t e_1, e_2, o_1, o_2; //Output Vectors
      svfloat64x2_t twid_e2, twid_o1, twid_o2; //Twiddle Factor Vectors
      svfloat64x2_t temp_1, temp_2, temp_3; //For negations and complex multiplies.
      
      for(int i = 0; i < fft_size/4; i += 4){

	double complex *even_1 = malloc(fft_size/4 * sizeof(double complex));;
	double complex *even_2 = malloc(fft_size/4 * sizeof(double complex));
	double complex *odd_1 = malloc(fft_size/4 * sizeof(double complex));
	double complex *odd_2 = malloc(fft_size/4 * sizeof(double complex));
	
	in_1 = svld2(pg_t, (double*)&input[i]);
	in_2 = svld2(pg_t, (double*)&input[i + fft_size/4]);
	in_3 = svld2(pg_t, (double*)&input[i + fft_size/2]);
	in_4 = svld2(pg_t, (double*)&input[i + 3 * fft_size/4]);

	twid_e2 = svld2(pg_t, (double*)&twiddles[twiddle_size * scale + fft_size/2 + i]);
	twid_o1 = svld2(pg_t, (double*)&twiddles[twiddle_size * scale + fft_size/4 + i]);
	twid_o2 = svld2(pg_t, (double*)&twiddles[twiddle_size * scale + 3 * fft_size/4 + i]);
	
	e_1.v0 = svadd_m(pg_t, in_1.v0, in_2.v0);
	e_1.v0 = svadd_m(pg_t, e_1.v0, in_3.v0);
	e_1.v0 = svadd_m(pg_t, e_1.v0, in_4.v0);
	e_1.v1 = svadd_m(pg_t, in_1.v1, in_2.v1);
	e_1.v1 = svadd_m(pg_t, e_1.v1, in_3.v1);
	e_1.v1 = svadd_m(pg_t, e_1.v1, in_4.v1);
	
	temp_1 = in_2;
	temp_2 = in_4;
	temp_1.v0 = svneg_m(temp_1.v0, pg_t, temp_1.v0);
	temp_2.v0 = svneg_m(temp_2.v0, pg_t, temp_1.v0);
	temp_1.v1 = svneg_m(temp_1.v1, pg_t, temp_1.v1);
	temp_2.v1 = svneg_m(temp_2.v1, pg_t, temp_2.v1);

	e_2.v0 = svadd_m(pg_t, in_1.v0, temp_1.v0);
	e_2.v0 = svadd_m(pg_t, e_2.v0, in_3.v0);
	e_2.v0 = svadd_m(pg_t, e_2.v0, temp_2.v0);
	e_2.v1 = svadd_m(pg_t, in_1.v1, temp_1.v1);
	e_2.v1 = svadd_m(pg_t, e_2.v1, in_3.v1);
	e_2.v1 = svadd_m(pg_t, e_2.v1, temp_2.v1);
	e_2 = complex_multiply_sve(e_2.v0, e_2.v1, twid_e2.v0, twid_e2.v1, pg_t);

	temp_1 = complex_multiply_imm_sve(temp1.v0, temp1.v1, pg_t, cmp_i);
	temp_3 = svneg_m(temp_3, pg_t, in_3);
	temp_2 = in_4;
	temp_2 = complex_multiply_imm_sve(temp2.v0, temp2.v1, pg_t, cmp_i);
	
	o_1.v0 = svadd_m(pg_t, in_1.v0, temp_1.v0);
	o_1.v0 = svadd_m(pg_t, o_1.v0 temp_3.v0);
	o_1.v0 = svadd_m(pg_t, o_1.v0 temp_2.v0);
	o_1.v1 = svadd_m(pg_t, in_1.v1, temp_1.v1);
	o_1.v1 = svadd_m(pg_t, o_1.v1 temp_3.v1);
	o_1.v1 = svadd_m(pg_t, o_1.v1 temp_2.v1);
	o_1 = complex_multiply_sve(o_1.v0, o_1.v1, twid_o1.v0, twid_o1.v1, pg_t);

	temp_1 = in_2;
	temp_1 = complex_multiply_imm_sve(temp2.v0, temp2.v1, pg_t, cmp_i);
	temp_2 = in_3;
	temp_2 = svneg_m(temp_2, pg_t, temp_2);
	temp_3 = in_4;
	temp_3 = complex_multply_imm_sve(temp3.v0, temp3.v1, pg_t, cmp_i);
	temp_3 = svneg_m(temp_3, pg_t, temp_3);

	o_2.v0 = svadd_m(pg_t, in_1.v0, temp_1.v0);
	o_2.v0 = svadd_m(pg_t, o_2.v0, temp_2.v0);
	o_2.v0 = svadd_m(pg_t, o_2.v0, temp_3.v0);
	o_2.v1 = svadd_m(pg_t, in_1.v1, temp_1.v1);
	o_2.v1 = svadd_m(pg_t, o_2.v1, temp_2.v1);
	o_2.v1 = svadd_m(pg_t, o_2.v1, temp_3.v1);
	o_2 = complex_multiply_sve(o_2.v0, o_2.v1, twid_o2.v0, twid_o2.v1, pg_t);
	
	svst2(pg_t, (double*)&even_1[i],e_1);
	svst2(pg_t, (double*)&even_2[i],e_2);
	svst2(pg_t, (double*)&odd_1[i],o_1);
	svst2(pg_t, (double*)&odd_2[i],o_2);
	

      }


    }
    
    //Recurse
    fft_1d_DIF_radix4_SVE((double complex*)even_1,
			  twiddles,
			  twiddle_size,
			  fft_size/4,
			  pg_t,
			  pg_f,
			  pg_ft,
			  pg_tf,
			  vl);
    fft_1d_DIF_radix4_SVE((double complex *)even_2,
			  twiddles,
			  twiddle_size,
			  fft_size/4,
			  pg_t,
			  pg_f,
			  pg_ft,
			  pg_tf,
			  vl);
    fft_1d_DIF_radix4_SVE((double complex *)odd_1,
			  twiddles,
			  twiddle_size,
			  fft_size/4,
			  pg_t,
			  pg_f,
			  pg_ft,
			  pg_tf,
			  vl);
    fft_1d_DIF_radix4_SVE((double complex *)odd_2,
			  twiddles,
			  twiddle_size,
			  fft_size/4,
			  pg_t,
			  pg_f,
			  pg_ft,
			  pg_tf,
			  vl);
    
    //Stitch back together. Optimise with bit-reversal.
    for(int i = 0; i < fft_size/4; ++i){

      input[4*i] = even_1[i];
      input[4*i + 1] = odd_1[i];
      input[4*i + 2] = even_2[i];
      input[4*i + 3] = odd_2[i];
    }

    free(even_1);
    free(even_2);
    free(odd_1);
    free(odd_2);
  }
}



#endif


int main(){

  FILE * fin = fopen("fft.dat","r");
  int el = 0;
  double A;
  double B;
  
  while(!feof(fin)){
    fscanf(fin, "%le\t%le\n",&A,&B);
    ++el;
  }
  fclose(fin);

  double complex *input = malloc(el * sizeof(double complex));
  double complex *output = malloc(el * sizeof(double complex));
  el = 0;
  fin = fopen("fft.dat","r");
  while(!feof(fin)){
    fscanf(fin, "%le\t%le\n",&A,&B);
    input[el] = B + I*0;
    ++el;
  }
  fclose(fin);
  printf("\nFFT Size: %d \n\n",el);
  
  #ifdef DFT_CHECK
  
  double complex *dft_output = calloc(el, sizeof(double complex));

  for(int out_el = 0; out_el < el; ++out_el){

    for(int in_el = 0; in_el < el; ++in_el){

      double re = creal(input[in_el]);
      double im = cimag(input[in_el]);
      double angle = 2.0 * M_PI * in_el * out_el/el;
      double re_out = re * cos(angle) + im * sin(angle);
      double imag_out = -re * sin(angle) +  im * cos(angle);
      dft_output[out_el] += re_out + I * imag_out;
      
    }

  }

  for(int i = 0; i< el; ++i){
    printf("%le + %lei\t", creal(dft_output[i]), cimag(dft_output[i]));
  }
  printf("\n\n");
  #endif 

  #ifdef FFTW_CHECK

  fftw_complex * fftw_in = fftw_malloc(el * sizeof(double complex));
  fftw_complex * fftw_out = fftw_malloc(el * sizeof(double complex));
  fftw_plan p;

  p = fftw_plan_dft_1d(el, fftw_in, fftw_out, FFTW_FORWARD, FFTW_PATIENT); // Patient to ensure a decent plan.
  // FFTW overwrites in/out memory during planner. Initialise after.
  for(int i = 0; i< el; ++i) fftw_in[i] = input[i];

  fftw_execute(p);

  for(int i = 0; i < el; ++i){
    printf("%le + %lei\t",creal(fftw_out[i]),cimag(fftw_out[i]));
  }
  printf("\n\n");

  free(fftw_in);
  free(fftw_out);
  
  #endif

  //Naive version is in-place.
  memcpy(output,input,sizeof(double complex) * el);
  fft_1d_DIT_radix4(output, el);

  for(int i = 0; i < el; ++i){
    printf("%le + %lei\t",creal(output[i]),cimag(output[i]));
  }
  printf("\n\n");

  //Cleanup
  free(output);
  free(input);
  
  return 0;
}
