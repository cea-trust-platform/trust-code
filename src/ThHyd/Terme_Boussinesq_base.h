/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Terme_Boussinesq_base.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Boussinesq_base_included
#define Terme_Boussinesq_base_included

#include <Ref_Champ_Don_base.h>
#include <Ref_Champ_Don.h>
#include <Champ_Don.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Parser_U.h>
#include <Ref_Convection_Diffusion_std.h>
#include <Vect_Parser_U.h>
#include <Convection_Diffusion_Temperature.h>
#include <Domaine.h>

class Convection_Diffusion_std;
//////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION
//    Classe Terme_Boussinesq_base
//    Cette classe represente le terme de gravite qui figure dans l'equation
//    de la dynamique divisee par la masse volumique de reference.
//    On est dans le cadre de l'hypothese de Boussinesq:la masse volumique est
//    supposee constante et egale a sa valeur de reference sauf dans le terme
//    des forces de volumes ou on prend en compte une petite variation de la
//    masse volumique en fonction d'un ou de plusieurs scalaires transportes
//    par l'ecoulement (la temperature et/ou une ou plusieurs concentrations).
//    Cas particulier de Terme_Boussinesq_base pour la temperature
//    Le terme de gravite a pour expression: beta*(T-T0) ou beta
//    represente la dilatabilite de la masse volumique et T0 une valeur
//    de reference pour la temperature
//    Cas particulier de Terme_Boussinesq_base pour une concentration
//    Le terme de gravite a pour expression : beta*(C-C0)
//    ou (beta[0]*(C[0]-C0[0]) + .....+ beta[i]*(C[i]-C0[i])) dans
//    le cas d'un vecteur de concentrations
//    beta represente la variation de la masse volumique en fonction
//    de la concentration du constituant et C0 une valeur de reference
//    pour la concentration
//////////////////////////////////////////////////////////////////////////////
class Terme_Boussinesq_base : public Source_base
{
  Declare_base(Terme_Boussinesq_base);

public :

  void associer_pb(const Probleme_base& pb) ;
  inline const Champ_Don_base& gravite() const
  {
    return la_gravite_.valeur();
  };
  inline int verification() const
  {
    return verif_;
  };
  inline double Scalaire0(int i) const
  {
    return Scalaire0_(i);
  };
  inline const Champ_Don& beta() const
  {
    return beta_.valeur();
  } ;
  inline const Convection_Diffusion_std& equation_scalaire() const
  {
    return equation_scalaire_.valeur();
  };
  DoubleTab& calculer(DoubleTab& resu) const
  {
    resu=0;
    return ajouter(resu);
  };
  void mettre_a_jour(double temps)
  {
    for (int i=0; i<Scalaire0_.size_array(); i++)
      {
        fct_Scalaire0_[i].setVar(0,temps);
        Scalaire0_[i] = fct_Scalaire0_[i].eval();
      }
  };

protected :

  REF(Champ_Don_base) la_gravite_;
  int verif_;
  ArrOfDouble Scalaire0_; // T0=Scalaire0_(0) ou C0(i)=Scalaire0_(i)
  Nom NomScalaire_; // Temperature ou Concentration
  VECT(Parser_U) fct_Scalaire0_;
  REF(Champ_Don) beta_;
  REF(Convection_Diffusion_std) equation_scalaire_;
  Entree& lire_donnees(Entree& );
  inline void check() const;
};

inline void Terme_Boussinesq_base::check() const
{
  // Pas de verification autre qu'au premier pas de temps, si verification pas desactivee et uniquement pour la temperature
  if (equation_scalaire().probleme().schema_temps().nb_pas_dt()>0 || equation_scalaire().probleme().reprise_effectuee() || verif_==0 || !sub_type(Convection_Diffusion_Temperature,equation_scalaire())) return;

  // Nouveau : on verifie que moyenne(T)==T0 au demarrage du calcul uniquement
  const double& T0 = Scalaire0(0);
  double moyenne_T = mp_moyenne_vect(equation_scalaire().inconnue().valeurs());
  if (inf_ou_egal(moyenne_T,T0-10) || sup_ou_egal(moyenne_T,T0+10))
    {
      Cerr << "New criteria in TRUST for the Boussinesq source term definition:" << finl;
      Cerr << "To avoid an incorrect choice for T0 value" << finl;
      Cerr << "the initial average temperature on the domain " << equation_scalaire().probleme().domaine().le_nom() << finl;
      Cerr << "should be between +/-10 degrees around T0 value." << finl;
      Cerr << "The initial average temperature is : " << moyenne_T << finl;
      Cerr << "T0 value is : " << T0 << finl;
      Cerr << "So, you need to change T0 or the initial temperature field to respect this criteria defined by default." << finl;
      Cerr << "If you want to overcome this criteria, which it is not recommended," << finl;
      Cerr << "you can specify, into the Boussinesq source term definition, the option:" << finl;
      Cerr << "verif_boussinesq 0" << finl;
      Process::exit();
    }
}

// Methode de calcul de la valeur sur un champ aux elements d'un champ uniforme ou non a plusieurs composantes
inline double valeur(const DoubleTab& valeurs, const int& elem, const int& dim)
{
  if(valeurs.nb_dim()==1)
    return valeurs(elem);
  else
    return valeurs(elem,dim);
}

// Methode de calcul de la valeur sur une face encadree par elem1 et elem2 d'un champ uniforme ou non a plusieurs composantes
inline double valeur(const DoubleTab& valeurs_champ, int elem1, int elem2, const int& compo)
{
  if (valeurs_champ.dimension(0)==1)
    return valeurs_champ(0,compo); // Champ uniforme
  else
    {
      if (elem2<0) elem2 = elem1; // face frontiere
      if (valeurs_champ.nb_dim()==1)
        return 0.5*(valeurs_champ(elem1)+valeurs_champ(elem2));
      else
        return 0.5*(valeurs_champ(elem1,compo)+valeurs_champ(elem2,compo));
    }
}

#endif
