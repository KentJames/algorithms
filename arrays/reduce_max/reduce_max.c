#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

//reduces matrix size by 2 in each dimension
void matrixreduce(double *inputmatrix, double *outputmatrix,int originaldimensions,int reduction){

    if(originaldimensions%reduction) errx(1,"dimension/window size mismatch!");
  
    int outputdimensions = originaldimensions/reduction;
    int i,j;
    for(j=0;j<=outputdimensions-1;j+=1){ 
        for(i=0;i<=outputdimensions-1;i+=1){
            
            outputmatrix[i*outputdimensions+j] = 0.0; // Need to initialise or you can get undefined data I found!
            int l,m;
            for(m=0;m<reduction;m++){
                for(l=0;l<reduction;l++){
                    outputmatrix[i*outputdimensions+j] = MAX(outputmatrix[i*outputdimensions+j],inputmatrix[(i*reduction+l)*originaldimensions+(j*reduction+m)]);
                }
            }
        }
    }

}


//sliding window
void slidingmatrixreduce(double *inputmatrix, double *outputmatrix, int dimensions,int reduction){

    if(dimensions%reduction != 0){
        errx(1,"dimension/window size mismatch!");
    }

    int outputdimensions = dimensions - 1;

    int i,j;

    for(j=0;j<outputdimensions;j++){
        for(i=0;i<outputdimensions;i++){
            int l,m;
            for(m=0;m<reduction;m++){
                for(l=0;l<reduction;l++){
                    outputmatrix[i*outputdimensions+j] = MAX(outputmatrix[i*outputdimensions+j],inputmatrix[(i+l)*dimensions+(j+m)]);
                }
            }
        }
    }

}

void initializematrix(double *matrix,int dimensions){

    int i,j;

    for(j=0;j<dimensions;j++){
        for(i=0;i<dimensions;i++){
            double theta = (double)i/((double)dimensions*M_PI);
            matrix[i*dimensions+j] = rand()%100; // I know rand() is crap but its not really important here.
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
    int reductions = strtol(argv[2],NULL,10);
    if(!dimensionmatrix){
        return 0;
    }

    double *matrix = malloc(dimensionmatrix * dimensionmatrix * sizeof(double));
    double *reducedmatrix = malloc(((dimensionmatrix*dimensionmatrix)/pow(reductions,2))*sizeof(double));
    double *slidingreducedmatrix = malloc((dimensionmatrix-1)*(dimensionmatrix-1)*sizeof(double));
    printf("Initial matrix: \n");
    initializematrix(matrix,dimensionmatrix);
    printmatrix(matrix,dimensionmatrix);
    matrixreduce(matrix,reducedmatrix,dimensionmatrix,reductions);

    printf("\n No overlap reduced matrix: \n");
    printmatrix(reducedmatrix,dimensionmatrix/reductions);

    slidingmatrixreduce(matrix,slidingreducedmatrix,dimensionmatrix,reductions);
    printf("\n Sliding window reduced matrix: \n");
    printmatrix(slidingreducedmatrix,dimensionmatrix-1);


    free(matrix);
    free(reducedmatrix);
    free(slidingreducedmatrix);
    return 0;
}
