/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Iterateur_CoviMAC_base.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs/Conv_iterateur
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Iterateur_CoviMAC_base_included
#define Iterateur_CoviMAC_base_included


//
// .DESCRIPTION class IterateurCoviMAC
//
// .SECTION voir aussi
//

#include <Champ_Face_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_Cl_dis.h>
#include <Periodique.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_entree_fluide.h>
#include <Neumann_paroi.h>
#include <Neumann_paroi_adiabatique.h>
#include <Symetrie.h>
#include <Echange_global_impose.h>
#include <Echange_externe_impose.h>
#include <Neumann_sortie_libre.h>
#include <NSCBC.h>
#include <Scalaire_impose_paroi.h>
#include <Evaluateur_CoviMAC.h>
#include <Matrice_Morse.h>
#include <Ref_Operateur_base.h>

enum Type_Cl_CoviMAC
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
  , nscbc                       //      NSCBC
  , scalaire_impose_paroi       //      Scalaire_impose_paroi
  , nouvelle_Cl_CoviMAC                //        Nouvelle_Cl_CoviMAC
};

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Iterateur_CoviMAC_base
//
//////////////////////////////////////////////////////////////////////////////

class Iterateur_CoviMAC_base : public Objet_U
{
  Declare_base(Iterateur_CoviMAC_base);

public:

  void associer(const Zone_CoviMAC&, const Zone_Cl_CoviMAC&, const Operateur_base&);
  void associer(const Zone_dis&, const Zone_Cl_dis&, const Operateur_base&);
  void associer_zone_cl_dis(const Zone_Cl_dis_base&);
  inline Type_Cl_CoviMAC type_cl(const Cond_lim&) const;
  virtual DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const=0;
  virtual void calculer_flux_bord(const DoubleTab&) const;
  virtual DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const=0;
  virtual void contribuer_au_second_membre(DoubleTab& ) const=0;
  virtual void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const=0;
  virtual Evaluateur_CoviMAC& evaluateur() =0;
  virtual const Evaluateur_CoviMAC& evaluateur() const=0;
  inline const Zone_CoviMAC& zone() const;
  inline const Zone_Cl_CoviMAC& zone_Cl() const;
  virtual void completer_()=0;
  virtual int impr(Sortie& os) const=0;

protected:

  REF(Zone_CoviMAC) la_zone;
  REF(Zone_Cl_CoviMAC) la_zcl;
  REF(Operateur_base) op_base;

private:

};
//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Iterateur_CoviMAC
//
//////////////////////////////////////////////////////////////////////////////

Declare_deriv(Iterateur_CoviMAC_base);
class Iterateur_CoviMAC : public DERIV(Iterateur_CoviMAC_base)
{
  Declare_instanciable(Iterateur_CoviMAC);

public:

  inline Iterateur_CoviMAC(const Iterateur_CoviMAC_base&);
  inline void associer(const Zone_CoviMAC&, const Zone_Cl_CoviMAC&, const Operateur_base&);
  inline void associer(const Zone_dis&, const Zone_Cl_dis&, const Operateur_base&);
  inline DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;
  inline DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  inline void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  inline Evaluateur_CoviMAC& evaluateur();
  inline const Evaluateur_CoviMAC& evaluateur() const;
  inline const Zone_CoviMAC& zone() const;
  inline const Zone_Cl_CoviMAC& zone_Cl() const;
  inline void completer_();
  inline int impr(Sortie& os) const;
protected:

  inline Type_Cl_CoviMAC type_cl(const Cond_lim&) const;

private:

};

//
//  Fonctions inline de la classe Iterateur_CoviMAC_base
//


inline const Zone_CoviMAC& Iterateur_CoviMAC_base::zone() const
{
  return la_zone.valeur();
}

inline const Zone_Cl_CoviMAC& Iterateur_CoviMAC_base::zone_Cl() const
{
  return la_zcl.valeur();
}

//// type_cl
//

inline Type_Cl_CoviMAC Iterateur_CoviMAC_base::type_cl(const Cond_lim& la_cl) const
{
  Type_Cl_CoviMAC retour=nouvelle_Cl_CoviMAC;
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
  else if ( sub_type(NSCBC, la_cl.valeur()) ) retour=nscbc;
  else if ( sub_type(Scalaire_impose_paroi, la_cl.valeur()) ) retour=scalaire_impose_paroi;
  //else if ( sub_type(Nouvelle_Cl_CoviMAC, la_cl.valeur()) ) retour=nouvelle_Cl_CoviMAC;
  return retour;
}

//
//  Fonctions inline de la classe Iterateur_CoviMAC
//

inline Iterateur_CoviMAC::Iterateur_CoviMAC(const Iterateur_CoviMAC_base& Opb)
  : DERIV(Iterateur_CoviMAC_base)()
{
  DERIV(Iterateur_CoviMAC_base)::operator=(Opb);
}

inline Evaluateur_CoviMAC& Iterateur_CoviMAC::evaluateur()
{
  return valeur().evaluateur();
}

inline const Evaluateur_CoviMAC& Iterateur_CoviMAC::evaluateur() const
{
  return valeur().evaluateur();
}

inline const Zone_CoviMAC& Iterateur_CoviMAC::zone() const
{
  return valeur().zone();
}

inline const Zone_Cl_CoviMAC& Iterateur_CoviMAC::zone_Cl() const
{
  return valeur().zone_Cl();
}

Type_Cl_CoviMAC Iterateur_CoviMAC::type_cl(const Cond_lim& cl) const
{
  return valeur().type_cl(cl);
}

inline DoubleTab& Iterateur_CoviMAC::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  return valeur().ajouter(inco,resu);
}

inline DoubleTab& Iterateur_CoviMAC::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  return valeur().calculer(inco, resu);
}
inline void Iterateur_CoviMAC::contribuer_au_second_membre(DoubleTab& resu) const
{
  valeur().contribuer_au_second_membre(resu);
}
inline void Iterateur_CoviMAC::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  valeur().ajouter_contribution(inco, matrice);
}

inline void Iterateur_CoviMAC::associer(const Zone_CoviMAC& zone_vdf,
                                        const Zone_Cl_CoviMAC& zone_cl_vdf, const Operateur_base& op)
{
  valeur().associer(zone_vdf,zone_cl_vdf,op);
}

inline void Iterateur_CoviMAC::associer(const Zone_dis& zone_dis,
                                        const Zone_Cl_dis& zone_cl_dis, const Operateur_base& op)
{
  valeur().associer(zone_dis,zone_cl_dis,op);
}

inline void Iterateur_CoviMAC::completer_()
{
  valeur().completer_();
}
inline int Iterateur_CoviMAC::impr(Sortie& os) const
{
  return valeur().impr(os);
}


#endif
