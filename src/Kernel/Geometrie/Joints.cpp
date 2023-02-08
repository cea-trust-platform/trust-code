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

#include <Joints.h>

Implemente_instanciable(Joints, "Joints", LIST(Joint));

Sortie& Joints::printOn(Sortie& os) const { return LIST(Joint)::printOn(os); }

Entree& Joints::readOn(Entree& is) { return LIST(Joint)::readOn(is); }

/*! @brief Associe un domaine a tous les joints de la liste.
 *
 * Les joints de la liste feront partie de cet domaine.
 *
 * @param (Domaine& un_domaine) le domaine a associer aux joints
 */
void Joints::associer_domaine(const Domaine& un_domaine)
{
  for (auto& itr : *this) itr.associer_domaine(un_domaine);
}

/*! @brief Renvoie le nombre de face total des Joints de la liste.
 *
 * (la somme des faces de tous les
 *     joints de la liste).
 *
 * @return (int) le nombre de face total des Joints de la liste
 */
int Joints::nb_faces() const
{
  int nombre = 0;
  for (const auto &itr : *this) nombre += itr.nb_faces();

  return nombre;
}

/*! @brief Renvoie le nombre de faces du type specifie contenues dans la liste de joints.
 *
 *     (somme des faces de ce type sur tous les
 *      joints de la liste)
 *
 * @param (Type_Face type) le type des faces a compter
 * @return (int) le nombre de faces du type specifie contenues dans la liste de joints
 */
int Joints::nb_faces(Type_Face type) const
{
  int nombre = 0;
  for (const auto &itr : *this)
    if (type == itr.faces().type_face())
      nombre += itr.nb_faces();

  return nombre;
}

/*! @brief Comprime la liste de joints.
 *
 */
void Joints::comprimer()
{
  Cerr << "Joints::comprimer() - Start" << finl;
  IntVect fait(size());
  int rang1 = 0, rang2 = 0;

  auto& list = get_stl_list();
  for (auto &itr : list)
    {
      if (!fait(rang1))
        {
          fait(rang1) = 1;
          Joint& joint1 = itr;
          rang2 = rang1;

          for (auto &itr2 : list)
            {
              if (!fait(rang2))
                {
                  Joint& joint2 = itr2;
                  if (joint1.PEvoisin() == joint2.PEvoisin())
                    {
                      Cerr << "agglomeration of joints " << joint1.le_nom() << " and " << joint2.le_nom() << finl;
                      fait(rang2) = 1;
                      joint1.nommer(joint1.le_nom() + joint2.le_nom());

                      // Concatenation des faces
                      joint1.ajouter_faces(joint2.les_sommets_des_faces());

                      // Concatenation des sommets ???
                      Cerr << "Concatenation of nodes ?" << finl;
                      Process::exit();

                      joint2.dimensionner(0);
                    }
                }
              rang2++;
            }
        }
      ++rang1;
    }

  for (auto itr = list.begin(); itr != list.end(); )
    {
      Joint& joint1 = *itr;
      if (joint1.nb_faces() == 0)
        itr = list.erase(itr);
      else
        ++itr;
    }
  Cerr << "Joints::comprimer() - End" << finl;
}

/*! @brief Renvoie le nombre de Joint de la liste
 *
 * @return (int) le nombre de Joint de la liste
 */
int Joints::nb_joints() const
{
  return size();
}

/*! @brief Renvoie une reference sur le joint qui a pour voisin le PE specifie.
 *
 * @param (int pe) un numero de PE
 * @return (Joint&) le joint qui a pour voisin le PE specifie
 * @throws PE voisin specifie non trouve dans la liste des joints.
 */
Joint& Joints::joint_PE(int pe)
{
  for (auto &itr : *this)
    {
      Joint& joint = itr;
      int pe_joint = joint.PEvoisin();
      if (pe == pe_joint) return joint;
    }
  Cerr << "Error in Joints::joint_PE : joint not found" << finl;
  Process::exit();
  throw;
}

