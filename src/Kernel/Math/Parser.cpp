/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <StringTokenizer.h>
#include <algorithm>
#include <Parser.h>
#include <queue>

void debug(StringTokenizer*);

/* Les identificateurs suivants doivent etre definis de maniere unique pour chaque fonction */

Parser::Parser()
{
  init_parser();
  state=0;
  root=nullptr;
  str= new std::string("0");
  impuls_tn = -1.;
  impuls_T = 1.;
  impuls_t0 = 0.;
  impuls_tempo = 0.;
  setNbVar(1);
}

Parser::Parser(const Parser& p)
{
  init_parser();
  state=p.state;
  root=nullptr;
  str= new std::string(*p.str);
  impuls_tn = p.impuls_tn;
  impuls_T = p.impuls_T;
  impuls_t0 = p.impuls_t0;
  impuls_tempo = p.impuls_tempo;
  setNbVar(p.maxvar);
  ivar=p.ivar;
  for (int i = 0; i < ivar; i++)
    {
      les_var[i] = p.les_var[i];
      les_var_names[i] = p.les_var_names[i];
    }
  parseString();
}

Parser::Parser(std::string& s, int n)
{
  init_parser();
  state=0;
  root=nullptr;
  str= new std::string(s);
  impuls_tn = -1.;
  impuls_T = 1.;
  impuls_t0 = 0.;
  impuls_tempo = 0.;
  setNbVar(n);
}

void Parser::init_parser()
{
  srand48(1);
  map_function_["SIN"] = static_cast<int>(FUNCTION::SIN);
  map_function_["ASIN"] = static_cast<int>(FUNCTION::ASIN);
  map_function_["COS"] = static_cast<int>(FUNCTION::COS);
  map_function_["ACOS"] = static_cast<int>(FUNCTION::ACOS);
  map_function_["TAN"] = static_cast<int>(FUNCTION::TAN);
  map_function_["ATAN"] = static_cast<int>(FUNCTION::ATAN);
  map_function_["LN"] = static_cast<int>(FUNCTION::LN);
  map_function_["EXP"] = static_cast<int>(FUNCTION::EXP);
  map_function_["SQRT"] = static_cast<int>(FUNCTION::SQRT);
  map_function_["INT"] = static_cast<int>(FUNCTION::ENT);
  map_function_["ERF"] = static_cast<int>(FUNCTION::ERF);
  map_function_["RND"] = static_cast<int>(FUNCTION::RND);
  map_function_["COSH"] = static_cast<int>(FUNCTION::COSH);
  map_function_["SINH"] = static_cast<int>(FUNCTION::SINH);
  map_function_["TANH"] = static_cast<int>(FUNCTION::TANH);
  map_function_["NOT"] = static_cast<int>(FUNCTION::NOT);
  map_function_["ABS"] = static_cast<int>(FUNCTION::ABS);
  map_function_["SGN"] = static_cast<int>(FUNCTION::SGN);
  map_function_["ATANH"] = static_cast<int>(FUNCTION::ATANH);
  c_pi.nommer(Nom("PI"));
  c_pi.setValue(2*asin(1.));
  addCst(c_pi);
}

void destroy(PNode* p)
{
  if (p!=nullptr)
    {
      if (p->left!=nullptr) destroy(p->left);
      if (p->right!=nullptr) destroy(p->right);
      delete p;
      p=nullptr;
    }
}

Parser::~Parser()
{
  if (root !=nullptr) destroy(root);
  delete str;
}

void Parser::setNbVar(int nvar)
{
  maxvar=nvar;
  ivar=0;
  les_var.resize(maxvar);
  les_var_names.dimensionner_force(maxvar);
}

