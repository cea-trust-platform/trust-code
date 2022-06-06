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

#include <Masse_VDF_base.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Debog.h>
#include <Equation_base.h>
#include <TRUSTTrav.h>

Implemente_base_sans_constructeur(Masse_VDF_base,"Masse_VDF_base",Solveur_Masse_base);
Masse_VDF_base::Masse_VDF_base() : penalisation_matrice_(0),penalisation_secmem_(0) {}


//     printOn()
/////

Sortie& Masse_VDF_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_VDF_base::readOn(Entree& s)
{
  return s ;
}


///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_VDF_base
//
//////////////////////////////////////////////////////////////

void Masse_VDF_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inc = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inc), mat2;
  const DoubleTab& champ_inconnue = equation().inconnue().valeurs();
  int size = champ_inconnue.dimension_tot(0) * champ_inconnue.line_size();

  IntTab indice(size, 2);
  for(int i=0; i<size; ++i)
    {
      indice(i,0) = indice(i,1) = i;
    }
  mat.dimensionner(indice);

}

void Masse_VDF_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  const DoubleTab& passe = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().passe();
  const DoubleTab& inco = equation().inconnue().valeurs();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
  DoubleTrav diag(inco);

  const int sz = equation().inconnue().valeurs().dimension_tot(0) * diag.line_size();
  diag = 1.;
  appliquer(diag); // M-1
  int prems = 0;
  if (penalisation_flag_)
    {
      if (mat && penalisation_matrice_ == 0)
        {
          double penal = 0;
          for (int i = 0; i < sz; i++)
            penal = std::max(penal, (*mat)(i, i));
          penal = mp_max(penal);
          penalisation_matrice_ = (mp_max_vect(diag) / dt + penal) * 1.e3;
          prems = 1;
        }
      if (penalisation_secmem_ == 0)
        penalisation_secmem_ = mp_max_vect(diag) * 1.e3;
    }
  else
    {
      penalisation_matrice_ = 0;
      penalisation_secmem_ = 1;
    }

  for (int i = 0; i < sz; i++)
    {
      if (diag.addr()[i])
        {
          if (mat)
            (*mat)(i, i) += 1. / (diag.addr()[i] * dt); // M/dt
          secmem.addr()[i] += 1. / (diag.addr()[i] * dt) * passe.addr()[i];
        }
      else
        {
          if (prems)
            {
              if (mat)
                (*mat)(i, i) += penalisation_matrice_;
              prems = 0;
            }
          if (mat)
            (*mat)(i, i) += penalisation_matrice_ / dt;
          secmem.addr()[i] = penalisation_secmem_ * passe.addr()[i];
        }
    }
}

void Masse_VDF_base::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_VDF = ref_cast(Zone_VDF, la_zone_dis_base);
}

void Masse_VDF_base::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, la_zone_Cl_dis_base);
}
