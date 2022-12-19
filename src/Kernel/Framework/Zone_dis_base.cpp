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

#include <Zone_dis_base.h>
#include <Frontiere_dis_base.h>
#include <Zone.h>

Implemente_base(Zone_dis_base,"Zone_dis_base",Objet_U);


/*! @brief Surcharge Objet_U::printOn(Sortie&) NE FAIT RIEN
 *
 *     A surcharger dans les classes derivees.
 *     Imprime la zone discretisee sur un flot de sortie
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Zone_dis_base::printOn(Sortie& os) const
{
  return os ;
}


/*! @brief Surcharge Objet_U::readOn(Sortie&) NE FAIT RIEN
 *
 *     A surcharger dans les classes derivees.
 *     Lit une zone discretisee a partir d'un flot d'entree
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Zone_dis_base::readOn(Entree& is)
{
  return is ;
}

/*! @brief Associe une Zone a l'objet.
 *
 * @param (Zone& une_zone) la zone a associee a la zone discretisee
 */
void Zone_dis_base::associer_zone(const Zone& une_zone)
{
  la_zone=une_zone;
}

/*! @brief Associe un Domaine_dis a l'objet.
 *
 * @param (Domaine_dis& un_domaine_dis) le domaine discretise a associer.
 */
void Zone_dis_base::associer_domaine_dis(const Domaine_dis& un_domaine_dis)
{
  le_domaine_dis=un_domaine_dis;
}

/*! @brief Renvoie la frontiere de Nom nom.
 *
 * (On indexe les frontiere avec leur nom)
 *
 * @param (Nom& nom) le nom de la frontiere a indexer
 * @return (Frontiere_dis_base&) la frontiere discretisee indexee
 * @throws frontiere de nom inconnu leve par rang_frontiere(Nom&)
 */
const Frontiere_dis_base& Zone_dis_base::frontiere_dis(const Nom& nom) const
{
  return frontiere_dis(rang_frontiere(nom));
}

Frontiere_dis_base& Zone_dis_base::frontiere_dis(const Nom& nom)
{
  return frontiere_dis(rang_frontiere(nom));
}

/*! @brief Renvoie le rang de la frontiere de Nom nom Renvoie -1 si aucune frontiere ne s'appelle nom.
 *
 * @param (Nom& nom) le nom de la frontiere dont cherche le rang
 * @return (int) le rang de la frontiere si elle existe -1 sinon
 * @throws pas de frontiere de Nom nom trouvee
 */
int Zone_dis_base::rang_frontiere(const Nom& nom) const
{
  for(int i=0; i<nb_front_Cl(); i++)
    if (frontiere_dis(i).le_nom()==nom)
      return i;
  Cerr << "We did not find the boundary name " << nom << " on the domain " << zone().domaine().le_nom() << finl;
  Cerr << "Check the given boundary name or the domain." << finl;
  exit();
  return -1;
}

int Zone_dis_base::rang_frontiere(const Nom& nom)
{
  for(int i=0; i<nb_front_Cl(); i++)
    if (frontiere_dis(i).le_nom()==nom)
      return i;
  Cerr << "We did not find the boundary name " << nom << " on the domain " << zone().domaine().le_nom() << finl;
  Cerr << "Check the given boundary name or the domain." << finl;
  exit();
  return -1;
}
/*! @brief Ecriture des noms des bords sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 */
void Zone_dis_base::ecrire_noms_bords(Sortie& os) const
{
  zone().ecrire_noms_bords(os);
}

void Zone_dis_base::creer_elements_fictifs(const Zone_Cl_dis_base&)
{
  Cerr << "Zone_dis_base::creer_elements_fictifs should be overloaded by "<< que_suis_je() <<finl;
  assert(0);
  exit();
}

IntTab& Zone_dis_base::face_sommets()
{
  Cerr << que_suis_je() << "does not implement the method face_sommets()"
       << finl;
  exit();
  throw;
}

const IntTab& Zone_dis_base::face_sommets() const
{
  Cerr << que_suis_je() << "does not implement the method face_sommets()"
       << finl;
  exit();
  throw;
}

IntTab& Zone_dis_base::face_voisins()
{
  Cerr << que_suis_je() << "does not implement the method face_voisins()"
       << finl;
  exit();
  throw;
}

const IntTab& Zone_dis_base::face_voisins() const
{
  Cerr << que_suis_je() << "does not implement the method face_voisins()"
       << finl;
  exit();
  throw;
}
