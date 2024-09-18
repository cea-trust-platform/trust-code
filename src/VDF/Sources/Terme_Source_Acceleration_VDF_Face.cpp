/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Terme_Source_Acceleration_VDF_Face.h>
#include <Navier_Stokes_std.h>
#include <Champ_Fonc_P0_VDF.h>
#include <Milieu_base.h>

#include <Domaine_Cl_VDF.h>
#include <Periodique.h>
#include <Domaine_VDF.h>

Implemente_instanciable(Terme_Source_Acceleration_VDF_Face,"Acceleration_VDF_Face",Terme_Source_Acceleration);

Sortie& Terme_Source_Acceleration_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

/*! @brief Appel a Terme_Source_Acceleration::lire_data
 *
 */
Entree& Terme_Source_Acceleration_VDF_Face::readOn(Entree& s )
{
  lire_data(s);
  return s;
}

/*! @brief Methode appelee par Source_base::completer() apres associer_domaines Remplit les ref.
 *
 * aux domaines et domaine_cl
 *
 */
void Terme_Source_Acceleration_VDF_Face::associer_domaines(const Domaine_dis_base& domaine_dis,
                                                           const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  if (je_suis_maitre())
    Cerr << "Terme_Source_Acceleration_VDF_Face::associer_domaines" << finl;
  le_dom_VDF_    = ref_cast(Domaine_VDF, domaine_dis);
  le_dom_Cl_VDF_ = ref_cast(Domaine_Cl_VDF, domaine_Cl_dis);
}

/*! @brief Fonction outil pour Terme_Source_Acceleration_VDF_Face::ajouter Ajout des contributions d'une liste contigue de faces du terme source de translation:
 *
 *    s_face = terme_source * rho
 *    resu  += integrale (s_face) sur le volume de controle de la vitesse.
 *   On traite les cas suivants:
 *     rho = reference nulle (=> rho = 1.)  sinon rho != nul
 *     faces de bord => sortie libre
 *     periodicite
 *     faces_internes
 *
 */
static void TSAVDF_ajouter_liste_faces(const int premiere_face, const int derniere_face,
                                       const DoubleVect& volumes_entrelaces,
                                       const DoubleVect& volumes_elements,
                                       const DoubleVect& porosite_surf,
                                       const IntVect&     orientation,
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
    (ref_rho.non_nul()) ? &(ref_rho->valeurs()) : 0;

  for (num_face=premiere_face; num_face<derniere_face; num_face++)
    {
      const double vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
      const int ncomp = orientation(num_face);
      const double src = terme_source(num_face, ncomp);

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

      double a = src * rho;
      s_face(num_face) = a;
      // Integrale sur le volume de controle :
      resu(num_face) += a * vol;
    }
}

/*! @brief Ajoute le terme (la_source_ * rho * volume_entrelace) au champ resu.
 *
 * On suppose que resu est discretise comme la vitesse en VDF.
 *  Effet de bord:
 *   On met (la_source_ * rho) dans terme_source_post_
 *
 */
void Terme_Source_Acceleration_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VDF&    domaine_VDF           = le_dom_VDF_.valeur();
  const Domaine_Cl_VDF& domaine_Cl_VDF        = le_dom_Cl_VDF_.valeur();
  const IntTab&      face_voisins       = domaine_VDF.face_voisins();
  const IntVect&     orientation        = domaine_VDF.orientation();
  const DoubleVect& porosite_surf      = equation().milieu().porosite_face();
  const DoubleVect& volumes_entrelaces = domaine_VDF.volumes_entrelaces();

  DoubleTab& s_face = get_set_terme_source_post()->valeurs();
  s_face = 0.;

  // Calcul de la_source_ en fonction des champs d'acceleration et de la
  // vitesse du fluide.
  const int dim     = Objet_U::dimension;
  const int nb_faces = secmem.dimension(0);
  DoubleTab acceleration_aux_faces(nb_faces, dim);
  calculer_la_source(acceleration_aux_faces);

  // Boucle sur les conditions limites pour traiter les faces de bord

  for (int n_bord=0; n_bord<domaine_VDF.nb_front_Cl(); n_bord++)
    {
      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source
      const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      const int ndeb = le_bord.num_premiere_face();
      const int nfin = ndeb + le_bord.nb_faces();
      TSAVDF_ajouter_liste_faces(ndeb, nfin,
                                 volumes_entrelaces,
                                 le_dom_VDF_->volumes(),
                                 porosite_surf,
                                 orientation,
                                 face_voisins,
                                 ref_rho_,
                                 acceleration_aux_faces,
                                 s_face,
                                 secmem);
    }


  // Boucle sur les faces internes
  {
    const int ndeb = domaine_VDF.premiere_face_int();
    const int nfin = domaine_VDF.nb_faces();
    TSAVDF_ajouter_liste_faces(ndeb, nfin,
                               volumes_entrelaces,
                               le_dom_VDF_->volumes(),
                               porosite_surf,
                               orientation,
                               face_voisins,
                               ref_rho_,
                               acceleration_aux_faces,
                               s_face,
                               secmem);
  }
  {
    // Force la periodicite
    for (int n_bord=0; n_bord<domaine_VDF.nb_front_Cl(); n_bord++)
      {
        const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);
        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
            int nb_faces_bord=le_bord.nb_faces();
            ArrOfInt fait(nb_faces_bord);
            fait = 0;
            for (int ind_face=0; ind_face<nb_faces_bord; ind_face++)
              {
                if (fait[ind_face] == 0)
                  {
                    int ind_face_associee = la_cl_perio.face_associee(ind_face);
                    fait[ind_face] = 1;
                    fait[ind_face_associee] = 1;
                    int face = le_bord.num_face(ind_face);
                    int face_associee = le_bord.num_face(ind_face_associee);
                    double val = 0.5*(secmem(face_associee)+secmem(face));
                    secmem(face)=secmem(face_associee) = val;
                  }// if fait
              }// for face
          }// sub_type Perio
      }
  }
}

