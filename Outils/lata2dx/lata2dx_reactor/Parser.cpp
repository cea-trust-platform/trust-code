#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "PNode.h"
#include "String2.h"
#include "StringTokenizer.h"
#include "Parser.h"
#include "Stack.h"

void debug(StringTokenizer*);

const int Parser::ADD='+';
const int Parser::SUBTRACT='-';
const int Parser::DIVIDE='/';
const int Parser::MULTIPLY='*';
const int Parser::POWER='^';
const int Parser::GRP='(';
const int Parser::ENDGRP=')';
const int Parser::LT='<';
const int Parser::GT='>';
const int Parser::LE='[';
const int Parser::GE=']';

const int Parser::COS='c';
const int Parser::SIN='s';
const int Parser::TAN='t';
const int Parser::LN='l';
const int Parser::EXP='e';
const int Parser::SQRT='q';
const int Parser::ABS='a';

const char* Parser::COS_="COS";
const char* Parser::SIN_="SIN";
const char* Parser::TAN_="TAN";    
const char* Parser::LN_="LN";  
const char* Parser::EXP_="EXP";     
const char* Parser::SQRT_="SQRT";    
const char* Parser::ABS_="ABS";    



Parser::Parser()
{
cerr << "bponjour" << endl;
state=0;
root=NULL;
str=new String2((char*)"0");
maxvar=1;
ivar=0;
les_var = new Variable*[maxvar];
}


Parser::Parser(String2& s,int n)
{
state=0;
root=NULL;
str=new String2(s);
maxvar=n;
ivar=0;
les_var = new Variable*[maxvar];
}




void destroy(PNode* p)
{
if (p!=NULL)
{
if (p->left!=NULL) destroy(p->left);
if (p->right!=NULL) destroy(p->right);
delete p;
}
}

Parser::~Parser()
{
if (root !=NULL) destroy(root);
for (int i =0;i<ivar;i++)
	delete les_var[i];
delete str;
delete [] les_var;
}

	
void Parser::parseString2()
{
Stack<PNode*>* st_ob = new Stack<PNode*>(20);
Stack<int>* st_op = new Stack<int>(20);

StringTokenizer* tok=new StringTokenizer(*str);

state = 0;

//int type;
while ( (tok->nextToken()) != StringTokenizer::EOS)
	{
	//debug(tok);
	switch (state)
	{
	case 0:
		parserState0(tok,st_ob,st_op);
		break;
	case 1:
		parserState1(tok,st_ob,st_op);
		break;
	case 2:
		parserState2(tok,st_ob,st_op);
		break;
	case 3:
		break;
	default :
		break;
	}


	}
parserState2(tok,st_ob,st_op);


root = (PNode*) *st_ob->getBase();
//		cerr << "type root " << root->type << endl;
delete st_ob;
delete st_op;
delete tok;
}
		
double Parser::eval()
{
return eval(root);
}	
		
double Parser::eval(PNode* node)
{
double value = 0.;

if (node == NULL) 
	{
	cerr << "method eval : NULL node !!!" << endl;
	exit(-1);
	}
//cerr << "trype node = " << node->type	<< endl;
if  (node->type == PNode::OP) 
	{
	value = evalOp(node);
	//cerr << "resu = " << value << endl;
	}
else if  (node->type == PNode::VALUE)
	{
	value = node->nvalue;
	}
else if  (node->type == PNode::FUNCTION)
	{
	value = evalFunc(node);
	}
else if  (node->type == PNode::VAR)
	{
	value = evalVar(node);
	}
else
	{
		cerr << "method eval : Unknown type for this node !!!" << endl;
		exit(-1);
	}
		
	
return value;
}

double Parser::evalOp(PNode* node)
{
double value = 0.;

switch (node->value)
	{
	case ADD:
		if(node->left != NULL) value = eval(node->left);
		value += eval(node->right);		     
	 	break;
	case SUBTRACT:
		if(node->right != NULL) value = eval(node->right);
		value = eval(node->left) - value;
	 	break;
	case DIVIDE:
		value = eval(node->left);
		value /= eval(node->right);
		break;
	case MULTIPLY:
		value = eval(node->left);
		value *= eval(node->right);
		break;
	case POWER:
		value = pow(eval(node->left),eval(node->right));
		break;
	case LT:
		if (eval(node->left) < eval(node->right) )
			value = 1.;
		else value = 0.;
		break;
	case GT:
		if (eval(node->left) > eval(node->right) )
			value = 1.;
		else value = 0.;
		break;
	case LE:
		if (eval(node->left) <= eval(node->right) )
			value = 1.;
		else value = 0.;
		break;
	case GE:
		if (eval(node->left) >= eval(node->right) )
			value = 1.;
		else value = 0.;
		break;
	default:
		cerr << "method evalOp : Unknown op " << (char) node->value << "!!!" << endl;
		exit(-1);
	}
return value;
}

