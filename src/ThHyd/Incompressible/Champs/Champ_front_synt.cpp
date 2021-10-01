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
// File:        Champ_front_synt.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Champs
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_synt.h>
#include <Frontiere_dis_base.h>
#include <Frontiere.h>
#include <Navier_Stokes_std.h>
#include <Zone_VF.h>
#include <Fluide_base.h>
#include <Champ_Uniforme.h>
#include <Schema_Temps.h>

#include <random>
#include <fstream>
#include <string>


Implemente_instanciable(Champ_front_synt,"Champ_front_synt",Ch_front_var_instationnaire_dep);

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
Sortie& Champ_front_synt::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}

// Description:
// Mise a jour du temps

int Champ_front_synt::initialiser(double tps, const Champ_Inc_base& inco)
{
  if (!Ch_front_var_instationnaire_dep::initialiser(tps,inco))
    return 0;

  ref_inco_ = inco;
  mettre_a_jour(tps);

  return 1;
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
Entree& Champ_front_synt::readOn(Entree& is)
{
  timeScale = 0.;
  lenghtScale =0.;
  nbModes=0;
  turbKinEn=0.;
  p=0.;

  int dim;
  dim=lire_dimension(is,que_suis_je());
  if( dim != 3)
    {
      Cerr << "Error the dimension must be equal to 3" << finl;
      Process::exit();
    }
  Motcle motlu;
  Motcles les_mots(7);
  les_mots[0]="moyenne";
  les_mots[1]="lenghtScale";
  les_mots[2]="nbModes";
  les_mots[3]="turbKinEn";
  les_mots[4]="p";
  les_mots[5]="timeScale";
  les_mots[6]="dir_fluct";

  is >> motlu;
  if (motlu != "{")
    {
      Cerr << "Error while reading Champ_front_synt" << finl;
      Cerr << "We expected a { instead of " << motlu << finl;
      Process::exit();
    }
  int cpt = 0;
  is >> motlu;
  while (motlu!="}")
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0:
          {
            cpt++;
            moyenne.resize(dim);
            fixer_nb_comp(dim);
            for(int i=0; i<dim; i++)
              is >> moyenne(i);
            break;
          }
        case 1:
          {
            cpt++;
            is >> lenghtScale;
            break;
          }
        case 2:
          {
            cpt++;
            is >> nbModes;
            break;
          }
        case 3:
          {
            cpt++;
            is >> turbKinEn;
            break;
          }
        case 4:
          {
            cpt++;
            is >> p;
            break;
          }
        case 5:
          {
            cpt++;
            is >> timeScale;
            break;
          }
        case 6:
          {
            cpt++;
            dir_fluct.resize(dim);
            fixer_nb_comp(dim);
            for(int i=0; i<dim; i++)
              {
                is >> dir_fluct(i);
              }
            break;
          }
        default :
          {
            Cerr << "Error while reading Champ_front_synt" << finl;
            Cerr << motlu << "is not understood."<< finl;
            Cerr << "We are expecting a word among " << les_mots << finl;
            Process::exit();
          }
        }
      is >> motlu;
    }
  if(cpt != 7)
    {
      Cerr << "Error while reading Champ_front_synt: wrong number of parameters" << finl;
      Cerr << "You should specify all these parameters: " << les_mots << finl;
      Process::exit();
    }
  if( lenghtScale == 0 || nbModes == 0 || turbKinEn == 0 || p == 0 || timeScale == 0)
    {
      Cerr << "Error while reading Champ_front_synt" << finl;
      Cerr << "There is at least one parameter among: timeScale, lenghtScale, nbModes and turbKinEn set to 0" << finl;
      Process::exit();
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
Champ_front_base& Champ_front_synt::affecter_(const Champ_front_base& ch)
{
  return *this;
}

void Champ_front_synt::mettre_a_jour(double temps)
{

  // Acceder a l'equation depuis l'inconnue, ensuite acceder au milieu
  const Equation_base& equ = ref_inco_.valeur().equation();
  const Milieu_base& mil = equ.milieu();

  ////////////////////////////////////////////
  /// 	   donnees d'initialisation        ///
  ////////////////////////////////////////////

  double visc = ref_cast(Fluide_base,mil).viscosite_cinematique().valeur()(0,0);
  const Front_VF& front = ref_cast(Front_VF,la_frontiere_dis.valeur());
  int nb_face = front.nb_faces(); // real only
  const Faces& tabFaces = front.frontiere().faces(); // recuperation des faces

  DoubleTab& tab=valeurs_au_temps(temps);

  DoubleVect aireFaces; // Attention : contains real + virtual faces
  tabFaces.calculer_surfaces(aireFaces);

  double sum_aire=0.;
  for(int i=0; i<nb_face; i++) sum_aire += aireFaces[i];

  sum_aire = mp_sum(sum_aire);
  double dmin = sum_aire / mp_sum(nb_face) ;
  double turbScale = sqrt(2*turbKinEn/3);
  double eps = pow(turbKinEn,1.5);

  /////////////////////////////////////////////
  /// valeurs remarquables du nombre d'onde ///
  /////////////////////////////////////////////

  double kapa_max = pi/dmin; // plus grande nombre d'onde
  double kapa_e = 9*pi*amp/(55*lenghtScale); //pic d'energie
  double kapa_eta = pow((eps/(visc*visc*visc)),0.25); // pour le spectre de von Karman
  double kapa_min = kapa_e / p; // plus petit nombre d'onde
  double delta_kappa = (kapa_max - kapa_min) / nbModes; //subdivision du nombre d onde

  DoubleVect kapa_face(nbModes+1);
  DoubleVect kapa_center(nbModes);
  DoubleVect dkn(nbModes);

  DoubleVect kapa_x(nbModes);
  DoubleVect kapa_y(nbModes);
  DoubleVect kapa_z(nbModes);

  DoubleVect sigma_x(nbModes);
  DoubleVect sigma_y(nbModes);
  DoubleVect sigma_z(nbModes);

  DoubleVect fi(nbModes);
  DoubleVect alpha(nbModes);
  DoubleVect psi(nbModes);
  DoubleVect teta(nbModes);

  ////////////////////////////////////////////
  /// generation aleatoire des angles      ///
  ////////////////////////////////////////////

  for(int i = 0; i<nbModes; i++)
    {
      fi(i) = drand48()* 2*pi ;
      alpha(i) = drand48()* 2*pi ;
      psi(i) = drand48()* 2*pi ;
      teta(i) = drand48()* pi ;

      /// creation vecteur onde en coordonnee cartesienne ///
      kapa_x(i) = sin(teta(i))*cos(fi(i));
      kapa_y(i) = sin(teta(i))*sin(fi(i));
      kapa_z(i) = cos(teta(i));

      /// creation de la direction orthogonal au vecteur onde ///
      sigma_x(i) = cos(fi(i))*cos(teta(i))*cos(alpha(i)) - sin(fi(i))*sin(alpha(i));
      sigma_y(i) = sin(fi(i))*cos(teta(i))*cos(alpha(i)) + cos(fi(i))*sin(alpha(i));
      sigma_z(i) = -sin(teta(i))*cos(alpha(i));
    }

  for(int i = 0; i< nbModes+1; i++) kapa_face(i) = kapa_min + delta_kappa*i;
  for(int i = 0; i< nbModes; i++)
    {
      kapa_center(i) = 1.0/2.0*(kapa_face(i+1)+kapa_face(i));
      dkn(i) = kapa_face(i+1)- kapa_face(i);
    }

  /// recuperation des centres de gravites ///
  DoubleTab centreGrav(nb_face);
  tabFaces.calculer_centres_gravite(centreGrav);


  for( int i = 0; i<nb_face; i++ )
    {
      DoubleTab turb(nb_comp());
      //recuperer les points du millieu du maillage

      double x_center = centreGrav(i,0);
      double y_center = centreGrav(i,1);
      double z_center = centreGrav(i,2);

      for(int m = 0; m<nbModes; m++)
        {
          double kx = kapa_x(m) * kapa_center(m);
          double ky = kapa_y(m) * kapa_center(m);
          double kz = kapa_z(m) * kapa_center(m);


          double rk = sqrt(kx*kx + ky*ky + kz*kz);

          if(rk < kapa_e)
            {
              double arg = kx*x_center + ky*y_center + kz*z_center + psi(m);

              double tfunk = cos(arg);

              double karman_spectrum = amp/kapa_e * pow((kapa_center(m)/kapa_e),4)/pow(1+pow(kapa_center(m)/kapa_e, 2),17.0/6.0) * exp(-2*(pow(kapa_center(m)/kapa_eta, 2)));
              double amplitude = sqrt(karman_spectrum *  turbScale * turbScale * dkn(m));

              turb(0) += 2*amplitude*tfunk*sigma_x(m);
              turb(1) += 2*amplitude*tfunk*sigma_y(m);
              turb(2) += 2*amplitude*tfunk*sigma_z(m);

            }
        }

      //////////////////////////////////////
      /// MISE EN PLACE AUTOCORRELATION  ///
      //////////////////////////////////////

      double dt = equ.schema_temps().pas_de_temps();
      double a = exp(-dt/timeScale);
      double b = sqrt(1-a*a);

      tab(i,0) = moyenne(0) + dir_fluct(0) * (a * (tab(i,0)- moyenne(0)) + b * turb(0));
      tab(i,1) = moyenne(1) + dir_fluct(1) * (a * (tab(i,1)- moyenne(1)) + b * turb(1));
      tab(i,2) = moyenne(2) + dir_fluct(2) * (a * (tab(i,2)- moyenne(2)) + b * turb(2));

    }

  tab.echange_espace_virtuel();
}
