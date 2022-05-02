/****************************************************************************
* Copyright (c) 2022, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Parser.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/41
//
//////////////////////////////////////////////////////////////////////////////

#include <Parser.h>
#include <StdFunction.h>
#include <StringTokenizer.h>

void debug(StringTokenizer*);

Constante Parser::c_pi;
ListUnaryFunction Parser::unary_func;
ListeConstantes Parser::les_cst;

/* Les identificateurs suivants doivent etre definis de maniere unique pour chaque fonction */

Parser::Parser()
{
  init_parser();
  state=0;
  root=NULL;
  str= new std::string("0");
  maxvar=1;
  ivar=0;
  les_var = new Variable*[maxvar];
  impuls_tn = -1.;
  impuls_T = 1.;
  impuls_t0 = 0.;
  impuls_tempo = 0.;
}

Parser::Parser(const Parser& p)
{
  init_parser();
  impuls_tn = p.impuls_tn;
  impuls_T = p.impuls_T;
  impuls_t0 = p.impuls_t0;
  impuls_tempo = p.impuls_tempo;

  state=p.state;
  maxvar=p.maxvar;
  ivar=p.ivar;

  root=NULL;

  str= new std::string(*p.str);

  les_var = new Variable*[maxvar];
  for (int i = 0; i < ivar; i++)
    {
      les_var[i] = new Variable(*(p.les_var[i]));
    }

  parseString();
}


Parser::Parser(std::string& s,int n)
{
  init_parser();
  impuls_tn = -1.;
  impuls_T = 1.;
  impuls_t0 = 0.;
  impuls_tempo = 0.;

  state=0;
  root=NULL;
  str= new std::string(s);
  maxvar=n;
  ivar=0;
  les_var = new Variable*[maxvar];
}


void Parser::init_parser()
{
  srand48(1);
  if (unary_func.size()==0)
    {
      Sin SIN;
      Asin ASIN;
      Cos COS;
      Acos ACOS;
      Tan TAN;
      Atan ATAN;
      Ln LN;
      Exp EXP;
      Sqrt SQRT;
      Int INT;
      Erf ERF;
      Rnd RND;
      Cosh COSH;
      Sinh SINH;
      Tanh TANH;
      Atanh ATANH;
      Not NOT;
      Abs ABS;
      Sgn SGN;
      addFunc(SIN);
      addFunc(ASIN);
      addFunc(COS);
      addFunc(ACOS);
      addFunc(TAN);
      addFunc(ATAN);
      addFunc(LN);
      addFunc(EXP);
      addFunc(SQRT);
      addFunc(INT);
      addFunc(ERF);
      addFunc(RND);
      addFunc(COSH);
      addFunc(SINH);
      addFunc(TANH);
      addFunc(NOT);
      addFunc(ABS);
      addFunc(SGN);
      addFunc(ATANH);
      c_pi.nommer(Nom("PI"));
      c_pi.setValue(2*asin(1.));
      addCst(c_pi);
    }
}


void destroy(PNode* p)
{
  if (p!=NULL)
    {
      if (p->left!=NULL) destroy(p->left);
      if (p->right!=NULL) destroy(p->right);
      delete p;
      p=NULL;
    }
}

Parser::~Parser()
{
  if (root !=NULL) destroy(root);
  for (int i =0; i<ivar; i++)
    delete les_var[i];
  delete str;
  delete[] les_var;
}


void Parser::setNbVar(int nvar)
{
  if (ivar>0)
    for (int i =0; i<ivar; i++)
      delete les_var[i];
  delete[] les_var;
  maxvar=nvar;
  ivar=0;
  les_var = new Variable*[maxvar];
}

void Parser::parseString()
{
  if (root!=NULL) destroy(root);
  PSTACK(PNode) st_ob(20);
  STACK(int) st_op(20);
  StringTokenizer tok(*str);
  if (!tok.check_GRP())
    {
      Cerr << "Expression " << str->c_str() << " does not contain the same number of opening and closing parenthesis." << finl;
      Process::exit();
    }

  state = 0;

  while ( (tok.nextToken()) != StringTokenizer::EOS)
    {
      switch (state)
        {
        case 0:
          parserState0(&tok,&st_ob,&st_op);
          break;
        case 1:
          parserState1(&tok,&st_ob,&st_op);
          break;
        case 2:
          parserState2(&tok,&st_ob,&st_op);
          break;
        case 3:
          break;
        default :
          break;
        }
    }
  parserState2(&tok,&st_ob,&st_op);
  root = (PNode*) *st_ob.getBase();
}

