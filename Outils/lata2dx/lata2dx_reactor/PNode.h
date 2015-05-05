#ifndef PNode_inclus
#define PNode_inclus 




class PNode 
{
public :
static const int NOTHING;
static const int OP;
static const int VALUE;
static const int FUNCTION;
static const int VAR;
static const int PARAMETER;

PNode* left;
PNode* right;
int type;
int value;
double nvalue;

PNode();
PNode(PNode*);

void replace(PNode*);





private :




};




#endif
