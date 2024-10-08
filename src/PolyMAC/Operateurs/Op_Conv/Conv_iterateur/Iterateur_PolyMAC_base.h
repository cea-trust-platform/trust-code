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

#ifndef Iterateur_PolyMAC_base_included
#define Iterateur_PolyMAC_base_included

#include <Dirichlet_entree_fluide_leaves.h>
#include <Dirichlet_paroi_defilante.h>
#include <Neumann_paroi_adiabatique.h>
#include <Echange_externe_impose.h>
#include <Echange_global_impose.h>
#include <Scalaire_impose_paroi.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet_paroi_fixe.h>
#include <Evaluateur_PolyMAC.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyMAC.h>

#include <Neumann_paroi.h>
#include <TRUST_Deriv.h>
#include <Periodique.h>
#include <TRUST_Ref.h>
#include <Symetrie.h>

class Operateur_base;
class Matrice_Morse;

enum Type_Cl_PolyMAC
{
  symetrie                         //        Symetrie
  , sortie_libre                 //        Neumann_sortie_libre ou derivees
  , entree_fluide                //        Dirichlet_entree_fluide
  , paroi_fixe                        //        Dirichlet_paroi_fixe
  , paroi_defilante                //        Dirichlet_paroi_defilante
  , paroi_adiabatique                //        Neumann_paroi_adiabatique ou derivees
  , paroi                        //      Neumann_paroi
  , echange_externe_impose        //        Echange_externe_impose
  , echange_global_impose        //        Echange_global_impose
  , periodique                        //        periodique
  , scalaire_impose_paroi       //      Scalaire_impose_paroi
  , nouvelle_Cl_PolyMAC                //        Nouvelle_Cl_PolyMAC
};

class Iterateur_PolyMAC_base: public Objet_U
{
  Declare_base(Iterateur_PolyMAC_base);
public:
  void associer(const Domaine_PolyMAC&, const Domaine_Cl_PolyMAC&, const Operateur_base&);
  void associer(const Domaine_dis_base&, const Domaine_Cl_dis_base&, const Operateur_base&);
  void associer_domaine_cl_dis(const Domaine_Cl_dis_base&);
  inline Type_Cl_PolyMAC type_cl(const Cond_lim&) const;
  virtual DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const =0;
  virtual void calculer_flux_bord(const DoubleTab&) const;
  virtual DoubleTab& calculer(const DoubleTab&, DoubleTab&) const =0;
  virtual void contribuer_au_second_membre(DoubleTab&) const =0;
  virtual void ajouter_contribution(const DoubleTab&, Matrice_Morse&) const =0;
  virtual void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse&) const =0;
  virtual Evaluateur_PolyMAC& evaluateur() =0;
  virtual const Evaluateur_PolyMAC& evaluateur() const =0;

  inline const Domaine_PolyMAC& domaine() const { return le_domaine.valeur(); }
  inline const Domaine_Cl_PolyMAC& domaine_Cl() const { return la_zcl.valeur(); }

  virtual void completer_()=0;
  virtual int impr(Sortie& os) const =0;

protected:
  OBS_PTR(Domaine_PolyMAC) le_domaine;
  OBS_PTR(Domaine_Cl_PolyMAC) la_zcl;
  OBS_PTR(Operateur_base) op_base;
};

inline Type_Cl_PolyMAC Iterateur_PolyMAC_base::type_cl(const Cond_lim& la_cl) const
{
  Type_Cl_PolyMAC retour = nouvelle_Cl_PolyMAC;
  if (sub_type(Symetrie, la_cl.valeur()))
    retour = symetrie;
  else if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
    retour = sortie_libre;
  else if (sub_type(Dirichlet_entree_fluide, la_cl.valeur()))
    retour = entree_fluide;
  else if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
    retour = paroi_fixe;
  else if (sub_type(Dirichlet_paroi_defilante, la_cl.valeur()))
    retour = paroi_defilante;
  else if (sub_type(Neumann_paroi_adiabatique, la_cl.valeur()))
    retour = paroi_adiabatique;
  else if (sub_type(Neumann_paroi, la_cl.valeur()))
    retour = paroi;
  else if (sub_type(Echange_externe_impose, la_cl.valeur()))
    retour = echange_externe_impose;
  else if (sub_type(Echange_global_impose, la_cl.valeur()))
    retour = echange_global_impose;
  else if (sub_type(Periodique, la_cl.valeur()))
    retour = periodique;
  else if (sub_type(Scalaire_impose_paroi, la_cl.valeur()))
    retour = scalaire_impose_paroi;
  return retour;
}

#endif /* Iterateur_PolyMAC_base_included */
