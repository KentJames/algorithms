#include <stdio.h>
#include <stdlib.h>


#define N 512
#define MAX_DEPTH 1

typedef struct treeNode{

  double *grid; //Pointer to our part of the grid.
  int size; // Size of this subgrid

  //Pointers to the next level of the tree.
  struct treeNode *NorthWest; 
  struct treeNode *NorthEast;
  struct treeNode *SouthWest;
  struct treeNode *SouthEast;
} treeNode;

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
    Root->grid = (double*)malloc(size*size*sizeof(double));
    printf("Allocting Grid! \n");
  }

  return Root;
    
}

//Free all data at leaves of static tree.
treeNode* DeleteStaticTree(treeNode *Root){


  if(Root->NorthWest == NULL && Root->NorthEast == NULL){
    //Assume we are at a leaf
    printf("Free Leaf");
    free(Root->grid);
  }
  else{
    DeleteStaticTree(Root->NorthWest);
    DeleteStaticTree(Root->NorthEast);
    DeleteStaticTree(Root->SouthWest);
    DeleteStaticTree(Root->SouthEast);
  }

}
    

int main() {

  treeNode *tree = InitNode();
  StaticTree(tree,N,MAX_DEPTH);
  DeleteStaticTree(tree);

  
  return 0;
}
