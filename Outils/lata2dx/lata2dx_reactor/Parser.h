
#ifndef Parser_inclus
#define Parser_inclus

#include "String.h"
#include "StringTokenizer.h"
#include "Stack.h"
#include "PNode.h"
#include "Variable.h"


class Parser
{
public :


/**
* Initialise le parser avec uen chaine "0" : ne sert a rien !!
*/
Parser();
~Parser();


/**
* Construit un objet Parser avec une chaine specifiee et un nb max de variables a indiquer avec la methode addVar.
*/
Parser(String&,int n =1);


/**
* Construit l'arbre correspondant a la chaine de caracteres. Cet arbre doit etre construit une seule fois et la chaine de caractere est evaluee en parcourant cet arbre par la methode eval() autant de fois qu'on le souhaite.
*/
void parseString();


/**
* Sert a evaluer l'expression mathematique correspondante a la chaine de caracteres. Poru cela il faut avant toute chose construire l'arbre par la methode parseString(). 
*/double eval();

/**
* Fixe la valeur de la variable representee par une chaine String v.
*/
void setVar(char* sv, double val);

/**
* Fixe la valeur de la variable de numero specifie. Ce numero correspondt a l'ordre de l'ajout des variables par la methode addVar().
*/
void setVar(int i, double val);

/**
* permet d'ajouter une variable en specifiant sa chaine representative (par ex. : x, y1 etc...)
*/
void addVar(char *);





private:
 int test_op_binaire(int type);

static int precedence(int);
double eval(PNode*);
double evalOp(PNode*);
double evalVar(PNode*);
double evalFunc(PNode*);
void parserState0(StringTokenizer*,Stack<PNode*>* ,Stack<int>*);
void parserState1(StringTokenizer*,Stack<PNode*>* ,Stack<int>*);
void parserState2(StringTokenizer*,Stack<PNode*>* ,Stack<int>*);
int searchVar(const String&v);


int state;
static const int ADD;
static const int SUBTRACT;
static const int DIVIDE;    
static const int MULTIPLY;  
static const int POWER;     
static const int GRP;       
static const int ENDGRP;    
static const int LT;    
static const int GT;    
static const int LE;    
static const int GE;    

static const int COS;
static const int SIN;
static const int TAN;    
static const int LN;  
static const int EXP;     
static const int SQRT;     
static const int ABS;     

static const char* COS_;
static const char* SIN_;
static const char* TAN_;    
static const char* LN_;  
static const char* EXP_;     
static const char* SQRT_;     
static const char* ABS_;     

PNode* root;
String* str;
Variable** les_var;
int maxvar,ivar;
};

#endif
