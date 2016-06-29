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
// File:        Transport_K_KEps.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Transport_K_KEps.h>
#include <Modele_turbulence_hyd_K_Eps_2_Couches.h>
#include <Les_Pb_Turb.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Transport_K_KEps,"Transport_K_KEps",Transport_K_Eps_non_std);

Transport_K_KEps::Transport_K_KEps() : nb_couches(10), ystar_switch(160),  type_switch(0), nut_switch(30), impr(0) { }


// Description:
//    Imprime le type de l'equation sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Transport_K_KEps::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}


// Description:
//    Lit les specifications d'une equation de transport K-epsilon
//    a partir d'un flot d'entree.
//    Controle dynamique du type du terme source.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Transport_K_KEps::readOn(Entree& s )
{
  // Lecture des attributs de l'equation
  loi_2couches.typer("2couches_conv_nat");
  Transport_K_Eps_non_std::readOn(s);

  // Ajout automatique du terme source si pas instancie dans le jeu ed donnees
  if (les_sources.est_vide())
    {
      Source t;
      Source& so=les_sources.add(t);
      const Probleme_base& pb = probleme();
      Cerr << "Construction and typing for the source term of the Transport_K_KEps equation." << finl;
      if (sub_type(Pb_Hydraulique_Turbulent,pb))
        {
          Nom typ = "Source_Transport_K_KEps";
          so.typer(typ,*this);
        }
      else if (sub_type(Pb_Thermohydraulique_Turbulent,pb))
        {
          Nom typ = "Source_Transport_K_KEps_anisotherme";
          so.typer(typ,*this);
        }
      /*      else if (sub_type(Pb_Hydraulique_Concentration_Turbulent,pb))
              {
              Nom typ = "Source_Transport_K_Eps_aniso_concen";
              so.typer(typ,*this);
              }
              else if (sub_type(Pb_Thermohydraulique_Concentration_Turbulent,pb))
              {
              Nom typ = "Source_Transport_K_Eps_aniso_therm_concen";
              so.typer(typ,*this);
              }*/
      so->associer_eqn(*this);
    }
  return s;
}

void Transport_K_KEps::set_param(Param& param)
{
  Transport_K_Eps_non_std::set_param(param);
  param.ajouter("nb_couches",&nb_couches);
  param.ajouter("y*_switch",&ystar_switch);
  param.ajouter_non_std("conv_forcee",(this));
  param.ajouter_non_std("conv_nat",(this));
  param.ajouter_non_std("nut_switch",(this));
  param.ajouter_flag("impr",&impr);
  param.ajouter("loi_2couches",(this));
}

int Transport_K_KEps::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="conv_forcee")
    {
      loi_2couches.typer("2couches_conv_forcee");
      return 1;
    }
  else if (mot=="conv_nat")
    {
      loi_2couches.typer("2couches_conv_nat");
      return 1;
    }
  else if (mot=="nut_switch")
    {
      type_switch =1;
      is >> nut_switch;
      return 1;
    }
  else if (mot=="loi_2couches")
    {
      Motcle motbis, accolade_fermee="}", accolade_ouverte="{";
      is >> motbis;
      if(motbis!=accolade_ouverte)
        {
          Cerr << "A { was expected while reading loi_2couches" << finl;
          Cerr << "instead of : " << motbis << finl;
          exit();
        }
      is >> motbis;
      loi_2couches.typer(motbis);
      is >> motbis;
      if(motbis!=accolade_fermee)
        {
          Cerr << "A } was expected when ending to read loi_2couches" << finl;
          Cerr << "instead of : " << motbis << finl;
          exit();
        }
      return 1;
    }
  else
    return Transport_K_Eps_non_std::lire_motcle_non_standard(mot,is);
  return 1;
}

// Description:
//    Associe un modele de turbulence K-epsilon deux couches a l'equation.
// Precondition:
// Parametre: Modele_turbulence_hyd_K_Eps_2_Couches& modele
//    Signification: le modele de turbulence K-epsilon deux couches a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un modele de turbulence associe
void Transport_K_KEps::associer_modele_turbulence(const Mod_turb_hyd_RANS& modele)
{
  const Equation_base& eqn_hydr = modele.equation();
  associer(eqn_hydr);
  associer_milieu_base(eqn_hydr.milieu());
  associer_vitesse(eqn_hydr.inconnue());
  mon_modele = ref_cast(Modele_turbulence_hyd_K_Eps_2_Couches,modele);
  discretiser();
}


// Description:
//    Associe un milieu physique a l'equation.
//    Le Milieu_base passe en parametre est caste en Fluide_Incompressible.
// Precondition: le milieu doit etre de (forcable au) type Fluide_Incompressible
// Parametre: Milieu_base& un_milieu
//    Signification: le milieu physique a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit etre forcable au type Fluide_Incompressible
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Transport_K_KEps::associer_milieu_base(const Milieu_base& un_milieu)
{
  le_fluide = ref_cast(Fluide_Incompressible, un_milieu);
}


// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Transport_Keps".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Transport_Keps"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Transport_K_KEps::domaine_application() const
{
  static Motcle domaine = "Transport_Keps";
  return domaine;
}