double Parser::evalFunc(PNode* node)
{
double value = 0.;

switch (node->value)
	{
	case COS:
		value = cos(eval(node->left));
	 	break;
	case SIN:
		value = sin(eval(node->left));
	 	break;
	case TAN:
		value = tan(eval(node->left));
		break;
	case LN:
		value = log(eval(node->left));
		break;
	case EXP:
		value = exp(eval(node->left));
		break;
	case SQRT:
		value = sqrt(eval(node->left));
		break;
		
	case ABS:
		value = fabs(eval(node->left));
		break;
		
	default:
		cerr << "method evalFunc : Unknown func !!!" << endl;
		exit(-1);
	}
return value;
}



double Parser::evalVar(PNode* node)
{
double value = 0.;
value = les_var[node->value]->getValue();
return value;
}


int Parser::precedence(int op)
{
switch (op)
	{
	case ADD:
		return 1;
	case SUBTRACT:
		return 2;
	case MULTIPLY:
		return 4;
	case DIVIDE:
		return 3;
	case POWER:
		return 5;
	case GRP:
		return -1;
	case ENDGRP:
		return -1;
	case LT:
		return 0;
	case GT:
		return 0;
	case LE:
		return 0;
	case GE:
		return 0;
	}
return 100;
}






void Parser::parserState0(StringTokenizer* tokenizer, Stack<PNode*>* ob, Stack<int>* op)
{

if (tokenizer->type == StringTokenizer::STRING)
	{
	int trouv = searchVar(tokenizer->getSValue());
	
	if (trouv>-1)
		{
		//cerr << "variable = " << trouv << " " << tokenizer->getSValue() << endl;
		PNode *node = new PNode();
		node->type = PNode::VAR;
		node->value = trouv;
		ob->push(node);
		state = 2;
		}
	else
		{
		const String2 & func = tokenizer->getSValue();
		if (func.compare(COS_) == 0)
			{
			op->push(COS);
			}
		else if (func.compare(SIN_) == 0)
			{
			op->push(SIN);
			}
		else if (func.compare(TAN_) == 0)
			op->push(TAN);
		else if (func.compare(LN_) == 0)
			op->push(LN);
		else if (func.compare(EXP_) == 0)
			op->push(EXP);
		else if (func.compare(SQRT_) == 0)
			op->push(SQRT);
		else if (func.compare(ABS_) == 0)
			op->push(ABS);
		else
			{
			cerr << "identificateur " << func << " inconnu !! " << endl;
			exit(-1);
			}
		state = 0;
		}
	
	}

else if (tokenizer->type == StringTokenizer::NUMBER)
	{
	PNode* node;
	node = new PNode();
	node->type = PNode::VALUE;
//cerr << "NODE NUMBER = " << PNode::VALUE << endl;
	node->nvalue = tokenizer->getNValue();
	ob->push(node);
	state = 2;
	}
else if (tokenizer->type ==  ADD )
	{
	PNode* node;
	op->push(ADD);
	node = new PNode();
	node->type = PNode::VALUE;
	node->nvalue = 0.;
	ob->push(node);
	state = 1;
	}
else if (tokenizer->type ==  SUBTRACT)
	{
	PNode* node;
	op->push(SUBTRACT);
	node = new PNode();
	node->type = PNode::VALUE;
	node->nvalue = 0.;
	ob->push(node);
	state = 1;
	}
else if (tokenizer->type ==  GRP )
	{
	op->push(GRP);
	state = 0;
	}
}

