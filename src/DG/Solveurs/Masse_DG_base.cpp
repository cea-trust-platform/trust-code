/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Masse_DG_base.h>
#include <Domaine_Cl_DG.h>
#include <Domaine_DG.h>
#include <Champ_Elem_DG.h>
#include <Equation_base.h>
#include <Milieu_base.h>

Implemente_base(Masse_DG_base, "Masse_DG_base", Solveur_Masse_base);
Sortie& Masse_DG_base::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }
Entree& Masse_DG_base::readOn(Entree& s) { return s; }

void Masse_DG_base::associer_domaine_dis_base(const Domaine_dis_base& le_dom_dis_base)
{
  le_dom_dg_ = ref_cast(Domaine_DG, le_dom_dis_base);
}

void Masse_DG_base::associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& le_dom_Cl_dis_base)
{
  le_dom_Cl_dg_ = ref_cast(Domaine_Cl_DG, le_dom_Cl_dis_base);
}

void Masse_DG_base::appliquer_coef(DoubleVect& coef) const
{
  if (has_coefficient_temporel_)
    {
      OBS_PTR(Champ_base) ref_coeff;
      ref_coeff = equation().get_champ(name_of_coefficient_temporel_);

      DoubleTab values;
      if (sub_type(Champ_Inc_base,ref_coeff.valeur()))
        {
          const Champ_Inc_base& coeff = ref_cast(Champ_Inc_base,ref_coeff.valeur());
          ConstDoubleTab_parts val_parts(coeff.valeurs());
          values.ref(val_parts[0]);

        }
      else if (sub_type(Champ_Fonc_base,ref_coeff.valeur()))
        {
          const Champ_Fonc_base& coeff = ref_cast(Champ_Fonc_base,ref_coeff.valeur());
          values.ref(coeff.valeurs());

        }
      else if (sub_type(Champ_Don_base,ref_coeff.valeur()))
        {
          DoubleTab nodes;
          equation().inconnue().remplir_coord_noeuds(nodes);
          ref_coeff->valeur_aux(nodes,values);
        }
      tab_multiply_any_shape(coef, values, VECT_REAL_ITEMS);
    }
}

void Masse_DG_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inc = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inc);
  const DoubleTab& inco = equation().inconnue().valeurs();

  const int nb_elem_tot = inco.dimension_tot(0);
  const int nb_bfunc = inco.line_size();

  int size_indices = le_dom_dg_->gram_schmidt() ? nb_elem_tot*nb_bfunc : nb_elem_tot*nb_bfunc*nb_bfunc;

  IntTab indice(size_indices, 2);
  int current_indice = 0;
  if (le_dom_dg_->gram_schmidt())
    {
      for (int e = 0; e < nb_elem_tot; e++)
        {
          for (int i = 0; i < nb_bfunc; i++ )
            {
              indice(current_indice+i, 0) = current_indice+i;
              indice(current_indice+i, 1) = current_indice+i;
            }
          current_indice+=nb_bfunc;
        }
    }
  else
    {
      for (int e = 0; e < nb_elem_tot; e++)
        {
          for (int i = 0; i < nb_bfunc; i++ )
            for (int j = 0; j < nb_bfunc; j++ )
              {
                indice((e*nb_bfunc+i)*nb_bfunc+j, 0) = current_indice+i;
                indice((e*nb_bfunc+i)*nb_bfunc+j, 1) = current_indice+j;
              }
          current_indice+=nb_bfunc;
        }
    }
  mat.dimensionner(indice);
}

void Masse_DG_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  const DoubleTab& passe = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().passe();
  const DoubleTab& inco = equation().inconnue().valeurs();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
  const Matrice_Morse& mass_matrix = ch.get_mass_matrix();

  const DoubleVect& volume = le_dom_dg_->volumes();

  const int nb_elem_tot = inco.dimension_tot(0);
  const int nb_bfunc = inco.line_size();

  DoubleTrav coef(equation().milieu().porosite_elem());
  coef = 1.;
  appliquer_coef(coef);

  int current_indice = 0;
  Matrice_Morse test;

  if (le_dom_dg_->gram_schmidt())
    {
      for (int e = 0; e < nb_elem_tot; e++)
        {
          for (int i=0; i<nb_bfunc; i++)
            {
              if (mat)
                (*mat)(current_indice+i, current_indice+i) += coef[e]*volume[e] / dt;
              secmem(e,i) += coef[e]*volume[e]*passe(e,i) / dt;
            }
          current_indice+=nb_bfunc;
        }
    }
  else
    {
      for (int e = 0; e < nb_elem_tot; e++)
        {
          for (int i=0; i<nb_bfunc; i++)
            {
              for (int j=0; j<nb_bfunc; j++)
                {
                  if (mat)
                    (*mat)(current_indice+i, current_indice+j) += coef[e]*mass_matrix(current_indice+i, current_indice+j) / dt;
                  secmem(e,i) += coef[e]*mass_matrix(current_indice+i, current_indice+j)*passe(e,j) / dt;
                }
            }
          current_indice+=nb_bfunc;
        }
    }
}
