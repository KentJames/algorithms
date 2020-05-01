#include <stdio.h>
#include <stdint.h>

#define MASK ((1 << 16) -1) // Clear all MSB

float floating_point_fn(){

    const float a = 65.738;
    const float b = 23.49;

    return a*b;
}

uint32_t fixed_point_fn(){
    
    const uint32_t a = (65 << 16) + 738;
    const uint32_t b = (23 << 16) + 49;

    const uint32_t ahi = a >> 16;
    const uint32_t alo = a & MASK;
    const uint32_t bhi = b >> 16;
    const uint32_t blo = b & MASK;

    const uint32_t lowterm = alo * blo;
    const uint32_t highterm = ahi * bhi;
    const uint32_t crossterms = bhi*alo + ahi*blo;

    
    const uint32_t result = (a << 2) + highterm + (crossterms >> 16) + 1;
    
    return result;
}




int main(){


    printf("Floating Point Value: %f \n",floating_point_fn());
    long long fpf = fixed_point_fn();
    printf("Fixed Point Value: %lld %lld \n",fpf>>16, fpf&MASK);
    return 0;    
}
