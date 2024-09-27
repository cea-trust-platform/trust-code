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

#include <Op_Conv_Muscl3_VEF_Face.h>
#include <Convection_tools.h>

Implemente_instanciable_sans_constructeur(Op_Conv_Muscl3_VEF_Face,"Op_Conv_Muscl3_VEF_P1NC",Op_Conv_VEF_Face);
// XD convection_muscl3 convection_deriv muscl3 1 Keyword for a scheme using a ponderation between muscl and center schemes in VEF.
// XD attr alpha floattant alpha 1 To weight the scheme centering with the factor double (between 0 (full centered) and 1 (muscl), by default 1).

Sortie& Op_Conv_Muscl3_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Conv_Muscl3_VEF_Face::readOn(Entree& s )
{

  type_op=muscl;
  LIMITEUR=&vanleer;
  type_lim_int = type_lim_vanleer;
  ordre_=3;
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
          type_lim_int = type_lim_minmod;
          break;
        case 2 :
          LIMITEUR=&vanleer;
          type_lim_int = type_lim_vanleer;
          break;
        case 3 :
          LIMITEUR=&vanalbada;
          type_lim_int = type_lim_vanalbada;
          break;
        case 4 :
          LIMITEUR=&chakravarthy;
          type_lim_int = type_lim_chakravarthy;
          break;
        case 5 :
          LIMITEUR=&superbee;
          type_lim_int = type_lim_superbee;
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

