/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Modele_turbulence_Longueur_Melange_VDF.cpp
// Directory:   $TURBULENCE_ROOT/src/Specializations/VDF/Modeles_Turbulence/RANS/Hydr
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_Longueur_Melange_VDF.h>
#include <Probleme_base.h>
#include <Debog.h>
#include <TRUSTTrav.h>
#include <Domaine_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <Champ_Face_VDF.h>
#include <Schema_Temps_base.h>
#include <Domaine_Cl_dis.h>
#include <Equation_base.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_Longueur_Melange_VDF,"Modele_turbulence_hyd_Longueur_Melange_VDF",Mod_turb_hyd_RANS_0_eq);

Modele_turbulence_Longueur_Melange_VDF::Modele_turbulence_Longueur_Melange_VDF()
{
  alt_min=0.;
  alt_max=2.;
  direction=1;
}

/*! @brief Ecrit le type de l'objet sur un flot de sortie.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Modele_turbulence_Longueur_Melange_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


/*! @brief Simple appel a Mod_turb_hyd_RANS_0_eq::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Modele_turbulence_Longueur_Melange_VDF::readOn(Entree& s)
{
  return Mod_turb_hyd_RANS_0_eq::readOn(s);
}

void Modele_turbulence_Longueur_Melange_VDF::set_param(Param& param)
{
  Mod_turb_hyd_RANS_0_eq::set_param(param);
  param.ajouter("canal_hmin",&alt_min);
  param.ajouter("canal_hmax",&alt_max);
  param.ajouter("direction_normale_canal",&direction);
}

int Modele_turbulence_Longueur_Melange_VDF::preparer_calcul( )
{
  Modele_turbulence_hyd_base::preparer_calcul();
  mettre_a_jour(0.);
  return 1;
}

void Modele_turbulence_Longueur_Melange_VDF::associer(
  const Domaine_dis& domaine_dis,
  const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VDF = ref_cast(Domaine_VDF,domaine_dis.valeur());
  le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF,domaine_Cl_dis.valeur());
}

Champ_Fonc& Modele_turbulence_Longueur_Melange_VDF::calculer_viscosite_turbulente()
{
  double hauteur = std::fabs(alt_max-alt_min); // test alt_max>alt_min a faire, plutot que de prendre fabs ??
  //Attention, ici "hauteur" est la hauteur reelle du canal (pas la demi-hauteur)
  const double Kappa = 0.415;
  double Cmu = CMU;

  double temps = mon_equation->inconnue().temps();
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  DoubleVect& k = energie_cinetique_turb_.valeurs();
  const int nb_elem = domaine_VDF.nb_elem();
  const int nb_elem_tot = domaine_VDF.nb_elem_tot();
  const DoubleTab& xp = domaine_VDF.xp();

  Sij2.resize(nb_elem_tot);

  calculer_Sij2();

  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }

  Debog::verifier("Modele_turbulence_Longueur_Melange_VDF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  if (k.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent kinetic energy." << finl;
      exit();
    }

  //    CANAL PLAN suivant (Ox - h=2) **********************************

  for(int elem=0 ; elem<nb_elem ; elem ++)
    {
      double y = xp(elem,direction);
      y = std::fabs(y-alt_min);
      if(y>(hauteur/2.)) y=std::fabs(alt_max-y);

      visco_turb(elem) = Kappa*Kappa*y*y*(1.-2*y/hauteur)*sqrt(2.*Sij2(elem));
      k(elem)=pow(visco_turb(elem)/(Cmu*y),2);
    }

  Debog::verifier("Modele_turbulence_Longueur_Melange_VDF::calculer_viscosite_turbulente visco_turb 1",visco_turb);

  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Modele_turbulence_Longueur_Melange_VDF::calculer_energie_cinetique_turb()
{
  // PQ : 11/08/06 :    L'estimation de k repose sur les expressions :
  //                                 - nu_t = C_mu * k^2 / eps
  //                                - eps = k^(3/2) / l
  //

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // pour des raisons de commodite, l'estimation de k est realisee dans calculer_viscosite_turbulente()
  /////////////////////////////////////////////////////////////////////////////////////////////////

  double temps = mon_equation->inconnue().temps();

  energie_cinetique_turb_.changer_temps(temps);
}

void Modele_turbulence_Longueur_Melange_VDF::calculer_Sij2()
{
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF,mon_equation->inconnue().valeur());
  const Domaine_Cl_VDF& domaine_Cl_VDF = le_dom_Cl_VDF.valeur();
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  const int nb_elem = domaine_VDF.nb_elem_tot();

  assert (vitesse.line_size() == 1);
  DoubleTab duidxj(nb_elem,dimension,dimension, vitesse.line_size());
  int i,j;
  double Sij;

  Sij2=0.;

  ch.calcul_duidxj(vitesse,duidxj,domaine_Cl_VDF);

  for (int elem=0 ; elem<nb_elem ; elem++)
    {
      for ( i=0 ; i<dimension ; i++)
        for ( j=0 ; j<dimension ; j++)
          {
            //Deplacement du calcul de Sij
            Sij=0.5*(duidxj(elem,i,j,0) + duidxj(elem,j,i,0));
            Sij2(elem)+=Sij*Sij;
          }
    }
}
