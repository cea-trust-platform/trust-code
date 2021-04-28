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
// File:        Echange_impose_base.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_impose_base.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>


Implemente_base_sans_constructeur(Echange_impose_base,"Echange_impose_base",Cond_lim_base);


// Description:
//    Ecrit le type de l'objet sur un flot de sortie
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
Sortie& Echange_impose_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Lecture d'une condition aux limite de type Echange_impose_base
//    a partir d'un flot d'entree.
//    On doit lire le coefficient d'echange global H_imp et la temperature
//    a la frontiere T_ext.
//    Format:
//        "h_imp" type_champ_front bloc de lecture champ
//        "T_ext" type_champ_front bloc de lecture champ
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
Entree& Echange_impose_base::readOn(Entree& s )
{
  Motcle motlu;
  Motcles les_motcles(2);
  {
    les_motcles[0] = "h_imp";
    les_motcles[1] = "T_ext";
  }

  int ind = 0;
  while (ind < 2)
    {
      s >> motlu;
      int rang = les_motcles.search(motlu);

      switch(rang)
        {
        case 0:
          {
            s >> h_imp_;
            break;
          }
        case 1:
          {
            s >> T_ext();
            break;
          }
        default:
          {
            Cerr << "Erreur a la lecture de la condition aux limites de type Echange_impose " << finl;
            Cerr << "On attendait " << les_motcles << "a la place de " <<  motlu << finl;
            exit();
          }
        }

      ind++;

    }

  return s ;
}

// Description:
//    Renvoie la valeur de la temperature imposee
//    sur la i-eme composante du champ de frontiere.
// Precondition:
// Parametre: int i
//    Signification: l'indice de la composante du champ de
//                   de frontiere
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Echange_impose_base::T_ext(int i) const
{
  if (T_ext().valeurs().size()==1)
    return T_ext()(0,0);
  else if (T_ext().valeurs().dimension(1)==1)
    return T_ext()(i,0);
  else
    {
      Cerr << "Echange_impose_base::T_ext erreur" << finl;
      assert(0);
    }
  exit();
  return 0.;
}


// Description:
//    Renvoie la valeur de la temperature imposee
//    sur la (i,j)-eme composante du champ de frontiere.
// Precondition:
// Parametre: int i
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Echange_impose_base::T_ext(int i, int j) const
{
  if (T_ext().valeurs().dimension(0)==1)
    return T_ext()(0,j);
  else
    return T_ext()(i,j);
}


// Description:
//    Renvoie la valeur du coefficient d'echange
//    de chaleur impose sur la i-eme composante
//    du champ de frontiere.
// Precondition:
// Parametre: int i
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Echange_impose_base::h_imp(int i) const
{


  if (h_imp_.valeurs().size()==1)
    return h_imp_(0,0);
  else if (h_imp_.valeurs().dimension(1)==1)
    return h_imp_(i,0);
  else
    Cerr << "Echange_impose_base::h_imp erreur" << finl;

  exit();
  return 0.;
}


// Description:
//    Renvoie la valeur du coefficient d'echange
//    de chaleur impose sur la i-eme composante
//    du champ de frontiere.
// Precondition:
// Parametre: int i
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Echange_impose_base::h_imp(int i, int j) const
{

  if (h_imp_.valeurs().dimension(0)==1)
    return h_imp_(0,j);
  else
    return h_imp_(i,j);

}



// Description:
//    Effectue une mise a jour en temps
//    des conditions aux limites.
//    Lors du premier appel des initialisations sont effectuees:
//      h_imp(0,0) = (rho(0,0)*Cp(0,0))
// Precondition:
// Parametre: double temps
//    Signification: le temp de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Echange_impose_base::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);
  h_imp_.mettre_a_jour(temps);
}

int Echange_impose_base::initialiser(double temps)
{
  if (h_imp_.non_nul())
    h_imp_.valeur().initialiser(temps,zone_Cl_dis().equation().inconnue()), h_imp_.mettre_a_jour(temps);
  return Cond_lim_base::initialiser(temps);
}

int Echange_impose_base::compatible_avec_discr(const Discretisation_base& discr) const
{
  if ((discr.que_suis_je() == "VDF")
      || (discr.que_suis_je() == "VDF_Interface")
      || (discr.que_suis_je() == "VDF_Front_Tracking")
      || (discr.que_suis_je() == "VEFPreP1B")
      || (discr.que_suis_je() == "VEFPreP1B")
      || (discr.que_suis_je() == "PolyMAC")
      || (discr.que_suis_je() == "CoviMAC") )
    return 1;
  else
    {
      err_pas_compatible(discr);
      return 0;
    }
}


// Description:
//    Verifie la compatibilite des conditions aux limites avec
//    l'equation passee en parametre.
//   Les conditions aux limites de type Ech_imp_base  sont
//   compatibles avec des equations de type:
//         - Thermique
//         - indetermine
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle on doit verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne, 1 si compatible 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Echange_impose_base::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Thermique="Thermique";
  Motcle indetermine="indetermine";
  Motcle FracMass="fraction_massique";
  if ( (dom_app==Thermique) || (dom_app==indetermine) || (dom_app==FracMass) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}
// ajout de methode pour ne pas operer directement su le champ_front
void  Echange_impose_base::set_temps_defaut(double temps)
{
  if (h_imp_.non_nul())
    h_imp_.valeur().set_temps_defaut(temps);
  Cond_lim_base::set_temps_defaut(temps);
}
void  Echange_impose_base::fixer_nb_valeurs_temporelles(int nb_cases)
{
  if (h_imp_.non_nul())
    h_imp_.valeur().fixer_nb_valeurs_temporelles(nb_cases);
  Cond_lim_base::fixer_nb_valeurs_temporelles(nb_cases);
}
//
void  Echange_impose_base::changer_temps_futur(double temps,int i)
{
  if (h_imp_.non_nul())
    h_imp_.valeur().changer_temps_futur(temps,i);
  Cond_lim_base::changer_temps_futur(temps,i);
}
int  Echange_impose_base::avancer(double temps)
{
  if (h_imp_.non_nul())
    h_imp_.valeur().avancer(temps);
  return Cond_lim_base::avancer(temps);
}


int  Echange_impose_base::reculer(double temps)
{
  if (h_imp_.non_nul())
    h_imp_.valeur().reculer(temps);
  return Cond_lim_base::reculer(temps);
}
void  Echange_impose_base::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  if (h_imp_.non_nul())
    h_imp_.valeur().associer_fr_dis_base(fr);
  Cond_lim_base::associer_fr_dis_base(fr);
}