double Parser::evalFunc(PNode* node)
{
  /* OC : Nouvelle version : */
  if (node->value<=0)
    {
      UnaryFunction& f = unary_func[-node->value-1]; // OC attention, dans node->value c est l'oppose de l'indice de la func dans la liste
      // afin de distinguer operateur binaire (>0) et fonctions unaires (<0>
      // Il est donc necessaire de prendre -node->value ici pour referencer un element de la liste
      // De plus, on rajoute +1 car le zero ne doit pas etre utiliser pour les fonctions
      return f.eval(eval(node->left));
    }
  else
    {
      Cerr << "method evalFunc : Unknown func !!!" << finl;
      Process::exit();
      return -1;
    }
}

// Ne pas inliner car sinon Parser::eval(PNode* node) plus souvent appelee encore
// ne sera peut etre pas inlinee...
double Parser::evalOp(PNode* node)
{
  // PL 12/11/2010, reecriture avec switch pour optimisation
  double x = (node->left  != NULL ? eval(node->left)  : 0);
  double y = (node->right != NULL ? eval(node->right) : 0);
  switch (node->value)
    {
    case 0: // ADD
      return x + y;
    case 1: // SUBTRACT
      return  x - y;
    case 2: // MULTIPLY
      return x * y;
    case 3: // DIVIDE
      if (y==0)
        {
          Cerr << "Error in the Parser : x/y calculated with y equals 0. You are using a formulae with a division per 0." << finl;
          Process::exit();
        }
      return x/y;
    case 4: // POWER
      if (y != (int)(y) && x<0)
        {
          Cerr << "Error in the Parser : x^y calculated with negative value for x and y real." << finl;
          Process::exit();
        }
      return pow(x,y);
    case 5: // LT
      return (x<y)?1:0;
    case 6: // GT
      return (x>y)?1:0;
    case 7: // LE
      return (x<=y)?1:0;
    case 8: // GE
      return (x>=y)?1:0;
    case 9: // MOD
      return ((int)(x))%((int)(y));
    case 10: // MAX
      return (x>y)?x:y;
    case 11: // MIN
      return (x<y)?x:y;
    case 12: // AND
      return x && y;
    case 13: // OR
      return x || y;
    case 14: // EQ
      return (x == y);
    case 15: // NEQ
      return (x != y);
    default:
      Cerr << "Method evalOp : Unknown op " << (int)node->value << "!!!" << finl;
      Process::exit();
      return 0;
    }
}


int Parser::precedence(int op)
{
  int p=100;
  if (op == StringTokenizer::ADD)
    p=1;
  else if (op == StringTokenizer::SUBTRACT)
    p=2;
  else if (op ==  StringTokenizer::DIVIDE)
    p=3;
  else if (op ==  StringTokenizer::MULTIPLY)
    p=3;
  else if (op ==  StringTokenizer::POWER)
    p=5;
  else if (op ==  StringTokenizer::LT)
    p=0;
  else if (op ==  StringTokenizer::GT)
    p=0;
  else if (op ==  StringTokenizer::LE)
    p=0;
  else if (op ==  StringTokenizer::GE)
    p=0;
  else if (op ==  StringTokenizer::MOD)
    p=0;
  else if (op ==  StringTokenizer::MAX)
    p=0;
  else if (op ==  StringTokenizer::MIN)
    p=0;
  else if (op ==  StringTokenizer::AND)
    p=0;
  else if (op ==  StringTokenizer::OR)
    p=0;
  else if (op ==  StringTokenizer::EQ)
    p=0;
  else if (op ==  StringTokenizer::NEQ)
    p=0;
  else if (op ==  StringTokenizer::GRP)
    p=-1;
  else if (op ==  StringTokenizer::ENDGRP)
    p=-1;
  return p;
}

