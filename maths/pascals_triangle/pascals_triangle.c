#include <stdio.h>
#include <stdlib.h>

#define TOTAL_ROWS 5


void print_triangle(double *triangle, int rows){

    int pointerindex=0;
    for(int i=0;i<=rows;i++){
        printf("Row %d: ",i);
        for(int j=0;j<=i;j++){
            printf("%f ",*(triangle+pointerindex+j));

        }
        printf("\n");
        pointerindex+=i;

    }



}

int main(){

    int totaltrianglesize;

    //Iteratively work out our total triangle size in memory.
    for(int i=1;i<=TOTAL_ROWS;i++) totaltrianglesize+=i;

    double *pascalstriangle = malloc(totaltrianglesize*sizeof(double));

    //Need seperate variable for pointer index due to triangular nature of array.
    int pointerindex=0;
    for(int i=0;i<=TOTAL_ROWS;i++){
        
        
        for(int j=0;j<=i;j++){
        const int maximum_length = i;
            
            //Set triangle edges to equal 1, everything between is the binomial coefficients (sum of two numbers above it).
            if(j== 0){
                *((pascalstriangle)+pointerindex+j)=1;
            }
            else if(j==i){
                *((pascalstriangle)+pointerindex+j)=1;
            }
            else{
                *((pascalstriangle)+pointerindex+j)   =  *((pascalstriangle)+(pointerindex-i)+(j+1))+*((pascalstriangle)+(pointerindex-i)+j);
            }
        }

        pointerindex+=i;
        }
         
    //Print out our lovely triangle.
    print_triangle(pascalstriangle,TOTAL_ROWS);

    return(0);
}
