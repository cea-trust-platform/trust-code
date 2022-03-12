/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Diff_P1NC_barprim.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#include <Op_Diff_P1NC_barprim.h>
#include <Zone_dis.h>
#include <Zone_Cl_dis.h>
#include <Champ_Inc.h>
#include <math.h>
#include <Champ_P1NC.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc.h>
#include <TRUSTTrav.h>
#include <Domaine.h>
#include <Ref_Champ_P1NC.h>
#include <Debog.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Periodique.h>
#include <EFichier.h>
#include <Schema_Temps_base.h>

Implemente_instanciable(Op_Diff_P1NC_barprim,"Op_Diff_VEFBARPRIM_const_P1NC",Operateur_Diff_base);




Sortie& Op_Diff_P1NC_barprim::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//
Entree& Op_Diff_P1NC_barprim::readOn(Entree& s )
{
  return s;
}

//// associer
//


void Op_Diff_P1NC_barprim::associer(const Zone_dis& zone_dis,
                                    const Zone_Cl_dis& zone_cl_dis,
                                    const Champ_Inc& ch_diffuse)
{
  const Zone_VEF_PreP1b& zvef = ref_cast(Zone_VEF_PreP1b,zone_dis.valeur());
  const Zone_Cl_VEFP1B& zclvef = ref_cast(Zone_Cl_VEFP1B,zone_cl_dis.valeur());
  if (sub_type(Champ_P1NC,ch_diffuse.valeur()))
    {
      const Champ_P1NC& inco = ref_cast(Champ_P1NC,ch_diffuse.valeur());
      REF(Champ_P1NC) inconnue;
      inconnue = inco;
    }

  la_zone_vef = zvef;
  la_zcl_vef = zclvef;
}


void Op_Diff_P1NC_barprim::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
}

void Op_Diff_P1NC_barprim::completer()
{
  Operateur_base::completer();

}

const Champ_base& Op_Diff_P1NC_barprim::diffusivite() const
{
  return diffusivite_;
}


double Op_Diff_P1NC_barprim::calculer_dt_stab() const
{
  double dt_stab;
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  double alpha = 1.e-38;
  // pas mp_max ?
  if (diffusivite_pour_pas_de_temps().valeurs().size() > 0)
    alpha = local_max_vect(diffusivite_pour_pas_de_temps().valeurs());
  dt_stab = zone_VEF.carre_pas_du_maillage()/(2.*dimension*(alpha+DMINFLOAT));
  return dt_stab;
}


void Op_Diff_P1NC_barprim::calculer_divergence(const DoubleTab& grad,
                                               const DoubleVect& nu,
                                               DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  int nfe=zone.nb_faces_elem();
  int nb_elem_tot=zone.nb_elem_tot();
  int elem,indice,face,comp,comp2;
  ArrOfDouble sigma(dimension);
  double normale;
  for(elem=0; elem<nb_elem_tot; elem++)
    {
      for(indice=0; indice<nfe; indice++)
        {
          face = elem_faces(elem,indice);
          double signe=1;
          if(elem!=face_voisins(face,0))
            signe=-1;
          for(comp2=0; comp2<dimension; comp2++)
            {
              normale=signe*face_normales(face,comp2);
              for(comp=0; comp<dimension; comp++)
                resu(face, comp)-=nu(elem)*grad(elem,comp,comp2)*normale;
            }
        }
    }
  {
    int nb_bords=la_zone_vef->nb_front_Cl();
    for (int n_bord=0; n_bord<nb_bords; n_bord++)
      {
        const Cond_lim& la_cl = la_zcl_vef->les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        //Cerr << "la_cl = " << la_cl.valeur() << finl;
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if ((la_cl->que_suis_je() == "Paroi_fixe"))
          {
            //Cerr << "Paroi_fixe" << finl;
            for (face=num1; face<num2; face++)
              {
                for(comp2=0; comp2<dimension; comp2++)
                  {
                    normale=face_normales(face,comp2);
                    for(comp=0; comp<dimension; comp++)
                      resu(face, comp)=0.;
                  }
              }
          }
        else if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,
                                                     la_cl.valeur());
            ArrOfInt fait(le_bord.nb_faces());
            for (face=num1; face<num2; face++)
              {
                if(!fait(face-num1))
                  {
                    int face_associee=num1+la_cl_perio.face_associee(face-num1);
                    fait(face-num1)=(fait(face_associee-num1)=1);
                    for (comp=0; comp<dimension; comp++)
                      resu(face_associee,comp) =
                        (resu(face,comp)+= resu(face_associee,comp));
                  }
              }
          }
      }
  }
}



DoubleTab& Op_Diff_P1NC_barprim::ajouter(const DoubleTab& inconnue,
                                         DoubleTab& resu) const
{
  DoubleTab tmp(resu);
  calculer(inconnue,tmp);
  resu += tmp;
  return resu;
}

void calculer_gradientP1NC(const DoubleTab& vitesse,
                           const Zone_VEF& zone_VEF,
                           const Zone_Cl_VEF& zone_Cl_VEF,
                           DoubleTab& gradient_elem);
DoubleTab& Op_Diff_P1NC_barprim::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  //Cerr << "Op_Diff_P1NC_barprim::calculer" << finl;
  int nb_elem = la_zone_vef->zone().nb_elem_tot();
  //const DoubleTab& xp=la_zone_vef->xp();
  DoubleVect nu;
  {
    nu.resize(nb_elem);
    nu=(diffusivite().valeurs())(0,0);
  }
  resu=0;

  const Champ_P1NC& ch=ref_cast(Champ_P1NC,(equation().inconnue().valeur()));
  DoubleTab gradient_bar;
  DoubleTab ubar(ch.valeurs());
  ch.filtrer_L2(ubar);
  calculer_gradientP1NC(ubar, la_zone_vef.valeur(),la_zcl_vef.valeur() ,
                        gradient_bar);
  DoubleTab resu_bar(resu);
  calculer_divergence(gradient_bar, nu, resu_bar);
  ch.filtrer_resu(resu_bar);
  resu+=resu_bar;
  resu.echange_espace_virtuel();
  modifier_flux(*this);
  return resu;
}

