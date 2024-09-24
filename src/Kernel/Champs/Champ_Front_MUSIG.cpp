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

#include <Champ_Front_MUSIG.h>
#include <Motcle.h>

Implemente_instanciable( Champ_Front_MUSIG, "Champ_Front_MUSIG", Champ_Front_Composite ) ;
// XD champ_front_musig champ_front_composite champ_front_musig 0 MUSIG front field. Used in multiphase problems to associate data to each phase.
// XD  attr dim suppress_param dim 1 del
// XD  attr bloc bloc_lecture bloc 0 Not set

Sortie& Champ_Front_MUSIG::printOn(Sortie& os) const { return Champ_Front_Composite::printOn(os); }

Entree& Champ_Front_MUSIG::readOn( Entree& is )
{
  nommer(que_suis_je()); // pour printOn

  Motcle motlu, acc_ouv("{"), acc_fer("}");
  dim_ = 0;

  is >> motlu;
  if (motlu != acc_ouv)
    {
      Cerr << "Error in the readOn of Champ_Front_MUSIG ! We expected a { and not " << motlu << "  ! Call Maximus Decimus !" << finl;
      Process::exit();
    }

  for (is >> motlu; motlu != acc_fer; is >> motlu)
    {
      if (motlu != "NBPHASES")
        {
          Cerr << "Error in the readOn of Champ_Front_MUSIG ! You need yo specify the number of subphases with NBPHASES !" << finl;
          Process::exit();
        }
      int nbSubPhases;
      is >> nbSubPhases;
      OWN_PTR(Champ_front_base) tmp;
      is >> tmp;
      for(int j=0; j<nbSubPhases; j++)
        {
          OWN_PTR(Champ_front_base) tmp2(tmp);
          z_fld_.push_back(tmp2);
        }
      dim_ += nbSubPhases;
    }

  // XXX : On verifie qu'on a lu les memes types de champs ...
  for (int i = 1; i < dim_ ; i++)
    if (z_fld_[i]->que_suis_je() != z_fld_[0]->que_suis_je())
      {
        Cerr << "Champ_Front_MUSIG should define the same field types !" << finl;
        Process::exit();
      }

  // pour la methode valeurs()
  const int ncompo = z_fld_[0]->nb_comp() * dim_, nnodes = z_fld_[0]->valeurs().dimension_tot(0);
  fixer_nb_comp(ncompo);
  les_valeurs->valeurs().resize(nnodes, nb_compo_);
  fill_valeurs_composite();

  return is;
}
