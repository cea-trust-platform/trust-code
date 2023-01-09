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

#ifndef Domaine_included
#define Domaine_included

#include <Interprete_geometrique_base.h>
#include <Ref_Sous_Zone.h>
#include <TRUST_List.h>
#include <Zones.h>
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
/*! @brief Classe Domaine Un Domaine represente le domaine (spatial) de resolution d'un Probleme.
 *
 *     Un Domaine a un Nom, il est constitue d'une ou plusieurs Zone qui
 *     peuvent etre eventuellement decoupees en Sous_Zone.
 *     Un Domaine porte l'ensemble des sommets utilises par ses Zones.
 *     On peut vouloir resoudre des problemes de type different sur plusieurs
 *     domaines, geometriquement les domaines sont alors couples par des
 *     surfaces qu'on appelle Raccord.
 *
 * @sa Zone Sous_Zone Probleme Raccord, Dans Trio-U V1 les domaines sont constitues d'une seule zone
 */
class Domaine : public Objet_U
{
  Declare_instanciable_sans_constructeur(Domaine);

public :

  Domaine();
  inline const Nom& le_nom() const override;
  inline void nommer(const Nom& ) override;
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
  int associer_(Objet_U&) override;
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

/*! @brief Renvoie le nom du domaine.
 *
 * @return (Nom&) le nom du domaine
 */
inline const Nom& Domaine::le_nom() const
{
  return nom_;
}

/*! @brief Donne un nom au domaine.
 *
 * @param (Nom& un_nom) le nom a donner au domaine
 */
inline void Domaine::nommer(const Nom& un_nom)
{
  nom_=un_nom;
}

/*! @brief Renvoie le nombre de zone du domaine.
 *
 * @return (int) le nombre de zones du domaine
 */
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
/*! @brief Renvoie la i-ieme zone du domaine.
 *
 * (version const)
 *
 * @param (int i) l'indice de la zone a renvoyer
 * @return (Zone&) la i-ieme zone du domaine
 */
inline const Zone& Domaine::zone(int i) const
{
  check_domaine(nb_zones());
  return les_zones[i];
}

/*! @brief Renvoie la i-ieme zone du domaine.
 *
 * @param (int i) l'indice de la zone a renvoyer
 * @return (Zone&) la i-ieme zone du domaine
 */
inline Zone& Domaine::zone(int i)
{
  check_domaine(nb_zones());
  return les_zones[i];
}

/*! @brief Renvoie la Zone dont le nom est indique en parametre.
 *
 * (version const)
 *
 * @param (Nom& un_nom) le nom de la zone a renvoyer
 * @return (Zone&) la Zone dont le nom est indique en parametre
 */
inline const Zone& Domaine::zone(const Nom& un_nom) const
{
  check_domaine(nb_zones());
  return les_zones(un_nom);
}

/*! @brief Renvoie la Zone dont le nom est indique en parametre.
 *
 * @param (Nom& un_nom) le nom de la zone a renvoyer
 * @return (Zone&) la Zone dont le nom est indique en parametre
 */
inline Zone& Domaine::zone(const Nom& un_nom)
{
  check_domaine(nb_zones());
  return les_zones(un_nom);
}

/*! @brief Renvoie le nombre de sous-zone.
 *
 * @return (int) le nombre de sous-zone
 */
inline int Domaine::nb_ss_zones() const
{
  return les_ss_zones.size();
}

/*! @brief Renvoie la i-ieme sous-zone.
 *
 * (version const)
 *
 * @param (int i) l'indice de la zone a renvoyer
 * @return (Sous_Zone&) la i-ieme sous-zone
 */
inline const Sous_Zone& Domaine::ss_zone(int i) const
{
  return les_ss_zones[i].valeur();
}

/*! @brief Renvoie la i-ieme sous-zone.
 *
 * @param (int i) l'indice de la zone a renvoyer
 * @return (Sous_Zone&) la i-ieme sous-zone
 */
inline Sous_Zone& Domaine::ss_zone(int i)
{
  return les_ss_zones[i].valeur();
}

/*! @brief Renvoie la sous-zone dont le nom est indique en parametre.
 *
 * (version const)
 *
 * @param (Nom& un_nom) le nom de la sous-zone a renvoyer
 * @return (Sous_Zone&) la sous-zone dont le nom est indique en parametre
 */
inline const Sous_Zone& Domaine::ss_zone(const Nom& un_nom) const
{
  return les_ss_zones(un_nom).valeur();
}

/*! @brief Renvoie la sous-zone dont le nom est indique en parametre.
 *
 * @param (Nom& un_nom) le nom de la sous-zone a renvoyer
 * @return (Sous_Zone&) la sous-zone dont le nom est indique en parametre
 */
inline Sous_Zone& Domaine::ss_zone(const Nom& un_nom)
{
  return les_ss_zones(un_nom).valeur();
}

/*! @brief Renvoie la j-ieme coordonnee du i-ieme noeud.
 *
 * @param (int i) numero du noeud
 * @param (int j) indice de la composante a renvoyer
 * @return (double) la j-ieme coordonnee du i-ieme noeud
 */
inline double Domaine::coord(int i,int j) const
{
  return sommets(numero_sommet(i),j);
}

/*! @brief Renvoie une reference sur la j-ieme coordonnee du i-ieme noeud.
 *
 * (ce qui permet de modifier sa valeur)
 *
 * @param (int i) numero du noeud
 * @param (int j) indice de la composante a renvoyer
 * @return (double&) reference sur la j-ieme coordonnee du i-ieme noeud
 */
inline double& Domaine::coord(int i, int j)
{
  return sommets(numero_sommet(i),j);
}

/*! @brief Renvoie le tableau des coordonnees des noeuds (sommets).
 *
 * (acces en lecture)
 *
 * @return (DoubleTab&) le tableau des coordonnees des noeuds
 */
inline const DoubleTab& Domaine::coord_sommets() const
{
  return sommets;
}

/*! @brief Fixe epsilon, Si 2 points d'un domaine sont separes d'une distance
 *
 *     inferieure ou egale a epsilon ils sont consideres egaux.
 *
 * @param (double eps) la valeur a donner a epsilon
 */
inline void Domaine::fixer_epsilon(double eps)
{
  epsilon_=eps;
}

/*! @brief Renvoie epsilon.
 *
 * Si 2 points d'un domaine sont separes d'une distance
 *     inferieure ou egale a epsilon ils sont consideres egaux.
 *
 * @return (double) la distance a partir de laquelles 2 points sont confondus
 */
inline double Domaine::epsilon() const
{
  return epsilon_;
}

/*! @brief Renvoie le nombre total de sommets.
 *
 * @return (int) le nombre total de sommets
 */
inline DoubleTab& Domaine::les_sommets()
{
  return sommets;
}

/*! @brief Renvoie le tableau des sommets du domaine.
 *
 * @return (DoubleTab&) le tableau des sommets du domaine
 */
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




