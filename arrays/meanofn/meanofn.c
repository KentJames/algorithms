#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>

//reduces matrix size by 2 in each dimension
void matrixreduce(double *inputmatrix, double *outputmatrix,int originaldimensions){



    int outputdimensions = originaldimensions/2;
    //outputmatrix = malloc(outputdimensions*outputdimensions*sizeof(double));
    int i,j;
    for(j=0;j<=outputdimensions-1;j+=1){ 
        for(i=0;i<=outputdimensions-1;i+=1){
            outputmatrix[i*outputdimensions+j] = inputmatrix[(i*2)*originaldimensions+(j*2)] + inputmatrix[(i*2+1)*originaldimensions+(j*2)]
                + inputmatrix[(i*2)*originaldimensions+(j*2+1)] + inputmatrix[(i*2+1)*originaldimensions+(j*2+1)];
            outputmatrix[i*outputdimensions+j] /= 4;
        }
    }

}


//sliding window
void slidingmatrixreduce(double *inputmatrix, double *outputmatrix, int dimensions){

    int outputdimensions = dimensions - 1;

    int i,j;

    for(j=0;j<outputdimensions;j++){
        for(i=0;i<outputdimensions;i++){
            outputmatrix[i*outputdimensions+j] = inputmatrix[i*dimensions + j] + inputmatrix[(i+1)*dimensions + j]
                + inputmatrix[i*dimensions + (j+1)] + inputmatrix[(i+1)*dimensions+(j+1)];
            outputmatrix[i*outputdimensions+j] /= 4;
        }
    }

}

void initializematrix(double *matrix,int dimensions){

    int i,j;

    for(j=0;j<dimensions;j++){
        for(i=0;i<dimensions;i++){
            double theta = (double)i/((double)dimensions*M_PI);
            matrix[i*dimensions+j] = rand()%100;
        }
    }

}

void printmatrix(double *matrix,int dimensions){
    
    int i,j;

    for(j=0;j<dimensions;j++){
        for(i=0;i<dimensions;i++){
            printf("%f ",matrix[i*dimensions+j]);
        }
        printf("\n");
    }
}


int main(int argc, char *argv[]){
    
    if(!argc){
        return -1;
    }

    int dimensionmatrix = strtol(argv[1],NULL,10);

    if(!dimensionmatrix){
        return 0;
    }

    double *matrix = malloc(dimensionmatrix * dimensionmatrix * sizeof(double));
    double *reducedmatrix = malloc(((dimensionmatrix * dimensionmatrix)/4)*sizeof(double));
    double *slidingreducedmatrix = malloc((dimensionmatrix-1)*(dimensionmatrix-1)*sizeof(double));
    printf("Initial matrix: \n");
    initializematrix(matrix,dimensionmatrix);
    printmatrix(matrix,dimensionmatrix);
    matrixreduce(matrix,reducedmatrix,dimensionmatrix);

    printf("\n No overlap reduced matrix: \n");
    printmatrix(reducedmatrix,dimensionmatrix/2);

    slidingmatrixreduce(matrix,slidingreducedmatrix,dimensionmatrix);
    printf("\n Sliding window reduced matrix: \n");
    printmatrix(slidingreducedmatrix,dimensionmatrix-1);



return 0;
}
