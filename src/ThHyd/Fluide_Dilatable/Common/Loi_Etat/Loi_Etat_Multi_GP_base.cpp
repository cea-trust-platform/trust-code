/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Loi_Etat_Multi_GP_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common/Loi_Etat
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_Multi_GP_base.h>
#include <Fluide_Dilatable_base.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>
#include <Champ_Inc_base.h>
#include <Zone_VF.h>
#include <Debog.h>

Implemente_liste(REF(Champ_Inc_base));
Implemente_base(Loi_Etat_Multi_GP_base,"Loi_Etat_Multi_Gaz_Parfait_base",Loi_Etat_Melange_GP_base);
// XD melange_gaz_parfait loi_etat_base melange_gaz_parfait -1 Mixing of perfect gas.

Sortie& Loi_Etat_Multi_GP_base::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_Multi_GP_base::readOn(Entree& is)
{
  return is;
}


void Loi_Etat_Multi_GP_base::initialiser_inco_ch()
{
  const int num = liste_Y(0).valeur().valeurs().size();
  masse_mol_mel.resize(num,1);
  calculer_masse_molaire();
  Loi_Etat_base::initialiser_inco_ch();
}

void Loi_Etat_Multi_GP_base::calculer_masse_molaire()
{
  calculer_masse_molaire(masse_mol_mel);
  Debog::verifier("Masse_mol_mel",masse_mol_mel);
}

// Description:
// Associe l inconnue de chaque equation de
// fraction massique a la loi d'etat
// Precondition:
// Parametre: inconnue
//    Signification: l inconnue associee
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_base::associer_inconnue(const Champ_Inc_base& inconnue)
{
  REF(Champ_Inc_base) inco;
  inco = inconnue;
  liste_Y.add(inco);
}

void Loi_Etat_Multi_GP_base::calculer_Cp()
{
  Champ_Don& Cp = le_fluide->capacite_calorifique();
  DoubleTab& tab_Cp = Cp.valeurs();
  calculer_tab_Cp(tab_Cp);
  tab_Cp.echange_espace_virtuel();
  Debog::verifier("tab_Cp",tab_Cp);
}

// Description:
//    Calcule la conductivite
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
void Loi_Etat_Multi_GP_base::calculer_lambda()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda.valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  int i, n = tab_lambda.size();

  //La conductivite est soit un champ uniforme soit calculee a partir de la viscosite dynamique et du Pr
  if (sub_type(Champ_Fonc_Tabule,lambda.valeur()))
    {
      lambda.valeur().mettre_a_jour(temperature_.valeur().temps());
      return;
    }
  if (!sub_type(Champ_Uniforme,lambda.valeur()))
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++)  tab_lambda(i,0) = mu0 * Cp_ / Pr_;
        }
      else
        for (i=0 ; i<n ; i++) tab_lambda(i,0) = tab_mu(i,0) * tab_Cp(i,0) / Pr_;
    }
  tab_lambda.echange_espace_virtuel();
  Debog::verifier("tab_lambda",tab_lambda);
}

// Description:
//    Calcule la diffusivite
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
void Loi_Etat_Multi_GP_base::calculer_alpha()
{
  const Champ_Don& lambda = le_fluide->conductivite();
  const DoubleTab& tab_lambda = lambda.valeurs();
  Champ_Don& alpha=le_fluide->diffusivite();
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  int i, n = tab_alpha.size(), isVDF = 0;
  if (alpha.valeur().que_suis_je()=="Champ_Fonc_P0_VDF") isVDF = 1;

  if (isVDF)
    {
      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++)  tab_alpha(i,0) = lambda0 / ( tab_rho(i,0) * tab_Cp(i,0) );
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_alpha(i,0) = tab_lambda(i,0) / ( tab_rho(i,0) * tab_Cp(i,0) );
        }
    }
  else // VEF
    {
      const IntTab& elem_faces=ref_cast(Zone_VF,le_fluide->vitesse().zone_dis_base()).elem_faces();
      double rhoelem, Cpelem;
      int face, nfe = elem_faces.line_size();

      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++)
            {
              rhoelem = 0;
              Cpelem = 0;
              for (face=0; face<nfe; face++)
                {
                  rhoelem += tab_rho(elem_faces(i,face),0);
                  Cpelem += tab_Cp(elem_faces(i,face),0);
                }
              rhoelem /= nfe;
              Cpelem /= nfe;
              tab_alpha(i,0) = lambda0 / ( rhoelem * Cpelem );
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              rhoelem = 0;
              Cpelem = 0;
              for (face=0; face<nfe; face++)
                {
                  rhoelem += tab_rho(elem_faces(i,face),0);
                  Cpelem += tab_Cp(elem_faces(i,face),0);
                }
              rhoelem /= nfe;
              Cpelem /= nfe;
              tab_alpha(i,0) = tab_lambda(i,0) / ( rhoelem * Cpelem );
            }
        }
    }
  tab_alpha.echange_espace_virtuel();
  Debog::verifier("alpha",tab_alpha);
}

double Loi_Etat_Multi_GP_base::calculer_masse_volumique(double P, double T, double r) const
{
  if (inf_ou_egal(T,0))
    {
      Cerr << finl << "Warning: Temperature T must be defined in Kelvin." << finl;
      Cerr << "Check your data file." << finl;
      Process::exit();
    }
  return P / ( r * T );
}

double Loi_Etat_Multi_GP_base::calculer_masse_volumique(double P, double T) const
{
  Cerr << "You should not call the method Loi_Etat_Multi_GP_base::calculer_masse_volumique(double P, double T) !" << finl;
  Process::exit();
  return -1000.;
}

// Description:
//    Calcule la viscosite dynamique
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
void Loi_Etat_Multi_GP_base::calculer_mu()
{
  Champ_Don& mu = le_fluide->viscosite_dynamique();
  if (!sub_type(Champ_Uniforme,mu.valeur()))
    {
      if (sub_type(Champ_Fonc_Tabule,mu.valeur())) mu.mettre_a_jour(temperature_.valeur().temps());
      else calculer_mu_wilke();
    }
}

void Loi_Etat_Multi_GP_base::calculer_tab_mu(const DoubleTab& mu, int size)
{
  // Dynamic viscosity is on elem
  DoubleTab& tab_mu = le_fluide->viscosite_dynamique().valeurs();
  const int nb_elem = tab_mu.size();
  if (nb_elem==size) // VDF
    {
      for (int elem=0; elem < nb_elem; elem++) tab_mu(elem,0) = mu(elem);
    }
  else // VEF (average on elem from values on face);
    {
      const IntTab& elem_faces=ref_cast(Zone_VF,le_fluide->vitesse().zone_dis_base()).elem_faces();
      const int nfe = elem_faces.line_size();
      tab_mu = 0;
      for (int elem=0; elem<nb_elem; elem++)
        {
          for (int face = 0; face < nfe; face++) tab_mu(elem,0) += mu(elem_faces(elem, face));

          tab_mu(elem,0) /= nfe;
        }
    }
  tab_mu.echange_espace_virtuel();
  Debog::verifier("tab_mu", tab_mu);
}
