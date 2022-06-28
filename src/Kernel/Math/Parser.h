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

#ifndef Parser_included
#define Parser_included

#include <List_Deriv_UnaryFunction.h>
#include <List_Constante.h>
#include <Variable.h>
#include <Stack.h>
#include <math.h>
#include <string>
#include <algorithm>
class StringTokenizer;

/////////////////////////////////////////////////////////////////////////=
//// .DESCRIPTION
////    Representation des donnees de la classe Parser
//// .SECTION voir aussi
///////////////////////////////////////////////////////////////////////////=

class Parser
{
public :


  /**
   * Initialise le parser avec une chaine "0" : ne sert a rien !!
   */
  Parser();
  Parser(const Parser&);
  ~Parser();


  /**
   * Construit un objet Parser avec une chaine specifiee et un nb max de variables a indiquer avec la methode addVar.
   */
  Parser(std::string&,int n =1);

  /**
   *
   */
  void init_parser();


  /**
   * Construit l'arbre correspondant a la chaine de caracteres. Cet arbre doit etre construit une seule fois et la chaine de caractere est evaluee en parcourant cet arbre par la methode eval() autant de fois qu'on le souhaite.
   */
  void parseString();


  /**
   * Sert a evaluer l'expression mathematique correspondante a la chaine de caracteres. Poru cela il faut avant toute chose construire l'arbre par la methode parseString().
   */
  inline double eval();

  /**
   * Fixe la valeur de la variable representee par une chaine sv.
   */
  inline void setVar(const char* sv, double val);

  /**
  * Fixe la valeur de la variable representee par v.
  */
  inline void setVar(const std::string& v, double val);

  /**
  * Fixe la valeur de la variable de numero specifie. Ce numero correspondt a l'ordre de l'ajout des variables par la methode addVar().
  */
  inline void setVar(int i, double val);

  /**
   * Fixe le nombre max de variables a indiquer avec la methode addVar.
   */
  void setNbVar(int nvar);

  /**
   * permet d'ajouter une variable en specifiant sa chaine representative (par ex. : x, y1 etc...)
   */
  void addVar(const char *);



  inline std::string& getString()
  {
    return *str;
  }
  inline void setString(const std::string& s)
  {
    delete str;
    str = new std::string(s);
  }
  void addCst(const Constante& cst);
  void addFunc(const UnaryFunction& f);


  /**
   * Fixe le temps initial et la periode de la fonction impulsion
   */
  inline void setImpulsion(double tinit, double periode)
  {
    impuls_t0 = tinit;
    impuls_T = periode;
    impuls_tn = tinit-periode;
    impuls_tempo = impuls_t0;
  }

private:
  int test_op_binaire(int type);

  static int precedence(int);
  inline double eval(PNode*);
  double evalOp(PNode*);
  double evalVar(PNode*);
  double evalFunc(PNode*);
  void parserState0(StringTokenizer*,PSTACK(PNode)* ,STACK(int)*);
  void parserState1(StringTokenizer*,PSTACK(PNode)* ,STACK(int)*);
  void parserState2(StringTokenizer*,PSTACK(PNode)* ,STACK(int)*);
  inline int searchVar(const std::string&);
  inline int searchVar(const char*);
  int searchCst(const std::string& v);
  int searchFunc(const std::string& v);
  int state;
  static Constante c_pi ;
  double impuls_T;
  double impuls_t0;
  double impuls_tn;
  double impuls_tempo;

  PNode* root;
  std::string* str;
  Variable** les_var;
  static ListeConstantes les_cst;
  static ListUnaryFunction unary_func;
  int maxvar,ivar;
};

inline double Parser::eval()
{
  return eval(root);
}

inline double Parser::evalVar(PNode* node)
{
  double value = les_var[node->value]->getValue();
  return value;
}

inline void Parser::setVar(const char * sv, double val)
{
  setVar(searchVar(sv),val);
}

inline void Parser::setVar(const std::string& v, double val)
{
  setVar(searchVar(v),val);
}

inline void Parser::setVar(int i, double val)
{
  assert(i>-1 && i<ivar);
  les_var[i]->setValue(val);
  /* PL: Assert pour optimiser
     if (i>-1 && i<ivar) les_var[i]->setValue(val);
     else
     {
     Cerr << " Variable number " << i << " does not exist !! " << finl;
     Process::exit();
     }*/
}

inline int Parser::searchVar(const char * sv)
{
  std::string s(sv);
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
  for (int i=0; i<ivar; i++)
    if ((les_var[i]->getString()).compare(s) == 0 ) return i;
  return -1;
}

inline int Parser::searchVar(const std::string& v)
{
  std::string s(v);
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
  for (int i=0; i<ivar; i++)
    if ((les_var[i]->getString()).compare(s) == 0 ) return i;
  return -1;
}

inline double Parser::eval(PNode* node)
{
  assert(node!=NULL);
  switch(node->type)
    {
    case 1 :
      return evalOp(node);  // PNode::OP
    case 2 :
      return node->nvalue;  // PNode::VALUE
    case 3 :
      return evalFunc(node);// PNode::FUNCTION
    case 4 :
      return evalVar(node); // PNode::VAR
    default:
      Cerr << "method eval : Unknown type for this node !!!" << finl;
      Process::exit();
      return 0;
    }
}

#endif
