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

#include <Correlation_Vec_Sca_VDF.h>
#include <Domaine_VF.h>

Implemente_instanciable(Correlation_Vec_Sca_VDF,"Correlation_Vec_Sca_VDF",Champ_Fonc_P0_VDF);

Sortie& Correlation_Vec_Sca_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Correlation_Vec_Sca_VDF::readOn(Entree& s) { return s ; }

void Correlation_Vec_Sca_VDF::associer_champ_Vec(const Champ_base& le_champ_Vec)
{
  mon_champ_Vec_= le_champ_Vec;
}

void Correlation_Vec_Sca_VDF::associer_champ_Sca(const Champ_base& le_champ_Sca)
{
  mon_champ_Sca_= le_champ_Sca;
}

void Correlation_Vec_Sca_VDF::mettre_a_jour(double tps)
{
  int nb_elem=le_dom_VF.valeur().domaine().nb_elem();

  const DoubleTab& centres_de_gravites = le_dom_VF->xp();

  DoubleTab& correlation = valeurs();
  int elem, ncom;

  // Interpolation du champ scalaire de la correlation au
  // centre des mailles.
  DoubleTab valeurs_Sca(nb_elem, mon_champ_Sca_->nb_comp());
  mon_champ_Sca_->valeur_aux_centres_de_gravite(centres_de_gravites, valeurs_Sca);

  // Interpolation du champ vecteur de la correlation au
  // centre des mailles.
  DoubleTab valeurs_Vec(nb_elem, mon_champ_Vec_->nb_comp());
  mon_champ_Vec_->valeur_aux_centres_de_gravite(centres_de_gravites, valeurs_Vec);

  for(elem=0; elem<nb_elem; elem++)
    {
      correlation(elem,0) = valeurs_Sca(elem,0);
      for (ncom=0; ncom<dimension; ncom++)
        correlation(elem,ncom+1) = valeurs_Vec(elem,ncom);
    }

  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}
