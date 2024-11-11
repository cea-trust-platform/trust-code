/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <UnaryFunction.h>
#include <TRUST_Deriv.h>
#include <TRUST_List.h>
#include <Constante.h>
#include <algorithm>
#include <Stack.h>
#include <math.h>
#include <string>
#include <Noms.h>
#include <TRUSTArray.h>

class StringTokenizer;

/*! @brief Representation des donnees de la classe Parser
 *
 * @sa =
 */

class Parser
{
public :

  /**
   * Initialise le parser avec une chaine "0" : ne sert a rien !!
   */
  Parser();
  Parser(const Parser&);
  virtual ~Parser();

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
  virtual void parseString();

  /**
   * Sert a evaluer l'expression mathematique correspondante a la chaine de caracteres. Poru cela il faut avant toute chose construire l'arbre par la methode parseString().
   */
  inline double eval() { return eval(PNodes[0]); }

  /**
   * Fixe la valeur de la variable representee par une chaine sv.
   */
  inline void setVar(const char* sv, double val) { setVar(searchVar(sv),val); }

  /**
  * Fixe la valeur de la variable representee par v.
  */
  inline void setVar(const std::string& v, double val) { setVar(searchVar(v),val); }

  /**
  * Fixe la valeur de la variable de numero specifie. Ce numero correspondt a l'ordre de l'ajout des variables par la methode addVar().
  */
  inline void setVar(int i, double val)
  {
    assert(i>-1 && i<ivar);
    les_var[i] = val;
  }

  /**
   * Fixe le nombre max de variables a indiquer avec la methode addVar.
   */
  virtual void setNbVar(int nvar);

  /**
   * permet d'ajouter une variable en specifiant sa chaine representative (par ex. : x, y1 etc...)
   */
  void addVar(const char *);

  inline int getmaxVar()     { return maxvar; }
  inline int getNbVar()      { return ivar; }
  inline std::string& getString() { return *str; }
  inline void setString(const std::string& s)
  {
    delete str;
    str = new std::string(s);
  }
  void addCst(const Constante& cst);

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

protected:
  int test_op_binaire(int type);

  static int precedence(int);
  inline double eval(const PNodePod& node)
  {
    double x,y;
    switch(node.type)
      {
      case 1 :
        x = (node.left  != -1 ? eval(PNodes[node.left])  : 0);
        y = (node.right != -1 ? eval(PNodes[node.right]) : 0);
        return evalOp(node, x, y);  // PNode_type::OP
      case 2 :
        return node.nvalue;  // PNode_type::VALUE
      case 3 :
        x = node.value<=0 ? eval(PNodes[node.left]) : 0;
        return evalFunc(node, x);// PNode_type::FUNCTION
      case 4 :
        return les_var[node.value]; // PNode_type::VAR
      default:
        Process::exit("method eval : Unknown type for this node !!!");
        return 0;
      }
  }
  KOKKOS_FUNCTION double evalOp(const PNodePod&, double x, double y);
  KOKKOS_FUNCTION double evalFunc(const PNodePod&, double x);
  void parserState0(StringTokenizer*,PSTACK(PNode)* ,STACK(int)*);
  void parserState1(StringTokenizer*,PSTACK(PNode)* ,STACK(int)*);
  void parserState2(StringTokenizer*,PSTACK(PNode)* ,STACK(int)*);
  inline int searchVar(const std::string& s) { return searchVar(s.c_str()); }
  inline int searchVar(const char*);
  int searchCst(const std::string& v);
  int searchFunc(const std::string& v);
  int state;
  Constante c_pi ;
  double impuls_T;
  double impuls_t0;
  double impuls_tn;
  double impuls_tempo;

  PNode* root;                      // Liste chainee de PNode
  std::vector<PNodePod> PNodes;     // Vecteur de PNodePod
  std::string* str;
  ArrOfDouble les_var;
  Noms les_var_names;
  LIST(Constante) les_cst;
  std::map<std::string, int> map_function_;
  int maxvar,ivar;
};

inline int Parser::searchVar(const char * sv)
{
  std::string s(sv);
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
  return les_var_names.rang(s.c_str());
}

#endif
