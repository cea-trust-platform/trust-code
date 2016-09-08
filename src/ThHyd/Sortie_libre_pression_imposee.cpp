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
// File:        Sortie_libre_pression_imposee.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Sortie_libre_pression_imposee.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <Navier_Stokes_std.h>
#include <Fluide_Incompressible.h>

Implemente_instanciable_sans_constructeur(Sortie_libre_pression_imposee,"Frontiere_ouverte_pression_imposee",Neumann_sortie_libre);


Sortie_libre_pression_imposee::Sortie_libre_pression_imposee():d_rho(-123.) {}

// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
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
Sortie& Sortie_libre_pression_imposee::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Sortie_libre_pression_imposee sont compatibles
//    avec une equation dont le domaine est l'hydraulique (Navier_Stokes)
//    ou bien indetermine.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle il faut verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne,
//                   1 si les CL sont compatibles avec l'equation
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Sortie_libre_pression_imposee::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Hydraulique="Hydraulique";
  Motcle indetermine="indetermine";
  if ( (dom_app==Hydraulique) || (dom_app==indetermine) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

// Description:
//    Lit le champ de frontiere a partir d'un flot d'entree.
//    Type le_champ_ext en "Champ_front_uniforme".
//    Redimmensionne le_champ_ext en fonction de la dimension
//    de l'espace (dimension)
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Sortie_libre_pression_imposee::readOn(Entree& s )
{
  s >> le_champ_front;
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1,dimension);
  return s ;
}

// Description:
//    Complete les conditions aux limites.
//    Impose la masse volumique constante du milieu
//    physique de l'equation a d_rho.
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
void Sortie_libre_pression_imposee::completer()
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  if (sub_type(Fluide_Incompressible,mil))
    {
      if (sub_type(Champ_Uniforme,mil.masse_volumique().valeur()))
        {
          const Champ_Uniforme& rho=ref_cast(Champ_Uniforme,mil.masse_volumique().valeur());
          d_rho = rho(0,0);
        }
      else
        {
          d_rho = -1;

          // GF dans les cas a rho non const QC FT on ne doit pas diviser par rho
          // on met d_rho=1
          d_rho=1;
        }
    }
  else
    d_rho=1;
}

// Description:
//    Renvoie la valeur du flux impose sur la i-eme composante
//    du champ representant le flux a la frontiere.
//    Le champ a la frontiere est considere constant sur tous
//    les elements de la frontiere.
//    La valeur du flux impose a la frontiere est egale
//    a la valeur du champ (considere constant) a la frontiere divise par d_rho.
// Precondition:
// Parametre: int i
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes:
// Exception: deuxieme dimension du champ de frontiere superieur a 1
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Sortie_libre_pression_imposee::flux_impose(int i) const
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  double rho_;
  assert(!est_egal(d_rho,-123.));
  if (d_rho==-1)
    {
      const Champ_Don& rho=mil.masse_volumique();
      rho_ = rho.valeur()(i);
    }
  else
    {
      rho_ = d_rho;
    }

  if (le_champ_front.valeurs().size()==1)
    return le_champ_front(0,0)/rho_;
  else if (le_champ_front.valeurs().dimension(1)==1)
    return le_champ_front(i,0)/rho_;
  else
    Cerr << "Neumann::flux_impose erreur" << finl;
  exit();
  return 0.;
}

// Description:
//    Renvoie la valeur du flux impose sur la (i,j)-eme composante
//    du champ representant le flux a la frontiere.
//    Le champ a la frontiere n'est PAS constant sur tous les elements
//    la frontiere.
// Precondition:
// Parametre: int i
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int j
//    Signification: indice suivant la deuxieme dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Sortie_libre_pression_imposee::flux_impose(int i,int j) const
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  const Champ_Don& rho=mil.masse_volumique();
  double rho_;
  assert(!est_egal(d_rho,-123.));
  if (d_rho==-1)
    {
      rho_ = rho.valeur()(i);
    }
  else
    {
      rho_ = d_rho;
    }

  if (le_champ_front.valeurs().dimension(0)==1)
    return le_champ_front(0,j)/rho_;
  else
    return le_champ_front(i,j)/rho_;
}
