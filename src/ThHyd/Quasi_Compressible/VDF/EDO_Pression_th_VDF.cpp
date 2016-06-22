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
// File:        EDO_Pression_th_VDF.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/VDF
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <EDO_Pression_th_VDF.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Sortie_libre_pression_imposee_QC.h>
#include <Debog.h>
#include <Convection_Diffusion_Temperature.h>
#include <Champ_Face.h>
#include <Loi_Etat_Melange_GP.h>
#include <Check_espace_virtuel.h>

Implemente_base(EDO_Pression_th_VDF,"EDO_Pression_th_VDF",EDO_Pression_th_base);


Sortie& EDO_Pression_th_VDF::printOn(Sortie& os) const
{
  return EDO_Pression_th_base::printOn(os);
}
Entree& EDO_Pression_th_VDF::readOn(Entree& is)
{
  return EDO_Pression_th_base::readOn(is);
}

void  EDO_Pression_th_VDF::associer_zones(const Zone_dis& zone, const Zone_Cl_dis& zone_cl)
{
  la_zone = ref_cast(Zone_VDF,zone.valeur());
  la_zone_Cl = zone_cl;
  Champ_Face toto;
  toto.associer_zone_dis_base(zone.valeur());
  toto.fixer_nb_comp(1);
  toto.fixer_nb_valeurs_nodales(la_zone->nb_faces());
  //tab_rhoFa=toto.valeurs();
  tab_rho_face=toto.valeurs();
  tab_rho_face_demi=toto.valeurs();
  tab_rho_face_np1=toto.valeurs();
}


// Description:
//    Complete l'EDO : calcule rho sur les faces
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void EDO_Pression_th_VDF::completer()
{
  const DoubleTab& tab_ICh = le_fluide_->inco_chaleur().valeurs();
  double Pth=le_fluide_->pression_th();
  M0=masse_totale(Pth,tab_ICh);
  le_fluide_->checkTraitementPth(la_zone_Cl);
}

// Description:
//    Calcule la moyenne volumique de la grandeur P0 donnee
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: rho discretise par face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double EDO_Pression_th_VDF::moyenne_vol(const DoubleTab& tab) const
{
  int elem, nb_elem=la_zone->nb_elem();
  const DoubleVect& volumes = la_zone->volumes();
  assert(tab.dimension(0)==nb_elem);
  double v, moy=0, V=0;
  for (elem=0 ; elem<nb_elem ; elem++)
    {
      v = volumes(elem);
      V += v;
      moy += v*tab(elem);
    }
  v=Process::mp_sum(V);
  moy=Process::mp_sum(moy);
  return moy/v;
}
void EDO_Pression_th_VDF::calculer_rho_face_np1(const DoubleTab& tab_rhoP0)
{
  int face,nb_faces_tot = la_zone->nb_faces_tot();
  int elem;
  Debog::verifier("tab_rhoP0",tab_rhoP0);
  int i, nb_comp;
  IntTab& face_voisins = la_zone->face_voisins();
  for (face=0 ; face<nb_faces_tot ; face++)
    {
      nb_comp=0;
      tab_rho_face_np1(face) = 0;
      for (i=0 ; i<2 ; i++)
        {
          elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              tab_rho_face_np1(face) += tab_rhoP0(elem);
            }
        }
      tab_rho_face_np1(face) /= nb_comp;
    }
  tab_rho_face_np1.echange_espace_virtuel();
  Debog::verifier("tab_rho_face_np1",tab_rho_face_np1);
  for (face=0 ; face<nb_faces_tot ; face++)
    {
      tab_rho_face_demi(face)=(tab_rho_face_np1(face)+tab_rho_face(face))/2.;
    }
}

// Description:
//    Renvoie rho avec la meme discretisation que la vitesse :
//    une valeur par face en VDF
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: rho discretise par face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const DoubleTab& EDO_Pression_th_VDF::rho_discvit() const
{
  return tab_rho_face_demi;
}

// Description:
//    Renvoie div(u) avec la meme discretisation que la vitesse :
//    une valeur par face en VDF
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: div(u) discretise par face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void EDO_Pression_th_VDF::divu_discvit(const DoubleTab& secmem1, DoubleTab& secmem2)
{
  assert_espace_virtuel_vect(secmem1);
  int nb_faces_tot = la_zone->nb_faces_tot();
  IntTab& face_voisins = la_zone->face_voisins();
  //remplissage de div(u) sur les faces
  for (int face=0 ; face<nb_faces_tot; face++)
    {
      int nb_comp=0;
      secmem2(face)=0;
      for (int i=0 ; i<2 ; i++)
        {
          int elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              secmem2(face) += secmem1(elem);
            }
        }
      secmem2(face) /= nb_comp;
    }
  secmem2.echange_espace_virtuel();
}

