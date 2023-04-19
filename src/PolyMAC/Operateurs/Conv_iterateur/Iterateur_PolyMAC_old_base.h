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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Iterateur_PolyMAC_old_base.h
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Operateurs/Conv_iterateur
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Iterateur_PolyMAC_old_base_included
#define Iterateur_PolyMAC_old_base_included


//
// .DESCRIPTION class IterateurPolyMAC_old
//
// .SECTION voir aussi
//


#include <Domaine_PolyMAC_old.h>
#include <Domaine_Cl_PolyMAC_old.h>
#include <Domaine_Cl_dis.h>
#include <Periodique.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_entree_fluide_leaves.h>
#include <Neumann_paroi.h>
#include <Neumann_paroi_adiabatique.h>
#include <Symetrie.h>
#include <Echange_global_impose.h>
#include <Echange_externe_impose.h>
#include <Neumann_sortie_libre.h>
#include <Scalaire_impose_paroi.h>
#include <Evaluateur_PolyMAC_old.h>
#include <Matrice_Morse.h>
#include <TRUST_Ref.h>
#include <TRUST_Deriv.h>
class Operateur_base;

enum Type_Cl_PolyMAC_old
{
  symetrie                         //        Symetrie
  ,sortie_libre                 //        Neumann_sortie_libre ou derivees
  ,entree_fluide                //        Dirichlet_entree_fluide
  ,paroi_fixe                        //        Dirichlet_paroi_fixe
  ,paroi_defilante                //        Dirichlet_paroi_defilante
  ,paroi_adiabatique                //        Neumann_paroi_adiabatique ou derivees
  ,paroi                        //      Neumann_paroi
  ,echange_externe_impose        //        Echange_externe_impose
  ,echange_global_impose        //        Echange_global_impose
  , periodique                        //        periodique
  , scalaire_impose_paroi       //      Scalaire_impose_paroi
  , nouvelle_Cl_PolyMAC_old                //        Nouvelle_Cl_PolyMAC_old
};

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Iterateur_PolyMAC_old_base
//
//////////////////////////////////////////////////////////////////////////////

class Iterateur_PolyMAC_old_base : public Objet_U
{
  Declare_base(Iterateur_PolyMAC_old_base);

public:

  void associer(const Domaine_PolyMAC_old&, const Domaine_Cl_PolyMAC_old&, const Operateur_base&);
  void associer(const Domaine_dis&, const Domaine_Cl_dis&, const Operateur_base&);
  void associer_domaine_cl_dis(const Domaine_Cl_dis_base&);
  inline Type_Cl_PolyMAC_old type_cl(const Cond_lim&) const;
  virtual DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const=0;
  virtual void calculer_flux_bord(const DoubleTab&) const;
  virtual DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const=0;
  virtual void contribuer_au_second_membre(DoubleTab& ) const=0;
  virtual void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const=0;
  virtual void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse& ) const=0;
  virtual Evaluateur_PolyMAC_old& evaluateur() =0;
  virtual const Evaluateur_PolyMAC_old& evaluateur() const=0;
  inline const Domaine_PolyMAC_old& domaine() const;
  inline const Domaine_Cl_PolyMAC_old& domaine_Cl() const;
  virtual void completer_()=0;
  virtual int impr(Sortie& os) const=0;

protected:

  REF(Domaine_PolyMAC_old) la_domaine;
  REF(Domaine_Cl_PolyMAC_old) la_zcl;
  REF(Operateur_base) op_base;

private:

};
//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Iterateur_PolyMAC_old
//
//////////////////////////////////////////////////////////////////////////////

class Iterateur_PolyMAC_old : public DERIV(Iterateur_PolyMAC_old_base)
{
  Declare_instanciable(Iterateur_PolyMAC_old);

public:

  inline Iterateur_PolyMAC_old(const Iterateur_PolyMAC_old_base&);
  inline void associer(const Domaine_PolyMAC_old&, const Domaine_Cl_PolyMAC_old&, const Operateur_base&);
  inline void associer(const Domaine_dis&, const Domaine_Cl_dis&, const Operateur_base&);
  inline DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;
  inline DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  inline void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  inline void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  inline Evaluateur_PolyMAC_old& evaluateur();
  inline const Evaluateur_PolyMAC_old& evaluateur() const;
  inline const Domaine_PolyMAC_old& domaine() const;
  inline const Domaine_Cl_PolyMAC_old& domaine_Cl() const;
  inline void completer_();
  inline int impr(Sortie& os) const;
protected:

