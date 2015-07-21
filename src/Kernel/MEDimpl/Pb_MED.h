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
// File:        Pb_MED.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Pb_MED_included
#define Pb_MED_included

#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Champs_Fonc.h>

///////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// Classe Pb_MED
// Classe pour postraiter relire des fichiers med et les postraiter
// .SECTION voir aussi
//
//
///////////////////////////////////////////////////////////////////////////
class Pb_MED : public Probleme_base
{

  Declare_instanciable(Pb_MED);

public:

  int nombre_d_equations() const;
  const Equation_base& equation(int) const ;
  Equation_base& equation(int);
  int comprend_champ(const Motcle& ) const;

  inline const ArrOfDouble& temps_sauv() const
  {
    return temps_sauv_ ;
  };
  inline Champs_Fonc& get_champs_fonc_post()
  {
    return champs_fonc_post;
  };

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

protected :

  Champs_Fonc champs_fonc_post;

private:
  Nom nom_fic;
  Noms nomschampmed;
  Discretisation dis_bidon;
  ArrOfDouble temps_sauv_;

  //Une classe portant des champs postraitables possede normalement un
  //attribut champs_compris_ qui contient une reference a ces champs.
  //Dans le cas particulier des Pb_MED on ne declare pas cet attribut
  //afin de pouvoir coder une methhode get_champ() tres specifique a ce
  //type de probleme et qui n utilisera pas les methodes de Champs_compris.
  //La methode get_noms_champs_postraitables() et creer_champ() ne manipuleront
  //pas non plus de champs_compris_.

  //Champs_compris champs_compris_;


};

class Pbc_MED : public Probleme_Couple
{

  Declare_instanciable(Pbc_MED);
private:
  Schema_Temps sch_;
};
#endif




