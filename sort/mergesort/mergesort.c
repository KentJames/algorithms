/*
  A generic out-of-place implementation of merge sort.

  Uses comparator function as inspired by glibc's qsort.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Compare function for if (int)a < (int)b.
int int_comparator(const void *a, const void *b){

  int av = *(const int *)a;
  int bv = *(const int *)b;
  return(av<bv);
}

int char_comparator(const void *a, const void *b){
  return *(char*)a < *(char*)b;
}


//Merges two arrays.
void merge(char* base, char *left, char *right, size_t size, size_t num,
	    int (*comparator)(const void*, const void*)){

  size_t i_left=0;
  size_t i_right=0;
  size_t result_i = 0;
  while(i_left < num && i_right < num){

    //    if((int)left[i_left*size]<(int)right[i_right*size]){
    if(comparator((void*)&left[i_left*size],(void*)&right[i_right*size])){
      //printf("True\n");
      base[result_i*size] = left[i_left*size];
      ++i_left;
    } else {
      //printf("False\n");
      base[result_i*size] = right[i_right*size];
      ++i_right;
    }
    ++result_i;
  }

  //Some elements left. Append them.
  while(i_left < num){
    base[result_i*size] = left[i_left*size];
    ++i_left;
    ++result_i;
  }
  while(i_right < num){
    base[result_i*size] = right[i_right*size];
    ++i_right;
    ++result_i;
  }
}

//Merge Sort Entry.
void merge_sort(void *arr, size_t size, size_t num,
		int (*comparator)(const void*, const void*)){

  char *base = (char *)arr;

  if(num <= 1) return;

  if((num % 2) ==1){
    realloc(base, size * (num+1)); //Even is always better.
    num = num + 1;
  }

  char *left = malloc(size * num/2);;
  char *right = malloc(size * num/2);;

  //Mem copies are SLOW. Probably a better way to do this.
  //Similar to the SWAP macro in libc/qsort.c?
  memcpy(left, base, size * num/2);
  memcpy(right, base+(size*num/2), size*num/2);

  merge_sort(left, size, num/2, comparator);
  merge_sort(right, size, num/2, comparator);

  merge(base, left, right, size, num/2, comparator);

  arr = base;

}

int main(){


  int array[16]={3,5,1,6,7,11,8,5,5,1,5,7,6,2,4};
  int arr_size = 16;
  printf("Sorting Numbers: \n\n");
  printf("Original Array:\n");

  for(int i = arr_size; i>0; --i) printf("%d ",array[arr_size-i]);

  merge_sort((void*)&array, sizeof(int), arr_size, int_comparator);

  printf("Sorted List:\n");
  for(int i = 0; i<arr_size; ++i) printf("%d ",array[i]);
  printf("\n\nSorting Characters\n\n");

  char str_arr[16] = "Hello Worldxzfdv";

  printf("Original String: \n");
  for(int i = 0; i<sizeof(str_arr);++i) printf("%c", str_arr[i]);


  merge_sort((void*)&str_arr, sizeof(char), sizeof(str_arr), char_comparator);

  printf("\nSorted String: \n");
  for(int i = 0; i<sizeof(str_arr);++i) printf("%c", str_arr[i]);

  printf("\n");
  return 0;
}
