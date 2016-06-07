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
#include <Equation_base.h>
#include <Probleme_base.h>
#include<Discretisation_base.h>


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
  mon_equation = eqn;
}

void Modele_Fonc_Bas_Reynolds_Base::discretiser()
{
  const Discretisation_base& dis=mon_equation->discretisation();
  double temps=0;

  dis.discretiser_champ("temperature",mon_equation->zone_dis(),"D","?",1,temps,D_);
  champs_compris_.ajoute_champ(D_);
  dis.discretiser_champ("temperature",mon_equation->zone_dis(),"E","?",1,temps,E_);
  champs_compris_.ajoute_champ(E_);
  dis.discretiser_champ("temperature",mon_equation->zone_dis(),"F1","?",1,temps,F1_);
  champs_compris_.ajoute_champ(F1_);
  dis.discretiser_champ("temperature",mon_equation->zone_dis(),"F2","?",1,temps,F2_);
  champs_compris_.ajoute_champ(F2_);
  dis.discretiser_champ("champ_elem", mon_equation->zone_dis(),"distance_paroi","m",1,temps,BR_wall_length_);
  champs_compris_.ajoute_champ(BR_wall_length_);

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

int Modele_Fonc_Bas_Reynolds_Base::Calcul_is_Cmu_constant() const
{
  return 1;
}

int Modele_Fonc_Bas_Reynolds_Base::Calcul_is_Reynolds_stress_isotrope() const
{
  return 1;
}



DoubleTab& Modele_Fonc_Bas_Reynolds_Base::Calcul_Cmu(DoubleTab& Cmu,
                                                     const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                                     const DoubleTab& vitesse, const DoubleTab& K_Eps, const double EPS_MIN) const
{
  return Cmu;
}

DoubleTab& Modele_Fonc_Bas_Reynolds_Base::Calcul_Cmu_Paroi(DoubleTab& Cmu,
                                                           const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                                           const DoubleTab& visco, const DoubleTab& visco_turb,
                                                           const DoubleTab& loi_paroi,const int idt,
                                                           const DoubleTab& vitesse, const DoubleTab& K_Eps, const double EPS_MIN) const
{
  return Cmu;
}

DoubleTab Modele_Fonc_Bas_Reynolds_Base::calcul_tenseur_Re(const Discretisation_base& dis,
                                                           const Zone_dis&, const Zone_Cl_dis&,
                                                           const DoubleTab&, const DoubleTab&,
                                                           const Champ_base& K_Eps) const
{
  Cerr << "La viscosite anisotrope n'a pas ete developpe dans ce modele fonc" << finl;
  exit();
  return 0;
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

void Modele_Fonc_Bas_Reynolds_Base::lire_distance_paroi( )
{
  //Cerr << " Le calcul de la dsitance a la paroi n'est pas implemente avec le modele fonc que vous avez donnez " << finl;
  //exit();

}
/*DoubleTab& Modele_Fonc_Bas_Reynolds_Base::Calcul_F1( DoubleTab& F1, const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& P, const DoubleTab& K_eps_Bas_Re,const Champ_base& ch_visco) const
{
	return F1;
}
*/
