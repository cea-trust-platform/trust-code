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
// File:        StringTokenizer.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////
#include <StringTokenizer.h>
#include <Objet_U.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
using namespace std;

const int StringTokenizer::NUMBER=-2;
const int StringTokenizer::STRING=-3;
const int StringTokenizer::EOS=-1;


// On ne definit les identificateurs des operateurs qu'une seule fois entre la classe Parser et StringTokenizer.
// Pour l'instant, on les place dans cette classe et donc on les supprime de la classe Parser
// => on pourra envisager de les mettre dans une classe a part entiere si ca parait plus clair.
/* attention il faut que les constantes suivantes soit superieur a 0 pour eviter les conflits avec les fonctions unaires
 */
const int StringTokenizer::ADD = 0;
const int StringTokenizer::SUBTRACT = 1;
const int StringTokenizer::MULTIPLY = 2;
const int StringTokenizer::DIVIDE = 3;
const int StringTokenizer::POWER = 4;
const int StringTokenizer::LT = 5;
const int StringTokenizer::GT = 6;
const int StringTokenizer::LE = 7;
const int StringTokenizer::GE = 8;
const int StringTokenizer::MOD = 9;
const int StringTokenizer::MAX = 10;
const int StringTokenizer::MIN = 11;
const int StringTokenizer::AND = 12;
const int StringTokenizer::OR = 13;
const int StringTokenizer::EQ = 14;
const int StringTokenizer::NEQ = 15;
// les parentheses :
const int StringTokenizer::GRP = 1000;
const int StringTokenizer::ENDGRP = 1001;


// Nb d'operateurs et mot-cles pour les identifier (ne concerne pas les parentheses"(" et ")").
// Attention a l'ordre des operateurs ci dessous : ils doivent etre positionnes dans les tableaux  keyword_op et keyword_op_bis en fonction des valeurs des constantes statiques ADD, SUB etc... definies ci-dessus
//
const int StringTokenizer::nb_op=16;
const int StringTokenizer::nb_op_bis=11;
const char StringTokenizer::keyword_op[][10] = { "ADD", "SUB", "MUL", "DIV" , "POW", "LT", "GT", "LE", "GE", "MOD", "MAX", "MIN", "AND", "OR", "EQ", "NEQ" };
const char StringTokenizer::keyword_op_bis[][10] = { "+", "-", "*", "/", "^", "<", ">", "[", "]", "%", "$" };


StringTokenizer::StringTokenizer()
{
  sval =NULL;
  str = new String2("0");
  init_keyword_op();
  reste = str->s;
}

StringTokenizer::StringTokenizer(String2& s)
{
  sval =NULL;
  str = new String2(s);
  init_keyword_op();
  reste = str->s;
}

StringTokenizer::StringTokenizer(String2 s, String2 sep)
{
  sval =NULL;
  str = new String2(s);
  init_keyword_op();
  reste = str->s;
}

StringTokenizer::~StringTokenizer()
{
  delete str;

  for (int i=0; i<StringTokenizer::nb_op; i++)
    {
      delete[] op_sep[i];
    }
  delete[] op_sep;

  if ( sval != NULL) delete sval;
}



int StringTokenizer::check_GRP()
{
  char* ch = str->s;
  int nb_o=0;
  int nb_f=0;
  int sz = strlen(ch);
  for (int i=0; i<sz; i++)
    {
      if (*(ch+i)=='(')
        nb_o++;
      else if (*(ch+i)==')')
        nb_f++;
    }
  return (nb_o==nb_f);
}


