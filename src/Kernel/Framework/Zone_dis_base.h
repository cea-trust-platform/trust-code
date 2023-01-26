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

#ifndef Zone_dis_base_included
#define Zone_dis_base_included

#include <Zone.h>
#include <Ref_Zone_dis.h>
#include <Sous_zones_dis.h>

class Frontiere_dis_base;
class Zone_Cl_dis_base;
class Conds_lim;

Declare_ref(Sous_zones_dis);

/*! @brief classe Zone_dis_base Cette classe est la base de la hierarchie des zones discretisees.
 *
 *      Un objet Zone est associe a la zone discretise.
 *      A chaque methode numerique de Trio U correspond une classe derivant
 *      de Zone_dis_base implementant les outils necessaires a la methode.
 *
 * @sa Classe abstraite dont toutes les zones discretisees doivent deriver, Methodes abstraites:, void discretiser(), const Frontiere_dis_base& frontiere_dis(int ) const, void modifier_pour_Cl(const Conds_lim&)
 */
class Zone_dis_base : public Objet_U
{
  Declare_base(Zone_dis_base);

public :
  /// @@@@@@@ A VIRER
  inline int nombre_de_zones() const { return 1; }
  // associer_zone fait tout le taf:
  void associer_domaine(const Zone& dom) {  throw; }
  inline Zone& domaine()             {  return la_zone.valeur(); }
  inline const Zone& domaine() const {  return la_zone.valeur(); }

  ///
  /// Sous_zones_dis
  ///
  inline int nombre_de_sous_zones_dis() const              {  return les_sous_zones_dis->size();      }
  inline const Sous_zone_dis& sous_zone_dis(int i) const   {  return les_sous_zones_dis.valeur()[i];  }
  inline Sous_zone_dis& sous_zone_dis(int i)               {   return les_sous_zones_dis.valeur()[i]; }

  void discretiser(const Nom& );
  /// @@@@@@@


  void ecrire_noms_bords(Sortie&) const;
  void associer_zone(const Zone&);
  void associer_domaine_dis(const Zone_dis&);
  int rang_frontiere(const Nom& );
  int rang_frontiere(const Nom& ) const;
  const Frontiere_dis_base& frontiere_dis(const Nom& ) const;
  Frontiere_dis_base& frontiere_dis(const Nom& );

  virtual void creer_elements_fictifs(const Zone_Cl_dis_base&);
  virtual IntTab& face_sommets();
  virtual const IntTab& face_sommets() const;
  virtual IntTab& face_voisins();
  virtual const IntTab& face_voisins() const;

  virtual void discretiser() =0;
  virtual void modifier_pour_Cl(const Conds_lim&) =0;
  virtual Frontiere_dis_base& frontiere_dis(int ) =0;
  virtual const Frontiere_dis_base& frontiere_dis(int ) const =0;

  inline const Zone& zone() const { return la_zone.valeur(); }
  inline Zone& zone() { return la_zone.valeur(); }
  inline const Zone_dis& domaine_dis() const { return le_domaine_dis.valeur(); }
  inline Zone_dis& domaine_dis() { return le_domaine_dis.valeur(); }

  // Raccourcis :
  inline int nb_elem() const { return zone().nb_elem(); }
  inline int nb_elem_tot() const { return zone().nb_elem_tot(); }
  inline int nb_som() const { return zone().nb_som(); }
  inline int nb_som_tot() const { return zone().nb_som_tot(); }
  inline int nb_front_Cl() const { return zone().nb_front_Cl(); }

// Methodes pour le calcul et l'appel de la distance au bord solide le plus proche ; en entree on met le tableau des CL de la QDM
  virtual const DoubleTab& y_elem()  const {return y_elem_;}
  virtual const DoubleTab& y_faces() const {return y_faces_;}
  virtual inline void init_dist_paroi_globale(const Conds_lim& conds_lim)
  {
    Cerr << "Zone_dis_base::init_dist_paroi_globale() does nothing ! " << que_suis_je() << "Needs to overload it !" << finl;
    Process::exit();
  }

protected :
  REF(Zone) la_zone;
  REF(Zone_dis) le_domaine_dis;
  REF(Sous_zones_dis) les_sous_zones_dis;

  int dist_paroi_initialisee_ = 0;
  DoubleTab y_elem_, y_faces_;
};

#endif /* Zone_dis_base_included */