void Parser::parserState1(StringTokenizer* tokenizer, Stack<PNode*>* ob, Stack<int>* op)
{
if (tokenizer->type == StringTokenizer::STRING)
		{

	int trouv = searchVar(tokenizer->getSValue());
	
	if (trouv>-1)
		{
		//cerr << "variable = " << trouv << " " << tokenizer->getSValue() << endl;
		PNode *node = new PNode();
		node->type = PNode::VAR;
		node->value = trouv;
		ob->push(node);
		state = 2;
		}
	else
		{
		const String2 & func = tokenizer->getSValue();
		if (func.compare(COS_) == 0)
			{
			op->push(COS);
			}
		else if (func.compare(SIN_) == 0)
			{
			op->push(SIN);
			}
		else if (func.compare(TAN_) == 0)
			op->push(TAN);
		else if (func.compare(LN_) == 0)
			op->push(LN);
		else if (func.compare(EXP_) == 0)
			op->push(EXP);
		else if (func.compare(SQRT_) == 0)
			op->push(SQRT);
		else if (func.compare(ABS_) == 0)
			op->push(ABS);
		else
			{
			cerr << "identificateur " << func << " inconnu !! " << endl;
			exit(-1);
			}
		state = 0;
		}
/*			//System.out.println("s = "+tokenizer.sval);
			if (DEBUG) System.out.println(tokenizer.sval);
			Variable x = (Variable) les_var.get(tokenizer.sval);
			if (x != null)
				{
				PNode node = new PNode();
				node.type = PNode.VAR;
				node.svalue = tokenizer.sval;
				ob.push(node);
				state = 2;
				}
			else 
				{
				UnaryFunction f = (UnaryFunction) les_func.get(tokenizer.sval);
				if (f!=null)
					{
					op.push(f);
					state = 0;
					}
				else
					{
					Cst c = (Cst) les_cst.get(tokenizer.sval);
					if (c!=null)
						{
						PNode node = new PNode();
						node.type = PNode.VALUE;
						node.value = c.getValue();
						ob.push(node);
						state = 2;
						}
					else throw new ParseFunctionException("Unknown keyword "+tokenizer.sval+" !!");
					
					}
				}
			break;*/
	}
else if (tokenizer->type == StringTokenizer::NUMBER )
	{
	PNode* node = new PNode();
	node->type = PNode::VALUE;
	node->nvalue = tokenizer->getNValue();
	ob->push(node);
	state = 2;
	}
else if (tokenizer->type ==GRP)
	{
	op->push(GRP);
	state = 0;
	}
else
	{
	cerr << "etat 1 erreur !! " << endl;
	exit(-1);
	}

}
	