int StringTokenizer::nextToken(void)
{
  if ( sval != NULL) delete sval;
  sval = NULL;
  char *tmp;
  int type_sep, length;
  tmp=find_sep(reste, type_sep, length);
  if (tmp == NULL)
    {
      //cout << "Fin ? " << reste << endl;
      if (reste[0] == '\0')
        {
          type = EOS;
        }
      else
        {
          if ( ((reste[0] >= '0') && ( reste[0] <= '9')) || (reste[0] == '.')  )
            {
              // ajout OC pour provoquer une erreur avec 2x
              int ind=strlen(reste)-1;
              if ( ((reste[ind] < '0') || (reste[ind] > '9')) && (reste[ind] != '.') )
                {
                  Cerr << "The syntax " << reste << " is not allowed." << finl;
                  Process::exit();
                }
              type = NUMBER;
              stringstream stream;
              stream << reste ;
              stream >> nval;
              if (!stream.eof() )
                {
                  Cerr<<"Error conversion "<<reste<<" in number"<<finl;
                  Process::exit();
                }
            }
          else
            {
              type=STRING;
              sval = new String2(reste);
            }
          while ((*reste++) != '\0') ;
          reste--;
        }
    }
  else if (tmp==reste)
    {
      type = type_sep;
      reste=reste+length;
    }
  else
    {
      char* tok = new char[tmp-reste+1];
      int j=0;
      for (int i=0; i<tmp-reste; i++)
        {
          if ( reste[i] != ' ' )        tok[j++] = reste[i];
        }
      tok[j] = '\0';
      if (((tok[0] >= '0') && ( tok[0] <= '9')) || (tok[0] == '.')  )
        {
          int ind;
          double nval_tmp;

          type = NUMBER;
          stringstream stream;
          ind = tmp-reste-1;
          reste = tmp;
          if (tok[ind] == 'e' || tok[ind] == 'E' )
            {
              tok[ind]=' ';
              stream << tok ;
              stream >> nval_tmp;
              char c;
              stream >> c;

              if (!stream.eof() )
                {
                  Cerr<<"Error conversion "<<tok<<" in number"<<finl;
                  Process::exit();
                }
              nextToken();
              if (type == SUBTRACT)
                {
                  nextToken();
                  if (type != NUMBER)
                    {
                      Cerr << "Error while interpreting the string " << str << endl;
                      Process::exit();
                    }
                  nval_tmp*=pow(10,-nval);
                }
              else if (type == ADD)
                {
                  nextToken();
                  if (type != NUMBER)
                    {
                      Cerr << "Error while inetrpreting the string " << str << endl;
                      Process::exit();
                    }
                  nval_tmp*=pow(10.,nval);
                }
              else if (type == NUMBER)
                {
                  // GF suivant les versions on a soit
                  nval_tmp*=pow(10.,nval);
                  // soit
                  //nval_tmp = nval;
                  // J'interdis pour l'instant
                  //Cerr << "Possible Erreur en interpretant la chaine " << str << finl;
                  //Process::exit();
                }
              else
                {
                  Cerr << "Error while interpreting the string " << str << endl;
                  Process::exit();
                }
            }
          else if ( ((tok[ind] < '0') || (tok[ind] > '9')) && (tok[ind] != '.') )
            {
              Cerr << "  The syntax " << tok << " is not allowed." << finl;
              Process::exit();
            }
          else
            {
              stream << tok ;
              stream >> nval_tmp;
              if (!stream.eof() )
                {

                  Cerr<<"Error conversion "<<tok<<" in number"<<finl;
                  Process::exit();
                }
            }
          nval = nval_tmp;
        }
      else
        {
          type=STRING;
          sval = new String2(tok);
          reste=tmp;
        }
      delete[] tok;
    }
  return type;
}



//methodes privees :

void StringTokenizer::init_keyword_op()
{
  op_sep = new char*[nb_op];
  // on encadre le mot-cle de chaque operateur par des "_" :
  // LT =>  _LT_  etc...
  for (int i=0; i<nb_op; i++)
    {
      const char* blanc="_";
      op_sep[i] = new char[strlen(keyword_op[i])+3];
      strcpy(op_sep[i],blanc);
      strcat(op_sep[i], keyword_op[i]);
      strcat(op_sep[i], blanc);
    }
}


/**
 * Cherche dans la chaine "ch" la prochaine apparition d'un separateur.
 * Renvoie dans le parametre "type_sep", le type de separateur ((, ), +, - etc...)
 * Renvoie egalement la longueur de la chaine correspondante au separateur trouve
 */
char* StringTokenizer::find_sep(char* ch, int& type_sep, int& length)
{
  char * trouve=NULL;
  char * trouve_tmp;
  type_sep=-1;
  trouve_tmp = strstr(ch, "(");
  int pos=100000;
  if (trouve_tmp != NULL)
    {
      pos = trouve_tmp-ch;
      type_sep=GRP;
      length=1;
      trouve=trouve_tmp;
    }
  trouve_tmp = strstr(ch, ")");
  if ((trouve_tmp != NULL) && (trouve_tmp-ch<pos))
    {
      pos = trouve_tmp-ch;
      type_sep=ENDGRP;
      length=1;
      trouve=trouve_tmp;
    }
  for (int i=0; i<StringTokenizer::nb_op; i++)
    {
      trouve_tmp = strstr(ch, op_sep[i]);
      if ((trouve_tmp != NULL) && (trouve_tmp-ch<pos))
        {
          pos = trouve_tmp-ch;
          type_sep=i;
          length=strlen(op_sep[i]);
          trouve=trouve_tmp;
        }
    }
  for (int i=0; i<StringTokenizer::nb_op_bis; i++)
    {
      trouve_tmp = strstr(ch, keyword_op_bis[i]);
      if ((trouve_tmp != NULL) && (trouve_tmp-ch<pos))
        {
          pos = trouve_tmp-ch;
          type_sep=i;
          length=strlen(keyword_op_bis[i]);
          trouve=trouve_tmp;
        }
    }

  return trouve;
}



/*
  int main()
  {
  //String s("23+34*12-13+COS ( 12 )* 2^3");
  String2 s("2+3");
  StringTokenizer tk(s);
  cout << StringTokenizer::NUMBER << endl;
  cout << StringTokenizer::EOS << endl;
  cout << StringTokenizer::STRING << endl;

  while (tk.nextToken()!=StringTokenizer::EOS)
  {
  if (tk.type == StringTokenizer::STRING)
  {
  cout << "String = " << tk.getSValue() << endl;
  }
  else if (tk.type == StringTokenizer::NUMBER)
  {
  cout << "Value = " << tk.getNValue() << endl;
  }
  else
  {
  cout << "Operator = " << (char) tk.type << endl;
  }
  }
  }
*/
