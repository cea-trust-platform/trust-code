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

#include <Champ_Fonc_Q1_EF.h>
#include <Domaine_EF.h>
#include <Domaine.h>

Implemente_instanciable(Champ_Fonc_Q1_EF, "Champ_Fonc_Q1_EF", Champ_Fonc_Q1_base);

Sortie& Champ_Fonc_Q1_EF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_Q1_EF::readOn(Entree& s) {  return s; }

const Domaine_EF& Champ_Fonc_Q1_EF::domaine_EF() const
{
  return ref_cast(Domaine_EF, le_dom_VF.valeur());
}

void Champ_Fonc_Q1_EF::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}

int Champ_Fonc_Q1_EF::imprime(Sortie& os, int ncomp) const
{
  const Domaine_dis_base& domaine_dis = domaine_dis_base();
  const Domaine& domaine = domaine_dis.domaine();
  const DoubleTab& coord = domaine.coord_sommets();
  const int nb_som = domaine.nb_som();
  const DoubleTab& val = valeurs();
  int som;
  os << nb_som << finl;
  for (som = 0; som < nb_som; som++)
    {
      if (dimension == 3)
        os << coord(som, 0) << " " << coord(som, 1) << " " << coord(som, 2) << " ";
      if (dimension == 2)
        os << coord(som, 0) << " " << coord(som, 1) << " ";
      if (nb_compo_ == 1)
        os << val(som) << finl;
      else
        os << val(som, ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_Fonc_Q1_EF::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}
