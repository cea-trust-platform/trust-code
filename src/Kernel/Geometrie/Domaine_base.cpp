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

#include <Domaine_base.h>
#include <Array_tools.h>

#include <Frontiere.h>

Implemente_base_sans_constructeur(Domaine_base,"Domaine_base",Objet_U);

Sortie& Domaine_base::printOn(Sortie& s) const
{
  return s;
}


Entree& Domaine_base::readOn(Entree& s)
{
  return s;
}

Entree& Domaine_base::lire_bords_a_imprimer(Entree& is)
{
  Nom nom_lu;
  Motcle motlu, accolade_fermee = "}", accolade_ouverte = "{";
  is >> motlu;
  Cerr << "Reading boundaries to be print:" << finl;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected a list { ... } of boundaries to be print" << finl;
      Cerr << "and not : " << motlu << finl;
      exit();
    }
  is >> nom_lu;
  while (nom_lu != accolade_fermee)
    {
      Cerr << nom_lu << " ";
      bords_a_imprimer_.add(Nom(nom_lu));
      is >> nom_lu;
    }
  Cerr << finl;
  return is;
}

Entree& Domaine_base::lire_bords_a_imprimer_sum(Entree& is)
{
  Nom nom_lu;
  Motcle motlu, accolade_fermee = "}", accolade_ouverte = "{";
  is >> motlu;
  Cerr << "Reading boundaries to be print:" << finl;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected a list { ... } of boundaries to be print" << finl;
      Cerr << "and not : " << motlu << finl;
      exit();
    }
  is >> nom_lu;
  while (nom_lu != accolade_fermee)
    {
      Cerr << nom_lu << " ";
      bords_a_imprimer_sum_.add(Nom(nom_lu));
      is >> nom_lu;
    }
  Cerr << finl;
  return is;
}


/*! @brief Concatene les joints de meme nom
 *
 */
int Domaine_base::comprimer_joints()
{
  auto& list = mes_faces_joint_.get_stl_list();
  for (auto it = list.begin(); it != list.end(); ++it)
    {
      Frontiere& front = *it;
      for (auto it2 = std::next(it); it2 != list.end();)
        {
          Frontiere& front2 = *it2;
          if (front.le_nom() == front2.le_nom())
            {
              front.add(front2);
              it2 = list.erase(it2);
            }
          else
            ++it2;
        }
    }
  return 1;
}

/*! @brief Renumerotation des noeuds et des elements presents dans les items communs des joints
 *
 * Le noeud de numero k devient le noeud de numero Les_Nums[k] l'element de
 * numero e devient l'element de numero e+elem_offset
 *
 * @param (IntVect& Les_Nums) le vecteur contenant la nouvelle numerotation Nouveau_numero_noeud_i = Les_Nums[Ancien_numero_noeud_i]
 */
void Domaine_base::renum_joint_common_items(const IntVect& Les_Nums, const int elem_offset)
{
  for (int i_joint = 0; i_joint < nb_joints(); i_joint++)
    {
      ArrOfInt& sommets_communs = mes_faces_joint_[i_joint].set_joint_item(Joint::SOMMET).set_items_communs();
      for (int index = 0; index < sommets_communs.size_array(); index++)
        sommets_communs[index] = Les_Nums[sommets_communs[index]];

      ArrOfInt& elements_distants = mes_faces_joint_[i_joint].set_joint_item(Joint::ELEMENT).set_items_distants();
      elements_distants += elem_offset;
    }
}

