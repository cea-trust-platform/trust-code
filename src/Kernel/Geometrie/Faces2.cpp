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
#include <Faces2.h>

/*! @brief Fonction hors classe Renvoie le numero du plus petit (au sens de la
 *
 *     numerotation des sommets) sommet d'une face
 *
 * @param (Faces& faces) les faces
 * @param (int face) la face dont on cherche le plus petit sommet
 * @param (int nb_som) le nombre de sommet par face
 * @return (int) le numero du plus petit sommet la face specifiee
 */
int ppsf(const Faces& faces, int face, int nb_som)
{
  int sommet=faces.sommet(face,0);
  for(int imin=1; imin < nb_som; imin++)
    sommet=std::min(sommet,faces.sommet(face,imin));
  return sommet;
}


/*! @brief Fonction hors classe Compare 2 faces de 2 ensembles de faces f1 et f2
 *
 *     et Renvoie 1 si elles sont egales, 0 Sinon.
 *
 * @param (Faces& faces1) le premier ensemble de face
 * @param (int f1) l'indice de la face dans le premier ensemble de face
 * @param (Faces& faces2) le deuxieme ensemble de face
 * @param (int f2) l'indice de la face dans le deuxieme ensemble de face
 * @param (int nb_som) le nombre de sommet par face
 * @return (int) 1 si les 2 faces sont les memes 0 sinon
 */
int same(const Faces& faces1, int f1,
         const Faces& faces2, int f2,
         int nb_som)
{
  int ok=1;
  for(int i=0; ((i<nb_som) && ok ); i++)
    {
      int nok=1;
      for(int j=0; ((j<nb_som) && nok ); j++)
        {
          if( faces1.sommet(f1, i) == faces2.sommet(f2, j))
            nok=0;           // memes sommets
        }
      if(nok) ok=0;          // sommet pas trouve ==> pas meme face
    }
  return ok;
}


/*! @brief Fonction hors classe
 *
 * @param (Faces& faces)
 * @param (int& premier)
 * @param (Faces& les_faces)
 * @param (IntLists& listes)
 * @param (int nb_som_face)
 */
void ajouter(Faces& faces, int& premier,
             const Faces& les_faces,
             IntLists& listes,
             int nb_som_face)
{
  for(int face=0; face<les_faces.nb_faces(); face++)
    {
      faces.voisin(premier,0)=les_faces.voisin(face,0);
      faces.voisin(premier,1)=les_faces.voisin(face,1);
      int numero=ppsf(les_faces, face, nb_som_face);
      for(int i=0; i<nb_som_face; i++)
        faces.sommet(premier,i)=les_faces.sommet(face,i);
      listes[numero].add(premier++);
    }
}

