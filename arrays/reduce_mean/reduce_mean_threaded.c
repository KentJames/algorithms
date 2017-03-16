#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <err.h>

#define THREAD_NUM 4
//Data structure passed to each threaded routine to bound where it works on the array.
//
void printmatrix(double*,int);

struct arrayindex {

    int imin;
    int jmin;
    int imax;
    int jmax;
};

//Encapsulates all data for each thread.
struct thread_data {
    pthread_t thread_id;
    pthread_mutex_t thread_mutex;
    double **inputmatrix;
    double **outputmatrix;
    int dimensions;
    int reduction;
    struct arrayindex bounds;
};

//reduces matrix size by 2 in each dimension
void *matrixreduce(void *threadArg){

    struct thread_data *inst_thread_data = (struct thread_data *) threadArg;
 
    if(inst_thread_data->dimensions%inst_thread_data->reduction) errx(1,"dimension/reduction mismatch!");


    
    printf("\n Reduction: %d \n Dimensions: %d \n Imin: %d \n Imax: %d \n Jmin: %d \n Jmax: %d \n",inst_thread_data->reduction,inst_thread_data->dimensions,
            inst_thread_data->bounds.imin,inst_thread_data->bounds.imax,inst_thread_data->bounds.jmin,inst_thread_data->bounds.jmax);
    //printmatrix(*(inst_thread_data->inputmatrix),inst_thread_data->dimensions);
    int outputdimensions = inst_thread_data->dimensions/inst_thread_data->reduction;
    inst_thread_data->bounds.imax = outputdimensions;

    int i,j;
    //pthread_mutex_lock(&inst_thread_data->thread_mutex);
    for(j=inst_thread_data->bounds.jmin;j<=inst_thread_data->bounds.jmax;j+=1){ 
        for(i=inst_thread_data->bounds.imin;i<=inst_thread_data->bounds.imax;i+=1){
            int l,m;
            *(inst_thread_data->outputmatrix)[i*outputdimensions+j]=0.0;
            for(m=0;m<inst_thread_data->reduction;m++){
                for(l=0;l<inst_thread_data->reduction;l++){
                    printf("Attempt %d %d\n",l,m);
                    *(inst_thread_data->outputmatrix)[i*outputdimensions+j] += *(inst_thread_data->inputmatrix)[(i*inst_thread_data->reduction+l)*inst_thread_data->dimensions+(j*inst_thread_data->reduction+m)];
                }
            }
            *(inst_thread_data->outputmatrix)[i*outputdimensions+j] /= pow(2,inst_thread_data->reduction);

            //printf("Value: %f \n",*(inst_thread_data->outputmatrix)[i*outputdimensions+j]);
        }
   }
    //pthread_mutex_unlock(&inst_thread_data->thread_mutex);
}


//sliding window
void slidingmatrixreduce(double *inputmatrix, double *outputmatrix, int dimensions,int reduction){

    if(dimensions%reduction) errx(1,"dimension/reduction mismatch!");
    int outputdimensions = dimensions - 1;
    int i,j;
    for(j=0;j<outputdimensions;j++){
        for(i=0;i<outputdimensions;i++){
            int l,m;
            outputmatrix[i*outputdimensions+j] = 0.0;
            for(m=0;m<reduction;m++){
                for(l=0;l<reduction;l++){
                    outputmatrix[i*outputdimensions+j] += inputmatrix[(l+i)*dimensions+(j+m)];
                }
            }
            outputmatrix[i*outputdimensions+j] /= pow(2,reduction);
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
    int reduction = strtol(argv[2],NULL,10);
    int threads = strtol(argv[3],NULL,10);
    if(!dimensionmatrix){
        return 0;
    }

    struct thread_data thread_desc[threads];
    
    
    double *matrix = malloc(dimensionmatrix * dimensionmatrix * sizeof(double));
    double *reducedmatrix = malloc(((dimensionmatrix * dimensionmatrix))*sizeof(double));
//    double *slidingreducedmatrix = malloc((dimensionmatrix-1)*(dimensionmatrix-1)*sizeof(double));
    // Initialize our first matrix.
    //
    printf("Initial matrix: \n");
    initializematrix(matrix,dimensionmatrix);
    initializematrix(reducedmatrix,dimensionmatrix/2);
    printmatrix(matrix,dimensionmatrix);

    for(int i=0;i<threads;i++){

        thread_desc[i].bounds.imin=i*((dimensionmatrix/reduction)/threads);
        thread_desc[i].bounds.imax=(i+1)*((dimensionmatrix/reduction)/threads)-1;
        thread_desc[i].bounds.jmin=0;
        thread_desc[i].bounds.jmax=dimensionmatrix/reduction-1;
        thread_desc[i].dimensions=dimensionmatrix;
        thread_desc[i].reduction=reduction;

        thread_desc[i].inputmatrix = &matrix;
        thread_desc[i].outputmatrix = &reducedmatrix;
        
        if(pthread_create(&thread_desc[i].thread_id, NULL, matrixreduce, &thread_desc[i])) errx(1,"Thread creation failed.");
    }


    for(int i=0;i<threads;i++){
        pthread_join(thread_desc[i].thread_id,NULL);
    }


    //struct arrayindex reducearrayindex = {0};
    //matrixreduce(matrix,reducedmatrix,dimensionmatrix,reductions,reducearrayindex);


    printf("\n No overlap reduced matrix: \n");
    //printmatrix(reducedmatrix,2);
/*
    slidingmatrixreduce(matrix,slidingreducedmatrix,dimensionmatrix,reductions);
    printf("\n Sliding window reduced matrix: \n");
    printmatrix(slidingreducedmatrix,dimensionmatrix-1);
*/


return 0;
}
