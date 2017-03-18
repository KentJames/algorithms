#include <stdio.h>
#include <stdlib.h>

//Brute force method needs some sort of upper cap.
#define MAX_PF 5000000


int main(){

    char prime[16],*endp;
    int primes[10];
    printf("Enter a number to prime factor: ");
    fgets(prime,sizeof(prime),stdin);

    int primenumber = strtol(prime,&endp,0);
    int original_prime = primenumber;
    if(*endp == '\n' || *endp == '\0'){
        printf("Number to factorise is %d \n",primenumber);
    }
    else {
        printf("Bad format specified! \n");
    }
    int j=0;
    for(int i=1;i<=MAX_PF;i+=2){
        if (primenumber%i == 0) {
            printf("Prime factor found %d \n ",i);
            primenumber/=i;
            primes[j] = i;
            j++;
        }
        else {
            continue;
        }
    }
    int check_var = 1;

    for(int i=0;i<=j;i++){
        check_var = check_var* primes[i];
        if (check_var == original_prime) {
            printf("Original prime found. It works! \n");
            break;
        }
    }
    return(0);
}
