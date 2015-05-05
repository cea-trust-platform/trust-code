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
// File:        Champ_Ostwald.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
//   Classe mere qui gere le champ utilise pour le fluide d'Ostwald dans les deux
//   discretisations.
//   Possede les fonctions generiques aux deux discretisations.
//   Fait reference a un fluide d'Ostwald pour pouvoir utiliser les deux
//   parametres du fluide d'Ostwald : K et N.

#include <Champ_Ostwald.h>
#include <Zone_dis_base.h>

Implemente_instanciable(Champ_Ostwald,"Champ_Ostwald",Champ_Fonc_P0_base);


// Description:
//    Imprime le champ sur un flot de sortie.
//    Imprime le nb_de composantes.
// Precondition:
// Parametre: Sortie& os
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
Sortie& Champ_Ostwald::printOn(Sortie& os) const
{
  //const int nb_compo = valeurs()(0,0);
  //os << nb_compo;
  return os;
}


// Description:
//    Lit un champ a partir d'un flot d'entree.
//    On lit le nombre de composante du champ (nb_comp)
//    Format:
//      Champ_Ostwald nb_comp
// Precondition:
// Parametre: Entree& is
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
Entree& Champ_Ostwald::readOn(Entree& is)
{
  return is;
}


// Description:
//    NE FAIT RIEN, provoque une erreur
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Ostwald::mettre_a_jour(double )
{
  Cerr << "Champ_Ostwald::mettre_a_jour() ne fait rien" << finl;
  Cerr <<  que_suis_je() << "doit la surcharger !" << finl;
  exit();
}

// Description:
//    NE FAIT RIEN, provoque une erreur
//    A surcharger dans les classes derivees.
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
int Champ_Ostwald::initialiser(const double& un_temps)
{
  Cerr << "Champ_Ostwald::initialiser(temps) must be overloaded" << finl;
  Cerr << " by " << que_suis_je() << finl;
  exit();
  return 0;
}

// Description:
//    NE FAIT RIEN, provoque une erreur
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: double
//    Signification: tps ou se fait le calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Ostwald::me_calculer(double tps)
{
  Cerr << "Champ_Ostwald::me_calculer() ne fait rien" << finl;
  Cerr <<  que_suis_je() << "doit la surcharger !" << finl;
  exit();
}

// Description:
//    NE FAIT RIEN, provoque une erreur
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: Champ_base
//    Signification: un des champs derives
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
/*
  void Champ_Ostwald::associer_champ(const Champ_base& ch)
  {
  Cerr << "Champ_Ostwald::associer_champ() ne fait rien" << finl;
  Cerr <<  que_suis_je() << "doit la surcharger !" << finl;
  exit();
  }*/

// Description:
//    Fixe le nombre de degres de liberte par composante
// Precondition:
// Parametre: int nb_noeuds
//    Signification: le nombre de degre de liberte par composante
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de degres de liberte par composante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le tableaux des valeurs peut etre redimensionne
int Champ_Ostwald::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  // Note B.M.: encore un heritage a la noix qui m'empeche de factoriser
  // en utilisant creer_tableau_distribue:
  const Champ_Don_base& cdb = *this;
  const Zone& zone = cdb.zone_dis_base().zone();

  assert(nb_noeuds == zone.nb_elem());

  if(nb_compo_==1)
    valeurs_.resize(0);
  else
    valeurs_.resize(0, nb_compo_);
  zone.creer_tableau_elements(valeurs_);
  return nb_noeuds;
}


// Description:
//    NE FAIT RIEN, provoque une erreur
//    A surcharger dans les classes derivees.
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

void Champ_Ostwald::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  Cerr << "Champ_Ostwald::associer_zone_dis_base() ne fait rien" << finl;
  Cerr <<  que_suis_je() << "doit la surcharger !" << finl;
  exit();
}

// Postcondition:
Champ_base& Champ_Ostwald::affecter_(const Champ_base& ch)
{
  DoubleTab noeuds;
  IntVect polys;
  if(!remplir_coord_noeuds_et_polys(noeuds, polys))
    {
      remplir_coord_noeuds(noeuds);
      ch.valeur_aux(noeuds, valeurs());
    }
  else
    ch.valeur_aux_elems(noeuds, polys, valeurs());
  return *this;
}


const Zone_dis_base& Champ_Ostwald::zone_dis_base() const
{
  Cerr<<"const Zone_dis_base& Champ_Ostwald::zone_dis_base() const non code"<<finl;
  exit();
  return zone_dis_base() ;
}
