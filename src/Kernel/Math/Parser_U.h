/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Parser_U.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Parser_U_included
#define Parser_U_included

#include <Parser.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Parser_U
//     Version de la classe Parser, derivant de Objet_U.
//     Elle permet lors de son utilisation de beneficier de la gestion de la memoire propre aux Objet_U
//     a la difference de la classe Math/Parser
//
// .SECTION voir aussi
//     Parser
//////////////////////////////////////////////////////////////////////////////
class Parser_U : public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Parser_U);

public :

  Parser_U();
  Parser_U(const Parser_U&);
  ~Parser_U();

  const Parser_U& operator=(const Parser_U&);



  /**
   * Construit l'arbre correspondant a la chaine de caracteres. Cet arbre doit etre construit une seule fois et la chaine de caractere est evaluee en parcourant cet arbre par la methode eval() autant de fois qu'on le souhaite.
   */
  inline void parseString();


  /**
   * Sert a evaluer l'expression mathematique correspondante a la chaine de caracteres. Poru cela il faut avant         toute chose construire l'arbre par la methode parseString().
   */
  inline double eval();

  /**
   * Fixe la valeur de la variable representee par une chaine String2 v.
   */
  inline void setVar(const char* sv, double val);

  /**
   * Fixe la valeur de la variable de numero specifie. Ce numero correspondt a l'ordre de l'ajout des variables par la methode addVar().
   */
  inline void setVar(int i, double val);

  /**
   * Fixe le nombre max de variables a indiquer avec la methode addVar.
   */
  inline void setNbVar(int nvar);

  /**
   * permet d'ajouter une variable en specifiant sa chaine representative (par ex. : x, y1 etc...)
   */
  inline void addVar(const char *v);

  inline String2& getString() ;
  inline void setString(const String2& s) ;
  inline void setString(const Nom& nom) ;
  inline void addCst(const Constante& cst);
  inline void setImpulsion(double tinit, double periode);
  inline void addFunc(const UnaryFunction& f);



private :

  Parser *parser;

};



/**
 * Construit l'arbre correspondant a la chaine de caracteres. Cet arbre doit etre construit une seule fois et la chaine de caractere est evaluee en parcourant cet arbre par la methode eval() autant de fois qu'on le souhaite.
 */
inline void Parser_U::parseString()
{
  parser->parseString();
}


/**
 * Sert a evaluer l'expression mathematique correspondante a la chaine de caracteres. Poru cela il faut avant         toute chose construire l'arbre par la methode parseString().
 */
inline double Parser_U::eval()
{
  return parser->eval();
}

/**
 * Fixe la valeur de la variable representee par une chaine String2 v.
 */
inline void Parser_U::setVar(const char* sv, double val)
{
  parser->setVar(sv, val);
}

/**
 * Fixe la valeur de la variable de numero specifie. Ce numero correspondt a l'ordre de l'ajout des variables par la methode addVar().
 */
inline void Parser_U::setVar(int i, double val)
{
  parser->setVar(i, val);
}


/**
 * Fixe le nombre max de variables a indiquer avec la methode addVar.
 */
inline void Parser_U::setNbVar(int nvar)
{
  parser->setNbVar(nvar);
}


/**
 * permet d'ajouter une variable en specifiant sa chaine representative (par ex. : x, y1 etc...)
 */
inline void Parser_U::addVar(const char *v)
{
  parser->addVar(v);
}

inline String2& Parser_U::getString()
{
  return parser->getString();
}


inline void Parser_U::setString(const String2& s)
{
  parser->setString(s);
}

inline void Parser_U::setString(const Nom& nom)
{
  const char *s =  nom.getChar();
  String2 ss(s);
  setString(ss);
}

inline void Parser_U::addCst(const Constante& cst)
{
  parser->addCst(cst);
}

inline void Parser_U::addFunc(const UnaryFunction& f)
{
  parser->addFunc(f);
}

inline void Parser_U::setImpulsion(double tinit, double periode)
{
  parser->setImpulsion(tinit, periode);
}





#endif
