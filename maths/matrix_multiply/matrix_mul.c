/* Naive matrix multiplication implementation. 
 * Minor optimisation implemented through transposing second matrix.
 * This dramatically increases locality for little effort,
 * and has a large speedup. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>


#define m_1 768
#define n_1 768
#define m_2 768
#define n_2 768


// M: Columns
// N: Rows

void _populate_matrix_rowmajor(float *matrix, int m, int n){

    int i,j;

    for(j=0;j<n;j++){
        for(i=0;i<m;i++){
            *(matrix+j*m+i) = i+1;
        }
    }



}

void _populate_matrix_columnmajor(float *matrix, int m, int n){

    int i,j;

    for(j=0;j<m;j++){
        for(i=0;i<n;i++){
            *(matrix+j*n+i) = i+1;
        }
    }


}


void populate_matrix_zeros(float *matrix,int m, int n){

    int i,j;

    for(j=0;j<n;j++){
        for(i=0;i<m;i++){
            *(matrix+j*m+i) = 0.0;
        }
    }



}

void _print_matrix_rowmajor(float *matrix, int m, int n){

    int i,j;
    for(j=0;j<n;j++){
        for(i=0;i<m;i++){
            printf("%f ",*(matrix+j*m+i));
        }
        printf("\n");
    }
    printf("\n");



}

void _print_matrix_columnmajor(float *matrix, int m, int n){
    
    int i,j;

    for(j=0;j<m;j++){
        for(i=0;i<n;i++){
            printf("%f ",*(matrix+j*n+i));
        }
        printf("\n");
    }
    printf("\n");
}


void multiply_matrices(float *restrict a, float *restrict b, float *restrict c,  int m1, int n1, int m2, int n2){

   if(m1 != n2){
      err(-1,"Matrices dimensions not suitaale for multiplication!");
   } 

   int i,j,k;
   for(j=0;j<n1;j++){
       for(i=0;i<m2;i++){
            for(k=0;k<m1;k++){
                *(c+j*m2+i) += *(a+j*m1+k) * *(b+i*n2+k);
            }
       }
   }

}

/*
uninsged int LUT[256] = {
    0,
    1,
    1,
    2,
    1,
    2
};
unsigned int bitsset(uint8_t bits_in){
    return LUT[bits_in];

    unsigned int no_of_bits=0;
        for(int i = 0; i<8;i++){
            if (bits_in&0x01) no_of_bits++;
            bits_in >>= 1;
        }

}
*/

int main(){

    float *a = malloc(m_1 * n_1 * sizeof(float));
    float *b = malloc(m_2 * n_2 * sizeof(float));

    float *c = malloc(n_1 * m_2 * sizeof(float));

    _populate_matrix_rowmajor(a,m_1,n_1);
    _populate_matrix_columnmajor(b,m_2,n_2);
    populate_matrix_zeros(c,m_2,n_1);
    //_print_matrix_rowmajor(a,m_1,n_1);
    //_print_matrix_columnmajor(b,m_2,n_2);

    multiply_matrices(a,b,c,m_1,n_1,m_2,n_2);

    //_print_matrix_rowmajor(c,m_2,n_1);

    return(0);
}
