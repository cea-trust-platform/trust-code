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
#include <Sous_Zone.h>
#include <Sous_zone_dis.h>
#include <Sous_zones_dis.h>

Implemente_base(Zone_dis_base,"Zone_dis_base",Objet_U);

Sortie& Zone_dis_base::printOn(Sortie& os) const
{
  return os ;
}

Entree& Zone_dis_base::readOn(Entree& is)
{
  return is ;
}

int Zone_dis_base::nombre_de_sous_zones_dis() const
{
  return les_sous_zones_dis.size();
}

const Sous_zone_dis& Zone_dis_base::sous_zone_dis(int i) const
{
  return les_sous_zones_dis[i];
}

Sous_zone_dis& Zone_dis_base::sous_zone_dis(int i)
{
  return les_sous_zones_dis[i];
}

/*! @brief Associe une Zone a l'objet.
 *
 * @param (Zone& une_zone) la zone a associee a la zone discretisee
 */
void Zone_dis_base::associer_zone(const Zone& une_zone)
{
  la_zone=une_zone;
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
  Cerr << "We did not find the boundary name " << nom << " on the domain " << zone().le_nom() << finl;
  Cerr << "Check the given boundary name or the domain." << finl;
  exit();
  return -1;
}

int Zone_dis_base::rang_frontiere(const Nom& nom)
{
  for(int i=0; i<nb_front_Cl(); i++)
    if (frontiere_dis(i).le_nom()==nom)
      return i;
  Cerr << "We did not find the boundary name " << nom << " on the domain " << zone().le_nom() << finl;
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

void Zone_dis_base::discretiser_root(const Nom& typ)
{
//  const Domaine& dom=le_domaine.valeur();
//  i_am_allocator_of_les_zones = domaine().le_nom()+"_"+type; // Nom unique
//  if (domaines_dis.find(i_am_allocator_of_les_zones.getChar()) != domaines_dis.end()) //on a deja discretise ce domaine!
//    {
//      *this = domaines_dis[i_am_allocator_of_les_zones.getChar()].valeur();
//      i_am_allocator_of_les_zones = "";
//      return;
//    }

  // Should we handle faces
  Nom ze_typ(typ);
  ze_typ.suffix("NO_FACE_");
  bool face_ok = (ze_typ == typ);

  const Zone& dom = la_zone.valeur();

  if (face_ok)
    discretiser();
  else
    discretiser_no_face();

  // Remplit les sous_zones_dis, les type, et leur associe les zone_dis et les sous_zone correspondantes.
  Sous_zones_dis& sszd = sous_zones_dis();
  sszd.dimensionner(dom.nb_ss_zones());

  for (int i=0; i<dom.nb_ss_zones(); i++)
    {
      const Zone& zone_from_ss_zone = dom.ss_zone(i).zone();
      if (!zone_from_ss_zone.est_egal_a(dom))
        {
          Cerr << "In Zone_dis_base::discretiser(), impossible to find a Domain corresponding to sous_zone " << i << finl;
          exit();
        }
      if (face_ok)
        typer_discretiser_ss_zone(i);
    }
  //    //memoization
  //    domaines_dis[i_am_allocator_of_les_zones.getChar()] = *this;
  //    domaines_dis[Nom(Nom("NO_FACE_") + i_am_allocator_of_les_zones).getChar()] = *this;
}
