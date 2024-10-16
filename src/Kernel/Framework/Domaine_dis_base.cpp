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

#include <Discretisation_base.h>
#include <Frontiere_dis_base.h>
#include <Domaine_dis_base.h>
#include <Probleme_base.h>
#include <Sous_Domaine.h>
#include <Domaine.h>

Implemente_base(Domaine_dis_base,"Domaine_dis_base",Objet_U);

Sortie& Domaine_dis_base::printOn(Sortie& os) const
{
  return os ;
}

Entree& Domaine_dis_base::readOn(Entree& is)
{
  return is ;
}

int Domaine_dis_base::nombre_de_sous_domaines_dis() const
{
  return les_sous_domaines_dis.size();
}

const Sous_domaine_dis_base& Domaine_dis_base::sous_domaine_dis(int i) const
{
  return les_sous_domaines_dis[i].valeur();
}

Sous_domaine_dis_base& Domaine_dis_base::sous_domaine_dis(int i)
{
  return les_sous_domaines_dis[i].valeur();
}

/*! @brief Associe un Domaine a l'objet.
 *
 * @param (Domaine& un_domaine) le domaine a associer au domaine discretise
 */
void Domaine_dis_base::associer_domaine(const Domaine& un_domaine)
{
  le_dom=un_domaine;
}

/*! @brief Renvoie la frontiere de Nom nom.
 *
 * (On indexe les frontiere avec leur nom)
 *
 * @param (Nom& nom) le nom de la frontiere a indexer
 * @return (Frontiere_dis_base&) la frontiere discretisee indexee
 * @throws frontiere de nom inconnu leve par rang_frontiere(Nom&)
 */
const Frontiere_dis_base& Domaine_dis_base::frontiere_dis(const Nom& nom) const
{
  return frontiere_dis(rang_frontiere(nom));
}

Frontiere_dis_base& Domaine_dis_base::frontiere_dis(const Nom& nom)
{
  return frontiere_dis(rang_frontiere(nom));
}

/*! @brief Renvoie le rang de la frontiere de Nom nom Renvoie -1 si aucune frontiere ne s'appelle nom.
 *
 * @param (Nom& nom) le nom de la frontiere dont cherche le rang
 * @return (int) le rang de la frontiere si elle existe -1 sinon
 * @throws pas de frontiere de Nom nom trouvee
 */
int Domaine_dis_base::rang_frontiere(const Nom& nom) const
{
  for(int i=0; i<nb_front_Cl(); i++)
    if (frontiere_dis(i).le_nom()==nom)
      return i;
  Cerr << "We did not find the boundary name " << nom << " on the domain " << domaine().le_nom() << finl;
  Cerr << "Check the given boundary name or the domain." << finl;
  exit();
  return -1;
}

int Domaine_dis_base::rang_frontiere(const Nom& nom)
{
  for(int i=0; i<nb_front_Cl(); i++)
    if (frontiere_dis(i).le_nom()==nom)
      return i;
  Cerr << "We did not find the boundary name " << nom << " on the domain " << domaine().le_nom() << finl;
  Cerr << "Check the given boundary name or the domain." << finl;
  exit();
  return -1;
}

/*! @brief Ecriture des noms des bords sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 */
void Domaine_dis_base::ecrire_noms_bords(Sortie& os) const
{
  domaine().ecrire_noms_bords(os);
}

void Domaine_dis_base::creer_elements_fictifs(const Domaine_Cl_dis_base&)
{
  Cerr << "Domaine_dis_base::creer_elements_fictifs should be overloaded by "<< que_suis_je() <<finl;
  assert(0);
  exit();
}

IntTab& Domaine_dis_base::face_sommets()
{
  Cerr << que_suis_je() << "does not implement the method face_sommets()"
       << finl;
  exit();
  throw;
}

const IntTab& Domaine_dis_base::face_sommets() const
{
  Cerr << que_suis_je() << "does not implement the method face_sommets()"
       << finl;
  exit();
  throw;
}

IntTab& Domaine_dis_base::face_voisins()
{
  Cerr << que_suis_je() << "does not implement the method face_voisins()"
       << finl;
  exit();
  throw;
}

const IntTab& Domaine_dis_base::face_voisins() const
{
  Cerr << que_suis_je() << "does not implement the method face_voisins()"
       << finl;
  exit();
  throw;
}

void Domaine_dis_base::discretiser_root(const Nom& typ)
{
  // Should we handle faces
  Nom ze_typ(typ);
  ze_typ.suffix("NO_FACE_");
  bool face_ok = (ze_typ == typ);

  const Domaine& dom = le_dom.valeur();

  if (face_ok)
    discretiser();
  else
    discretiser_no_face();

  // Remplit les sous_domaines_dis, les type, et leur associe les domaine_dis et les sous_domaine correspondantes.
  les_sous_domaines_dis.dimensionner(dom.nb_ss_domaines());

  for (int i=0; i<dom.nb_ss_domaines(); i++)
    {
      const Domaine& domaine_from_ss_domaine = dom.ss_domaine(i).domaine();
      if (!domaine_from_ss_domaine.est_egal_a(dom))
        {
          Cerr << "In Domaine_dis_base::discretiser(), impossible to find a Domain corresponding to sous_domaine " << i << finl;
          exit();
        }
      if (face_ok)
        typer_discretiser_ss_domaine(i);
    }
}

void Domaine_dis_base::creer_champ(const Motcle& motlu, const Probleme_base& pb)
{
  if (motlu == "VOLUME_MAILLE" && volume_maille_.est_nul())
    {
      pb.discretisation().volume_maille(pb.schema_temps(), pb.domaine_dis(), volume_maille_);
      champs_compris_.ajoute_champ(volume_maille_);
    }
  else if (motlu == "MESH_NUMBERING" && mesh_numbering_.est_nul())
    {
      pb.discretisation().mesh_numbering(pb.schema_temps(), pb.domaine_dis(), mesh_numbering_);
      champs_compris_.ajoute_champ(mesh_numbering_);
    }
}

const Champ_base& Domaine_dis_base::get_champ(const Motcle& un_nom) const
{
  if (un_nom == "VOLUME_MAILLE")
    return volume_maille();
  else if (un_nom == "MESH_NUMBERING")
    return mesh_numbering();

  throw Champs_compris_erreur();
}

bool Domaine_dis_base::has_champ(const Motcle& un_nom, OBS_PTR(Champ_base) &ref_champ) const
{
  if (un_nom == "VOLUME_MAILLE")
    {
      ref_champ = volume_maille();
      return true;
    }
  else if (un_nom == "MESH_NUMBERING")
    {
      ref_champ = mesh_numbering();
      return true;
    }
  return false;
}

bool Domaine_dis_base::has_champ(const Motcle& un_nom) const
{
  if (un_nom == "VOLUME_MAILLE" || un_nom == "MESH_NUMBERING")
    return true;
  else
    return false;
}

void Domaine_dis_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Noms noms_compris = champs_compris_.liste_noms_compris();
  noms_compris.add("VOLUME_MAILLE");
  noms_compris.add("MESH_NUMBERING");
  if (opt==DESCRIPTION)
    Cerr<<que_suis_je()<<" : "<< noms_compris <<finl;
  else
    nom.add(noms_compris);
}

