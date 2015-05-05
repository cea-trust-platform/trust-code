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
// File:        PlaqThVDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Cond_Lim
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <PlaqThVDF.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ_front_calc.h>
#include <Convection_Diffusion_Temperature.h>
#include <Modele_turbulence_scal_base.h>
#include <Zone_VDF.h>

Implemente_instanciable(PlaqThVDF,"Plaque_Thermique_VDF",Echange_global_impose);

Sortie& PlaqThVDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& PlaqThVDF::readOn(Entree& s )
{
  s >> h;
  T_ext().typer("Champ_front_calc");
  return s;
}

void PlaqThVDF::mettre_a_jour(double )
{
  const Equation_base& eqn = ma_zone_cl_dis->equation();
  const Zone_VDF& la_zone_VDF=ref_cast(Zone_VDF, eqn.zone_dis().valeur());
  const Front_VF& front= ref_cast(Front_VF,frontiere_dis());

  const Milieu_base& le_milieu=eqn.probleme().milieu();
  h/=(le_milieu.masse_volumique()(0,0)*le_milieu.capacite_calorifique()(0,0));

  // Calcul de himp :
  const RefObjU& modele_turbulence = eqn.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
    {
      const Modele_turbulence_scal_base& modele = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Turbulence_paroi_scal& loipar = ref_cast(Turbulence_paroi_scal,modele.loi_paroi());
      Champ_front_calc& ch=ref_cast(Champ_front_calc, T_ext().valeur());
      ch.creer(eqn.probleme().le_nom(),frontiere_dis().le_nom(),eqn.inconnue().le_nom());
      //const Milieu_base& le_milieu=eqn.probleme().milieu();
      h_imp_.typer("Champ_front_uniforme");
      DoubleTab& tab = h_imp_->valeurs();
      h_imp_->fixer_nb_comp(1);
      int nbfs2 = front.nb_faces()/2;
      tab.resize(front.nb_faces(),1);
      int boundary_index=-1;
      int nb_boundaries=la_zone_VDF.zone().nb_front_Cl();
      for (int n_bord=0; n_bord<nb_boundaries; n_bord++)
        {
          if (la_zone_VDF.front_VF(n_bord).le_nom() == front.le_nom())
            boundary_index=n_bord;
        }
      for(int face=0; face < nbfs2; face++)
        {
          // double e1 = loipar.d_equiv(face);
          // double e2 = loipar.d_equiv(nbfs2+face);
          int local_face=la_zone_VDF.front_VF(boundary_index).num_local_face(face);
          int local_face2=la_zone_VDF.front_VF(boundary_index).num_local_face(nbfs2+face);
          double e1 = loipar.valeur().equivalent_distance(boundary_index,local_face);
          double e2 = loipar.valeur().equivalent_distance(boundary_index,local_face2);
          tab(face,0) = tab(nbfs2+face,0) =
                          2./(1./h+e1/le_milieu.diffusivite()(0,0)
                              +e2/le_milieu.diffusivite()(0,0));
        }
    }
  else if (sub_type(Convection_Diffusion_Temperature,eqn))
    {
      Champ_front_calc& ch = ref_cast(Champ_front_calc,T_ext().valeur());
      ch.creer(eqn.probleme().le_nom(),frontiere_dis().le_nom(),
               eqn.inconnue().le_nom());
      //const Milieu_base& le_milieu=eqn.probleme().milieu();
      h_imp_.typer("Champ_front_uniforme");
      DoubleTab& tab= h_imp_->valeurs();
      tab.resize(1,1);
      h_imp_->fixer_nb_comp(1);
      double e1 = la_zone_VDF.dist_norm_bord(front.num_premiere_face());
      double e2 = la_zone_VDF.dist_norm_bord(front.nb_faces()/2);
      tab(0,0) = 2./(1./h+e1/le_milieu.diffusivite()(0,0)
                     +e2/le_milieu.diffusivite()(0,0));
    }

  //Calcul de T_ext :
  const DoubleTab& Temp= eqn.inconnue()->valeurs();
  DoubleTab& tab= T_ext()->valeurs();
  tab.resize(front.nb_faces(),1);
  int face, el1, el2;
  int premiere = front.num_premiere_face();
  int nbfs2=front.nb_faces()/2;
  int derniere = premiere + nbfs2;
  for(face=premiere; face < derniere; face++)
    {
      int num=face-premiere;
      el1=la_zone_VDF.face_voisins(face,0);
      el2=la_zone_VDF.face_voisins(face+nbfs2,1);
      tab(num+nbfs2,0)=tab(num,0)=(Temp(el1)+Temp(el2))*0.5;
    };
}
