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

#include <Op_Curl_VEFP1B.h>
#include <Nom.h>
#include <TRUSTTab.h>

#include <Zone_Cl_VEF.h>

#include <Zone_dis.h>
#include <Zone_Cl_dis.h>
#include <Zone_VEF_PreP1b.h>

Implemente_instanciable(Op_Curl_VEFP1B,"Op_Curl_VEFPreP1B_P1NC",Operateur_base);

// printOn et readOn
Sortie& Op_Curl_VEFP1B::printOn(Sortie& s) const
{
  return s;
}
Entree& Op_Curl_VEFP1B::readOn(Entree& is)
{
  return is;
}


const Zone_VEF_PreP1b& Op_Curl_VEFP1B::zone_Vef() const
{
  return ref_cast(Zone_VEF_PreP1b, la_zone_vef.valeur());
}

/////////////////////////////////////
// Fonctions locales a ce fichier
/////////////////////////////////////

inline void add_curl_som(int nps, int sommet, int face, double flux,
                         DoubleTab& curl, const Domaine& domaine)
{
  curl(nps+domaine.get_renum_som_perio(sommet)) += flux;
}

inline void traiter_flux(DoubleTab& curl, double flux, int element1,
                         int element2, int npe)
{
  curl(npe+element1)+=flux;
  curl(npe+element2)-=flux;
}
///////////////////////////////////////
//
//////////////////////////////////////

void Op_Curl_VEFP1B::associer(const Zone_dis& zone_dis,
                              const Zone_Cl_dis& zone_Cl_dis,
                              const Champ_Inc& inco)
{
  const Zone_VEF& zvef = ref_cast(Zone_VEF, zone_dis.valeur());
  const Zone_Cl_VEF& zclvef = ref_cast(Zone_Cl_VEF, zone_Cl_dis.valeur());
  la_zone_vef = zvef;
  la_zcl_vef = zclvef;

  elements_pour_sommet();
}

DoubleTab& Op_Curl_VEFP1B::calculer(const DoubleTab& vitesse, DoubleTab& curl) const
{
  curl = 0;
  return ajouter(vitesse,curl);
}

