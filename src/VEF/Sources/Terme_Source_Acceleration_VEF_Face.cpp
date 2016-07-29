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
// File:        Terme_Source_Acceleration_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Acceleration_VEF_Face.h>
#include <Zone_VEF_PreP1b.h>
#include <Zone_Cl_VEFP1B.h>
#include <Periodique.h>
#include <Navier_Stokes_std.h>
#include <Champ_Fonc_P0_VEF.h>

Implemente_instanciable(Terme_Source_Acceleration_VEF_Face,"Acceleration_VEF_P1NC",Terme_Source_Acceleration);

Sortie& Terme_Source_Acceleration_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

// Description: Appel a Terme_Source_Acceleration::lire_data
Entree& Terme_Source_Acceleration_VEF_Face::readOn(Entree& s )
{
  lire_data(s);
  return s;
}

// Description:
//  Methode appelee par Source_base::completer() apres associer_zones
//  Remplit les ref. aux zones et zone_cl
void Terme_Source_Acceleration_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                                        const Zone_Cl_dis& zone_Cl_dis)
{
  if (je_suis_maitre())
    Cerr << "Terme_Source_Acceleration_VEF_Face::associer_zones" << finl;
  la_zone_VEF_    = ref_cast(Zone_VEF_PreP1b, zone_dis.valeur());
  la_zone_Cl_VEF_ = ref_cast(Zone_Cl_VEFP1B, zone_Cl_dis.valeur());
}

// Description:
//  Fonction outil pour Terme_Source_Acceleration_VEF_Face::ajouter
//  Ajout des contributions d'une liste contigue de faces du terme source de translation:
//   s_face = terme_source * rho
//   resu  += integrale (s_face) sur le volume de controle de la vitesse.
//  On traite les cas suivants:
//    rho = reference nulle (=> rho = 1.)  sinon rho != nul
//    faces de bord => sortie libre
//    periodicite
//    symetrie car en VEF la vitesse sur la face de bord peut ne pas etre nul (V_tangentielle)
//    faces_internes
static void TSAVEF_ajouter_liste_faces(const int premiere_face, const int derniere_face,
                                       const DoubleVect& volumes_entrelaces,
                                       const DoubleVect& volumes_elements,
                                       const DoubleVect& porosite_surf,
                                       const IntTab&      face_voisins,
                                       const REF(Champ_base) & ref_rho,
                                       const DoubleTab&   terme_source,
                                       DoubleTab& s_face,
                                       DoubleTab& resu)
{
  int num_face;
  // Pointeur constant sur tableau constant.
  // Pointeur nul si ref_rho_ est une reference nulle.
  const DoubleTab * const rho_elem =
    (ref_rho.non_nul()) ? &(ref_rho.valeur().valeurs()) : 0;
  const int dim = Objet_U::dimension;

  for (num_face=premiere_face; num_face<derniere_face; num_face++)
    {
      const double vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
      double src[3] = {0., 0., 0.};
      int j;

      for (j = 0; j < dim; j++)
        src[j] = terme_source(num_face, j);

      double rho = 1.;

      // Calcul d'un rho moyen sur le volume de controle de la vitesse
      if (rho_elem)
        {
          const int elem0 = face_voisins(num_face,0);
          const int elem1 = face_voisins(num_face,1);
          double rho0 = 0, rho1 = 0, vol0 = 0, vol1 = 0;
          if (elem0 >= 0)
            {
              rho0 = (*rho_elem)(elem0);
              vol0 = volumes_elements(elem0);
            }
          if (elem1 >= 0)
            {
              rho1 = (*rho_elem)(elem1);
              vol1 = volumes_elements(elem1);
            }
          rho = (rho0 * vol0 + rho1 * vol1) / (vol0 + vol1);
        }

      for (j = 0; j < dim; j++)
        {
          double a = src[j] * rho;
          s_face(num_face, j) = a;
          // Integrale sur le volume de controle :
          resu(num_face, j) += a * vol;
        }
    }
}