  inline Type_Cl_PolyMAC_old type_cl(const Cond_lim&) const;

private:

};

//
//  Fonctions inline de la classe Iterateur_PolyMAC_old_base
//


inline const Domaine_PolyMAC_old& Iterateur_PolyMAC_old_base::domaine() const
{
  return la_domaine.valeur();
}

inline const Domaine_Cl_PolyMAC_old& Iterateur_PolyMAC_old_base::domaine_Cl() const
{
  return la_zcl.valeur();
}

//// type_cl
//

inline Type_Cl_PolyMAC_old Iterateur_PolyMAC_old_base::type_cl(const Cond_lim& la_cl) const
{
  Type_Cl_PolyMAC_old retour=nouvelle_Cl_PolyMAC_old;
  if ( sub_type(Symetrie,la_cl.valeur()) ) retour=symetrie;
  else if ( sub_type(Neumann_sortie_libre, la_cl.valeur()) ) retour=sortie_libre;
  else if ( sub_type(Dirichlet_entree_fluide, la_cl.valeur()) ) retour=entree_fluide;
  else if ( sub_type(Dirichlet_paroi_fixe, la_cl.valeur()) ) retour=paroi_fixe;
  else if ( sub_type(Dirichlet_paroi_defilante, la_cl.valeur()) ) retour=paroi_defilante;
  else if ( sub_type(Neumann_paroi_adiabatique, la_cl.valeur()) ) retour=paroi_adiabatique;
  else if ( sub_type(Neumann_paroi, la_cl.valeur()) ) retour=paroi;
  else if ( sub_type(Echange_externe_impose, la_cl.valeur()) ) retour=echange_externe_impose;
  else if ( sub_type(Echange_global_impose, la_cl.valeur()) ) retour=echange_global_impose;
  else if ( sub_type(Periodique, la_cl.valeur()) ) retour=periodique;
  else if ( sub_type(Scalaire_impose_paroi, la_cl.valeur()) ) retour=scalaire_impose_paroi;
  return retour;
}

//
//  Fonctions inline de la classe Iterateur_PolyMAC_old
//

inline Iterateur_PolyMAC_old::Iterateur_PolyMAC_old(const Iterateur_PolyMAC_old_base& Opb)
  : DERIV(Iterateur_PolyMAC_old_base)()
{
  DERIV(Iterateur_PolyMAC_old_base)::operator=(Opb);
}

inline Evaluateur_PolyMAC_old& Iterateur_PolyMAC_old::evaluateur()
{
  return valeur().evaluateur();
}

inline const Evaluateur_PolyMAC_old& Iterateur_PolyMAC_old::evaluateur() const
{
  return valeur().evaluateur();
}

inline const Domaine_PolyMAC_old& Iterateur_PolyMAC_old::domaine() const
{
  return valeur().domaine();
}

inline const Domaine_Cl_PolyMAC_old& Iterateur_PolyMAC_old::domaine_Cl() const
{
  return valeur().domaine_Cl();
}

Type_Cl_PolyMAC_old Iterateur_PolyMAC_old::type_cl(const Cond_lim& cl) const
{
  return valeur().type_cl(cl);
}

inline DoubleTab& Iterateur_PolyMAC_old::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  return valeur().ajouter(inco,resu);
}

inline DoubleTab& Iterateur_PolyMAC_old::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  return valeur().calculer(inco, resu);
}
inline void Iterateur_PolyMAC_old::contribuer_au_second_membre(DoubleTab& resu) const
{
  valeur().contribuer_au_second_membre(resu);
}
inline void Iterateur_PolyMAC_old::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  valeur().ajouter_contribution(inco, matrice);
}
inline void Iterateur_PolyMAC_old::ajouter_contribution_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  valeur().ajouter_contribution_vitesse(inco, matrice);
}

inline void Iterateur_PolyMAC_old::associer(const Domaine_PolyMAC_old& domaine_vdf,
                                            const Domaine_Cl_PolyMAC_old& domaine_cl_vdf, const Operateur_base& op)
{
  valeur().associer(domaine_vdf,domaine_cl_vdf,op);
}

inline void Iterateur_PolyMAC_old::associer(const Domaine_dis& domaine_dis,
                                            const Domaine_Cl_dis& domaine_cl_dis, const Operateur_base& op)
{
  valeur().associer(domaine_dis,domaine_cl_dis,op);
}

inline void Iterateur_PolyMAC_old::completer_()
{
  valeur().completer_();
}
inline int Iterateur_PolyMAC_old::impr(Sortie& os) const
{
  return valeur().impr(os);
}


#endif
