/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Domaine.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Domaine_included
#define Domaine_included

#include <List_Ref_Sous_Zone.h>
#include <Zones.h>
#include <Vect.h>
#include <Interprete_geometrique_base.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingUMesh.hxx>
using MEDCoupling::MEDCouplingUMesh;
using MEDCoupling::MCAuto;
#endif

class Conds_lim;
class Zone_dis;
class Domaine_dis;
class Probleme_base;
class Zone_VF;
class Zone_dis_base;
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Domaine
//    Un Domaine represente le domaine (spatial) de resolution d'un Probleme.
//    Un Domaine a un Nom, il est constitue d'une ou plusieurs Zone qui
//    peuvent etre eventuellement decoupees en Sous_Zone.
//    Un Domaine porte l'ensemble des sommets utilises par ses Zones.
//    On peut vouloir resoudre des problemes de type different sur plusieurs
//    domaines, geometriquement les domaines sont alors couples par des
//    surfaces qu'on appelle Raccord.
// .SECTION voir aussi
//     Zone Sous_Zone Probleme Raccord
//     Dans Trio-U V1 les domaines sont constitues d'une seule zone
//////////////////////////////////////////////////////////////////////////////
class Domaine : public Objet_U
{
  Declare_instanciable_sans_constructeur(Domaine);

public :

  Domaine();
  inline const Nom& le_nom() const;
  inline void nommer(const Nom& );
  inline int nb_zones() const;
  inline const Zone& zone(int) const;
  inline Zone& zone(int);
  inline const Zone& zone(const Nom&) const;
  inline Zone& zone(const Nom&);
  inline int nb_ss_zones() const;
  inline const Sous_Zone& ss_zone(int) const;
  inline Sous_Zone& ss_zone(int);
  inline const Sous_Zone& ss_zone(const Nom&) const;
  inline Sous_Zone& ss_zone(const Nom&);
  inline double coord(int, int) const;
  inline double& coord(int, int);
  inline const DoubleTab& coord_sommets() const;
  virtual int nb_som() const;
  virtual int nb_som_tot() const;
  void add(const Sous_Zone&);
  int associer_(Objet_U&);
  inline DoubleTab& les_sommets();
  inline const DoubleTab& les_sommets() const;
  DoubleTab getBoundingBox() const;
  //

  Zone& add(Zone&);
  void ajouter(const DoubleTab&, IntVect&);
  void ajouter(const DoubleTab&);
  void comprimer();
  inline void fixer_epsilon(double);
  inline double epsilon() const;
  void reordonner();
  inline int numero_sommet(int i) const
  {
    return i;
  };
  inline const Zones& zones() const;
  inline Zones& zones() ;
  inline const LIST(REF(Sous_Zone))& ss_zones() const;
  inline int deformable() const
  {
    return deformable_;
  };
  inline int& deformable()
  {
    return deformable_;
  };
  inline int get_renum_som_perio(int ) const;
  void construire_renum_som_perio(const Conds_lim&, const Zone_dis&) ;
  virtual void initialiser (double temps, Domaine_dis&, Probleme_base&) {}
  virtual void set_dt(double& dt_) {}
  virtual void mettre_a_jour(double temps, Domaine_dis&, Probleme_base&) {}
  inline void set_fichier_lu(Nom& nom)
  {
    fichier_lu_=nom;
  };
  inline const Nom& get_fichier_lu() const
  {
    return fichier_lu_;
  };