void Parser::parseString()
{
  if (root!=nullptr) destroy(root);
  PSTACK(PNode) st_ob(20);
  STACK(int) st_op(20);
  StringTokenizer tok(*str);
  if (!tok.check_GRP())
    {
      Cerr << "Expression " << *str << " does not contain the same number of opening and closing parenthesis." << finl;
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

  // Conversion
  PNodes.clear();
  std::unordered_map<PNode*, int> nodeToIndex;
  std::queue<PNode*> queue;
  int currentIndex = 0;

  // Start the traversal from the root
  queue.push(root);
  nodeToIndex[root] = currentIndex++;

  while (!queue.empty())
    {
      PNode* current = queue.front();
      queue.pop();

      // Create a PNodePod from the current PNode
      PNodePod pod;
      pod.type = current->type;
      pod.value = current->value;
      pod.nvalue = current->nvalue;

      // Process the left child
      if (current->left)
        {
          if (nodeToIndex.find(current->left) == nodeToIndex.end())
            {
              nodeToIndex[current->left] = currentIndex++;
              queue.push(current->left);
            }
          pod.left = nodeToIndex[current->left];
        }
      else
        {
          pod.left = -1; // Indicate no child
        }

      // Process the right child
      if (current->right)
        {
          if (nodeToIndex.find(current->right) == nodeToIndex.end())
            {
              nodeToIndex[current->right] = currentIndex++;
              queue.push(current->right);
            }
          pod.right = nodeToIndex[current->right];
        }
      else
        {
          pod.right = -1; // Indicate no child
        }

      PNodes.push_back(pod);
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
          node->type = PNode_type::VAR;
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
                  node->type = PNode_type::VALUE;
                  node->nvalue = les_cst(trouv).getValue();
                  ob->push(node);
                  state = 2;
                }
              else
                {
                  Cerr << "Error in Parser::parserState0 during interpretation of the following string :\n ";
                  Cerr << *str << "\n";
                  Cerr << " identifier " << func << " unknown " << finl;
                  // permet d avoir 0 erreur valgrind avec cppunit
                  root=(PNode*) *(ob->getBase());
                  Cerr << "List of known var "<<finl;
                  for (auto name : les_var_names)
                    Cerr<<name<< " ";
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
      node->type = PNode_type::VALUE;
      //Cout << "NODE NUMBER = " << PNode_type::VALUE << finl;
      node->nvalue = tokenizer->getNValue();
      ob->push(node);
      state = 2;
    }
  else if (tokenizer->type ==  StringTokenizer::ADD )
    {
      PNode* node;
      op->push(StringTokenizer::ADD);
      node = new PNode();
      node->type = PNode_type::VALUE;
      node->nvalue = 0.;
      ob->push(node);
      state = 1;
    }
  else if (tokenizer->type ==  StringTokenizer::SUBTRACT)
    {
      PNode* node;
      op->push(StringTokenizer::SUBTRACT);
      node = new PNode();
      node->type = PNode_type::VALUE;
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
          node->type = PNode_type::VAR;
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
                  node->type = PNode_type::VALUE;
                  node->nvalue = les_cst(trouv).getValue();
                  ob->push(node);
                  state = 2;
                }
              else
                {
                  Cerr << "Error in Parser::parserState1 during interpretation of the following string :\n ";
                  Cerr << *str << "\n";
                  Cerr << " identifier " << func << " unknown !! " << finl;
                  Cerr << "List of known var "<<finl;
                  for (auto name : les_var_names)
                    Cerr<<name<< " ";
                  Cerr<<finl;
                  Process::exit();
                }
            }
        }
    }
  else if (tokenizer->type == StringTokenizer::NUMBER )
    {
      PNode* node = new PNode();
      node->type = PNode_type::VALUE;
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
                  node->type = PNode_type::OP;
                  node->value=tmp;
                  node->left = un;
                  node->right = deux;
                  ob->push(node);
                }
              else if (tmp<=0) // c est une fonction unaire
                {
                  PNode* un = (PNode*) ob->pop();
                  PNode* node = new PNode();
                  node->type=PNode_type::FUNCTION;
                  node->value=tmp;
                  node->left = un;
                  node->right=nullptr;
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
                  node->type = PNode_type::OP;
                  node->value=tmp;
                  node->left = un;
                  node->right=deux;
                  ob->push(node);
                }
              else if (tmp<=0) // c est une fonction unaire
                {
                  PNode* un = (PNode*) ob->pop();
                  PNode* node = new PNode();
                  node->type=PNode_type::FUNCTION;
                  node->value=tmp;
                  node->left = un;
                  node->right=nullptr;
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
              node->type = PNode_type::OP;
              node->value=tmp;
              node->left = un;
              node->right=deux;
              ob->push(node);
            }
          else if (tmp<=0) // c est une fonction unaire
            {
              PNode* un = (PNode*) ob->pop();
              PNode* node = new PNode();
              node->type=PNode_type::FUNCTION;
              node->value=tmp;
              node->left = un;
              node->right=nullptr;
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
      Cerr << "Error state 2 !!! " <<tokenizer->type<<"  "<<tokenizer->getSValue()<<"  "<<tokenizer->getNValue()<< finl;
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
    {
      Nom s(vv);
      s.majuscule();
      les_var.resize(ivar+1);
      les_var_names[ivar] = s;
      ivar++;
    }
  else
    {
      Cerr << "Maximum " << maxvar << " variables !! " << finl;
      Process::exit();
    }
}

int Parser::searchCst(const std::string& v)
{
  int i=0;
  Nom nv(v);
  for (auto& itr : les_cst)
    {
      Constante& cst = ref_cast(Constante,itr);
      std::string ss(cst.le_nom());
      std::transform(ss.begin(), ss.end(), ss.begin(), ::toupper);
      if (nv == Nom(ss)) return i;
      i++;
    }
  return -1;
}


int Parser::searchFunc(const std::string& f)
{
  std::string function_name(f);
  std::transform(function_name.begin(), function_name.end(), function_name.begin(), ::toupper);
  auto it = map_function_.find(function_name);
  if (it != map_function_.end())
    return it->second + 1; // OC: pour ne pas utiliser le zero car sinon conflit avec la nouvelle numerotation des operateurs binaires.
  else
    return -1;
}

void Parser::addCst(const Constante& cst)
{
  les_cst.add(cst);
}

void debug(StringTokenizer * t)
{
  if (t->type == StringTokenizer::STRING)
    Cout << "STRING : " << t->getSValue() << finl;
  else if (t->type == StringTokenizer::NUMBER)
    Cout << "NUMBER : " << t->getNValue() << finl;
  else if (t->type == StringTokenizer::EOS)
    Cout << "END " << finl;
  else
    Cout << "OP : " << (int)t->type << finl;
}


