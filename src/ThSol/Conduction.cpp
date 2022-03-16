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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Conduction.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/35
//
//////////////////////////////////////////////////////////////////////////////

#include <Conduction.h>
#include <Solide.h>
#include <Discret_Thermique.h>
#include <Frontiere_dis_base.h>
#include <Probleme_base.h>
#include <Param.h>
#include <Champ_Uniforme.h>

Implemente_instanciable_sans_constructeur(Conduction,"Conduction",Equation_base);
// XD Conduction eqn_base Conduction -1 Heat equation.
Conduction::Conduction()
{
  // champs_compris_.ajoute_nom_compris("temperature_paroi");
  champs_compris_.ajoute_nom_compris("temperature_residu");
}
// Description:
//    Ecrit le type de l'equation sur un flot de
//    sortie.
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
Sortie& Conduction::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    cf Equation_base::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: terme diffusif non specifie
// Effets de bord:
// Postcondition:
Entree& Conduction::readOn(Entree& is )
{
  Equation_base::readOn(is);

  //Nom unite;
  //if (dimension+bidim_axi==2) unite="[W/m]";
  //else unite="[W]";
  terme_diffusif.set_fichier("Diffusion_chaleur");
  //terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) "+unite);
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");
  return is;
}

void Conduction::set_param(Param& param)
{
  Equation_base::set_param(param);
  param.ajouter_non_std("diffusion",(this));
  param.ajouter_condition("is_read_diffusion","The diffusion operator must be read, select negligeable type if you want to neglect it.");
  param.ajouter_non_std("Traitement_particulier",(this));
}

//  Modification par rapport a Conduction::lire_motcle_non_standard()!
int Conduction::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;

      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      is >> terme_diffusif;
      // le champ pour le dt_stab est le meme que celui de l'operateur. On prend toujours la diffusivite()
      // comme dans le cas de Conduction
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(milieu().diffusivite());
      solveur_masse->set_name_of_coefficient_temporel("rho_cp_comme_T");

      return 1;
    }
  else if (mot=="Traitement_particulier")
    {
      Cerr << "Reading and typing of the Traitement_particulier object : " << finl;
      Nom type="Traitement_particulier_";
      Motcle motbidon;
      is >> motbidon;
      if (motbidon == "{")
        {
          Motcle le_cas;
          is >> le_cas;
          if (le_cas == "}")
            le_cas ="";
          else
            {
              type+= le_cas;
              type+= "_";
            }
          Nom discr=discretisation().que_suis_je();
          if (discr == "VEFPreP1B")
            discr = "VEF";
          type+=discr;
          Cerr << type << finl;
          le_traitement_particulier.typer(type);
          le_traitement_particulier.associer_eqn(*this);
          le_traitement_particulier.lire(is);
          le_traitement_particulier.preparer_calcul_particulier();
          return 1;
        }
      else
        {
          Cerr << "Error while reading Traitement_particulier" << finl;
          Cerr << "A { is expected." << finl;
          exit();
          return -1;
        }
    }
  else
    return Equation_base::lire_motcle_non_standard(mot,is);
  return 1;
}

// retourne la *conductivite* et non la diffusivite comme dans Conduction
const Champ_Don& Conduction::diffusivite_pour_transport() const
{
  return milieu().conductivite();
}

const Champ_base& Conduction::diffusivite_pour_pas_de_temps() const
{
  return terme_diffusif.diffusivite();
}

// Description:
//    Associe un milieu physique a l'equation,
//    le milieu est en fait caste en Solide.
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit pourvoir etre force au type "Solide"
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Conduction::associer_milieu_base(const Milieu_base& le_milieu)
{
  associer_solide(ref_cast(Solide,le_milieu));
}

// Description:
//    Associe le milieu solide a l'equation.
// Precondition:
// Parametre: Solide& un_solide
//    Signification: le milieu solide a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un milieu physique associe
void Conduction::associer_solide(const Solide& un_solide)
{
  le_solide = un_solide;
}

// Description:
//    Renvoie le nombre d'operateurs de l'equation
//    pour l'equation de conduction standart renvoie toujours 1.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'operateurs de l'equation
//    Contraintes: toujours egal a 1
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Conduction::nombre_d_operateurs() const
{
  return 1;
}

