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

#include <Modele_turbulence_hyd_sous_maille_Fst_VEF.h>
#include <Champ_P1NC.h>
#include <TRUSTTrav.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Domaine_VEF.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_hyd_sous_maille_Fst_VEF, "Modele_turbulence_hyd_sous_maille_fst_VEF", Modele_turbulence_hyd_ss_maille_VEF_base);

Modele_turbulence_hyd_sous_maille_Fst_VEF::Modele_turbulence_hyd_sous_maille_Fst_VEF()
{
  C1 = 0.777 * 0.18247 * 0.18247;
}

Sortie& Modele_turbulence_hyd_sous_maille_Fst_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Modele_turbulence_hyd_sous_maille_Fst_VEF::readOn(Entree& is)
{
  return Modele_turbulence_hyd_ss_maille_VEF_base::readOn(is);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Modele_turbulence_hyd_sous_maille_Fst_VEF
//
//////////////////////////////////////////////////////////////////////////////

Champ_Fonc& Modele_turbulence_hyd_sous_maille_Fst_VEF::calculer_viscosite_turbulente()
{
  //  static double C1 = 0.02587;
  //  static double C1 =0.777*0.18247*0.18247; // PQ:07/09/05 affectation de C1 dans le Readon
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  const int nb_elem = domaine_VEF.nb_elem();

  Racine.resize(nb_elem_tot);

  calculer_racine();

  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }

  Debog::verifier("Modele_turbulence_hyd_sous_maille_Fst_VEF::calculer_viscosite_turbulente visco_turb 0", visco_turb);

  for (int elem = 0; elem < nb_elem; elem++)
    visco_turb(elem) = C1 * l_(elem) * l_(elem) * sqrt(Racine[elem]);

  Debog::verifier("Modele_turbulence_hyd_sous_maille_Fst_VEF::calculer_viscosite_turbulente visco_turb 1", visco_turb);

  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

// PQ : 17/08/06 : on garde cette methode en commentaire au cas ou ...
//
//const Champ_Fonc& Modele_turbulence_hyd_sous_maille_Fst_VEF::calculer_energie_cinetique_turb()
//{
//  static double C2 = 0.777*0.777*0.18247*0.18247*9.56;
////  double C2 = cs*cs*9.56;
//  double temps = mon_equation->inconnue().temps();
//  DoubleVect& k = energie_cinetique_turb_.valeurs();
//  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
//  const int nb_elem = domaine_VEF.nb_elem();
//
//  if (k.size() != nb_elem)
//    {
//      Cerr << "erreur dans la taille du DoubleVect valeurs de l'energie cinetique turbulente" << finl;
//      exit();
//    }
//
//  for (int elem=0 ; elem<nb_elem; elem++)
//    k(elem)=C2*C2*l(elem)*l(elem)*Racine[elem];
//
//  energie_cinetique_turb_.changer_temps(temps);
//  return energie_cinetique_turb_;
//}

void Modele_turbulence_hyd_sous_maille_Fst_VEF::calculer_racine()
{
  const DoubleTab& la_vitesse = mon_equation->inconnue().valeurs();
  const Domaine_Cl_VEF& domaine_Cl_VEF = le_dom_Cl_VEF.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
  const int nb_elem = domaine_VEF.nb_elem();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();
  const DoubleVect& vol = domaine_VEF.volumes();

  DoubleTab Sij(nb_elem_tot, dimension, dimension);

  // MAINTENANT : on prend la racine cubique du volume

  for (int elem = 0; elem < nb_elem; elem++)
    l_(elem) = exp(log(vol[elem]) / double(dimension));

  DoubleTab duidxj(nb_elem, dimension, dimension);
  Champ_P1NC::calcul_gradient(la_vitesse, duidxj, domaine_Cl_VEF);

  for (int elem = 0; elem < nb_elem_tot; elem++)
    for (int i = 0; i < dimension; i++)
      for (int j = 0; j < dimension; j++)
        Sij(elem, i, j) = 0.5 * (duidxj(elem, i, j) + duidxj(elem, j, i));

  DoubleTrav vorticite(nb_elem, dimension);
  vorticite = 0;
  Champ_P1NC& vit = ref_cast(Champ_P1NC, mon_equation->inconnue().valeur());
  vit.cal_rot_ordre1(vorticite);

  // On calcule Racine
  for (int elem = 0; elem < nb_elem; elem++)
    {
      double temp = 0.;
      for (int i = 0; i < dimension; i++)
        {
          for (int j = 0; j < dimension; j++)
            temp += 2. * Sij(elem, i, j) * Sij(elem, i, j);

          temp += vorticite(elem, i) * vorticite(elem, i);
        }
      Racine(elem) = temp;
    }
}
