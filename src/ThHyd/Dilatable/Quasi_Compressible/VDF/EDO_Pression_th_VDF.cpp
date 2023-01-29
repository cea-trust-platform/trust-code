/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Sortie_libre_pression_imposee_QC.h>
#include <Convection_Diffusion_Temperature.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <EDO_Pression_th_VDF.h>
#include <communications.h>
#include <Domaine_Cl_VDF.h>
#include <Champ_Face_VDF.h>
#include <Domaine_VDF.h>
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

void  EDO_Pression_th_VDF::associer_domaines(const Domaine_dis& domaine, const Domaine_Cl_dis& domaine_cl)
{
  le_dom = ref_cast(Domaine_VDF,domaine.valeur());
  le_dom_Cl = domaine_cl;
}

/*! @brief Complete l'EDO : calcule rho sur les faces
 *
 */
void EDO_Pression_th_VDF::completer()
{
  const DoubleTab& tab_ICh = le_fluide_->inco_chaleur().valeurs();
  double Pth=le_fluide_->pression_th();
  M0=masse_totale(Pth,tab_ICh);
  le_fluide_->checkTraitementPth(le_dom_Cl.valeur());
}

/*! @brief Renvoie la valeur de la pression calculee pour conserver la masse
 *
 * @return (Double) pression calculee
 */
double EDO_Pression_th_VDF::masse_totale(double P,const DoubleTab& T)
{
  int elem, nb_elem=le_dom->nb_elem();
  const DoubleVect& volumes = le_dom->volumes();
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
  IntTab& face_voisins = le_dom->face_voisins();
  IntVect& orientation = le_dom->orientation();
  DoubleVect& porosite_surf = le_fluide_->porosite_face();
  DoubleTab& xp = le_dom->xp();
  DoubleVect& volume_entrelaces = le_dom->volumes_entrelaces();

  // Boucle sur les faces internes
  for (face=le_dom->premiere_face_int(); face<le_dom->nb_faces(); face++)
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
  for (int n_bord=0; n_bord<le_dom->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl->les_conditions_limites(n_bord);
      if (sub_type(Sortie_libre_pression_imposee_QC, la_cl.valeur()))
        {
          Sortie_libre_pression_imposee_QC& cl = ref_cast_non_const(Sortie_libre_pression_imposee_QC,la_cl.valeur());
          cl.set_Pth(P);
        }
    }
}
