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

#include <Fluide_Weakly_Compressible.h>
#include <Loi_Etat_Binaire_GP_WC.h>
#include <Debog.h>

Implemente_instanciable(Loi_Etat_Binaire_GP_WC,"Loi_Etat_Binaire_Gaz_Parfait_WC",Loi_Etat_Binaire_GP_base);
// XD binaire_gaz_parfait_WC loi_etat_gaz_parfait_base binaire_gaz_parfait_WC -1 Class for perfect gas binary mixtures state law used with a weakly-compressible fluid under the iso-thermal and iso-bar assumptions.
// XD attr molar_mass1 double molar_mass1 0 Molar mass of species 1 (in kg/mol).
// XD attr molar_mass2 double molar_mass2 0 Molar mass of species 2 (in kg/mol).
// XD attr mu1 double mu1 0 Dynamic viscosity of species 1 (in kg/m.s).
// XD attr mu2 double mu2 0 Dynamic viscosity of species 2 (in kg/m.s).
// XD attr temperature double temperature 0 Temperature (in Kelvin) which will be constant during the simulation since this state law only works for iso-thermal conditions.
// XD attr diffusion_coeff double diffusion_coeff 0 Diffusion coefficient assumed the same for both species (in m2/s).

Sortie& Loi_Etat_Binaire_GP_WC::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_Binaire_GP_WC::readOn(Entree& is)
{
  Cerr<<"Lecture de la loi d'etat Melange Binaire WC ... "<<finl;
  return Loi_Etat_Binaire_GP_base::readOn(is);
}

double Loi_Etat_Binaire_GP_WC::calculer_masse_volumique(double P, double Y1) const
{
  return Loi_Etat_Binaire_GP_base::calculer_masse_volumique(P,Y1);
}

// Description:
//    Calcule la masse volumique
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Binaire_GP_WC::calculer_masse_volumique()
{
  const DoubleTab& tab_Y1 = le_fluide->inco_chaleur().valeurs();
  Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  const DoubleTab& pres = FWC.pression_th_tab();
  const int n = tab_rho.size();
  for (int som=0 ; som<n ; som++)
    {
      tab_rho_np1(som) = calculer_masse_volumique(pres(som,0),tab_Y1(som,0));
      tab_rho(som,0) = 0.5 * ( tab_rho_n(som) + tab_rho_np1(som) );
    }
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
  Debog::verifier("Loi_Etat_Binaire_GP_WC::calculer_masse_volumique, tab_rho_np1",tab_rho_np1);
  Debog::verifier("Loi_Etat_Binaire_GP_WC::calculer_masse_volumique, tab_rho",tab_rho);
}
