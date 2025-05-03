/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Champ_front_uniforme.h>
#include <Frontiere_dis_base.h>
#include <Schema_Temps_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_front_synt.h>
#include <Champ_Uniforme.h>
#include <Fluide_base.h>
#include <Domaine_VF.h>
#include <Frontiere.h>
#include <SFichier.h>
#include <Param.h>
#include <random>
#include <string>

Implemente_instanciable(Champ_front_synt, "Champ_front_synt", Ch_front_var_instationnaire_dep);

/*! @brief Impression sur un flot de sortie au format: taille
 *
 *     valeur(0) ... valeur(i)  ... valeur(taille-1)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Champ_front_synt::printOn(Sortie& os) const
{
  const DoubleTab& tab = valeurs();
  os << tab.size() << " ";
  for (int i = 0; i < tab.size(); i++)
    os << tab(0, i);
  return os;
}

/*! @brief Lecture a partir d'un flot d'entree
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws mot clef inconnu a cet endroit
 * @throws accolade fermante attendue
 */
Entree& Champ_front_synt::readOn(Entree& is)
{
  dim_ = lire_dimension(is, que_suis_je());
  if (dim_ != 3)
    {
      Cerr << "Error while reading Champ_front_synt:" << finl;
      Cerr << "The dimension must be equal to 3 to use Champ_front_synt" << finl;
      Process::exit();
    }

  Param param(que_suis_je());
  param.ajouter("moyenne", &moyenne_, Param::REQUIRED);
  param.ajouter("turbKinEn", &turbKinEn_, Param::REQUIRED);
  param.ajouter("turbDissRate", &turbDissRate_, Param::REQUIRED);
  param.ajouter("nbModes", &nbModes_, Param::REQUIRED);
  param.ajouter("KeOverKmin", &KeOverKmin_, Param::REQUIRED);
  param.ajouter("ratioCutoffWavenumber", &ratioCutoffWavenumber_, Param::REQUIRED);
  param.ajouter("ecriture", &ecriture_, Param::REQUIRED);
  param.ajouter_non_std("dir_fluct", (this), Param::REQUIRED);
  // deprecated options. TODO FIXME should remove ?
  param.ajouter_non_std("p", (this));
  param.ajouter_non_std("timeScale|lenghtScale", (this));
  param.lire_avec_accolades_depuis(is);

  if (nbModes_ == 0 || KeOverKmin_ == 0 || ratioCutoffWavenumber_ == 0)
    {
      Cerr << "Error while reading Champ_front_synt" << finl;
      Cerr << "There is at least one parameter among: nbModes, turbKinEn, ratioCutoffWavenumber, set to 0" << finl;
      Process::exit();
    }

  return is;
}

int Champ_front_synt::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  int retval = -1;
  if (mot == "dir_fluct")
    {
      dir_fluct_.resize(dim_);
      fixer_nb_comp(dim_);
      for (int i = 0; i < dim_; i++)
        is >> dir_fluct_(i);
      retval = 1;
    }
  else if (mot == "p")
    {
      Cerr << "Error while reading Champ_front_synt:" << finl;
      Cerr << "  Parameter " << mot << " has been renamed to KeOverKmin since TRUST v1.9.0" << finl;
      Cerr << "  Update your datafile." << finl;
      Process::exit();
    }
  else if (mot == "timeScale" || mot == "lenghtScale")
    {
      Cerr << "Error while reading Champ_front_synt:" << finl;
      Cerr << "  'lenghtScale' and 'timeScale' are not parameters anymore ; they are estimated internally based on turbKinEn and turbDissRate." << finl;
      Cerr << "  Update your datafile." << finl;
      Process::exit();
    }

  return retval;
}

/*! @brief Choix du spectre (isotropique) local
 *
 */
double Champ_front_synt::energy_spectrum_(const double& kappa, const double& tke, const double& ke, const double& keta)
{
  return (amp_ / ke) * (2. * tke / 3.) * pow(kappa / ke, 4) / pow(1. + pow(kappa / ke, 2), 17. / 6.) * exp(-2. * (pow(kappa / keta, 2)));  // Karman spectrum
}

/*! @brief Initialisation
 *
 */
