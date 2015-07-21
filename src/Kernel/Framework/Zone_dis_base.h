/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Zone_dis_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Zone_dis_base_included
#define Zone_dis_base_included

#include <Zone.h>
#include <Ref_Domaine_dis.h>

class Conds_lim;
class Frontiere_dis_base;
class Zone_Cl_dis_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Zone_dis_base
//     Cette classe est la base de la hierarchie des zones discretisees.
//     Un objet Zone est associe a la zone discretise.
//     A chaque methode numerique de Trio U correspond une classe derivant
//     de Zone_dis_base implementant les outils necessaires a la methode.
// .SECTION voir aussi
//     Classe abstraite dont toutes les zones discretisees doivent deriver
//     Methodes abstraites:
//       void discretiser()
//       const Frontiere_dis_base& frontiere_dis(int ) const
//       void modifier_pour_Cl(const Conds_lim&)
//////////////////////////////////////////////////////////////////////////////
class Zone_dis_base : public Objet_U
{

  Declare_base(Zone_dis_base);

public :

  void associer_zone(const Zone&);
  inline const Zone& zone() const;
  inline Zone& zone();
  void associer_domaine_dis(const Domaine_dis&);
  inline const Domaine_dis& domaine_dis() const;
  inline Domaine_dis& domaine_dis();
  virtual void discretiser() =0;
  virtual void creer_elements_fictifs(const Zone_Cl_dis_base&);
  virtual Frontiere_dis_base& frontiere_dis(int ) =0;
  virtual const Frontiere_dis_base& frontiere_dis(int ) const =0;
  int rang_frontiere(const Nom& );
  int rang_frontiere(const Nom& ) const;
  Frontiere_dis_base& frontiere_dis(const Nom& );
  const Frontiere_dis_base& frontiere_dis(const Nom& ) const;
  void ecrire_noms_bords(Sortie&) const;

  virtual void modifier_pour_Cl(const Conds_lim&) =0;
  virtual IntTab& face_sommets();
  virtual const IntTab& face_sommets() const;
  virtual IntTab& face_voisins();
  virtual const IntTab& face_voisins() const;

  // Raccourcis :

  inline int nb_elem() const ;
  inline int nb_elem_tot() const ;
  inline int nb_som() const ;
  inline int nb_som_tot() const ;
  inline int nb_front_Cl() const ;
protected :

  REF(Zone) la_zone;
  REF(Domaine_dis) le_domaine_dis;
};

// Description:
//    Renvoie la Zone associee a l'objet.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone&
//    Signification: la zone associee a la zone discretisee
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Zone& Zone_dis_base::zone() const
{
  return la_zone.valeur();
}

// Description:
//    Renvoie la Zone associee a l'objet.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone&
//    Signification: la zone associee a la zone discretisee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Zone& Zone_dis_base::zone()
{
  return la_zone.valeur();
}

inline const Domaine_dis& Zone_dis_base::domaine_dis() const
{
  return le_domaine_dis.valeur();
}

inline Domaine_dis& Zone_dis_base::domaine_dis()
{
  return le_domaine_dis.valeur();
}

inline int Zone_dis_base::nb_elem() const
{
  return zone().nb_elem();
}
inline int Zone_dis_base::nb_elem_tot() const
{
  return zone().nb_elem_tot();
}
inline int Zone_dis_base::nb_som() const
{
  return zone().nb_som();
}
inline int Zone_dis_base::nb_som_tot() const
{
  return zone().nb_som_tot();
}
inline int Zone_dis_base::nb_front_Cl() const
{
  return zone().nb_front_Cl();
}
#endif
