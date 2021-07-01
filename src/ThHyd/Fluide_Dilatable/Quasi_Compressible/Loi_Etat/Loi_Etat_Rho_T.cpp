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
// File:        Loi_Etat_Rho_T.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Loi_Etat
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_Rho_T.h>
#include <Param.h>
#include <Probleme_base.h>
#include <Interprete.h>
#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Zone_dis_base.h>
#include <Zone_VF.h>
#include <Table.h>

Implemente_instanciable_sans_constructeur( Loi_Etat_Rho_T, "Loi_Etat_Rho_T", Loi_Etat_GP ) ;
// XD rho_T loi_etat_base rho_T -1 Defining a state equation of form rho = f(T).

Loi_Etat_Rho_T::Loi_Etat_Rho_T() : is_exp_(false)
{}

// Description:
//    Imprime la loi sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Loi_Etat_Rho_T::printOn( Sortie& os ) const
{
  Loi_Etat_GP::printOn( os );
  return os;
}

// Description:
//    Lecture d'une loi sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition: l'objet est construit avec les parametres lus
Entree& Loi_Etat_Rho_T::readOn( Entree& is )
{
  Nom expression_;

  Param param(que_suis_je());
  param.ajouter("Cp",&Cp_,Param::REQUIRED);// XD_ADD_P double Specific heat at constant pressure of the gas Cp.
  param.ajouter("Prandtl",&Pr_); // XD_ADD_P double Prandtl number of the gas Pr=mu*Cp/lambda
  param.ajouter("rho_xyz",&rho_xyz_); // XD_ADD_P field_base Defined with a Champ_Fonc_xyz to define a constant rho with time (space dependent)
  param.ajouter("rho_t",&expression_); // XD_ADD_P chaine Expression of T used to calculate rho. This can lead to a variable rho, both in space and in time.
  param.ajouter("rho_constant_pour_debug",&rho_constant_pour_debug_); // XD_ADD_P flag To debug use this flag.
  param.lire_avec_accolades(is);

  if (expression_ != "??")
    {
      is_exp_ = true;
      Cerr << "Parsing the expression " << expression_ << finl;
      parser_.setNbVar(1);
      parser_.setString(expression_);
      parser_.addVar("T");
      parser_.parseString();
    }
  else
    {
      if (rho_xyz_.valeur().que_suis_je() != "Champ_Fonc_xyz" )
        {
          Cerr << "Error in Loi_Etat_Rho_T::readOn !" << finl;
          Cerr << "An expression of rho_t is not read in your data file !" << finl;
          Cerr << "Either use rho_t_expression followed by an expression of T," << finl;
          Cerr << "or use rho_t followed by a Champ_Fonc_xyz !" << finl;
          Process::exit();
        }
    }

  return is;
}

// Method used to initialize rho from a Champ_Fonc_xyz
// In this case the DoubleTab rho_ is initialized and remains constant with time
void Loi_Etat_Rho_T::initialiser_rho()
{
  assert(rho_xyz_.valeur().que_suis_je() == "Champ_Fonc_xyz" );

  // VDF/polyMAC or VEF ??
  int isVDF=0;
  if (le_fluide->masse_volumique().que_suis_je()=="Champ_Fonc_P0_VDF")
    isVDF=1;

  // We know that mu is always stored on elems
  int nb_elems = le_fluide->viscosite_dynamique().valeurs().size();
  // The Champ_Don rho_xyz_ is evaluated on elements
  assert (rho_xyz_.valeur().valeurs().size() == nb_elems);

  if (isVDF)
    {
      // Disc VDF/polyMAC => rho_ & rho_xyz_ on elems => we do nothing
      rho_.resize(nb_elems, 1);
      DoubleTab& fld = rho_xyz_.valeur().valeurs();

      for (int i=0; i<nb_elems; i++)
        rho_(i)=fld(i,0);
    }
  else
    {
      // Disc VEF => rho on faces ...
      int nb_faces =  le_fluide->masse_volumique().valeurs().size(); // rho on faces in VEF
      rho_.resize(nb_faces, 1);

      Champ_base& ch_rho = le_fluide->masse_volumique();
      ch_rho.affecter_(rho_xyz_);
      DoubleTab& fld = ch_rho.valeurs();

      for (int i = 0; i < nb_faces; i++)
        rho_(i)= fld[i];
    }
}

// Overloaded method from Loi_Etat_base

// Description:
//    Initialise l'inconnue de l'equation de chaleur
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
void Loi_Etat_Rho_T::initialiser_inco_ch()
{
  // required here for xyz !
  if ( !is_exp_ )
    initialiser_rho();

  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  tab_rho_n=tab_rho;
  tab_rho_np1=tab_rho;
  calculer_masse_volumique();
  mettre_a_jour(0.);
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
}

// Override

// Description:
//    Recalcule la masse volumique
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
double Loi_Etat_Rho_T::calculer_masse_volumique(double P, double T) const
{
  /* Not useful for this state law */
  return -3000.;
}

// Overload

// Description:
//    Recalcule la masse volumique
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
double Loi_Etat_Rho_T::calculer_masse_volumique(double P, double T, int ind) const
{
  if (rho_constant_pour_debug_.non_nul())
    {
      return rho_constant_pour_debug_(0,0);
    }

  if (inf_ou_egal(T,-1000))
    {

      Cerr << finl << "Error, we find a temperature of " << T << " !" << finl;
      Cerr << "Either your calculation has diverged or you don't define" << finl;
      Cerr << "temperature in Kelvin somewhere in your data file." << finl;
      Cerr << "It is mandatory for Quasi compressible model." << finl;
      Cerr << "Check your data file." << finl;
      Process::exit();
    }

  if (is_exp_)
    {
      parser_.setVar(0,T);
      return parser_.eval();
    }
  else
    {
      // Initialized from Champ_Fonc_xyz
      // dont change, return what is saved in rho_
      return rho_(ind);
    }
}

double Loi_Etat_Rho_T::inverser_Pth(double T, double rho)
{
  abort();
  throw;
}

// Description:
//    Recalcule la masse volumique
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
void Loi_Etat_Rho_T::calculer_masse_volumique()
{
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  double Pth = le_fluide->pression_th();
  int n=tab_rho.size();
  for (int som=0 ; som<n ; som++)
    {
      tab_rho_np1[som] =  calculer_masse_volumique(Pth,tab_ICh[som],som);
      tab_rho[som] = (tab_rho_n[som] + tab_rho_np1[som])/2.;
    }
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
}
