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

#include <Champ_front_synt.h>
#include <Frontiere_dis_base.h>
#include <Frontiere.h>
#include <Navier_Stokes_std.h>
#include <Zone_VF.h>
#include <Fluide_base.h>
#include <Champ_Uniforme.h>
#include <Schema_Temps.h>

#include <random>

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
  temps_d_avant_ = tps;

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
  lenghtScale = 0.;
  nbModes = 0;
  turbKinEn = 0.;
  turbDissRate = 0.;
  KeOverKmin = 0.;
  ratioCutoffWavenumber = 0;

  int dim;
  dim=lire_dimension(is,que_suis_je());
  if( dim != 3)
    {
      Cerr << "Error the dimension must be equal to 3" << finl;
      exit();
    }
  Motcle motlu;
  Motcles les_mots(9);
  les_mots[0]="moyenne";
  les_mots[1]="lenghtScale";
  les_mots[2]="nbModes";
  les_mots[3]="turbKinEn";
  les_mots[4]="turbDissRate";
  les_mots[5]="KeOverKmin";
  les_mots[6]="timeScale";
  les_mots[7]="ratioCutoffWavenumber";
  les_mots[8]="dir_fluct";

  is >> motlu;
  if (motlu != "{")
    {
      Cerr << "Error while reading Champ_front_synt" << finl;
      Cerr << "We expected a { instead of " << motlu << finl;
      exit();
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
            is >> turbDissRate;
            break;
          }
        case 5:
          {
            cpt++;
            is >> KeOverKmin;
            break;
          }
        case 6:
          {
            cpt++;
            is >> timeScale;
            break;
          }
        case 7:
          {
            cpt++;
            is >> ratioCutoffWavenumber;
            break;
          }
        case 8:
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
            if (motlu == "p")
              {
                Cerr << "Error while reading Champ_front_synt:" << finl;
                Cerr << "  Parameter " << motlu << " has been renamed to KeOverKmin since TRUST v1.9.0"<< finl;
                Cerr << "  Update your datafile."<< finl;
              }
            else
              {
                Cerr << "Error while reading Champ_front_synt:" << finl;
                Cerr << "  " << motlu << "is not understood."<< finl;
                Cerr << "  We are expecting a parameter among " << les_mots << finl;
              }
            exit();
          }
        }
      is >> motlu;
    }
  if(cpt != 9)
    {
      Cerr << "Error while reading Champ_front_synt: wrong number of parameters" << finl;
      Cerr << "You should specify all these parameters: " << les_mots << finl;
      exit();
    }
  if( lenghtScale == 0 || nbModes == 0 || turbKinEn == 0 || turbDissRate == 0 || KeOverKmin == 0 || timeScale == 0 || ratioCutoffWavenumber == 0 )
    {
      Cerr << "Error while reading Champ_front_synt" << finl;
      Cerr << "There is at least one parameter among: timeScale, lenghtScale, nbModes, turbKinEn, turbDissRate and ratioCutoffWavenumber set to 0" << finl;
      exit();
    }

  for(int i=0; i<dim; i++)
    {
      Cerr << dir_fluct(i) << " ";
    }
  Cerr << finl;


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

  /*
    Cerr << "*************************************************" << finl;
    Cerr << "mil = " << mil.masse_volumique().valeur()(0,0) << finl;
    Cerr << "temps = " << equ.inconnue().temps() << finl;
    Cerr << "dt = " << equ.schema_temps().pas_de_temps() << finl;
    Cerr << "visco cinematique = " << ref_cast(Fluide_base,mil).viscosite_cinematique().valeur()(0,0) << finl;
    Cerr << "visco dynamique = " << ref_cast(Fluide_base,mil).viscosite_dynamique().valeur()(0,0) << finl;
    Cerr << "*************************************************" << finl;

    const Champ_Don& visco = ref_cast(Fluide_base,mil).viscosite_dynamique();
    if (sub_type(Champ_Uniforme,visco.valeur()))
      Cerr << "visco dynamique = " << visco(0,0) << finl;
    else
      {
        const DoubleTab& val_visco = visco.valeur().valeurs();
        Cerr << "valeurs viscosite = " << val_visco << finl;\
      }
  */

  ////////////////////////////////////////////
  /// 	   donnees d'initialisation        ///
  ////////////////////////////////////////////

  double visc = ref_cast(Fluide_base,mil).viscosite_cinematique().valeur()(0,0);
  const Front_VF& front = ref_cast(Front_VF,la_frontiere_dis.valeur());
  int nb_face = front.nb_faces(); // real only
  const Faces& tabFaces = front.frontiere().faces(); // recuperation des faces

  //Cerr << "We store : temps_d_avant_ = "<<temps_d_avant_<<finl;
  DoubleTab& tab_avant=valeurs_au_temps(temps_d_avant_);
  DoubleTab& tab=valeurs_au_temps(temps);

  DoubleVect aireFaces; // Attention : contains real + virtual faces
  tabFaces.calculer_surfaces(aireFaces);

  double sum_aire=0.;
  for(int i=0; i<nb_face; i++)
    sum_aire += aireFaces[i];

  sum_aire = mp_sum(sum_aire);
  double dmin = sqrt( sum_aire / mp_sum(nb_face) ) ; // on prend la racine de l'aire moyenne des faces d'entree pour avoir une taille de maille caracteristique

  //double Uref = 0.; // vitesse de reference = norme du vecteur moyenne
  //for (int i=0; i<moyenne.size_reelle(); i++) Uref += moyenne(i)*moyenne(i);
  //Uref = sqrt(Uref);

  //double turbScale = turbIntensity * Uref; //urms=I*Uref
  //double turbKinEn = 3./2. * (turbIntensity * Uref) * (turbIntensity * Uref); // evaluation de l'energie cinetique turbulente = 3/2*(I*Uref)^2
  //double turbDissRate = pow(Cmu,0.75)*pow(turbKinEn,1.5)/lenghtScale; // calcul de epsilon pour un ecoulement etabli en conduite. A faire pour un ecoulement de grille ? (decroissance energetique)

  /////////////////////////////////////////////
  /// valeurs remarquables du nombre d'onde ///
  /////////////////////////////////////////////

  double kappa_max = (pi/dmin)*ratioCutoffWavenumber; // plus grand nombre d'onde (depend du maillage)
  double kappa_e = 9*pi*amp/(55*lenghtScale); // pic d'energie
  double kappa_eta = pow((turbDissRate/(visc*visc*visc)),0.25); // nombre d'onde de Kolmogorov
  double kappa_min = kappa_e / KeOverKmin; // plus petit nombre d'onde
  //double delta_kappa = (min(kappa_eta,kappa_max) - kappa_min) / nbModes; // repartition lineaire des modes => pas bon
  double delta_kappa = pow( (std::min(kappa_eta,kappa_max) / kappa_min ), 1./(nbModes-1.)); // repartition logarithmique des modes => OK

  //Cerr << "Remarkable wavenumbers for the method of synthetic turbulence generation:" << finl;
  //Cerr << "kappa_min = " << kappa_min << finl;
  //Cerr << "kappa_e = " << kappa_e << finl;
  //Cerr << "kappa_mesh = " << kappa_max << finl;
  //Cerr << "kappa_eta = " << kappa_eta << finl;
  //Cerr << "delta_kappa = " << delta_kappa << finl;

  DoubleVect kappa_face(nbModes+1);
  DoubleVect kappa_center(nbModes);
  DoubleVect dkn(nbModes);

  DoubleVect kappa_x(nbModes);
  DoubleVect kappa_y(nbModes);
  DoubleVect kappa_z(nbModes);

  DoubleVect sigma_x(nbModes);
  DoubleVect sigma_y(nbModes);
  DoubleVect sigma_z(nbModes);

  DoubleVect phi(nbModes);
  DoubleVect alpha(nbModes);
  DoubleVect psi(nbModes);
  DoubleVect tetha(nbModes);

  ////////////////////////////////////////////
  /// generation aleatoire des angles      ///
  ////////////////////////////////////////////

  for(int i = 0; i<nbModes; i++)
    {
      phi(i) = drand48()* 2*pi ;
      alpha(i) = drand48()* 2*pi ;
      psi(i) = drand48()* 2*pi ;
      //tetha(i) = drand48()* pi ; // pour une densite de probabilite de 1/pi => pas bon
      tetha(i) = acos(1-2*drand48()) ; // pour une densite de probabilite de 0.5*sin(theta) => OK

      /// creation vecteur onde en coordonnee cartesienne ///
      kappa_x(i) = sin(tetha(i))*cos(phi(i));
      kappa_y(i) = sin(tetha(i))*sin(phi(i));
      kappa_z(i) = cos(tetha(i));

      /// creation de la direction orthogonal au vecteur onde ///
      sigma_x(i) = cos(phi(i))*cos(tetha(i))*cos(alpha(i)) - sin(phi(i))*sin(alpha(i));
      sigma_y(i) = sin(phi(i))*cos(tetha(i))*cos(alpha(i)) + cos(phi(i))*sin(alpha(i));
      sigma_z(i) = -sin(tetha(i))*cos(alpha(i));
    }

  //for(int i = 0; i< nbModes+1; i++) kappa_face(i) = kappa_min + delta_kappa*i; // repartition lineaire
  for(int i = 0; i< nbModes+1; i++)
    {
      kappa_face(i) = kappa_min * pow(delta_kappa,i); // repartition logarithmique
      //Cerr << "Kappa(n) = " << kappa_face(i) << finl;
    }

  for(int i = 0; i< nbModes; i++)
    {
      kappa_center(i) = 1.0/2.0*(kappa_face(i+1)+kappa_face(i));
      dkn(i) = kappa_face(i+1)- kappa_face(i);
    }

  /// recuperation des centres de gravite ///
  DoubleTab centreGrav(nb_face);
  tabFaces.calculer_centres_gravite(centreGrav);

  for( int i = 0; i<nb_face; i++ )
    {
      DoubleTab turb(nb_comp());
      //recuperer les points du milieu du maillage

      double x_center = centreGrav(i,0);
      double y_center = centreGrav(i,1);
      double z_center = centreGrav(i,2);

      for(int m = 0; m<nbModes; m++)
        {
          double kx = kappa_x(m) * kappa_center(m);
          double ky = kappa_y(m) * kappa_center(m);
          double kz = kappa_z(m) * kappa_center(m);

          double arg = kx*x_center + ky*y_center + kz*z_center + psi(m);

          double tfunk = cos(arg);

          double karman_spectrum = amp/kappa_e * (2.*turbKinEn/3.) * pow((kappa_center(m)/kappa_e),4)/pow(1+pow(kappa_center(m)/kappa_e, 2),17.0/6.0) * exp(-2*(pow(kappa_center(m)/kappa_eta, 2)));
          double amplitude = sqrt(karman_spectrum * dkn(m));

          turb(0) += 2*amplitude*tfunk*sigma_x(m);
          turb(1) += 2*amplitude*tfunk*sigma_y(m);
          turb(2) += 2*amplitude*tfunk*sigma_z(m);

          //Cerr << "Kappa = " << kappa_center(m) << finl;
          //Cerr << "Kappa, Spectre = " << kappa_center(m) << " , " << karman_spectrum << finl;
          //Cerr << "Amplitude = " << amplitude << finl;
        }

      //////////////////////////////////////
      /// MISE EN PLACE AUTOCORRELATION  ///
      //////////////////////////////////////

      double dt = equ.schema_temps().pas_de_temps();
      double a = exp(-dt/timeScale);
      double b = sqrt(1-a*a);

      tab(i,0) = moyenne(0) + dir_fluct(0) * (a * (tab_avant(i,0) - moyenne(0)) + b * turb(0));
      tab(i,1) = moyenne(1) + dir_fluct(1) * (a * (tab_avant(i,1) - moyenne(1)) + b * turb(1));
      tab(i,2) = moyenne(2) + dir_fluct(2) * (a * (tab_avant(i,2) - moyenne(2)) + b * turb(2));

      //Cerr << "r, a, b = " << dt/timeScale << " , " << a << " , " << b << finl;
      //Cerr << "Uprime, uprime = " << tab(i,0) << " , " << turb(0) << finl;
      //Cerr << "face, Uprime, uprime = " << i << " , " << tab(i,0) << " , " << turb(0) << finl;
    }

  tab.echange_espace_virtuel();
  temps_d_avant_ = temps;

}
