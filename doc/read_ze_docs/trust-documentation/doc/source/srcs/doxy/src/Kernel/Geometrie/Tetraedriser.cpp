/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Tetraedriser.h>

Implemente_instanciable(Tetraedriser, "Tetraedriser", Triangulation_base);

Sortie& Tetraedriser::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Tetraedriser::readOn(Entree& is) { return Interprete::readOn(is); }

/*! @brief Fonction hors classe Decoupe toutes les faces d'un objet Faces
 *
 *     dont les faces on 4 sommets
 *     en 2 faces a 3 sommets.
 *
 * @param (Faces& faces) l'ensemble des faces a decouper
 */
static void decoupe(Faces& faces)
{
  IntTab& sommets = faces.les_sommets();
  int nb_faces = sommets.dimension(0);
  assert(sommets.dimension(1) == 4);
  IntTab nouveaux(2 * nb_faces, 3);
  faces.voisins().resize(2 * nb_faces, 2);
  faces.voisins() = -1;
  for (int i = 0; i < nb_faces; i++)
    {
      int i1 = sommets(i, 0);
      int i2 = sommets(i, 1);
      int i3 = sommets(i, 2);
      int i4 = sommets(i, 3);
      nouveaux(i, 0) = i1;
      nouveaux(i, 1) = i2;
      nouveaux(i, 2) = i3;
      nouveaux(nb_faces + i, 0) = i2;
      nouveaux(nb_faces + i, 1) = i3;
      nouveaux(nb_faces + i, 2) = i4;
    }
  sommets.ref(nouveaux);
}

/*! @brief Tetraedrise tous les elements d'un domaine: transforme les elements goemetriques du domaine en tetraedres.
 *
 *     Pour l'instant on ne sait tetraedriser que des Hexaedre.
 *     (on les coupe en 2).
 *     Les elements sont tetraedrises et tous les bords
 *     sont types en Triangle_3D.
 *
 * @param (Domaine& domaine) le domaine dont on veut tetraedriser les elements
 * @throws on ne sait pas tetraedriser les elements
 * geometriques de ce type
 */
void Tetraedriser::trianguler(Domaine& domaine) const
{
  if (domaine.type_elem()->que_suis_je() == "Hexaedre")
    {
      domaine.typer("Tetraedre");
      IntTab& les_elems = domaine.les_elems();
      int oldsz = les_elems.dimension(0);
      IntTab new_elems(6 * oldsz, 4);
      for (int i = 0; i < oldsz; i++)
        {
          int i0 = les_elems(i, 0);
          int i1 = les_elems(i, 1);
          int i2 = les_elems(i, 2);
          int i3 = les_elems(i, 3);
          int i4 = les_elems(i, 4);
          int i5 = les_elems(i, 5);
          int i6 = les_elems(i, 6);
          int i7 = les_elems(i, 7);
          {
            new_elems(i, 0) = i0;
            new_elems(i, 1) = i1;
            new_elems(i, 2) = i2;
            new_elems(i, 3) = i4;

            new_elems(i + oldsz, 0) = i1;
            new_elems(i + oldsz, 1) = i2;
            new_elems(i + oldsz, 2) = i3;
            new_elems(i + oldsz, 3) = i6;
            mettre_a_jour_sous_domaine(domaine, i, i + oldsz, 1);

            new_elems(i + 2 * oldsz, 0) = i1;
            new_elems(i + 2 * oldsz, 1) = i2;
            new_elems(i + 2 * oldsz, 2) = i4;
            new_elems(i + 2 * oldsz, 3) = i6;
            mettre_a_jour_sous_domaine(domaine, i, i + 2 * oldsz, 1);

            new_elems(i + 3 * oldsz, 0) = i3;
            new_elems(i + 3 * oldsz, 1) = i5;
            new_elems(i + 3 * oldsz, 2) = i6;
            new_elems(i + 3 * oldsz, 3) = i7;
            mettre_a_jour_sous_domaine(domaine, i, i + 3 * oldsz, 1);

            new_elems(i + 4 * oldsz, 0) = i1;
            new_elems(i + 4 * oldsz, 1) = i4;
            new_elems(i + 4 * oldsz, 2) = i5;
            new_elems(i + 4 * oldsz, 3) = i6;
            mettre_a_jour_sous_domaine(domaine, i, i + 4 * oldsz, 1);

            new_elems(i + 5 * oldsz, 0) = i1;
            new_elems(i + 5 * oldsz, 1) = i3;
            new_elems(i + 5 * oldsz, 2) = i5;
            new_elems(i + 5 * oldsz, 3) = i6;
            mettre_a_jour_sous_domaine(domaine, i, i + 5 * oldsz, 1);
          }
        }
      les_elems.ref(new_elems);
    }
  else
    {
      Cerr << "We do not yet know how to Tetraedriser the " << domaine.type_elem()->que_suis_je() << "s" << finl;
      Cerr << "Try to use Tetraedriser_homogene_compact instead." << finl;
      Process::exit();
    }

  for (auto &itr : domaine.faces_bord())
    {
      Faces& les_faces = itr.faces();
      les_faces.typer(Faces::triangle_3D);
      decoupe(les_faces);
    }

  for (auto &itr : domaine.faces_raccord())
    {
      Faces& les_faces = itr->faces();
      les_faces.typer(Faces::triangle_3D);
      decoupe(les_faces);
    }

  for (auto &itr : domaine.bords_int())
    {
      Faces& les_faces = itr.faces();
      les_faces.typer(Faces::triangle_3D);
      decoupe(les_faces);
    }

  for (auto &itr : domaine.groupes_faces())
    {
      Faces& les_faces = itr.faces();
      les_faces.typer(Faces::triangle_3D);
      decoupe(les_faces);
    }
}
