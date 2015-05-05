#include "PNode.h"
#include <iostream>



const int PNode::NOTHING=-1;
const int PNode::OP=1;
const int PNode::VALUE=2;
const int PNode::FUNCTION=3;
const int PNode::VAR=4;
const int PNode::PARAMETER=5;



PNode::PNode() 
{
left = NULL;
right = NULL;
type = NOTHING;
value = 0;
} 
        
PNode::PNode(PNode* n) 
{
replace(n);
}

void PNode::replace(PNode* n) 
{
if(n == NULL) return;
type = n->type;
left = n->left;
right = n->right;
value = n->value;
}