// Description:
//    Renvoie la valeur de la pression calculee pour conserver la masse
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Double
//    Signification: pression calculee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double EDO_Pression_th_VDF::masse_totale(double P,const DoubleTab& T)
{
  int elem, nb_elem=la_zone->nb_elem();
  const DoubleVect& volumes = la_zone->volumes();
  const Loi_Etat_base& loi_ = ref_cast(Loi_Etat_base,le_fluide_->loi_etat().valeur());
  double M=0;
  if (!sub_type(Loi_Etat_Melange_GP,loi_))
    {
      for (elem=0 ; elem<nb_elem ; elem++)
        {
          double v = volumes(elem);
          M += v*loi_.calculer_masse_volumique(P,T[elem]);
        }
    }
  else
    {
      const Loi_Etat_Melange_GP& loi_mel_GP = ref_cast(Loi_Etat_Melange_GP,loi_);
      const DoubleTab& Masse_mol_mel = loi_mel_GP.masse_molaire();

      for (elem=0 ; elem<nb_elem ; elem++)
        {
          double v = volumes(elem);
          double r = 8.314/Masse_mol_mel(elem);
          M += v*loi_mel_GP.calculer_masse_volumique_case(P,T[elem],r,elem);
        }
    }
  M=Process::mp_sum(M);
  return M;
}



// Description:
//    Calcule le second membre de l'equation de continuite :
//    div(rhoU) = W = -dZ/dT    avec Z=rho
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: rho discretise par face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void EDO_Pression_th_VDF::secmembre_divU_Z(DoubleTab& tab_W) const
{
  double dt = le_fluide().vitesse()->equation().schema_temps().pas_de_temps();

  int nb_faces = la_zone->nb_faces();
  int elem,nb_elem = la_zone->nb_elem();
  DoubleVect tab_dZ(nb_elem);
  DoubleTab tab_gradZ(nb_faces);

  const DoubleTab& tab_rhonP0 = le_fluide().loi_etat()->rho_n();
  const DoubleTab& tab_rhonp1P0 = le_fluide().loi_etat()->rho_np1();
  Debog::verifier("divU tab_rhonP0",tab_rhonP0);
  Debog::verifier("divU tab_rhonp1P0",tab_rhonp1P0);
  const DoubleVect& volumes = la_zone->volumes();
  for (elem=0 ; elem<nb_elem ; elem++)
    {
      //Corrections pour eviter l assemblage de la matrice de pression
      tab_dZ(elem) = (tab_rhonp1P0(elem)-tab_rhonP0(elem))/dt;
    }
  double tmp;
  for (elem=0 ; elem<nb_elem ; elem++)
    {
      tmp = tab_dZ(elem) ;
      tab_W(elem) = -tmp * volumes(elem);
    }
}

void  EDO_Pression_th_VDF::calculer_grad(const DoubleTab& inco, DoubleTab& resu)
{
  int face, n0, n1, ori;
  double coef;
  IntTab& face_voisins = la_zone->face_voisins();
  IntVect& orientation = la_zone->orientation();
  DoubleVect& porosite_surf = la_zone->porosite_face();
  DoubleTab& xp = la_zone->xp();
  DoubleVect& volume_entrelaces = la_zone->volumes_entrelaces();

  // Boucle sur les faces internes
  for (face=la_zone->premiere_face_int(); face<la_zone->nb_faces(); face++)
    {
      n0 = face_voisins(face,0);
      n1 = face_voisins(face,1);
      ori = orientation(face);
      coef = volume_entrelaces(face)*porosite_surf(face);
      coef=1;
      resu(face) += coef*(inco(n1)-inco(n0))/(xp(n1,ori)- xp(n0,ori));
    }

}

void EDO_Pression_th_VDF::mettre_a_jour(double temps)
{
  // a voir
  // copie de tab_rho_face_np1 dans tab_rho_face
  int n=tab_rho_face_np1.size_totale();
  for (int i=0; i<n; i++)
    {
      tab_rho_face(i)=tab_rho_face_np1(i);
      tab_rho_face_demi(i)=tab_rho_face_np1(i);
    }
}
void EDO_Pression_th_VDF::mettre_a_jour_CL(double P)
{
  for (int n_bord=0; n_bord<la_zone->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(n_bord);
      if (sub_type(Sortie_libre_pression_imposee_QC, la_cl.valeur()))
        {
          Sortie_libre_pression_imposee_QC& cl = ref_cast_non_const(Sortie_libre_pression_imposee_QC,la_cl.valeur());
          cl.set_Pth(P);
        }
    }
}