DoubleTab& Op_Curl_VEFP1B::ajouter(const DoubleTab& vitesse,
                                   DoubleTab& curl) const
{
  Cerr << "Je suis dans operateur curl" << finl;

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone& zone = zone_VEF.zone();
  //int prems=zone_VEF.premiere_face_int();
  if ( dimension != 2 )
    {
      Cerr << "Pour l'instant seule la 2D est etudiee. " << finl;
      Process::exit();
    }

  int face0=0,face1=0,face2=0;
  int numero_triangle=0;
  int face_globale=0,face_opp=0;

  DoubleTab vecteur_normal0(dimension);
  DoubleTab vecteur_normal1(dimension);
  DoubleTab vecteur_normal2(dimension);

  // On traite les faces internes i.e. sans conditions aux limites
  // ATTENTION: la base de la vorticite c'est: l'ensemble des
  // fonctions indicatrices des elements + l'ensemble des fonctions
  // chapeaux du P1 moins la derniere de ces fonctions de forme

  //Partie P0 de la vorticite
  for (int numero_elem = 0; numero_elem < zone.nb_elem(); numero_elem++)
    {

      //REM: on peut generaliser cette partie a la 3D en
      //faisant une boucle avec zone.nb_faces_element()

      //Il nous faut d'abord les numeros des 3 faces
      //qui appartiennent a cet element K
      face0 = zone_VEF.elem_faces(numero_elem,0);
      face1 = zone_VEF.elem_faces(numero_elem,1);
      face2 = zone_VEF.elem_faces(numero_elem,2);

      //Ensuite, il nous faut les vecteurs tangents de
      //ces trois faces.
      vecteur_normal0 = vecteur_normal(face0,numero_elem);
      vecteur_normal1 = vecteur_normal(face1,numero_elem);
      vecteur_normal2 = vecteur_normal(face2,numero_elem);

      int modulo;
      for (int composante=0; composante<dimension; composante++)
        {
          //La partie P0 a ete teste avec les fonctions (1,0);
          //(0,1);(x,0) et (0,x).
          //Tous les resultats sont corrects

          modulo = (composante+1)%2;
          curl(numero_elem) +=  pow(-1.,modulo) * (
                                  vitesse(face0,composante)*vecteur_normal0(modulo)+
                                  vitesse(face1,composante)*vecteur_normal1(modulo)+
                                  vitesse(face2,composante)*vecteur_normal2(modulo) );

        }

      Cerr << "Element curl(" << numero_elem << ") " << curl(numero_elem) << finl;
    }

  //Partie P1 de la vorticite

  for (int numero_som = 0; numero_som < zone.nb_som()-1; numero_som++)
    {
      for (int num_loc_elem = 0; num_loc_elem < elem_som_size(numero_som);
           num_loc_elem++)

        {
          // for num_loc_elem

          //On recupere le numero global du triangle
          numero_triangle = elements_pour_sommet(numero_som,num_loc_elem);

          //On recupere le numero global de la face opposee a "numero_som"
          //dans le triangle "numero_triangle"
          face_opp=zone_VEF.numero_sommet_local(numero_som,numero_triangle);
          face_opp=zone_VEF.elem_faces(numero_triangle,face_opp);

          //On recupere le vecteur normal de cette face opposee.
          vecteur_normal1=vecteur_normal(face_opp,numero_triangle);

          for (int num_loc_face = 0 ; num_loc_face < zone.nb_faces_elem();
               num_loc_face++)

            {
              // for num_loc_face

              //On recupere le numero global de la face "num_loc_face"
              face_globale=zone_VEF.elem_faces(numero_triangle,num_loc_face);

              //               //Si "face_globale" est une arete interne alors on effectue le bon
              //               //traitement
              //               if (face_globale >= zone_VEF.premiere_face_int() )
              {
                //On calcule les vecteurs normaux associes a ces faces.
                vecteur_normal0=vecteur_normal(face_globale,numero_triangle);

                //Enfin on calcule la contribution au curl de chacune de
                //ces faces pour chacun des 2 triangles.
                int modulo;
                for (int composante=0; composante<dimension; composante++)
                  {
                    //Partie P1 teste avec les fonctions (1,0);(0,1)
                    //(x,0) et (0,x).
                    //Tous les tests sont corrects

                    modulo = (composante+1)%2;

                    //Partie (lambda_s,curl u)
                    curl(zone.nb_elem()+numero_som) += -pow(-1.,modulo) *
                                                       1./(dimension+1) *
                                                       vitesse(face_globale,composante) *
                                                       vecteur_normal0(modulo);

                    //Partie (rot lambda_s, u)
                    curl(zone.nb_elem()+numero_som) += pow(-1.,modulo)*
                                                       1./(dimension*(dimension+1))*
                                                       vitesse(face_globale,composante)*
                                                       vecteur_normal1(modulo);
                  }

              } // fin du if

            }// fin du for num_loc_face

          /* Pour le moment, on ne travaille que sur des vitesse H10 */
          /* C'est le travail de these */
          /* Par consequent, inutile de traiter les faces du bord */

        }// fin du for num_loc_elem

      Cerr << "Sommet curl(" << numero_som << ") " << curl(zone.nb_elem()+numero_som) << finl;

    }// fin du for sur les sommets

  Cerr << "je sors de OpCurl" << finl;

  return curl;
}

DoubleTab
Op_Curl_VEFP1B::vecteur_normal(const int face, const int elem) const
{
  assert(dimension == 2);

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  DoubleTab le_vecteur_normal(dimension);

  for (int composante = 0; composante<dimension; composante++)

    le_vecteur_normal(composante) = zone_VEF.face_normales(face,composante)
                                    * zone_VEF.oriente_normale(face,elem);

  return le_vecteur_normal;
}


// Tableau qui stocke a la place "i", tous les elements
// du maillage qui contiennent le sommet de numero global "i"
int Op_Curl_VEFP1B::elements_pour_sommet()
{
  const Zone& zone = la_zone_vef.valeur().zone();
  int numero_global_som;
  elements_pour_sommet_.dimensionner(zone.nb_som());

  for (int numero_elem = 0; numero_elem < zone.nb_elem(); numero_elem++)
    for (int numero_som_loc=0; numero_som_loc < zone.nb_som_elem(); numero_som_loc++)
      {
        numero_global_som = zone.sommet_elem(numero_elem,numero_som_loc);
        elements_pour_sommet_[numero_global_som].add_if_not(numero_elem);
      }

  return 1;
}

// Fonction qui renvoie le numero global de l'element qui contient "sommet"
// et qui est situe a la place "indice" de la liste "elements_pour_sommet_"
int Op_Curl_VEFP1B::elements_pour_sommet(const int sommet,const int indice) const
{
  return elements_pour_sommet_[sommet][indice];
}

// Fonction qui renvoie la taille de la liste situe a l'emplacement "sommet"
// du tableau "elements_pour_sommet_"
int Op_Curl_VEFP1B::elem_som_size(const int sommet) const
{
  return elements_pour_sommet_[sommet].size();
}
