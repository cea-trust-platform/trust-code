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
// File:        Terme_Source_Acceleration.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Acceleration.h>
#include <Zone_dis.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Discretisation_base.h>
#include <Zone_VF.h>
#include <Schema_Temps_base.h>

Implemente_base_sans_constructeur(Terme_Source_Acceleration,"Terme_Source_Acceleration",Source_base);

Terme_Source_Acceleration::Terme_Source_Acceleration()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(1);
    nom[0]="ACCELERATION_TERME_SOURCE";
  */
}

Sortie& Terme_Source_Acceleration::printOn(Sortie& s ) const
{
  Cerr << "Terme_Source_Acceleration::printOn : appel invalide" << finl;
  assert(0);
  exit();
  return s;
}

// Description: Appel a Terme_Source_Acceleration::lire_data
Entree& Terme_Source_Acceleration::readOn(Entree& s )
{
  Cerr << "Terme_Source_Acceleration::readOn : appel invalide" << finl;
  assert(0);
  exit();
  return s;
}

// Description:
// Methode appelee par readOn des classes derivees
//  Terme_Source_Acceleration_VDF_Face, ...
//
// Le format attendu est le suivant
//  {
//    [ omega           Un_Champ_uniforme
//      domegadt        Un_Champ_uniforme
//      centre_rotation Un_Champ_uniforme
//      option          TERME_COMPLET | CORIOLIS_SEUL | ENTRAINEMENT_SEUL ]
//    [ acceleration    Un_Champ_uniforme ]
//  }
// Un_Champ_uniforme est typiquement:
//      Champ_Uniforme 3 x y z
//    ou
//      Champ_Fonc_t DOM 3 fx(t) fy(t) fz(t)

void Terme_Source_Acceleration::lire_data(Entree& s)
{
  if (je_suis_maitre())
    Cerr << "Terme_Source_Acceleration::lire_data" << finl;

  // Valeurs par defaut:
  option_ = TERME_COMPLET;

  Motcles les_mots(7);
  les_mots[0] = "omega";
  les_mots[1] = "domegadt";
  les_mots[2] = "centre_rotation";
  les_mots[3] = "option";
  les_mots[4] = "acceleration";
  les_mots[5] = "}";
  les_mots[6] = "vitesse";

  Motcle motlu;
  s >> motlu;
  if (motlu != "{")
    {
      Cerr << "Erreur dans Terme_Source_Acceleration::lire_data" << finl;
      Cerr << " On attendait {" << finl;
      assert(0);
      exit();
    }
  int fini = 0;
  while (!fini)
    {
      s >> motlu;
      const int i_mot = les_mots.search(motlu);
      if (je_suis_maitre() && i_mot < 5)
        Cerr << "Lecture de " << motlu << " : ";
      switch(i_mot)
        {
        case 0:
          s >> omega_;
          if (je_suis_maitre())
            Cerr << omega_.que_suis_je() << finl;
          break;
        case 1:
          s >> domegadt_;
          if (je_suis_maitre())
            Cerr << domegadt_.que_suis_je() << finl;
          break;
        case 2:
          s >> centre_rotation_;
          if (je_suis_maitre())
            Cerr << centre_rotation_.que_suis_je() << finl;
          break;
        case 3:
          {
            Motcles les_options(3);
            les_options[0] = "terme_complet";
            les_options[1] = "coriolis_seul";
            les_options[2] = "entrainement_seul";
            s >> motlu;
            const int i = les_options.search(motlu);
            switch (i)
              {
              case 0:
                option_ = TERME_COMPLET;
                break;
              case 1:
                option_ = CORIOLIS_SEUL;
                break;
              case 2:
                option_ = ENTRAINEMENT_SEUL;
                break;
              default:
                Cerr << "Erreur, les options valides sont : " << finl;
                Cerr << les_options << finl;
                assert(0);
                exit();
              }
            if (je_suis_maitre())
              Cerr << motlu << finl;
            break;
          }
        case 4:
          s >> champ_acceleration_;
          if (je_suis_maitre())
            Cerr << champ_acceleration_.que_suis_je() << finl;
          break;
        case 5:
          fini = 1;
          break;
        case 6:
          s >> champ_vitesse_;
          if (je_suis_maitre())
            Cerr << champ_vitesse_.que_suis_je() << finl;
          break;
        default:
          Cerr << "Erreur dans Terme_Source_Acceleration::lire_data" << finl;
          Cerr << " On ne comprend pas le mot " << motlu << finl;
          Cerr << " Les mots compris sont : " << les_mots << finl;
          assert(0);
          exit();
        }
    }

  // On verifie que c'est coherent:
  int n = 0;
  if (omega_.non_nul()) n++;
  if (domegadt_.non_nul()) n++;
  if (centre_rotation_.non_nul()) n++;
  if (n != 0 && n != 3)
    {
      Cerr << "Erreur dans Terme_Source_Acceleration::lire_data" << finl;
      Cerr << " Si OMEGA ou DOMEGADT ou CENTRE_GRAVITE est donne" << finl;
      Cerr << " alors les trois doivent etre donnes" << finl;
      assert(0);
      exit();
    }
  if (n > 0)
    {
      if (omega_.valeur().valeurs().dimension(0) != 1
          || domegadt_.valeur().valeurs().dimension(0) != 1
          || omega_.valeur().valeurs().dimension(1) != 3
          || domegadt_.valeur().valeurs().dimension(1) != 3)
        {
          Cerr << "Erreur dans Terme_Source_Acceleration::lire_data" << finl;
          Cerr << " les champs OMEGA et DOMEGADT doivent etre des champs" << finl;
          Cerr << " uniformes a trois composantes (vecteur aligne sur Z en 2D)" << finl;
          assert(0);
          exit();
        }
    }
  if (champ_acceleration_.non_nul())
    {
      if (champ_acceleration_.valeur().valeurs().dimension(0) != 1)
        {
          Cerr << "Erreur dans Terme_Source_Acceleration::lire_data" << finl;
          Cerr << " Le champ ACCELERATION doit etre un champ uniforme" << finl;
          assert(0);
          exit();
        }
    }

  // Discretisation de la_source et terme_source_post
  // Il faut absolument le faire ici pour disposer des champs au moment de
  // la lecture des postraitements (appel a a_pour_Champ_Fonc)
  {
    const Equation_base& eqn = equation();
    if (!sub_type(Navier_Stokes_std, eqn))
      {
        Cerr << "Erreur dans Terme_Source_Acceleration::lire_data\n"
             << " Ce terme source ne peut etre insere que dans une equation\n"
             << " derivee de Navier_Stokes_std" << finl;
        assert(0);
        exit();
      }
    const Discretisation_base& dis   = eqn.discretisation();
    const Zone_dis_base&        zone  = eqn.zone_dis().valeur();
    const double                temps = eqn.schema_temps().temps_courant();
    dis.discretiser_champ("vitesse", zone, "acceleration_terme_source", "kg/ms^2",
                          Objet_U::dimension, /* composantes */
                          temps,
                          get_set_terme_source_post());
    champs_compris_.ajoute_champ(terme_source_post_);
  }
}

