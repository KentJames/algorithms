#include <stdio.h>
#include <stdlib.h>


static inline void initkernel(double *kernel, int size_x, int size_y, double init_value){

    int i,j;
    for(j=0;j<size_y;j++){
        for(i=0;i<size_x;i++){
            *(kernel + j*size_x + i) = init_value;
        }
    }


}

static inline void printkernel(double *kernel, int size_x, int size_y){

    int i,j;
    for(j=0;j<size_y;j++){
        for(i=0;i<size_x;i++){
            printf("%.2f ",*(kernel+j*size_x + i));
        }
        printf("\n");
    }
    printf("\n");
}



void convolve2d_kernels(double *kernel1, double *kernel2, double *output_kernel, int d_in1_x, int d_in1_y, int d_in2_x, int d_in2_y, int d_out_x, int d_out_y){

    //Set middle of output kernel to kernel1. Then we convolve over that kernel with kernel2.
    //
    int intk_x = d_out_x + (d_in2_x-1);
    int intk_y = d_out_y + (d_in2_y-1);
    double *intermediate_kernel = malloc(intk_x * intk_y * sizeof(double));
    initkernel(intermediate_kernel,intk_x,intk_y,0.0);
    int i,j;


    for(j=(d_in2_y-1);j<(d_in2_y+(d_in2_y-1));j++){
        for(i=(d_in2_x-1);i<(d_in2_x+(d_in2_x-1));i++){
            *(intermediate_kernel+j*intk_x+i) = *(kernel1+(j-(d_in2_y-1))*d_in1_x+(i-(d_in2_x-1)));
        }
    }

    printf("Intermediate Kernel: \n");
    printkernel(intermediate_kernel,intk_x,intk_y);

    int ii,jj;
    for(j=0;j<d_out_y;j++){
        for(i=0;i<d_out_x;i++){

            for(jj=0;jj<d_in2_y;jj++){
                for(ii=0;ii<d_in2_x;ii++){
                
                   *(output_kernel+j*d_out_y+i)+= *(intermediate_kernel+(j+jj)*intk_x+(i+ii)) * *(kernel2+jj*d_in2_y+ii);
                }
            }
        }
    }

    free(intermediate_kernel);


}






int main(){

    int size_x = 3;
    int size_y = 3;

    int output_size_x = size_x*2 -1;
    int output_size_y = size_y*2 -1;


    double *kernel1 = malloc(size_x * size_y * sizeof(double));
    double *kernel2 = malloc(size_x * size_y * sizeof(double));
    double *output_kernel = malloc((output_size_x + 2) * (output_size_y+2) * sizeof(double));

    initkernel(kernel1,size_x,size_y,1.0);
    initkernel(kernel2,size_x,size_y,2.0);
    initkernel(output_kernel,output_size_x,output_size_y,0.0);

    printf("Convolution Function 1/2: \n");
    printkernel(kernel1,size_x,size_y);
    
    convolve2d_kernels(kernel1,kernel2,output_kernel,size_x,size_y,size_x,size_y,output_size_x,output_size_y);
     
    printf("Output Kernel: \n");
    printkernel(output_kernel,output_size_x,output_size_y);

    int sizeko_x = size_x+2;
    int sizeko_y = size_y+2;

    int sizek3_x = 5;
    int sizek3_y = 5; 

    int sizeko2_x = output_size_x+(sizek3_x-1);
    int sizeko2_y = output_size_y+(sizek3_y-1);
    double *output_kernel2 = malloc(sizeko2_x*sizeko2_y*sizeof(double));
    double *kernel3 = malloc((sizek3_x) * (sizek3_y) * sizeof(double));

    initkernel(output_kernel2,sizeko2_x,sizeko2_y,0.0);
    initkernel(kernel3,sizek3_x,sizek3_y,1.0);

    convolve2d_kernels(output_kernel,kernel3,output_kernel2,sizeko_x,sizeko_y,sizek3_x,sizek3_y,sizeko2_x,sizeko2_y);

    printkernel(output_kernel2,sizeko2_x,sizeko2_y);


    free(kernel1);
    free(kernel2);
    free(kernel3);
    free(output_kernel);
    free(output_kernel2);


    return(0);
}