// Description:
//  Ajoute le terme (la_source_ * rho * volume_entrelace) au champ resu.
//  On suppose que resu est discretise comme la vitesse.
//  L'espace virtuel de "resu" n'est PAS mis a jour !
//
//  Commentaire sur le schema: L'acceleration d/dt(v) est calculee aux elements,
//   puis multipliee par "rho" aux elements, puis evaluee aux faces par une
//   moyenne sur les elements voisins de la face. C'est un premier essai, pas
//   forcement la meilleure idee. Comme l'acceleration depend de la vitesse qui
//   est aux faces, on passe par deux interpolations successives.
// Effet de bord:
//  On met (la_source_ * rho) dans terme_source_post_
DoubleTab& Terme_Source_Acceleration_VEF_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VF&     zone               = la_zone_VEF_.valeur();
  const Zone_Cl_dis_base& zone_Cl       = la_zone_Cl_VEF_.valeur();
  const IntTab&      face_voisins       = zone.face_voisins();
  const DoubleVect& porosite_surf      = zone.porosite_face();
  const DoubleVect& volumes_entrelaces = zone.volumes_entrelaces();

  DoubleTab& s_face = get_set_terme_source_post().valeur().valeurs();
  s_face = 0.;

  // Calcul de la_source_ en fonction des champs d'acceleration et de la
  // vitesse du fluide.
  const int dim     = Objet_U::dimension;
  const int nb_faces = resu.dimension(0);
  DoubleTab acceleration_aux_faces(nb_faces, dim);
  calculer_la_source(acceleration_aux_faces);

  // Boucle sur les conditions limites pour traiter les faces de bord

  for (int n_bord = 0; n_bord < zone.nb_front_Cl(); n_bord++)
    {
      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet on ne fait rien
      // Si face de Neumann, Periodique ou de Symetrie on calcule la contribution au terme source
      const Cond_lim& la_cl = zone_Cl.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = le_bord.num_premiere_face();
      const int nfin = ndeb + le_bord.nb_faces();

      TSAVEF_ajouter_liste_faces(ndeb, nfin,
                                 volumes_entrelaces,
                                 zone.volumes(),
                                 porosite_surf,
                                 face_voisins,
                                 ref_rho_,
                                 acceleration_aux_faces,
                                 s_face,
                                 resu);

    }
  // Boucle sur les faces internes
  {
    const int ndeb = zone.premiere_face_int();
    const int nfin = zone.nb_faces();
    TSAVEF_ajouter_liste_faces(ndeb, nfin,
                               volumes_entrelaces,
                               zone.volumes(),
                               porosite_surf,
                               face_voisins,
                               ref_rho_,
                               acceleration_aux_faces,
                               s_face,
                               resu);
  }

  {
    // Force la periodicite
    int nb_comp=resu.dimension(1);
    for (int n_bord=0; n_bord<zone.nb_front_Cl(); n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl.les_conditions_limites(n_bord);
        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
            int nb_faces_bord=le_bord.nb_faces();
            ArrOfInt fait(nb_faces_bord);
            fait = 0;
            for (int ind_face=0; ind_face<nb_faces_bord; ind_face++)
              {
                if (fait(ind_face) == 0)
                  {
                    int ind_face_associee = la_cl_perio.face_associee(ind_face);
                    fait(ind_face) = 1;
                    fait(ind_face_associee) = 1;
                    int face = le_bord.num_face(ind_face);
                    int face_associee = le_bord.num_face(ind_face_associee);
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        double val = 0.5*(resu(face_associee, comp)+resu(face, comp));
                        resu(face, comp)=resu(face_associee, comp) = val;
                      }
                  }// if fait
              }// for face
          }// sub_type Perio
      }
  }
  return resu;
}

// Description:
//  Calcul du champ de vitesse trois composantes aux faces a partir
//  du champ de vitesse aux faces de eq_hydraulique_.inconnue().
//  En VEF: rien a faire. On ne se sert pas du stockage fourni en
//  parametre, on renvoie N.S.inconnue()
// Parametre: v_faces_stockage
// Signification: tableau ou stocker le resultat s'il y a des calculs a faire.
//                En vef, on ne s'en sert pas
const DoubleTab& Terme_Source_Acceleration_VEF_Face::calculer_vitesse_faces(
  DoubleTab& v_faces_stockage) const
{
  const Champ_Inc& v_faces = get_eq_hydraulique().inconnue();
  return v_faces.valeur().valeurs();
}

// Description:
//   Associe le champ de masse volumique=>
//   Le terme source calcule sera alors homogene a d/dt(integrale(rho*v)).
// Parametre:     champ_rho
// Signification: un champ de type Champ_Fonc_P0_VEF qui sera utilise
//                lors des appels a "ajouter()" pour evaluer la masse volumique.

void Terme_Source_Acceleration_VEF_Face::associer_champ_rho(const Champ_base& champ_rho)
{
  // Il faut que le champ soit discretise aux elements: possibilite
  // d'autoriser d'autres types si besoin (Champ_Don, Champ_Inc, etc.)
  // du moment que c'est des champs P0.
  if (!sub_type(Champ_Fonc_P0_VEF, champ_rho))
    {
      Cerr << "Erreur dans Terme_Source_Acceleration_VEF_Face::associer_champ_rho" << finl;
      Cerr << " Le champ de masse volumique doit etre de type Champ_Fonc_P0_VEF" << finl;
      Cerr << " Type du champ associe : " << champ_rho.que_suis_je() << finl;
      Cerr << " Nom du champ associe :  " << champ_rho.le_nom() << finl;
      assert(0);
      exit();
    }
  ref_rho_ = champ_rho;
}