int Champ_front_synt::initialiser(double tps, const Champ_Inc_base& inco)
{
  if (!Ch_front_var_instationnaire_dep::initialiser(tps, inco))
    return 0;

  ref_inco_ = inco; // n'est utilise que pour acceder a l'equation : remplacer l'attribut par un attribut Equation_base& equ_ ???
  temps_d_avant_ = tps;

  ////// RECUPERATION INFORMATIONS FACES //////
  const Front_VF& front = ref_cast(Front_VF, la_frontiere_dis.valeur());
  nb_face_ = front.nb_faces(); // real only                        ////// CALCUL DE nb_face_ //////
  //
  centreGrav_.resize(nb_face_, 3);
  const Faces& tabFaces = front.frontiere().faces();
  tabFaces.calculer_centres_gravite(centreGrav_);                  ////// CALCUL DE centreGrav_ //////

  if (bool(ecriture_) )
    {
      nom_fichier_ = nom_du_cas();
      nom_fichier_ += "_vitesse_bord_synt.txt";

      int nb_face_com = nb_face_;
      envoyer(nb_face_com, Process::me(), 0, Process::me()); // pour Proc maitre

      if (Process::je_suis_maitre())
        {
          SFichier fiche_vals(nom_fichier_);
          fiche_vals << "# Temps ";

          for (int p = 0; p < Process::nproc(); p++)
            {
              recevoir(nb_face_com, p, 0, p);

              for (int i = 0; i < nb_face_com; i++)
                fiche_vals << "\t face_" << i << "_" << p
                           << "\t face_" << i << "_" << p
                           << "\t face_" << i<< "_" << p;

            }
          fiche_vals << finl;
        }

      // XXX Elie Saikali : Nota bene
      // pour centreGrav_, on ne doit pas passer par les buffers car l'op sortie fonctionne pas en //
      //          envoyer(centreGrav_com, Process::me(), 0, Process::me()); // ce truc pas possible

      auto span_vu = centreGrav_.get_span();

      if (Process::nproc() > 1 && Process::me()) // pas maitre
        {
          int taille = span_vu.size();
          MPI_Send(&taille, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
          MPI_Send(span_vu.data(), taille, MPI_DOUBLE, 0, 101, MPI_COMM_WORLD);
        }
      else // maitre !
        {
          SFichier fiche_vals(nom_fichier_, ios::app);

          // on commence par l'info sur proc 0
          fiche_vals << "# CentreGrav ";
          for (int i = 0; i < nb_face_; i++)
            fiche_vals << "\t " << centreGrav_(i, 0)
                       << "\t " << centreGrav_(i, 1)
                       << "\t " << centreGrav_(i, 2);

          if (Process::nproc() > 1)
            for (int p = 1; p < Process::nproc(); p++)
              {
                int taille = 0;

                MPI_Recv(&taille, 1, MPI_INT, p, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                std::vector<double> buffer(taille);
                MPI_Recv(buffer.data(), taille, MPI_DOUBLE, p, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int i = 0; i < taille; i++)
                  fiche_vals << "\t " << buffer[i];
              }

          fiche_vals << finl;
        }
    }

  ////// INITIALISATION DES CHAMPS LUS //////
  moyenne_->associer_fr_dis_base(la_frontiere_dis);
  moyenne_->initialiser(tps, inco);
  turbKinEn_->associer_fr_dis_base(la_frontiere_dis);
  turbKinEn_->initialiser(tps, inco);
  turbDissRate_->associer_fr_dis_base(la_frontiere_dis);
  turbDissRate_->initialiser(tps, inco);

  ////// CALCUL DES QUANTITES AUXILIAIRES CONSTANTES EN TEMPS: timeScale_, kappa_center_, amplitude_, ncM_ //////

  // ncM_
  ncM_ = !(moyenne_->valeurs().dimension(0) == 1);                     ////// CALCUL DE ncM_ //////

  // recuperation de la viscosite cinematique
  const Equation_base& equ = ref_inco_.valeur().equation();
  const Milieu_base& mil = equ.milieu();
  const double visc = ref_cast(Fluide_base,mil).viscosite_cinematique().valeurs()(0, 0); //

  // Echelles turbuelence et nombres d'ondes

  timeScale_.resize(nb_face_);
  kappa_center_.resize(nbModes_);

  DoubleVect kappa_node(nbModes_ + 1); // kappa sur les noeuds (bords des segments) de la discretisation
  DoubleVect dkn(nbModes_); // longueurs des segments

  DoubleVect lenghtScale(nb_face_); // echelle de turbulence spatiale
  DoubleVect kappa_eta(nb_face_); // nombre d'onde de Kolmogorov
  DoubleVect kappa_e(nb_face_); // pic d'energie

  //// remplissage des tableaux
  const int ncK = !(turbKinEn_->valeurs().dimension(0) == 1);
  const int ncEPS = !(turbDissRate_->valeurs().dimension(0) == 1);
  double ki, epsi, lScale;
  for (int i = 0; i < nb_face_; i++)
    {
      ki = turbKinEn_->valeurs()(ncK * i);
      epsi = turbDissRate_->valeurs()(ncEPS * i);
      lScale = sqrt(pow(2. * ki / 3., 3)) / epsi;
      //
      lenghtScale(i) = lScale;
      timeScale_(i) = ki / epsi;                                          ////// CALCUL DE timeScale_ //////
      kappa_eta(i) = pow(epsi / pow(visc, 3), 0.25);
      kappa_e(i) = 9 * M_PI * amp_ / (55. * lScale);
    }

  //// kappa min
  double kappa_min = mp_min_vect(kappa_e) / KeOverKmin_; // plus petit nombre d'onde

  //// kappa_delta et kappa_max
  DoubleVect aireFaces; // Attention : contains real + virtual faces
  tabFaces.calculer_surfaces(aireFaces);
  double sum_aire = 0.;

  for (int i = 0; i < nb_face_; i++)
    sum_aire += aireFaces[i];
  sum_aire = mp_sum(sum_aire);
  const double dmin = sqrt(sum_aire / mp_sum(nb_face_)); // const ???
  //
  const double kappa_delta = (M_PI / dmin) * ratioCutoffWavenumber_; // nombre d'onde characteristique de la plus petite echelle du maillage
  const double kappa_max = kappa_delta; // plus grand nombre d'onde
  if (kappa_max <= kappa_min)
    {
      Cerr << "Error: kappa_max(=" << kappa_max << ") <= kappa_min(=" << kappa_min << ")" << finl;
      Cerr << "You should either refine your mesh or increase the ratioCutoffWavenumber_ value in " << que_suis_je() << finl;
      Process::exit();
    }

  //// kappa_node
  const double delta_kappa = pow(kappa_max / kappa_min, 1. / nbModes_); // repartition logarithmique des modes //
  kappa_node(0) = kappa_min;
  for (int n = 1; n < nbModes_ + 1; n++)
    {
      kappa_node(n) = kappa_node(n - 1) * delta_kappa;
    }

  for (int n = 0; n < nbModes_; n++)
    {
      // kappa_center(i) = 0.5*(kappa_node(n+1)+kappa_node(n)); moyenne arithmetique
      kappa_center_(n) = sqrt(kappa_node(n + 1) * kappa_node(n)); // moyenne geometrique, mieux adaptee pour une variation logarithmique de E(kappa)  ////// CALCUL DE kappa_center_ //////
      dkn(n) = kappa_node(n + 1) - kappa_node(n);
    }

  // Amplitudes
  amplitude_.resize(nb_face_, nbModes_);
  for (int i = 0; i < nb_face_; i++)
    {
      for (int n = 0; n < nbModes_; n++)
        {
          amplitude_(i, n) = sqrt(dkn(n) * energy_spectrum_(kappa_center_(n), turbKinEn_->valeurs()(ncK * i), kappa_e(i), kappa_eta(i)));            ////// CALCUL DE amplitude_ //////
        }
    }

  // Pour une raison obscure le mettre_a_jour est necessaire pour pouvoir utiliser un schema en temps explicite.
  // Sinon div(U)=0 n'est meme pas respecte.
  mettre_a_jour(tps);

  return 1;
}

/*! @brief Pas code !!
 *
 * @param (Champ_front_base& ch)
 * @return (Champ_front_base&)
 */
Champ_front_base& Champ_front_synt::affecter_(const Champ_front_base& ch)
{
  return *this;
}

/*! @brief Mise a jour du temps
 *
 */
void Champ_front_synt::mettre_a_jour(double temps)
{

  // Pas de mise à jour des champs utilises (moyenne, turbKinEn ...). Ils doivent être des champs stationnaire.
  // Notez que e.g. moyenne->mettre_a_jour(temps); ne marche pas si moyenne est un champ_front_recyclage stationnaire

  // Acceder au pas de temps depuis l'equation, depuis l'inconnue
  const Equation_base& equ = ref_inco_.valeur().equation();
  double dt = equ.schema_temps().pas_de_temps();

  //Cerr << "We store : temps_d_avant_ = "<<temps_d_avant_<<finl;
  DoubleTab& tab_avant = valeurs_au_temps(temps_d_avant_);
  DoubleTab& tab = valeurs_au_temps(temps);

  // Variables utilisees
  DoubleTab kappa(nbModes_, 3);  // vecteurs d'onde des modes
  DoubleTab sigma(nbModes_, 3);  // directions des modes

  ////////////////////////////////////////////
  /// generation aleatoire des angles      ///
  ////////////////////////////////////////////

  DoubleVect phi(nbModes_);
  DoubleVect tetha(nbModes_);
  DoubleVect alpha(nbModes_);
  DoubleVect psi(nbModes_);

  for (int n = 0; n < nbModes_; n++)
    {
      phi(n) = drand48() * 2 * M_PI;
      tetha(n) = acos(1. - 2 * drand48()); // pour une densite de probabilite de 0.5*sin(theta) ( (phi,theta) isotropique )
      alpha(n) = drand48() * 2 * M_PI;
      psi(n) = drand48() * 2 * M_PI;

      /// creation vecteur onde en coordonnee cartesienne ///
      kappa(n, 0) = kappa_center_(n) * sin(tetha(n)) * cos(phi(n));
      kappa(n, 1) = kappa_center_(n) * sin(tetha(n)) * sin(phi(n));
      kappa(n, 2) = kappa_center_(n) * cos(tetha(n));

      /// creation de la direction orthogonal au vecteur onde ///
      sigma(n, 0) = cos(phi(n)) * cos(tetha(n)) * cos(alpha(n)) - sin(phi(n)) * sin(alpha(n));
      sigma(n, 1) = sin(phi(n)) * cos(tetha(n)) * cos(alpha(n)) + cos(phi(n)) * sin(alpha(n));
      sigma(n, 2) = -sin(tetha(n)) * cos(alpha(n));
    }

  for (int i = 0; i < nb_face_; i++)
    {
      // recuperation moyenne
      DoubleVect moy(3);
      moy(0) = moyenne_->valeurs()(ncM_ * i, 0);
      moy(1) = moyenne_->valeurs()(ncM_ * i, 1);
      moy(2) = moyenne_->valeurs()(ncM_ * i, 2);

      // calcul fluctuation
      DoubleVect turb(3);
      for (int n = 0; n < nbModes_; n++)
        {
          double arg = kappa(n, 0) * centreGrav_(i, 0) + kappa(n, 1) * centreGrav_(i, 1) + kappa(n, 2) * centreGrav_(i, 2) + psi(n);
          double tfunk = cos(arg);
          //
          turb(0) += 2 * amplitude_(i, n) * tfunk * sigma(n, 0);
          turb(1) += 2 * amplitude_(i, n) * tfunk * sigma(n, 1);
          turb(2) += 2 * amplitude_(i, n) * tfunk * sigma(n, 2);
        }

      // calcul autocorrelation temporelle
      double a = exp(-dt / timeScale_(i));
      double b = sqrt(1. - a * a);

      // calcul final vitesse
      tab(i, 0) = moy(0) + dir_fluct_(0) * (a * (tab_avant(i, 0) - moy(0)) + b * turb(0));
      tab(i, 1) = moy(1) + dir_fluct_(1) * (a * (tab_avant(i, 1) - moy(1)) + b * turb(1));
      tab(i, 2) = moy(2) + dir_fluct_(2) * (a * (tab_avant(i, 2) - moy(2)) + b * turb(2));
    }
  tab.echange_espace_virtuel();

  if (bool(ecriture_))
    {
      auto span_vu = tab.get_span();
      if (Process::nproc() > 1 && Process::me()) // pas maitre
        {
          int taille = span_vu.size();
          MPI_Send(&taille, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
          MPI_Send(span_vu.data(), taille, MPI_DOUBLE, 0, 101, MPI_COMM_WORLD);
        }
      else // maitre !
        {
          SFichier fiche_vals(nom_fichier_, ios::app);
          fiche_vals.setf(ios::scientific);
          fiche_vals << temps;

          // on commence par l'info sur proc 0
          for (int i = 0; i < nb_face_; i++)
            fiche_vals << "\t " << tab(i, 0)
                       << "\t " << tab(i, 1)
                       << "\t " << tab(i, 2);

          if (Process::nproc() > 1)
            for (int p = 1; p < Process::nproc(); p++)
              {
                int taille = 0;

                MPI_Recv(&taille, 1, MPI_INT, p, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                std::vector<double> buffer(taille);
                MPI_Recv(buffer.data(), taille, MPI_DOUBLE, p, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int i = 0; i < taille; i++)
                  fiche_vals << "\t " << buffer[i];
              }

          fiche_vals << finl;
        }
    }

  temps_d_avant_ = temps;
}
