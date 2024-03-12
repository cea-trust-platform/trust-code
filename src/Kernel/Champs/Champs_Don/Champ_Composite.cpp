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

#include <Champ_Composite.h>
#include <Motcle.h>

Implemente_instanciable(Champ_Composite,"Champ_Composite",Champ_Don_base);
// XD champ_composite champ_don_base champ_composite 0 Composite field. Used in multiphase problems to associate data to each phase.
// XD attr dim entier dim 0 Number of field components.
// XD attr bloc bloc_lecture bloc 0 Values Various pieces of the field, defined per phase. Part 1 goes to phase 1, etc...

Sortie& Champ_Composite::printOn(Sortie& os) const { return Champ_Don_base::printOn(os); }

Entree& Champ_Composite::readOn(Entree& is)
{
  nommer(que_suis_je()); // pour printOn

  Motcle motlu, acc_ouv("{"), acc_fer("}");
  is >> dim_;
  assert (dim_ > 0);
  z_fld_.resize(dim_);

  is >> motlu;
  if (motlu != acc_ouv)
    {
      Cerr << "Error in the readOn of Champ_Composite ! We expected a { and not " << motlu << "  ! Call the 911 !" << finl;
      Process::exit();
    }

  for (int i = 0; i < dim_; i++) is >> z_fld_[i];

  is >> motlu;
  if (motlu != acc_fer)
    {
      Cerr << "Error in the readOn of Champ_Composite ! We expected a } and not " << motlu << "  ! Call the 911 !" << finl;
      Process::exit();
    }

  // XXX : On verifie qu'on a lu les memes types de champs ...
  for (int i = 1; i < dim_ ; i++)
    if (z_fld_[i]->que_suis_je() != z_fld_[0]->que_suis_je())
      {
        Cerr << "Champ_Composite should define the same field types !" << finl;
        Process::exit();
      }

  // pour la methode valeurs()
  const int ncompo = z_fld_[0]->nb_comp() * dim_, nnodes = z_fld_[0]->valeurs().dimension_tot(0);
  fixer_nb_comp(ncompo);
  fixer_nb_valeurs_nodales(nnodes);
  fill_valeurs_composite();
  return is;
}

/*
 * On stock comme ca : X X X ... Y Y Y ... Z Z Z ...
 */
void Champ_Composite::fill_valeurs_composite()
{
  for (int nbp = 0; nbp < dim_; nbp++)
    for (int i = 0; i < nb_valeurs_nodales(); i++)
      for (int j = 0; j < z_fld_[0]->nb_comp(); j++)
        valeurs_(i, nbp + j * dim_) = z_fld_[nbp]->valeurs()(i, j);
}

void Champ_Composite::mettre_a_jour(double tps)
{
  for (auto &fld : z_fld_)
    fld->mettre_a_jour(tps);
}

DoubleTab& Champ_Composite::valeur_aux(const DoubleTab& xv, DoubleTab& tab_valeurs) const
{
  for (int nbp = 0; nbp < dim_; nbp++)
    {
      DoubleTab tmp(tab_valeurs.dimension_tot(0), tab_valeurs.line_size() / dim_);
      z_fld_[nbp]->valeur_aux(xv,tmp);
      assert (tab_valeurs.dimension_tot(0) == tmp.dimension_tot(0));
      assert (tmp.line_size() * dim_ == tab_valeurs.line_size());
      for (int i = 0; i < tab_valeurs.dimension_tot(0); i++)
        for (int j = 0; j < tmp.line_size(); j++)
          tab_valeurs(i, nbp + j * dim_) = tmp(i,j);
    }
  tab_valeurs.echange_espace_virtuel();

  return tab_valeurs;
}