void Terme_Source_Acceleration::associer_pb(const Probleme_base& pb)
{
}

const Champ_Fonc_base& Terme_Source_Acceleration::get_terme_source_post() const
{
  return terme_source_post_.valeur();
}

Champ_Fonc& Terme_Source_Acceleration::get_set_terme_source_post() const
{
  // terme_source_post_ est mutable, on peut donc le renvoyer "non const"
  return terme_source_post_;
}

// Description:
//  Calcul de la valeur du champ la_source aux faces en fonction de
//  - calculer_vitesse_faces()
//  - champ_acceleration_
//  - omega_
//  - domegadt_
//  - centre_rotation_
// Parametre: champ_source
// Signification: un champ discretise aux elements de la zone_VF dans lequel
//                on stocke le resultat du calcul. Espace virtuel a jour.
const DoubleTab&
Terme_Source_Acceleration::calculer_la_source(DoubleTab& acceleration_aux_faces) const
{
  const Zone_VF&    zone_VF = ref_cast(Zone_VF, equation().zone_dis().valeur());;

  const DoubleTab& centre_faces = zone_VF.xv();

  double a[3] = {0., 0., 0.};  // champ acceleration
  double w[3] = {0., 0., 0.};  // omega
  double dw[3] = {0., 0., 0.}; // domegadt
  double c[3] = {0., 0., 0.};  // centre_gravite
  const int dim = Objet_U::dimension;

  // ****************************************************************
  // INITIALISATION DE a, w, dw, c
  int rotation_solide = 0;
  {
    int j;

    if (champ_acceleration_.non_nul())
      {
        const DoubleTab& a_ = champ_acceleration_.valeur().valeurs();
        for (j = 0; j < dim; j++)
          a[j] = a_(0, j);
      }

    if (omega_.non_nul())
      {
        // L'utilisateur a specifie un mouvement de rotation solide
        const DoubleTab& champ_w  = omega_          .valeur().valeurs();
        const DoubleTab& champ_dw = domegadt_       .valeur().valeurs();
        const DoubleTab& champ_c  = centre_rotation_.valeur().valeurs();
        // Attention: en 2D, le vecteur rotation est typiquement oriente
        // dans la direction Z : donc boucle jusqu'a 3 dans tous les cas:
        for (j = 0; j < 3; j++) w[j]  = champ_w(0,j);
        for (j = 0; j < 3; j++) dw[j] = champ_dw(0,j);
        for (j = 0; j < dim; j++) c[j]  = champ_c(0,j);
        rotation_solide = 1;
      }
  }

  // Si on a un terme source de rotation solide, on a besoin des trois
  // composantes de la vitesse du fluide aux faces:

  // Un espace de stockage pour la vitesse si elle doit etre
  // calculee (en VDF, on calcule les trois composantes a chaque face)
  DoubleTab vitesse_faces_stockage;
  // Calcul du champ de vitesse aux faces (a partir de l'inconnue
  // de l'equation de N.S.)
  const DoubleTab& vitesse_faces =
    (rotation_solide)
    ? calculer_vitesse_faces(vitesse_faces_stockage)
    : vitesse_faces_stockage;

  // **************************************************
  // BOUCLE SUR LES FACES
  int i_face;
  const int nb_faces = acceleration_aux_faces.dimension(0);

  // Vecteur vitesse du fluide dans l'element courant
  //  (attention: initialisation de la composante Z pour le 2D)
  double v[3] = {0., 0., 0.};
  // Vecteur (centre_face - centre_rotation)
  //  (idem)
  double xgr[3] = {0., 0., 0.};

  for (i_face = 0; i_face < nb_faces; i_face++)
    {

      int j;
      double src[3];
      src[0] = -a[0]; // Terme d'acceleration
      src[1] = -a[1];
      src[2] = -a[2]; // =0. en 2D

      // Formules identiques en 2D et en 3D :
      //  en 2D, les composantes z de v et xgr sont nulles.
      if (rotation_solide)
        {
          for (j = 0; j < dim; j++) // en 2D, v[3]=0.
            v[j] = vitesse_faces(i_face, j);

          for (j = 0; j < dim; j++) // en 2D, xgr[3]=0.
            xgr[j] = centre_faces(i_face, j) - c[j];

          if (option_ != ENTRAINEMENT_SEUL)
            {
              src[0] += -2. * (w[1] * v[2] - w[2] * v[1]);
              src[1] += -2. * (w[2] * v[0] - w[0] * v[2]);
              src[2] += -2. * (w[0] * v[1] - w[1] * v[0]);
              src[0] += dw[2] * xgr[1] - dw[1] * xgr[2];
              src[1] += dw[0] * xgr[2] - dw[2] * xgr[0];
              src[2] += dw[1] * xgr[0] - dw[0] * xgr[1];
            }
          if (option_ != CORIOLIS_SEUL)
            {
              src[0] += w[2]*w[2]*xgr[0] - w[0]*w[2]*xgr[2] - w[0]*w[1]*xgr[1] + w[1]*w[1]*xgr[0];
              src[1] += w[0]*w[0]*xgr[1] - w[0]*w[1]*xgr[0] - w[1]*w[2]*xgr[2] + w[2]*w[2]*xgr[1];
              src[2] += w[1]*w[1]*xgr[2] - w[1]*w[2]*xgr[1] - w[2]*w[0]*xgr[0] + w[0]*w[0]*xgr[2];
            }
        }
      for (j = 0; j < dim; j++)
        acceleration_aux_faces(i_face, j) = src[j];
    }
  return acceleration_aux_faces;
}

