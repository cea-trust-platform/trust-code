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
// File:        Op_Conv_Muscl3_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_Muscl3_VEF_Face.h>

Implemente_instanciable_sans_constructeur(Op_Conv_Muscl3_VEF_Face,"Op_Conv_Muscl3_VEF_P1NC",Op_Conv_VEF_Face);


//// printOn
//
Sortie& Op_Conv_Muscl3_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//
Entree& Op_Conv_Muscl3_VEF_Face::readOn(Entree& s )
{

  type_op=muscl;
  LIMITEUR=&vanleer;
  ordre=3;
  alpha_=1;
  // Lecture eventuelle de alpha_
  Motcle motlu, accouverte = "{" , accfermee = "}" ;
  Motcles les_mots(6);
  {
    les_mots[0] = "alpha";
    les_mots[1] = "minmod";
    les_mots[2] = "vanleer";
    les_mots[3] = "vanalbada";
    les_mots[4] = "chakravarthy";
    les_mots[5] = "superbee";
  }
  s >> motlu;
  if (motlu!=accouverte)
    {
      Cerr << "We wait for a { after reading muscl3 keyword." << finl;
      Process::exit();
    }
  s >> motlu;
  while(motlu!=accfermee)
    {
      int rang=les_mots.search(motlu);

      switch(rang)
        {
        case 0 :
          s >> alpha_;
          break;
        case 1 :
          LIMITEUR=&minmod;
          break;
        case 2 :
          LIMITEUR=&vanleer;
          break;
        case 3 :
          LIMITEUR=&vanalbada;
          break;
        case 4 :
          LIMITEUR=&chakravarthy;
          break;
        case 5 :
          LIMITEUR=&superbee;
          break;
        default:
          Cerr << "Keyword " << motlu << " not recognized. List of keywords available:" << finl;
          Cerr << les_mots << finl;
          exit();
          break;
        }
      s >> motlu;
    }
  return s ;
}

