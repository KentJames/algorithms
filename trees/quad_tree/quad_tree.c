#include <stdio.h>
#include <stdlib.h>

#define N 8
#define MAX_DEPTH 2

typedef struct treeNode{

  double *grid; //Pointer to our part of the grid.
  int size; // Size of this subgrid

  //Pointers to the next level of the tree.
  struct treeNode *NorthWest; 
  struct treeNode *NorthEast;
  struct treeNode *SouthWest;
  struct treeNode *SouthEast;
} treeNode;


// Fills array with some numbers
void FillGrid(double* grid,int size){

  int x,y;

  for(y=0;y<size;y++){
    for(x=0;x<size;x++){
      *(grid + y*size + x) = (x+1)*(y+1);
    }
  }

}

// Prints out every number in array.
void PrintGrid(double* grid, int size){

  int x,y;

  for(y=0;y<size;y++){
    for(x=0;x<size;x++){
      printf("%f ",*(grid+y*size+x));
    }
    printf("\n");
  }

}




//Returns empty treeNode.
treeNode* InitNode(){

  treeNode *temp = (treeNode*)malloc(sizeof(treeNode));
  temp->NorthWest = temp->NorthEast =
    temp->SouthWest = temp->SouthEast = NULL;
  return temp;

}

//Statically divided quad tree. Data allocated at leaves.
treeNode* StaticTree(treeNode *Root, int size, int depth){

  if(depth>0){
    Root->NorthWest = StaticTree(InitNode(),size/2,depth-1);
    Root->NorthEast = StaticTree(InitNode(),size/2,depth-1);
    Root->SouthWest = StaticTree(InitNode(),size/2,depth-1);
    Root->SouthEast = StaticTree(InitNode(),size/2,depth-1);
  }
  else{
    //Substitute this for whatever allocation you want.
    Root->grid = (double*)malloc(size*size*sizeof(double));
  }

  return Root;
    
}
//TODO: change to callback.
//Free all data at leaves of static tree.
void DeleteStaticTree(treeNode *Root){


  if(Root->NorthWest == NULL && Root->NorthEast == NULL &&
     Root->SouthWest == NULL && Root->SouthEast == NULL){
    //Assume we are at a leaf
    free(Root->grid);
  }
  else{
    DeleteStaticTree(Root->NorthWest);
    DeleteStaticTree(Root->NorthEast);
    DeleteStaticTree(Root->SouthWest);
    DeleteStaticTree(Root->SouthEast);

    free(Root->NorthWest);
    free(Root->NorthEast);
    free(Root->SouthWest);
    free(Root->SouthEast);
  }

}

//Statically distributes a grid to equal subgrids on the leaf of a quad-tree.
treeNode* DistributeGridTree(double *grid, treeNode *Root, int size, int depth){


  if(depth > 0){
    
    int sizen = size/2;
    printf("Size: %d\n",sizen);
    //Allocate our subgrids.
    double *temp_nw = malloc(sizen * sizen * sizeof(double));
    double *temp_ne = malloc(sizen * sizen * sizeof(double));
    double *temp_sw = malloc(sizen * sizen * sizeof(double));
    double *temp_se = malloc(sizen * sizen * sizeof(double));

    //Define new bounds in sub-memory.
    int nw_min_x, nw_max_x, nw_min_y, nw_max_y;
    int ne_min_x, ne_max_x, ne_min_y, ne_max_y;
    int sw_min_x, sw_max_x, sw_min_y, sw_max_y;
    int se_min_x, se_max_x, se_min_y, se_max_y;



    //Now Copy our Memory. This can be made more efficient with memcpy.
    //More trying to demonstrate the point.

   
    int x,y;

    //NorthWest

    for (y = 0; y < sizen; y++){
      for(x = 0; x < sizen; x++){
	*(temp_nw+y*sizen+x) = *(grid + y*size + x);
      }
    }
 
    //NorthEast
    for (y = 0; y < sizen; y++){
      for(x = 0; x < sizen; x++){
	*(temp_ne+y*sizen+x) = *(grid + y*size + (x+sizen));
      }
    }
   
    //SouthWest
   
    for (y = 0; y< sizen; y++){
      for (x = 0; x< sizen; x++){
	*(temp_sw + y*sizen+x) = *(grid + (y+sizen)*size + x);
      }
    }

    //SouthEast
   
    for (y = 0; y< sizen; y++){
      for (x = 0; x< sizen; x++){
	*(temp_se + y*sizen+x) = *(grid + (y+sizen)*size + (x+sizen));
      }
    }


    //Print Grids..
    printf("NW Grid: \n");
    PrintGrid(temp_nw,size/2);
    printf("NE Grid: \n");
    PrintGrid(temp_ne,size/2);
    printf("SW Grid: \n");
    PrintGrid(temp_sw,size/2);
    printf("SE Grid: \n");
    PrintGrid(temp_se,size/2);
    
    //Finally, recurse down a level!
    Root->NorthWest = DistributeGridTree(temp_nw,InitNode(),size/2,depth-1);
    Root->NorthEast = DistributeGridTree(temp_ne,InitNode(),size/2,depth-1);
    Root->SouthWest = DistributeGridTree(temp_sw,InitNode(),size/2,depth-1);
    Root->SouthEast = DistributeGridTree(temp_se,InitNode(),size/2,depth-1);
    
    
  }
  else{
    //Thise node is a leaf. Assign grid here.
    Root->grid = grid;
  }

  return Root;


}

    

int main() {

  //Basic Quad-Tree
  treeNode *tree = InitNode();
  StaticTree(tree,N,MAX_DEPTH);
  DeleteStaticTree(tree);


  //Distribute a grid recursively...
  double *grid = malloc(N * N * sizeof(double));
  FillGrid(grid,N);
  PrintGrid(grid,N);
  treeNode *disroot = InitNode();
  printf("DistributeGridTree: \n");
  DistributeGridTree(grid, disroot, N, MAX_DEPTH);
  DeleteStaticTree(disroot);
  
  return 0;
}
