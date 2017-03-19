#include <stdio.h>
#include <stdlib.h>

//Our tree node datatype
typedef struct treeNode{
    int key;
    int value;
    struct treeNode *left;
    struct treeNode *right;
} treeNode;

treeNode* search_tree(treeNode *node, int key){
    //If it is the key then return it. Similarly if the key points nowhere then also return.
    if (node->key==key || node==NULL){
       return node;
    }
    else if(key > node->key){
       return search_tree(node->right, key);
    }
    
    return search_tree(node->left, key);
    

}

treeNode *insert(treeNode *Root,int key, int value){
    if(Root==NULL){
        treeNode *temp = (treeNode*)malloc(sizeof(treeNode));
        temp->key=key;
        temp->value=value;
        temp->left = temp->right = NULL;
        return temp;
    }
    else if(key> Root->key){
        Root->right = insert(Root->right,key,value);
    }
    else{
        Root->left = insert(Root->left,key,value);
    }
    return Root;

}

treeNode* minValueNode(treeNode *Root){

    treeNode *current = Root;
    while(current->left != NULL){
        current = current->left;
    }
    return current;
}

treeNode* delete(treeNode *Root,int key){

    if(Root == NULL) return Root;
    
    if(key > Root->key){
        Root->right = delete(Root->right,key);
    }
    else if(key < Root->key){
        Root->left = delete(Root->left,key);
    }
    else {
        if(Root->left == NULL){
            treeNode *temp = Root->right;
            free(Root);
            return temp;
        }
        else if(Root->right == NULL){
            treeNode *temp = Root->left;
            free(Root);
            return temp;
        }
        else {
            treeNode *temp = minValueNode(Root->right);
            Root->key = temp->key;
            Root->value = temp->value; 
            Root->right = delete(Root->right,temp->key);
        }
    }
    return Root;

}

int main(){


    treeNode *root = insert(NULL,5,12);
    insert(root,7,15);
    insert(root,4,10);
    insert(root,11,10);
    insert(root,12,10);
    insert(root,6,12);
    insert(root,13,15);

    
    //printf("Temp left: %d \n",temp->right);

    treeNode *searchnode = search_tree(root,12);
    if(searchnode != NULL){
        printf("Value: %d \n",searchnode->value);
    }
    delete(root,4);

    return(0);
}
