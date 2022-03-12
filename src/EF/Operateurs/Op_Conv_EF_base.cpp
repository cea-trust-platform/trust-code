/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Op_Conv_EF_base.cpp
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_base.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <TRUSTTrav.h>
#include <Discretisation_base.h>
#include <Champ.h>

Implemente_base(Op_Conv_EF_base,"Op_Conv_EF_base",Operateur_Conv_base);


//// printOn
//

Sortie& Op_Conv_EF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Op_Conv_EF_base::readOn(Entree& s )
{
  return s ;
}



// Description: definit si l'on convecte psi avec phi*u ou avec u
int  Op_Conv_EF_base::phi_u_transportant(const Equation_base& eq) const
{
  if (eq.inconnue().le_nom()=="vitesse")
    return 0;
  return 1;
}


void Op_Conv_EF_base::associer_vitesse(const Champ_base& vit)
{
  vitesse_ = ref_cast(Champ_Inc_base,vit);
}

const Champ_Inc_base& Op_Conv_EF_base::vitesse() const
{
  return vitesse_.valeur();
}

Champ_Inc_base& Op_Conv_EF_base::vitesse()
{
  return vitesse_.valeur();
}

void Op_Conv_EF_base::abortTimeStep()
{
  Operateur_Conv_base::abortTimeStep();
}

double Op_Conv_EF_base::calculer_dt_stab() const
{
  Cerr<<__FILE__<<(int)__LINE__<<" dt_stab mal code "<<finl;
  double dt_stab=1e30;
  /*  const Zone_Cl_EF& zone_Cl_EF = la_zcl_EF.valeur();
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  const DoubleVect& volumes_entrelaces = zone_EF.volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = zone_Cl_EF.volumes_entrelaces_Cl();
  remplir_fluent(fluent);


  double dt_face,dt_stab =1.e30;

  // On traite les conditions aux limites
  // Si une face porte une condition de Dirichlet on n'en tient pas compte
  // dans le calcul de dt_stab
  for (int n_bord=0; n_bord<zone_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet,la_cl.valeur()))
     ||
     (sub_type(Dirichlet_homogene,la_cl.valeur()))
     )
  ;
      else
  {
    const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
    int ndeb = le_bord.num_premiere_face();
    int nfin = ndeb + le_bord.nb_faces();
    for (int num_face=ndeb; num_face<nfin; num_face++) {
      dt_face = volumes_entrelaces_Cl(num_face)/(fluent[num_face]+1.e-30);
      dt_stab = (dt_face < dt_stab) ? dt_face : dt_stab;
    }
  }
    }

  // On traite les faces internes non standard
  int ndeb = zone_EF.premiere_face_int();
  int nfin = zone_EF.premiere_face_std();

  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      dt_face = volumes_entrelaces_Cl(num_face)/(fluent[num_face]+1.e-30);
      dt_stab =(dt_face < dt_stab) ? dt_face : dt_stab;
    }

  // On traite les faces internes standard
  ndeb = nfin;
  nfin = zone_EF.nb_faces();
  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      dt_face = volumes_entrelaces(num_face)/(fluent[num_face]+1.e-30);
      dt_stab =(dt_face < dt_stab) ? dt_face : dt_stab;
    }
  */
  dt_stab = Process::mp_min(dt_stab);
  // astuce pour contourner le type const de la methode
  Op_Conv_EF_base& op = ref_cast_non_const(Op_Conv_EF_base,*this);
  op.fixer_dt_stab_conv(dt_stab);
  return dt_stab;
}

//calcul des valeurs du pas de temps de stabilite de l operateur pour postraitement
//-discretisation de l espace de stockage (aux faces)
//	espace_stockage nomme "localisation" pour completer le Champ_Generique
//	espace_stockage nomme "??" sinon
//-calcul des valeurs
//-verifie en debug la compatibilite avec dt_stab considere pour le calcul
// cf Op_Conv_EF_base::calculer_dt_stab() pour choix de calcul de dt_stab

void Op_Conv_EF_base::calculer_pour_post(Champ& espace_stockage,const Nom& option,int) const
{
  Cerr<<__FILE__<<(int)__LINE__<<" dt_stab mal code "<<finl;
  exit();

}

void Op_Conv_EF_base::associer_zone_cl_dis(const Zone_Cl_dis_base& zone_cl_dis)
{
  const Zone_Cl_EF& zclEF = ref_cast(Zone_Cl_EF,zone_cl_dis);
  la_zcl_EF = zclEF;
}

void Op_Conv_EF_base::associer(const Zone_dis& zone_dis,
                               const Zone_Cl_dis& zone_cl_dis,
                               const Champ_Inc& )
{
  const Zone_EF& zEF = ref_cast(Zone_EF,zone_dis.valeur());
  const Zone_Cl_EF& zclEF = ref_cast(Zone_Cl_EF,zone_cl_dis.valeur());

  la_zone_EF = zEF;
  la_zcl_EF = zclEF;
  /*
  fluent.resize(la_zone_EF->nb_faces());
  // On cree l'espace virtuel de fluent
  int nbjoints = zEF.nb_joints();
  int i;
  for(i=0;i<nbjoints;i++){
    const Joint& joint=zEF.joint(i);
    int PEvoisin = joint.PEvoisin();
    const ArrOfInt& esp_dist_face=joint.esp_dist_faces();
    fluent.ajoute_espace_distant(PEvoisin, esp_dist_face);
  }
  fluent.echange_espace_virtuel();
  */
}


int Op_Conv_EF_base::impr(Sortie& os) const
{
  return Op_EF_base::impr(os, *this );
}

DoubleTab& Op_Conv_EF_base::calculer(const DoubleTab& transporte,
                                     DoubleTab& resu) const
{
  resu = 0;
  return ajouter(transporte,resu);
}
void Op_Conv_EF_base::remplir_fluent(DoubleVect& ) const
{
  // Remplissage du tableau fluent par appel a ajouter
  // C'est cher mais au moins cela corrige (en attendant
  // d'optimiser) le probleme d'un pas de temps de convection
  // calcule avec des vitesses du passe
  DoubleTrav tmp(equation().inconnue().valeurs());
  DoubleTab flux_bords_sauve(flux_bords_);  // On sauve les flux_bords car sinon mis a 0
  ajouter(tmp,tmp);
  flux_bords_=flux_bords_sauve;
  // PL: C'est vraiment lourd, mais comment faire? fluent est dependant
  // du schema et donc on peut coder quelque chose comme fluent=vitesse*surface*porosite
  // dans cette presente methode
}
void Op_Conv_EF_base::completer()
{
  Operateur_Conv_base::completer();
  marque_elem(equation());

}
