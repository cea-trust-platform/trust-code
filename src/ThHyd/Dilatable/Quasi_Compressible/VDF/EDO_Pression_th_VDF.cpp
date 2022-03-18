/****************************************************************************
* Copyright (c) 2022, CEA
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
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/VDF
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Sortie_libre_pression_imposee_QC.h>
#include <Convection_Diffusion_Temperature.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <Check_espace_virtuel.h>
#include <EDO_Pression_th_VDF.h>
#include <communications.h>
#include <Zone_Cl_VDF.h>
#include <Champ_Face.h>
#include <Zone_VDF.h>
#include <TRUSTTab.h>
#include <Debog.h>

Implemente_base_sans_constructeur(EDO_Pression_th_VDF,"EDO_Pression_th_VDF",EDO_Pression_th_base);

EDO_Pression_th_VDF::EDO_Pression_th_VDF() : M0(-1.) { }

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
  if (!sub_type(Loi_Etat_Multi_GP_QC,loi_))
    {
      for (elem=0 ; elem<nb_elem ; elem++)
        {
          double v = volumes(elem);
          M += v*loi_.calculer_masse_volumique(P,T[elem]);
        }
    }
  else
    {
      const Loi_Etat_Multi_GP_QC& loi_mel_GP = ref_cast(Loi_Etat_Multi_GP_QC,loi_);
      const DoubleTab& Masse_mol_mel = loi_mel_GP.masse_molaire();

      for (elem=0 ; elem<nb_elem ; elem++)
        {
          double v = volumes(elem);
          double r = 8.3143/Masse_mol_mel(elem);
          M += v*loi_mel_GP.calculer_masse_volumique(P,T[elem],r);
        }
    }
  M=Process::mp_sum(M);
  return M;
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
