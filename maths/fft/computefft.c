/*

Example of FFT including ARM SVE Optimisations.
Only works for powers of two and four.

Three example implementations are presented of DIT and DIF FFT's.
No optimisation is attempted on these, they are purely an easy 
to follow piece of reference code.



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