// Description:
//    Renvoie l'operateur d'index specifie de l'equation.
//    Renvoie terme_diffusif si i=0
//     exit si i>0
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: index de l'operateur a renvoyer
//    Valeurs par defaut:
//    Contraintes: i = 0
//    Acces:
// Retour: Operateur&
//    Signification: l'operateur d'index specifie
//                  (uniquement terme_diffusif)
//    Contraintes: reference constante
// Exception: l'equation de conduction standard ne contient
//            qu'un operateur
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Operateur& Conduction::operateur(int i) const
{
  if (i == 0)
    return terme_diffusif;
  else
    {
      Cerr << "Conduction::operateur("<<i<<") !! " << finl;
      Cerr << "Equation Conduction has only one operator." << finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

// Description:
//    Renvoie l'operateur d'index specifie de l'equation.
//    Renvoie terme_diffusif si i=0
//     exit si i>0
// Precondition:
// Parametre: int i
//    Signification: index de l'operateur a renvoyer
//    Valeurs par defaut:
//    Contraintes: i = 0
//    Acces:
// Retour: Operateur&
//    Signification: l'operateur d'index specifie
//                  (uniquement terme_diffusif)
//    Contraintes:
// Exception: l'equation de conduction standard ne contient
//            qu'un operateur
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Operateur& Conduction::operateur(int i)
{
  if (i == 0)
    return terme_diffusif;
  else
    {
      Cerr << "Conduction::operateur("<<i<<") !! " << finl;
      Cerr << "Equation Conduction has only one operator." << finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

// Description:
//    Discretise l'equation
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
void Conduction::discretiser()
{
  const Discret_Thermique& dis=ref_cast(Discret_Thermique, discretisation());
  Cerr << "Conduction equation discretization" << finl;
  dis.temperature(schema_temps(), zone_dis(), la_temperature);
  champs_compris_.ajoute_champ(la_temperature);
  Equation_base::discretiser();
}


// Description:
//    Renvoie le milieu physique associe a l'equation.
//    Ici Solide upcaste en Milieu_base.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le milieu physique associe a l'equation
//                   (Solide upcaste en Milieu_base)
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Conduction::milieu() const
{
  return solide();
}

// Description:
//    Renvoie le milieu physique associe a l'equation.
//    Ici Solide upcaste en Milieu_base.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le milieu physique associe a l'equation
//                   (Solide upcaste en Milieu_base)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Milieu_base& Conduction::milieu()
{
  return solide();
}

// Description:
//    Renvoie le milieu solide associe a l'equation.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Solide&
//    Signification: le milieu solide associe a l'equation
//    Contraintes: reference constante
// Exception: pas de milieu solide associe a l'equation
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Solide& Conduction::solide() const
{
  if(!le_solide.non_nul())
    {
      Cerr << "You forgot to associate the solid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_solide.valeur();
}

void Conduction::creer_champ(const Motcle& motlu)
{
  Equation_base::creer_champ(motlu);
  Motcle nom_mot(motlu),temp_mot(nom_mot);
  /*  if (motlu == "temperature_paroi")
      {
        if (!temperature_paroi.non_nul())
          {
            const Discret_Thermique& dis=ref_cast(Discret_Thermique, discretisation());
            dis.t_paroi(zone_dis(),zone_Cl_dis(),*this,temperature_paroi);
            champs_compris_.ajoute_champ(temperature_paroi);
          }
      } */
}

const Champ_base& Conduction::get_champ(const Motcle& nom) const
{
  /*
    if (nom=="rho_cp")
      {
        const Champ_base& rho_cp=solide().get_rho_cp();
        return rho_cp;
      }
  */
  try
    {
      return Equation_base::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {

    }

  if (le_traitement_particulier.non_nul())
    try
      {
        return le_traitement_particulier->get_champ(nom);
      }
    catch (Champs_compris_erreur)
      {

      }
  throw Champs_compris_erreur();
  REF(Champ_base) ref_champ;
  return ref_champ;
}

void Conduction::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Equation_base::get_noms_champs_postraitables(nom,opt);
  if (opt==DESCRIPTION)
    Cerr<<"Conduction : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());

  if (le_traitement_particulier.non_nul())
    le_traitement_particulier->get_noms_champs_postraitables(nom,opt);
}

// Description:
//    Renvoie le milieu solide associe a l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Solide&
//    Signification: le milieu solide associe a l'equation
//    Contraintes:
// Exception: pas de milieu solide associe a l'equation
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Solide& Conduction::solide()
{
  if(!le_solide.non_nul())
    {
      Cerr << "A solide medium has not been associated to a Conduction equation"<<finl;
      exit();
    }
  return le_solide.valeur();
}

// Description:
//    Imprime le terme diffusif sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Conduction::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}


// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Thermique".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Thermique"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Conduction::domaine_application() const
{
  static Motcle domaine = "Thermique";
  return domaine;
}

void Conduction::mettre_a_jour(double temps)
{
  Equation_base::mettre_a_jour(temps);

  if (le_traitement_particulier.non_nul())
    le_traitement_particulier.post_traitement_particulier();
}