// Description:
//   resu=0; ajouter(resu); (appel a ajouter() de la classe derivee)
DoubleTab& Terme_Source_Acceleration::calculer(DoubleTab& resu) const
{
  resu = 0;
  ajouter(resu);
  resu.echange_espace_virtuel();
  return resu;
}

// Description:
//  Evalue les champs d'acceleration et de rotation au temps t.
void Terme_Source_Acceleration::mettre_a_jour(double temps)
{
  //Cerr << "Terme_Source_Acceleration::mettre_a_jour temps=" << temps << finl;
  if (champ_acceleration_.non_nul())
    champ_acceleration_.mettre_a_jour(temps);
  if (omega_.non_nul())
    {
      omega_.mettre_a_jour(temps);
      domegadt_.mettre_a_jour(temps);
      centre_rotation_.mettre_a_jour(temps);
    }
  get_set_terme_source_post().mettre_a_jour(temps);
}

// Description:
//  Methode surchargee de Source_base.
//  Les mots compris sont:
//  "ACCELERATION" => renvoie terme_source_post_, homogene a d/dt(rho*v)
//  Attention a l'effet de bord de ajouter() (voir commentaires de ajouter())
int Terme_Source_Acceleration::a_pour_Champ_Fonc(const Motcle& mot,
                                                 REF(Champ_base) & ch_ref) const
{
  int ok = 0;
  if (mot == "ACCELERATION_TERME_SOURCE")
    {
      ch_ref = terme_source_post_.valeur();
      ok = 1;
    }
  return ok;
}

// Description: Renvoie eq_hydraulique_ !
const Navier_Stokes_std& Terme_Source_Acceleration::get_eq_hydraulique() const
{
  const Navier_Stokes_std& ns = ref_cast(Navier_Stokes_std, equation());
  return ns;
}
