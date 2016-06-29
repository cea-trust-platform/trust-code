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
// File:        Champ_front_bruite.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_bruite.h>
#include <Frontiere_dis_base.h>
#include <Frontiere.h>

Implemente_instanciable(Champ_front_bruite,"Champ_front_bruite",Ch_front_var_instationnaire_indep);

// Description:
//    Impression sur un flot de sortie au format:
//    taille
//    valeur(0) ... valeur(i)  ... valeur(taille-1)
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
Sortie& Champ_front_bruite::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}

// Description:
//    Lecture a partir d'un flot d'entree au format:
//    nombre_de_composantes
//    moyenne moyenne(0) ... moyenne(nombre_de_composantes-1)
//    moyenne amplitude(0) ... amplitude(nombre_de_composantes-1)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: mot clef inconnu a cet endroit
// Exception: accolade fermante attendue
// Effets de bord:
// Postcondition:
Entree& Champ_front_bruite::readOn(Entree& is)
{
  nx=0;
  lx=0.;
  int dim;
  dim=lire_dimension(is,que_suis_je());
  Motcle motlu;
  Motcles les_mots(4);
  les_mots[0]="moyenne";
  les_mots[1]="amplitude";
  les_mots[2]="L";
  les_mots[3]="N";
  is >> motlu;
  if (motlu != "{")
    {
      Cerr << "Error while reading a Champ_front_bruite" << finl;
      Cerr << "We expected a { instead of " << motlu << finl;
      exit();
    }
  is >> motlu;
  while (motlu!="}")
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            moyenne.resize(dim);
            fixer_nb_comp(dim);
            for(int i=0; i<dim; i++)
              is >> moyenne(i);
            break;
          }
        case 1:
          {
            amplitude.resize(dim);
            for(int i=0; i<dim; i++)
              is >> amplitude(i);
            break;
          }
        case 2:
          {
            is >> lx;
            break;
          }
        case 3:
          {
            is >> nx;
            break;
          }
        default :
          {
            Cerr << "Error while reading a Champ_front_bruite" << finl;
            Cerr << motlu << "is not understand here "<< finl;
            Cerr << "We were expecting a word from " << les_mots << finl;
            exit();
          }
        }
      is >> motlu;
    }
  return is;
}


// Description:
//    Pas code !!
// Precondition:
// Parametre: Champ_front_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Champ_front_base&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_front_base& Champ_front_bruite::affecter_(const Champ_front_base& ch)
{
  return *this;
}

// Description:
//     Mise a jour du temps
//     et retirage aleatoire des valeurs du bruit.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_front_bruite::mettre_a_jour(double temps)
{
  const Frontiere& front=la_frontiere_dis->frontiere();
  int nb_faces=front.nb_faces();
  DoubleTab& tab=valeurs_au_temps(temps);

  if (nx!=0)
    {
      //
      // N!=0 modelisation du bruit blanc (R. Howard)
      //
      double onde, bruit_blanc, pi, invx, d_x, tot_vel;
      pi=2.*acos(0.);
      invx=1./nx;
      d_x=lx/nx;
      for(int i=0; i<nb_faces; i++)
        for(int j=0; j<nb_comp(); j++)
          {
            tot_vel=moyenne(j)+amplitude(j);
            onde=0.;
            for (int in_x=0; in_x<nx; in_x++)
              onde=onde+sin((2.*pi*temps/(4*d_x+in_x*d_x)+(10.2*i+5.3*j))*tot_vel+5.6*pi);

            bruit_blanc=onde*invx;
            tab(i,j)=moyenne(j)+2.*amplitude(j)*bruit_blanc;
          }
    }
  else
    //
    // N=0 donc modelisation bruit aleatoire
    //
    for(int i=0; i<nb_faces; i++)
      for(int j=0; j<nb_comp(); j++)
        {
          tab(i,j)=moyenne(j)+amplitude(j)*(-1.+2.*drand48());
        }
  tab.echange_espace_virtuel();
}