  virtual void creer_tableau_sommets(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual void creer_tableau_elements(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual const MD_Vector& md_vector_sommets() const;
  inline void set_renum_som_perio(IntTab& renum)
  {
    renum_som_perio=renum;
  };

  // Domaines frontieres
  void creer_mes_domaines_frontieres(const Zone_VF& zone_vf);

  // Informations
  void imprimer() const;
  inline const LIST(REF(Domaine))& domaines_frontieres() const
  {
    return domaines_frontieres_;
  };
#ifdef MEDCOUPLING_
  // MEDCoupling:
  MCAuto<MEDCouplingUMesh>& getUMesh() const
  {
    return mesh_;
  };
  MCAuto<MEDCouplingUMesh>& getUFacesMesh() const
  {
    return faces_mesh_;
  };
  void setUMesh(MCAuto<MEDCouplingUMesh>& m) const
  {
    mesh_ = m->clone(true);
  };
#endif
  void buildUFacesMesh(const Zone_dis_base& zone_dis_base) const;
  bool Axi1d() const
  {
    return axi1d;
  }
  bool Axi1d()
  {
    return axi1d;
  }

  void read_vertices(Entree& s);

protected :

  LIST(REF(Domaine)) domaines_frontieres_;
  Nom nom_;
  DoubleTab sommets;
  LIST(REF(Sous_Zone)) les_ss_zones;
  Zones les_zones;
  ArrOfInt renum_som_perio;
  double epsilon_;
  int deformable_;
  Nom fichier_lu_;
#ifdef MEDCOUPLING_
  ///! MEDCoupling version of the domain:
  mutable MCAuto<MEDCouplingUMesh> mesh_;
  ///! MEDCoupling version of the faces domain:
  mutable MCAuto<MEDCouplingUMesh> faces_mesh_;
#endif
  int axi1d;
};
Declare_liste(Domaine);

// Description:
//    Renvoie le nom du domaine.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du domaine
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Domaine::le_nom() const
{
  return nom_;
}

// Description:
//    Donne un nom au domaine.
// Precondition:
// Parametre: Nom& un_nom
//    Signification: le nom a donner au domaine
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le domaine a un nom
inline void Domaine::nommer(const Nom& un_nom)
{
  nom_=un_nom;
}

// Description:
//    Renvoie le nombre de zone du domaine.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de zones du domaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Domaine::nb_zones() const
{
  return les_zones.size();
}

// Verification que le domaine est bien rempli
inline void check_domaine(int n)
{
  if (n<=0)
    {
      Cerr << "You try to use a domain which is not built yet." << finl;
      Cerr << "Check your data file." << finl;
      Process::exit();
    }
}
// Description:
//    Renvoie la i-ieme zone du domaine.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice de la zone a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone&
//    Signification: la i-ieme zone du domaine
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Zone& Domaine::zone(int i) const
{
  check_domaine(nb_zones());
  return les_zones[i];
}

// Description:
//    Renvoie la i-ieme zone du domaine.
// Precondition:
// Parametre: int i
//    Signification: l'indice de la zone a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone&
//    Signification: la i-ieme zone du domaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Zone& Domaine::zone(int i)
{
  check_domaine(nb_zones());
  return les_zones[i];
}

// Description:
//    Renvoie la Zone dont le nom est indique en parametre.
//    (version const)
// Precondition:
// Parametre: Nom& un_nom
//    Signification: le nom de la zone a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Zone&
//    Signification: la Zone dont le nom est indique en parametre
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Zone& Domaine::zone(const Nom& un_nom) const
{
  check_domaine(nb_zones());
  return les_zones(un_nom);
}

// Description:
//    Renvoie la Zone dont le nom est indique en parametre.
// Precondition:
// Parametre: Nom& un_nom
//    Signification: le nom de la zone a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Zone&
//    Signification: la Zone dont le nom est indique en parametre
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Zone& Domaine::zone(const Nom& un_nom)
{
  check_domaine(nb_zones());
  return les_zones(un_nom);
}

// Description:
//    Renvoie le nombre de sous-zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sous-zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Domaine::nb_ss_zones() const
{
  return les_ss_zones.size();
}

// Description:
//    Renvoie la i-ieme sous-zone.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice de la zone a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sous_Zone&
//    Signification: la i-ieme sous-zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Sous_Zone& Domaine::ss_zone(int i) const
{
  return les_ss_zones[i].valeur();
}

// Description:
//    Renvoie la i-ieme sous-zone.
// Precondition:
// Parametre: int i
//    Signification: l'indice de la zone a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sous_Zone&
//    Signification: la i-ieme sous-zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Sous_Zone& Domaine::ss_zone(int i)
{
  return les_ss_zones[i].valeur();
}

// Description:
//    Renvoie la sous-zone dont le nom est indique en parametre.
//    (version const)
// Precondition:
// Parametre: Nom& un_nom
//    Signification: le nom de la sous-zone a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Sous_Zone&
//    Signification: la sous-zone dont le nom est indique en parametre
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Sous_Zone& Domaine::ss_zone(const Nom& un_nom) const
{
  return les_ss_zones(un_nom).valeur();
}

// Description:
//    Renvoie la sous-zone dont le nom est indique en parametre.
// Precondition:
// Parametre: Nom& un_nom
//    Signification: le nom de la sous-zone a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Sous_Zone&
//    Signification: la sous-zone dont le nom est indique en parametre
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Sous_Zone& Domaine::ss_zone(const Nom& un_nom)
{
  return les_ss_zones(un_nom).valeur();
}

// Description:
//    Renvoie la j-ieme coordonnee du i-ieme noeud.
// Precondition:
// Parametre: int i
//    Signification: numero du noeud
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: indice de la composante a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: la j-ieme coordonnee du i-ieme noeud
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Domaine::coord(int i,int j) const
{
  return sommets(numero_sommet(i),j);
}

// Description:
//    Renvoie une reference sur la j-ieme coordonnee
//    du i-ieme noeud. (ce qui permet de modifier sa valeur)
// Precondition:
// Parametre: int i
//    Signification: numero du noeud
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: indice de la composante a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: reference sur la j-ieme coordonnee du i-ieme noeud
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& Domaine::coord(int i, int j)
{
  return sommets(numero_sommet(i),j);
}

// Description:
//    Renvoie le tableau des coordonnees des noeuds (sommets).
//   (acces en lecture)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des coordonnees des noeuds
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const DoubleTab& Domaine::coord_sommets() const
{
  return sommets;
}

// Description:
//    Renvoie le nombre de sommets du domaine.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets du domaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
// Description:
//    Fixe epsilon,
//    Si 2 points d'un domaine sont separes d'une distance
//    inferieure ou egale a epsilon ils sont consideres egaux.
// Precondition:
// Parametre: double eps
//    Signification: la valeur a donner a epsilon
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Domaine::fixer_epsilon(double eps)
{
  epsilon_=eps;
}

// Description:
//    Renvoie epsilon.
//    Si 2 points d'un domaine sont separes d'une distance
//    inferieure ou egale a epsilon ils sont consideres egaux.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: la distance a partir de laquelles 2 points
//                   sont confondus
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Domaine::epsilon() const
{
  return epsilon_;
}

// Description:
//    Renvoie le nombre total de sommets.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre total de sommets
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
// Renvoie le tableau sommets
inline DoubleTab& Domaine::les_sommets()
{
  return sommets;
}

// Description:
//    Renvoie le tableau des sommets du domaine.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des sommets du domaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline const DoubleTab& Domaine::les_sommets() const
{
  return sommets;
}
inline const Zones& Domaine::zones() const
{
  return les_zones;
}
inline Zones& Domaine::zones()
{
  return les_zones;
}
inline const LIST(REF(Sous_Zone))& Domaine::ss_zones() const
{
  return les_ss_zones;
}

inline int Domaine::get_renum_som_perio(int i) const
{
  return renum_som_perio[i];
}

#endif




