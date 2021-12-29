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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Conv_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Conv
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Conv_VDF_included
#define Op_Conv_VDF_included

#include <Iterateur_VDF_base.h>
#include <Op_VDF_Elem.h>
#include <Op_VDF_Face.h>
#include <type_traits>

template <typename OP_TYPE>
class Op_Conv_VDF : public Op_VDF_Elem, public Op_VDF_Face
{
protected:
  // pour operateurs elem
  inline void dimensionner_elem(Matrice_Morse& matrice) const { Op_VDF_Elem::dimensionner(iter_()->zone(), iter_()->zone_Cl(), matrice); }
  inline void dimensionner_bloc_vitesse_elem(Matrice_Morse& matrice) const { Op_VDF_Elem::dimensionner_bloc_vitesse(iter_()->zone(), iter_()->zone_Cl(), matrice); }
  inline void modifier_pour_Cl_elem(Matrice_Morse& matrice, DoubleTab& secmem) const { Op_VDF_Elem::modifier_pour_Cl(iter_()->zone(), iter_()->zone_Cl(), matrice, secmem); }

  // pour operateurs face
  inline void dimensionner_face(Matrice_Morse& matrice) const { Op_VDF_Face::dimensionner(iter_()->zone(), iter_()->zone_Cl(), matrice); }
  inline void modifier_pour_Cl_face(Matrice_Morse& matrice, DoubleTab& secmem) const { Op_VDF_Face::modifier_pour_Cl(iter_()->zone(), iter_()->zone_Cl(), matrice, secmem); }

  // pour les deux !
  template <Type_Operateur _TYPE_ , typename EVAL_TYPE>
  inline typename std::enable_if<_TYPE_ == Type_Operateur::Op_CONV_ELEM, void>::type
  associer_impl(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis, const Champ_Inc& ch_transporte)
  {
    constexpr bool is_QUICK = std::is_same<EVAL_TYPE,Eval_Quick_VDF_Elem>::value, is_CENTRE4 = std::is_same<EVAL_TYPE,Eval_Centre4_VDF_Elem>::value;
    const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_transporte.valeur());
    associer_<EVAL_TYPE,is_QUICK,is_CENTRE4>(zone_dis,zone_cl_dis).associer_inconnue(inco); // Cheerssssssssss !!!
  }

  template <Type_Operateur _TYPE_ , typename EVAL_TYPE>
  inline typename std::enable_if<_TYPE_ == Type_Operateur::Op_CONV_FACE, void>::type
  associer_impl(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis, const Champ_Inc& ch_vit)
  {
    constexpr bool is_QUICK = std::is_same<EVAL_TYPE,Eval_Quick_VDF_Face>::value, is_CENTRE4 = std::is_same<EVAL_TYPE,Eval_Centre4_VDF_Face>::value;
    const Champ_Face& vit = ref_cast(Champ_Face,ch_vit.valeur());
    associer_<EVAL_TYPE,is_QUICK,is_CENTRE4>(zone_dis,zone_cl_dis).associer_inconnue(vit); // Cheerssssssssss !!!
  }

  template <typename EVAL_TYPE>
  void associer_vitesse_impl(const Champ_base& ch_vit)
  {
    const Champ_Face& vit = ref_cast(Champ_Face, ch_vit);
    EVAL_TYPE& eval_conv = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    eval_conv.associer(vit);
  }

  template <typename EVAL_TYPE>
  const Champ_base& vitesse_impl() const
  {
    const EVAL_TYPE& eval_conv = static_cast<const EVAL_TYPE&>(iter_()->evaluateur());
    return eval_conv.vitesse();
  }

  template <typename EVAL_TYPE>
  Champ_base& vitesse_impl()
  {
    EVAL_TYPE& eval_conv = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    return eval_conv.vitesse();
  }

private:
  // CRTP pour recuperer l'iter
  inline const Iterateur_VDF& iter_() const { return static_cast<const OP_TYPE *>(this)->get_iter(); }
  inline Iterateur_VDF& iter_() { return static_cast<OP_TYPE *>(this)->get_iter(); }

  // Methode enorme pour tout le monde !
  template <typename EVAL_TYPE, bool is_QUICK, bool is_CENTRE4>
  EVAL_TYPE& associer_(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis)
  {
    const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
    const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
    iter_()->associer(zvdf,zclvdf,static_cast<OP_TYPE&>(*this)); // Et ouiiiiiiiii
    EVAL_TYPE& eval_conv = static_cast<EVAL_TYPE&> (iter_()->evaluateur()); // Mais ouiiiiiiiiiiii
    eval_conv.associer_zones(zvdf, zclvdf );

    if (is_QUICK || is_CENTRE4)
      if ( Process::nproc()>1 && zvdf.zone().nb_joints() && zvdf.zone().joint(0).epaisseur()<2)
        {
          Cerr << "Overlapping width (given by larg_joint option) of  " << zvdf.zone().joint(0).epaisseur() << finl;
          Cerr << "is not enough for Quick scheme in VDF parallel calculation." << finl;
          Cerr << "Please, partition your mesh with an overlapping width of 2 with larg_joint option." << finl;
          Process::exit();
        }
    return eval_conv;
  }
};

#endif /* Op_Conv_VDF_included */