void Parser::parserState0(StringTokenizer* tokenizer, PSTACK(PNode)* ob, STACK(int)* op)
{

  if (tokenizer->type == StringTokenizer::STRING)
    {
      int trouv = searchVar(tokenizer->getSValue());

      if (trouv>-1)
        {
          PNode *node = new PNode();
          node->type = PNode::VAR;
          node->value = trouv;
          ob->push(node);
          state = 2;
        }
      else
        {
          const std::string& func = tokenizer->getSValue();
          trouv = searchFunc(func);
          if (trouv>-1)
            {
              op->push(-trouv); // OC 01/2005 : Attention, on stocke l'oppose de l'indice trouve afin de bien dissocier les operateurs binaires des fonctions unaires.
              state = 0;
            }
          else
            {
              trouv = searchCst(func);
              if (trouv>=0)
                {
                  PNode *node = new PNode();
                  node->type = PNode::VALUE;
                  node->nvalue = les_cst(trouv).getValue();
                  ob->push(node);
                  state = 2;
                }
              else
                {
                  Cerr << "Error in Parser::parserState0 during interpretation of the following string :\n ";
                  Cerr << str->c_str() << "\n";
                  Cerr << " identifier " << func.c_str() << " unknown " << finl;
                  // permet d avoir 0 erreur valgrind avec cppunit
                  root=(PNode*) *(ob->getBase());
                  Cerr << "List of known var "<<finl;
                  for (int i=0; i<ivar; i++)
                    Cerr<<les_var[i]->getString().c_str()<< " ";
                  Cerr<<finl;
                  Process::exit();
                }

            }

        }

    }

  else if (tokenizer->type == StringTokenizer::NUMBER)
    {
      PNode* node;
      node = new PNode();
      node->type = PNode::VALUE;
      //Cout << "NODE NUMBER = " << PNode::VALUE << finl;
      node->nvalue = tokenizer->getNValue();
      ob->push(node);
      state = 2;
    }
  else if (tokenizer->type ==  StringTokenizer::ADD )
    {
      PNode* node;
      op->push(StringTokenizer::ADD);
      node = new PNode();
      node->type = PNode::VALUE;
      node->nvalue = 0.;
      ob->push(node);
      state = 1;
    }
  else if (tokenizer->type ==  StringTokenizer::SUBTRACT)
    {
      PNode* node;
      op->push(StringTokenizer::SUBTRACT);
      node = new PNode();
      node->type = PNode::VALUE;
      node->nvalue = 0.;
      ob->push(node);
      state = 1;
    }
  else if (tokenizer->type ==  StringTokenizer::GRP )
    {
      op->push(StringTokenizer::GRP);
      state = 0;
    }
}

void Parser::parserState1(StringTokenizer* tokenizer, PSTACK(PNode)* ob, STACK(int)* op)
{
  if (tokenizer->type == StringTokenizer::STRING)
    {

      int trouv = searchVar(tokenizer->getSValue());

      if (trouv>-1)
        {
          //Cerr << "variable = " << trouv << " " << tokenizer->getSValue() << finl;
          PNode *node = new PNode();
          node->type = PNode::VAR;
          node->value = trouv;
          ob->push(node);
          state = 2;
        }
      else
        {
          const std::string& func = tokenizer->getSValue();
          trouv = searchFunc(func);
          if (trouv>-1)
            {
              op->push(-trouv);
              state = 0;
            }
          else
            {
              trouv = searchCst(func);
              if (trouv>=0)
                {
                  PNode *node = new PNode();
                  node->type = PNode::VALUE;
                  node->nvalue = les_cst(trouv).getValue();
                  ob->push(node);
                  state = 2;
                }
              else
                {
                  Cerr << "Error in Parser::parserState1 during interpretation of the following string :\n ";
                  Cerr << str->c_str() << "\n";
                  Cerr << " identifier " << func.c_str() << " unknown !! " << finl;
                  Cerr << "List of known var "<<finl;
                  for (int i=0; i<ivar; i++)
                    Cerr<<les_var[i]->getString().c_str()<< " ";
                  Cerr<<finl;
                  Process::exit();
                }

            }
        }
    }
  else if (tokenizer->type == StringTokenizer::NUMBER )
    {
      PNode* node = new PNode();
      node->type = PNode::VALUE;
      node->nvalue = tokenizer->getNValue();
      ob->push(node);
      state = 2;
    }
  else if (tokenizer->type ==StringTokenizer::GRP)
    {
      op->push(StringTokenizer::GRP);
      state = 0;
    }
  else
    {
      Cerr << "state 1 error !! " << finl;
      Process::exit();
    }

}



