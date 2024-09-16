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

#include <Viscosite_turbulente_LES_base.h>
#include <Pb_Multiphase.h>
#include <Domaine_VF.h>

Implemente_base(Viscosite_turbulente_LES_base, "Viscosite_turbulente_LES_base", Viscosite_turbulente_base);

Sortie& Viscosite_turbulente_LES_base::printOn(Sortie& os) const { return os; }

Entree& Viscosite_turbulente_LES_base::readOn(Entree& is)
{
  calculer_longueurs_caracteristiques();
  return is;
}

void Viscosite_turbulente_LES_base::calculer_longueurs_caracteristiques()
{
  Cerr << "LES filter length scale calculation ..." << finl;
  const Domaine_VF& domaine_VF = ref_cast(Domaine_VF, pb_->domaine_dis());
  const DoubleVect& volume = domaine_VF.volumes();
  const int nb_elem = domaine_VF.domaine().nb_elem();

  l_.resize(nb_elem);
  for (int elem = 0; elem < nb_elem; elem++)
    l_(elem) = exp((log(volume[elem])) / double(Objet_U::dimension));
}
