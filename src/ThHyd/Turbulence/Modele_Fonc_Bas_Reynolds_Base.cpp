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
// File:        Modele_Fonc_Bas_Reynolds_Base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_Fonc_Bas_Reynolds_Base.h>
#include <Transport_K_Eps_Bas_Reynolds.h>
#include <Probleme_base.h>


Implemente_base(Modele_Fonc_Bas_Reynolds_Base,"Modele_Fonc_Bas_Reynolds_Base",Objet_U);

// printOn et readOn

Sortie& Modele_Fonc_Bas_Reynolds_Base::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();;
}

Entree& Modele_Fonc_Bas_Reynolds_Base::readOn(Entree& is )
{
  return is;
}

void Modele_Fonc_Bas_Reynolds_Base::completer()
{
  ;
}

void Modele_Fonc_Bas_Reynolds_Base::associer_pb(const Probleme_base& pb )
{
  Cerr << "Dans Modele_Jones_Launder_VDF::associer_pb" << finl;
  eq_hydraulique = pb.equation(0);
}

void Modele_Fonc_Bas_Reynolds_Base::associer_eqn( const Equation_base& eqn)
{
  mon_equation = ref_cast(Transport_K_Eps_Bas_Reynolds,eqn);
}

void Modele_Fonc_Bas_Reynolds_Base::discretiser()
{
  //  const Discret_Thyd_Turb& dis=
  //  ref_cast(Discret_Thyd_Turb, mon_equation->discretisation());
  Cerr << "Discretisation du modele Bas Reynolds terminee" << finl;
}


int Modele_Fonc_Bas_Reynolds_Base::preparer_calcul()
{
  return 0;
}


int Modele_Fonc_Bas_Reynolds_Base::sauvegarder(Sortie& ) const
{
  return 0;
}

int Modele_Fonc_Bas_Reynolds_Base::reprendre(Entree& )
{
  return 0;
}

void Modele_Fonc_Bas_Reynolds_Base::creer_champ(const Motcle& motlu)
{
}

const Champ_base& Modele_Fonc_Bas_Reynolds_Base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}

void Modele_Fonc_Bas_Reynolds_Base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<"Modele_Fonc_Bas_Reynolds_Base : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}