int Parser::test_op_binaire(int type)
{
  return ((type == StringTokenizer::ADD )||( type == StringTokenizer::SUBTRACT )||( type == StringTokenizer::MULTIPLY )||( type == StringTokenizer::DIVIDE )||( type == StringTokenizer::POWER )||( type == StringTokenizer::LT )||( type == StringTokenizer::GT )||( type == StringTokenizer::LE )||( type == StringTokenizer::GE )||( type == StringTokenizer::MOD) || ( type == StringTokenizer::MAX) || ( type == StringTokenizer::MIN) || ( type == StringTokenizer::AND)||( type == StringTokenizer::OR)||( type == StringTokenizer::EQ)||( type == StringTokenizer::NEQ));
}


void Parser::parserState2(StringTokenizer* tokenizer, PSTACK(PNode)* ob, STACK(int)* op)
{
  int op_read;
  if (test_op_binaire( tokenizer->type))
    {
      //                case   StringTokenizer::ADD: case StringTokenizer::SUBTRACT : case StringTokenizer::MULTIPLY: case StringTokenizer::DIVIDE: case StringTokenizer::POWER:
      op_read = tokenizer->type;
      //Cerr << "op read = " << (char) op_read << finl;
      if (!op->isEmpty())
        {
          int tmpi =  op->peek();
          //Cerr << "op tmpi = " <<  (char) tmpi << finl;
          while (precedence(tmpi) >= precedence(op_read) )
            {
              int tmp = op->pop();
              //Cerr << "op tmp = " <<  (char) tmp << finl;
              if (test_op_binaire( tmp))  // c est un operateur binaire
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
              else if (tmp<=0) // c est une fonction unaire
                {
                  PNode* un = (PNode*) ob->pop();
                  PNode* node = new PNode();
                  node->type=PNode::FUNCTION;
                  node->value=tmp;
                  node->left = un;
                  node->right=NULL;
                  ob->push(node);
                }
              else if ( (tmp == StringTokenizer::GRP) || (tmp == StringTokenizer::ENDGRP) )
                {
                }
              else
                {
                  Cerr << "Unknown operation !!! " << finl;
                  Process::exit();
                  break;
                }

              if (!op->isEmpty()) tmpi =  op->peek();
              else break ;
            }
        }
      op->push(op_read);
      state =1;
    }
  else if (tokenizer->type ==  StringTokenizer::ENDGRP)
    {
      op_read = tokenizer->type;
      if (!op->isEmpty())
        {
          int tmp = op->pop();
          while (tmp != StringTokenizer::GRP )
            {
              if (test_op_binaire(tmp))  // c est un operateur binaire
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
              else if (tmp<=0) // c est une fonction unaire
                {
                  PNode* un = (PNode*) ob->pop();
                  PNode* node = new PNode();
                  node->type=PNode::FUNCTION;
                  node->value=tmp;
                  node->left = un;
                  node->right=NULL;
                  ob->push(node);
                }
              else if ( (tmp == StringTokenizer::GRP) || (tmp == StringTokenizer::ENDGRP) )
                {
                }
              else
                {
                  Cerr << "Unknown operation !!! " << finl;
                  Process::exit();
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
          //Cout << "fin" << finl;
          int tmp =  op->pop();
          if (test_op_binaire(tmp))// c est un operateur binaire
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
          else if (tmp<=0) // c est une fonction unaire
            {
              PNode* un = (PNode*) ob->pop();
              PNode* node = new PNode();
              node->type=PNode::FUNCTION;
              node->value=tmp;
              node->left = un;
              node->right=NULL;
              ob->push(node);
            }
          else if ( (tmp == StringTokenizer::GRP) || (tmp == StringTokenizer::ENDGRP) )
            {
            }
          else
            {
              Cerr << "Unknown operation !!! " << finl;
              Process::exit();
              break;
            }
        }
      state = 0;
    }
  else
    {
      Cerr << "Error state 2 !!! " <<tokenizer->type<<"  "<<tokenizer->getSValue().c_str()<<"  "<<tokenizer->getNValue()<< finl;
      Cerr<<" StringTokenizer::ADD "<<StringTokenizer::ADD<<" StringTokenizer::SUBTRACT "<<StringTokenizer::SUBTRACT<<" StringTokenizer::MULTIPLY "<<StringTokenizer::MULTIPLY;
      Cerr<<" StringTokenizer::DIVIDE "<<StringTokenizer::DIVIDE<<" StringTokenizer::POWER "<<StringTokenizer::POWER<<" StringTokenizer::LT "<<StringTokenizer::LT<<" StringTokenizer::GT "<<StringTokenizer::GT;
      Cerr<<" StringTokenizer::LE "<<StringTokenizer::LE<<" StringTokenizer::GE "<<StringTokenizer::GE<<" StringTokenizer::MOD "<<StringTokenizer::MOD<<" ENDStringTokenizer::GRP "<<StringTokenizer::ENDGRP;
      Cerr<<" StringTokenizer::EOS "<<StringTokenizer::EOS<<" StringTokenizer::NUMBER ";
      Cerr<<StringTokenizer::NUMBER<<" StringTokenizer::STRING "<<StringTokenizer::STRING<<finl;
      Process::exit();
    }
}

void Parser::addVar(const char *vv)
{
  if (searchVar(vv)!=-1)
    {
      Cerr<<"Warning in Parser::addVar "<< vv << " already in Parser"<<finl;
      if (strcmp(vv,"x")==0 || strcmp(vv,"y")==0 || strcmp(vv,"z")==0 || strcmp(vv,"t")==0
          || strcmp(vv,"X")==0 || strcmp(vv,"Y")==0 || strcmp(vv,"Z")==0 || strcmp(vv,"T")==0)
        {
          Process::exit();
        }
    }
  if (ivar<maxvar)
    les_var[ivar++] = new Variable(vv);
  else
    {
      Cerr << "Maximum " << maxvar << " variables !! " << finl;
      Process::exit();
    }
}

int Parser::searchCst(const std::string& v)
{
  LIST_CURSEUR(Constante) curseur(les_cst);
  int i=0;
  Nom nv(v.c_str());
  while(curseur)
    {
      Constante& cst = ref_cast(Constante,curseur.valeur());
      std::string ss(cst.le_nom());  // TODO a voir majuscules
      if (nv == Nom(ss.c_str())) return i;
      ++curseur;
      i++;
    }
  return -1;
}


int Parser::searchFunc(const std::string& v)
{
  LIST_CURSEUR(DERIV(UnaryFunction)) curseur(unary_func);
  int i=0;
  Nom nv(v.c_str());
  nv.majuscule();
  while(curseur)
    {
      UnaryFunction& f = curseur.valeur().valeur();
      Nom n2(f.getName());
      n2.majuscule();
      if (nv == n2) return i+1 ; // OC: pour ne pas utiliser le zero car sinon conflit avec la nouvelle numerotation des operateurs binaires.
      ++curseur;
      i++;
    }
  return -1;
}

void Parser::addCst(const Constante& cst)
{
  les_cst.add(cst);
}

void Parser::addFunc(const UnaryFunction& f)
{
  int size = unary_func.size();
  for (int i=0; i<size; i++)
    {
      if (unary_func[i]->getName() == f.getName())
        {
          Cerr << "Function " <<  f.getName() << " has already been defined !" << finl;
          Process::exit();
        }
    }
  DERIV(UnaryFunction)& df = unary_func.add(DERIV(UnaryFunction)());
  df = f;
}

void debug(StringTokenizer * t)
{
  if (t->type == StringTokenizer::STRING)
    Cout << "STRING : " << t->getSValue().c_str() << finl;
  else if (t->type == StringTokenizer::NUMBER)
    Cout << "NUMBER : " << t->getNValue() << finl;
  else if (t->type == StringTokenizer::EOS)
    Cout << "END " << finl;
  else
    Cout << "OP : " << (int)t->type << finl;
}