/*! @brief Calcul des trois composantes du champ de vitesse fluide au centre de chaque face.
 *
 * Le resultat est stocke dans v_faces_stockage et on renvoie
 *   une reference au tableau. Pas d'espace virtuel dans le tableau.
 *   La composante normale a la face est deja connue: c'est la valeur discrete.
 *   Les autres composantes sont calculees en faisant la moyenne des vitesse
 *   des faces des elements voisins qui ont la bonne orientation, ponderee
 *   par le volume de l'element.
 *   Traitement des bords : on prend la vitesse de l'element voisin (a priori
 *    pas bon pour les conditions aux limites de vitesse imposee au bord,
 *    mais le vpoint est corrige ensuite pour imposer la vitesse).
 *
 */
const DoubleTab& Terme_Source_Acceleration_VDF_Face::calculer_vitesse_faces(
  DoubleTab& v_faces_stockage) const
{
  const Domaine_VDF&    domaine_VDF      = le_dom_VDF_.valeur();
  const IntVect&     orientation   = domaine_VDF.orientation();
  const IntTab&      faces_voisins = domaine_VDF.face_voisins();
  const DoubleVect& volumes       = domaine_VDF.volumes();  // volumes des elements
  const IntTab&      elem_faces    = domaine_VDF.elem_faces();
  const DoubleTab&   v_faces  = get_eq_hydraulique().inconnue().valeurs();
  const int       dim      = Objet_U::dimension;
  const int       nb_faces = v_faces.dimension(0);
  v_faces_stockage.resize(nb_faces, dim);
  int i_face;
  ArrOfDouble composante_vitesse(3);
  for (i_face = 0; i_face < nb_faces; i_face++)
    {
      const int orientation_face = orientation(i_face);
      composante_vitesse=0;
      int composante;

      // Numeros des deux elements voisins de la face (-1 si face de bord)
      int elem[2];
      elem[0] = faces_voisins(i_face, 0);
      elem[1] = faces_voisins(i_face, 1);

      // Volumes de ces deux elements (0. si pas de voisin)
      double volume_elem[2] = {0., 0.};
      if (elem[0] >= 0)
        volume_elem[0] = volumes(elem[0]);
      if (elem[1] >= 0)
        volume_elem[1] = volumes(elem[1]);

      const double i_volume_total = 1. / (volume_elem[0] + volume_elem[1]);

      for (composante = 0; composante < dim; composante++)
        {
          if (composante == orientation_face)
            {
              composante_vitesse[composante] = v_faces(i_face);
            }
          else
            {
              // Calcul de la moyenne des vitesses sur les faces voisines
              // qui ont la bonne orientation:
              composante_vitesse[composante] = 0.;
              int i_elem;
              for (i_elem = 0; i_elem < 2; i_elem++)
                {
                  if (elem[i_elem] >= 0)
                    {
                      const int element = elem[i_elem];
                      const int face1 = elem_faces(element, composante);
                      const int face2 = elem_faces(element, composante + dim);
                      const double v1 = v_faces(face1);
                      const double v2 = v_faces(face2);
                      const double p = volume_elem[i_elem]; // Ponderation par le volume
                      composante_vitesse[composante] += (v1 + v2) * 0.5 * p;
                    }
                }
              composante_vitesse[composante] *= i_volume_total;
            }
        }
      for (composante = 0; composante < dim; composante++)
        {
          v_faces_stockage(i_face, composante) = composante_vitesse[composante];
        }
    }
  return v_faces_stockage;
}

/*! @brief Associe le champ de masse volumique=> Le terme source calcule sera alors homogene a d/dt(integrale(rho*v)).
 *
 * @param (champ_rho) un champ de type Champ_Fonc_P0_VDF qui sera utilise lors des appels a "ajouter()" pour evaluer la masse volumique.
 */

void Terme_Source_Acceleration_VDF_Face::associer_champ_rho(const Champ_base& champ_rho)
{
  // Il faut que le champ soit discretise aux elements: possibilite
  // d'autoriser d'autres types si besoin (Champ_Don, Champ_Inc, etc.)
  // du moment que c'est des champs P0.
  if (!sub_type(Champ_Fonc_P0_VDF, champ_rho))
    {
      Cerr << "Erreur dans Terme_Source_Acceleration_VDF_Face::associer_champ_rho" << finl;
      Cerr << " Le champ de masse volumique doit etre de type Champ_Fonc_P0_VDF" << finl;
      Cerr << " Type du champ associe : " << champ_rho.que_suis_je() << finl;
      Cerr << " Nom du champ associe :  " << champ_rho.le_nom() << finl;
      assert(0);
      exit();
    }
  ref_rho_ = champ_rho;
}

