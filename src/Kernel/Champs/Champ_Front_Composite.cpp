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

#include <Champ_Front_Composite.h>
#include <Motcle.h>

Implemente_instanciable(Champ_Front_Composite,"Champ_Front_Composite",Champ_front_base);

Sortie& Champ_Front_Composite::printOn(Sortie& os) const {  return Champ_front_base::printOn(os); }

Entree& Champ_Front_Composite::readOn(Entree& is)
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
      Cerr << "Champ_Front_Composite should define the same field types !" << finl, Process::exit();

  // pour la methode valeurs()
  const int ncompo = z_fld_[0]->nb_comp() * dim_, nnodes = z_fld_[0]->valeurs().dimension_tot(0);
  fixer_nb_comp(ncompo);
  les_valeurs->valeurs().resize(nnodes, nb_compo_);
  fill_valeurs_composite();
//  set_temps_defaut(z_fld_[0]->get_temps_defaut());

  return is;
}
/*
 * On stock comme ca : X X X ... Y Y Y ... Z Z Z ...
 */
void Champ_Front_Composite::fill_valeurs_composite()
{
  for (int nbp = 0; nbp < dim_; nbp++)
    for (int i = 0; i < z_fld_[0]->valeurs().dimension_tot(0); i++)
      for (int j = 0; j < z_fld_[0]->nb_comp(); j++)
        les_valeurs->valeurs()(i, nbp + j * dim_) = z_fld_[nbp]->valeurs()(i, j);
}

void Champ_Front_Composite::mettre_a_jour(double temps)
{
  for (auto &fld : z_fld_) fld->mettre_a_jour(temps);
}

int Champ_Front_Composite::initialiser(double temps, const Champ_Inc_base& inco)
{
  for (auto &fld : z_fld_) fld->initialiser(temps,inco);
  return 1;
}

void Champ_Front_Composite::valeurs_face(int face, DoubleVect& var) const
{
  // Bricorama ....
  assert (nb_compo_ == z_fld_[0]->nb_comp() * dim_); // pour micho :-)
  var.resize(nb_compo_);
  for (int nbp = 0; nbp < dim_; nbp++)
    {
      DoubleVect tmp;
      z_fld_[nbp]->valeurs_face(face,tmp);
      assert (var.size() == dim_ * tmp.size()); // pour micho :-)
      for (int j = 0; j < tmp.size(); j++)
        var(nbp + j * dim_) = tmp(j);
    }
}
