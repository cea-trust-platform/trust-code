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

#include <Modele_turbulence_hyd_LES_VDF_base.h>

#include <Domaine_Cl_VDF.h>
#include <Domaine_VDF.h>

Implemente_base(Modele_turbulence_hyd_LES_VDF_base, "Modele_turbulence_hyd_LES_VDF_base", Modele_turbulence_hyd_LES_base);

Sortie& Modele_turbulence_hyd_LES_VDF_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Modele_turbulence_hyd_LES_VDF_base::readOn(Entree& is)
{
  return Modele_turbulence_hyd_LES_base::readOn(is);
}

void Modele_turbulence_hyd_LES_VDF_base::calculer_longueurs_caracteristiques()
{
  const Domaine_VDF& domaine_VDF = ref_cast(Domaine_VDF, le_dom_VF_.valeur());
  int nb_elem = domaine_VDF.domaine().nb_elem();
  const IntTab& elem_faces = domaine_VDF.elem_faces();
  const IntVect& orientation = domaine_VDF.orientation();

  l_.resize(nb_elem);

  ArrOfDouble h(dimension);
  double dist_tot, dist_min, dist_max, dist_moy;
  double a1, a2, f_scotti;

  const int modele_scotti = (methode_ == Motcle("Scotti"));
  if (modele_scotti && (dimension == 2))
    {
      Cerr << "The Scotti correction can be used only for dimension 3." << finl;
      exit();
    }

  for (int elem = 0; elem < nb_elem; elem++)
    {
      for (int i = 0; i < dimension; i++)
        h[i] = domaine_VDF.dim_elem(elem, orientation(elem_faces(elem, i)));

      if (dimension == 2)
        l_(elem) = exp((log(h[0] * h[1])) / 2);
      else
        l_(elem) = exp((log(h[0] * h[1] * h[2])) / 3);

      if (modele_scotti)
        {
          dist_tot = h[0] + h[1] + h[2];

          dist_min = min_array(h);
          dist_max = max_array(h);
          dist_moy = dist_tot - dist_min - dist_max;

          a1 = dist_min / dist_max;
          a2 = dist_moy / dist_max;

          f_scotti = cosh(sqrt((4. / 27.) * (log(a1) * log(a1) - log(a1) * log(a2) + log(a2) * log(a2))));

          l_(elem) *= f_scotti;
        }
    }
}
