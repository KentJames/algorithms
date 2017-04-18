#include <stdio.h>
#include <stdlib.h>


void initkernel(double *kernel, int size_x, int size_y, double init_value){

    int i,j;
    for(j=0;j<size_y;j++){
        for(i=0;i<size_x;i++){
            *(kernel + j*size_x + i) = init_value;
        }
    }


}

void printkernel(double *kernel, int size_x, int size_y){

    int i,j;
    for(j=0;j<size_y;j++){
        for(i=0;i<size_x;i++){
            printf("%.2f ",*(kernel+j*size_x + i));
        }
        printf("\n");
    }
    printf("\n");
}



void convolve2d_kernels(double *kernel1, double *kernel2, double *output_kernel, int d_in_x, int d_in_y, int d_out_x, int d_out_y){

    //Set middle of output kernel to kernel1. Then we convolve over that kernel with kernel2.
    //
    int intk_x = d_out_x + 2;
    int intk_y = d_out_y + 2;
    double *intermediate_kernel = malloc(intk_x * intk_y * sizeof(double));
    initkernel(intermediate_kernel,(intk_x+2),(intk_y+2),0.0);
    int i,j;
    for(j=(d_in_y-1);j<(d_in_y+2);j++){
        for(i=(d_in_x-1);i<(d_in_x+2);i++){
            *(intermediate_kernel+j*intk_x+i) = *(kernel1+(j-2)*d_in_x+(i-2));
        }
    }

    printf("Intermediate Kernel: \n");
    printkernel(intermediate_kernel,intk_x,intk_y);

    int ii,jj;
    for(j=0;j<d_out_y;j++){
        for(i=0;i<d_out_x;i++){

            for(jj=0;jj<d_in_y;jj++){
                for(ii=0;ii<d_in_x;ii++){
                
                   *(output_kernel+j*d_out_y+i)+= *(intermediate_kernel+(j+jj)*intk_x+(i+ii));
                }
            }
        }
    }


}






int main(){

    int size_x = 3;
    int size_y = 3;

    double *kernel1 = malloc(size_x * size_y * sizeof(double));
    double *kernel2 = malloc(size_x * size_y * sizeof(double));
    double *output_kernel = malloc((size_x + 2) * (size_y+2) * sizeof(double));

    initkernel(kernel1,size_x,size_y,1.0);
    initkernel(kernel2,size_x,size_y,1.0);
    initkernel(output_kernel,(size_x+2),(size_y+2),0.0);

    printf("Convolution Function 1/2: \n");
    printkernel(kernel1,size_x,size_y);
    
    convolve2d_kernels(kernel1,kernel2,output_kernel,size_x,size_y,size_x+2,size_y+2);
     
    printf("Output Kernel: \n");
    printkernel(output_kernel,size_x+2,size_y+2);


    return(0);
}
