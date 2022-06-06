/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Iterateur_VDF_base_included
#define Iterateur_VDF_base_included

#include <Ref_Operateur_base.h>
#include <CL_Types_include.h>
#include <Evaluateur_VDF.h>
#include <Matrice_Morse.h>
#include <Zone_Cl_VDF.h>
#include <Zone_Cl_dis.h>
#include <Champ_Face.h>
#include <Zone_VDF.h>

enum class Type_Operateur { Op_CONV_ELEM , Op_CONV_FACE , Op_DIFF_ELEM , Op_DIFT_ELEM , Op_DIFF_FACE , Op_DIFT_FACE } ; // ne touche pas !

enum Type_Cl_VDF
{
  symetrie // Symetrie
  ,sortie_libre // Neumann_sortie_libre ou derivees
  ,entree_fluide // Dirichlet_entree_fluide
  ,paroi_fixe // Dirichlet_paroi_fixe
  ,paroi_defilante // Dirichlet_paroi_defilante
  ,paroi_adiabatique // Neumann_paroi_adiabatique ou derivees
  ,paroi // Neumann_paroi
  ,echange_externe_impose // Echange_externe_impose
  ,echange_global_impose // Echange_global_impose
  ,periodique // periodique
  ,nouvelle_Cl_VDF // Nouvelle_Cl_VDF
};

/* *************************
 * Classe Iterateur_VDF_base
 * *************************/
class Iterateur_VDF_base : public Objet_U
{
  Declare_base(Iterateur_VDF_base);
public:
  void associer(const Zone_VDF&, const Zone_Cl_VDF&, const Operateur_base&);
  void associer(const Zone_dis&, const Zone_Cl_dis&, const Operateur_base&);
  void associer_zone_cl_dis(const Zone_Cl_dis_base&);
  virtual void calculer_flux_bord(const DoubleTab&) const;
  virtual void ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>&) const;

  virtual int impr(Sortie& os) const=0;
  virtual void contribuer_au_second_membre(DoubleTab& ) const=0;
  virtual void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const=0;
  virtual void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse& ) const=0;
  virtual void completer_()=0;
  virtual DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const=0;
  virtual DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const=0;
  virtual Evaluateur_VDF& evaluateur() =0;
  virtual const Evaluateur_VDF& evaluateur() const=0;

  inline const Zone_VDF& zone() const { return la_zone.valeur(); }
  inline const Zone_Cl_VDF& zone_Cl() const { return la_zcl.valeur(); }
  inline Type_Cl_VDF type_cl(const Cond_lim&) const;

protected:
  REF(Zone_VDF) la_zone;
  REF(Zone_Cl_VDF) la_zcl;
  REF(Operateur_base) op_base;
};

inline Type_Cl_VDF Iterateur_VDF_base::type_cl(const Cond_lim& la_cl) const
{
  Type_Cl_VDF retour=nouvelle_Cl_VDF;
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
  return retour;
}

/* ********************
 * Classe Iterateur_VDF
 * ********************/

Declare_deriv(Iterateur_VDF_base);
class Iterateur_VDF : public DERIV(Iterateur_VDF_base)
{
  Declare_instanciable(Iterateur_VDF);
public:
  inline int impr(Sortie& os) const { return valeur().impr(os); }
  inline void completer_() { valeur().completer_(); }
  inline void associer(const Zone_VDF& zvdf, const Zone_Cl_VDF& zcl_vdf, const Operateur_base& op) { valeur().associer(zvdf,zcl_vdf,op); }
  inline void associer(const Zone_dis& zdis, const Zone_Cl_dis& zcl_dis, const Operateur_base& op) { valeur().associer(zdis,zcl_dis,op); }
  inline void contribuer_au_second_membre(DoubleTab& resu) const { valeur().contribuer_au_second_membre(resu); }
  inline void ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const { valeur().ajouter_contribution(inco, matrice); }
  inline void ajouter_contribution_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const { valeur().ajouter_contribution_vitesse(inco, matrice); }
  inline void ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>&) const;
  inline Iterateur_VDF(const Iterateur_VDF_base&);
  inline Evaluateur_VDF& evaluateur() { return valeur().evaluateur(); }
  inline const Evaluateur_VDF& evaluateur() const { return valeur().evaluateur(); }
  inline const Zone_VDF& zone() const { return valeur().zone(); }
  inline const Zone_Cl_VDF& zone_Cl() const { return valeur().zone_Cl(); }
  inline DoubleTab& ajouter(const DoubleTab& inco, DoubleTab& resu) const { return valeur().ajouter(inco,resu); }
  inline DoubleTab& calculer(const DoubleTab& inco, DoubleTab& resu) const { return valeur().calculer(inco, resu); }

protected:
  inline Type_Cl_VDF type_cl(const Cond_lim& cl) const { return valeur().type_cl(cl); }
};

inline Iterateur_VDF::Iterateur_VDF(const Iterateur_VDF_base& Opb)
  : DERIV(Iterateur_VDF_base)()
{
  DERIV(Iterateur_VDF_base)::operator=(Opb);
}

inline void Iterateur_VDF::ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>& f2e) const
{
  return valeur().ajouter_contribution_autre_pb(inco, matrice, la_cl, f2e);
}

#endif /* Iterateur_VDF_base_included */
