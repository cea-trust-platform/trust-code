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
// File:        Convection_Diffusion_Concentration.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/37
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Concentration.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Constituant.h>
#include <Discret_Thyd.h>
#include <Frontiere_dis_base.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Convection_Diffusion_Concentration,"Convection_Diffusion_Concentration",Convection_Diffusion_std);

Convection_Diffusion_Concentration::Convection_Diffusion_Concentration():masse_molaire_(-1.)
{
  //On dimensionne liste_noms_compris et on remplit dans readOn()
}
// Description:
//    Simple appel a: Convection_Diffusion_std::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& is
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
Sortie& Convection_Diffusion_Concentration::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

// Description:
//    Verifie si l'equation a une concentration et un constituant associe
//    et appelle Convection_Diffusion_std::readOn(Entree&).
// Precondition: l'objet a une concentration associee
// Precondition: l'objet a un constituant associe
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Convection_Diffusion_Concentration::readOn(Entree& is)
{
  assert(la_concentration.non_nul());
  Convection_Diffusion_std::readOn(is);
  Nom nom="Convection_";
  nom+=inconnue().le_nom(); // On ajoute le nom de l'inconnue pour prevoir une equation de scalaires passifs
  terme_convectif.set_fichier(nom);
  terme_convectif.set_description((Nom)"Convective mass transfer rate=Integral(-C*u*ndS)[m"+(Nom)(dimension+bidim_axi)+".Mol.s-1]");
  nom="Diffusion_";
  nom+=inconnue().le_nom();
  terme_diffusif.set_fichier(nom);
  terme_diffusif.set_description((Nom)"Diffusion mass transfer rate=Integral(alpha*grad(C)*ndS) [m"+(Nom)(dimension+bidim_axi)+".Mol.s-1]");
  return is;
}

// Description :
// renvoit la masse_molaire, celle ci doit avoir ete lue avant
const double& Convection_Diffusion_Concentration::masse_molaire() const
{
  assert(masse_molaire_>0);
  return masse_molaire_;
}

void Convection_Diffusion_Concentration::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
  param.ajouter_non_std("nom_inconnue",(this));
  param.ajouter_non_std("alias",(this));
  param.ajouter("masse_molaire",&masse_molaire_);
}

int Convection_Diffusion_Concentration::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="nom_inconnue")
    {
      Motcle nom; // Question: veut-on le mettre en majuscules ?
      is >> nom;
      Cerr << "The unknow of a Convection_Diffusion_Concentration equation is renamed"
           << "\n Old name : " << inconnue().valeur().le_nom()
           << "\n New name : " << nom << finl;
      inconnue().valeur().nommer(nom);
      return 1;
    }
  else if (mot=="alias")
    {
      Motcle nom; // Question: veut-on le mettre en majuscules ?
      is >> nom;
      Cerr << "nom_inconnue: On renomme l'equation et son inconnue"
           << "\n Ancien nom : " << inconnue().valeur().le_nom()
           << "\n Nouveau nom : " << nom << finl;
      inconnue().valeur().nommer(nom);
      return 1;
    }
  else
    return Convection_Diffusion_std::lire_motcle_non_standard(mot,is);
  return 1;
}

// Description:
//    Associe un milieu physique a l'equation,
//    le milieu est en fait caste en Constituant et associe a l'equation.
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit pourvoir etre force au type "Constituant"
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: diffusivite du constituant dans le fluide non definie
// Effets de bord:
// Postcondition:
void Convection_Diffusion_Concentration::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Constituant& un_constituant = ref_cast(Constituant,un_milieu);
  if (un_constituant.diffusivite_constituant().non_nul())
    associer_constituant(un_constituant);
  else
    {
      Cerr << "The dye (constituant) diffusivity has not been defined." << finl ;
      exit();
    }
}

const Champ_Don& Convection_Diffusion_Concentration::diffusivite_pour_transport()
{
  return constituant().diffusivite_constituant();
}

// Description:
//    Discretise l'equation.
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
// Postcondition: l'equation est discretisee
void Convection_Diffusion_Concentration::discretiser()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Transport concentration(s) equation discretization " << finl;
  nb_constituants_ = constituant().nb_constituants();
  dis.concentration(schema_temps(), zone_dis(), la_concentration, nb_constituants_);
  champs_compris_.ajoute_champ(la_concentration);
  Equation_base::discretiser();
}


// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Constituant upcaste en Milieu_base)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Constituant upcaste en Milieu_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Convection_Diffusion_Concentration::milieu() const
{
  return constituant();
}


// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Constituant upcaste en Milieu_base)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Constituant upcaste en Milieu_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& Convection_Diffusion_Concentration::milieu()
{
  return constituant();
}


// Description:
//    Renvoie le constituant (si il a ete associe).
//    (version const)
// Precondition: un constituant doit avoir ete associe a l'equation.
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Constituant&
//    Signification: le constituant associe a l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Constituant& Convection_Diffusion_Concentration::constituant() const
{
  if(!le_constituant.non_nul())
    {
      Cerr << "You forgot to associate the constituent to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_constituant.valeur();
}


// Description:
//    Renvoie le constituant (si il a ete associe).
// Precondition: un constituant doit avoir ete associe a l'equation.
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Constituant&
//    Signification: le constituant associe a l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Constituant& Convection_Diffusion_Concentration::constituant()
{
  if(!le_constituant.non_nul())
    {
      Cerr << "No constituant has been associated "
           << "with a Convection_Diffusion_Concentration equation." << finl;
      exit();
    }
  return le_constituant.valeur();
}

int Convection_Diffusion_Concentration::preparer_calcul()
{
  Equation_base::preparer_calcul();
  double temps=schema_temps().temps_courant();
  constituant().initialiser(temps);
  return 1;
}

void Convection_Diffusion_Concentration::mettre_a_jour(double temps)
{
  Equation_base::mettre_a_jour(temps);
  constituant().mettre_a_jour(temps);
}

// Description:
//    Impression des flux sur les bords sur un flot de sortie.
//    Appelle Equation_base::impr(Sortie&)
// Precondition: Sortie&
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Convection_Diffusion_Concentration::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

// Description:
//    Renvoie 1 si le mot clef specifie designe un type de
//    champ inconnue de l'equation.
//    Renvoie 1 si mot = "concentration"
//    Renvoie 0 sinon
//    Si la methode renvoie 1 ch_ref fait reference au champ, dont
//    le type a ete specifie.
// Precondition:
// Parametre: Motcle& mot
//    Signification: le type du champ dont on veut recuperer la reference
//    Valeurs par defaut:
//    Contraintes: reference constante, valeur parmi "concentration"
//    Acces: entree
// Parametre: REF(Champ_base)& ch_ref
//    Signification: la reference sur le champ du type specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: int
//    Signification: renvoie 1 si le champ a ete trouve, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int string2int(char* digit, int& result)
{
  result = 0;

  //--- Convert each digit char and add into result.
  while (*digit >= '0' && *digit <='9')
    {
      result = (result * 10) + (*digit - '0');
      digit++;
    }

  //--- Check that there were no non-digits at end.
  if (*digit != 0)
    {
      return 0;
    }

  return 1;
}



// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Concentration".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Concentration"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Convection_Diffusion_Concentration::domaine_application() const
{
  static Motcle domaine = "Concentration";
  return domaine;
}