void Parser::parserState2(StringTokenizer* tokenizer, Stack<PNode*>* ob, Stack<int>* op)
{
int op_read;
if (test_op_binaire(tokenizer->type))
	{	
//		case   ADD: case SUBTRACT : case MULTIPLY: case DIVIDE: case POWER:  
	op_read = tokenizer->type;
	//cerr << "op read = " << (char) op_read << endl;		
	if (!op->isEmpty()) 
		{
		int tmpi =  op->peek();
	//cerr << "op tmpi = " <<  (char) tmpi << endl;		
		while (precedence(tmpi) >= precedence(op_read) )
			{
			int tmp = op->pop();
	//cerr << "op tmp = " <<  (char) tmp << endl;		
			switch(tmp)
				{
				case ADD : case SUBTRACT : case MULTIPLY : case DIVIDE : case POWER : case LT : case GT : case LE : case GE :
					{
					PNode* deux = (PNode*) ob->pop();
					PNode* un = (PNode*) ob->pop();
					PNode* node = new PNode();
					node->type = PNode::OP;
					node->value=tmp;
					node->left = un;
					node->right = deux;
					ob->push(node);
					}
					break;
				case COS : case SIN : case TAN : case LN : case EXP : case SQRT : case ABS :
					{
					PNode* un = (PNode*) ob->pop();
					PNode* node = new PNode();
					node->type=PNode::FUNCTION;
					node->value=tmp;
					node->left = un;
					node->right=NULL;
					ob->push(node);
					}
					break;
				case GRP: case ENDGRP :
					break;
					
				default:
					cerr << "Operation inconnue !!! " << endl;
					exit(-1);
					break;
				}			
			if (!op->isEmpty()) tmpi =  op->peek();
			else break ;
			}
		}
	op->push(op_read);
	state =1;
	}
else if (tokenizer->type ==  ENDGRP)
	{
	op_read = tokenizer->type;
	if (!op->isEmpty()) 
		{
		int tmp = op->pop();
		while (tmp != GRP )
			{
			switch(tmp)
				{
				case ADD : case SUBTRACT : case MULTIPLY : case DIVIDE : case POWER : case LT : case GT : case LE : case GE :
					{
					PNode* deux = (PNode*) ob->pop();
					PNode* un = (PNode*) ob->pop();
					PNode* node = new PNode();
					node->type = PNode::OP;
					node->value=tmp;
					node->left = un;
					node->right=deux;
					ob->push(node);
					}
					break;
				case COS : case SIN : case TAN : case LN : case EXP : case SQRT : case ABS:
					{
					PNode* un = (PNode*) ob->pop();
					PNode* node = new PNode();
					node->type=PNode::FUNCTION;
					node->value=tmp;
					node->left = un;
					node->right=NULL;
					ob->push(node);
					}
					break;
				case GRP: case ENDGRP :
					break;
				default:
					cerr << "Operation inconnue !!! " << endl;
					exit(-1);
					break;
				}
					
			if (!op->isEmpty()) tmp =  op->pop();
			else break ;
			}
		}
	state = 2;
	
	}
else if (tokenizer->type == StringTokenizer::EOS)
	{
	while (!op->isEmpty())
		{
		//cerr << "fin" << endl;
		int tmp =  op->pop();
		switch(tmp)
			{
			case ADD : case SUBTRACT : case MULTIPLY : case DIVIDE : case POWER : case LT : case GT : case LE : case GE :
				{
				PNode* deux = (PNode*) ob->pop();
				PNode* un = (PNode*) ob->pop();
				PNode* node = new PNode();
				node->type = PNode::OP;
				node->value=tmp;
				node->left = un;
				node->right=deux;
				ob->push(node);
				}
				break;
			case COS : case SIN : case TAN : case LN : case EXP : case SQRT : case ABS:
				{
				PNode* un = (PNode*) ob->pop();
				PNode* node = new PNode();
				node->type=PNode::FUNCTION;
				node->value=tmp;
				node->left = un;
				node->right=NULL;
				ob->push(node);
				}
				break;
				case GRP: case ENDGRP :
					break;
			default:
				cerr << "Operation inconnue !!! " << endl;
				exit(-1);
				break;
			}
		/*PNode* deux = (PNode*) ob->pop();
		PNode* un = (PNode*) ob->pop();
		PNode* node = new PNode();
		node->type = PNode::OP;
		node->value=tmp;
		node->left = un;
		node->right=deux;
		ob->push(node);*/
		}
	state = 0;
	}
else
	{
	cerr << "Erreur etat 2 !!! " << endl;
	exit(-1);
	}
}
	

void Parser::setVar(char * sv, double val)
	{
	//cerr << " Dans Parser::setVar(const String2& v, double val " << endl;
	String2 v(sv);
	int i = searchVar(v);
	if (i>-1) les_var[i]->setValue(val);
	else
		{
		cerr << " Variable " << v << " non existante !! " << endl;
		exit(-1);
		}
	}
	
void Parser::setVar(int i, double val)
	{
	if (i>-1 && i<maxvar) les_var[i]->setValue(val);
	else
		{
		cerr << " Variable numero " << i << " non existante !! " << endl;
		exit(-1);
		}
	}
	
void Parser::addVar(char *vv)
	{
	if (ivar<maxvar)
		les_var[ivar++] = new Variable(vv);
	else
		{
		cerr << "Maximum " << maxvar << " variables !! " << endl;
		exit(-1);
		}
	}
	
int Parser::searchVar(const String2&v) 
{
for (int i=0;i<ivar;i++)
	if (les_var[i]->getString2().compare(v) == 0 ) return i;
return -1;
}

int Parser::test_op_binaire(int type)
{
  return ( (type==Parser::ADD) || (type==Parser::SUBTRACT) || (type==Parser::DIVIDE) 
	   || (type==Parser::MULTIPLY) || (type==POWER) || (type==LT) || (type==GT)
	   || (type==LE) || (type==GE) );
}


void debug(StringTokenizer * t)
	{
	if (t->type == StringTokenizer::STRING)
		cerr << "STRING : " << t->getSValue() << endl;
	else if (t->type == StringTokenizer::NUMBER)
		cerr << "NUMBER : " << t->getNValue() << endl;
	else if (t->type == StringTokenizer::EOS)
		cerr << "FIN " << endl;
	else 
		cerr << "OP : " << (char) t->type << endl;
	}

