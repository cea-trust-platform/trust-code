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
// File:        MItSouPolyMAC_oldFa.h
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Sources
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MItSouPolyMAC_oldFa_H
#define MItSouPolyMAC_oldFa_H

#include <Iterateur_Source_PolyMAC_old_base.h>
//#include <Les_Cl.h>
#include <Domaine_PolyMAC_old.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS Iterateur_Source_PolyMAC_old_Face
//
//////////////////////////////////////////////////////////////////////////////

#define declare_It_Sou_PolyMAC_old_Face(_TYPE_)                                        \
  class It_Sou_PolyMAC_old_Face(_TYPE_) : public Iterateur_Source_PolyMAC_old_base        \
  {                                                                        \
                                                                        \
    Declare_instanciable(It_Sou_PolyMAC_old_Face(_TYPE_));                        \
                                                                        \
  public:                                                                \
                                                                        \
    inline It_Sou_PolyMAC_old_Face(_TYPE_)(const It_Sou_Poly_MAC_Face(_TYPE_)&);        \
    inline Evaluateur_Source_PolyMAC_old& evaluateur();                                \
                                                                        \
    DoubleTab& calculer(DoubleTab& ) const;                                \
    DoubleTab& ajouter(DoubleTab& ) const;                                \
    inline void completer_();                                                \
                                                                        \
  protected:                                                                \
                                                                        \
    _TYPE_ evaluateur_source_face;                                        \
                                                                        \
    DoubleTab& ajouter_faces_internes(DoubleTab& ) const;                \
    DoubleTab& ajouter_faces_internes(DoubleTab& ,int ) const;                \
    DoubleTab& ajouter_faces_bords(DoubleTab& ) const;                        \
    DoubleTab& ajouter_faces_bords(DoubleTab& ,int ) const;                \
    inline const int& faces_doubles(int num_face) const { return le_dom->faces_doubles()[num_face];}; \
                                                                        \
    int nb_faces;                                                        \
    int premiere_face_interne;                                                \
    mutable DoubleTab coef;                                                \
                                                                        \
  };                                                                        \
                                                                        \
  inline It_Sou_PolyMAC_old_Face(_TYPE_)::                                        \
  It_Sou_PolyMAC_old_Face(_TYPE_)(const It_Sou_Poly_MAC_Face(_TYPE_)& iter)                \
                                                                        \
    : Iterateur_Source_PolyMAC_old_base(iter),                                        \
      evaluateur_source_face(iter.evaluateur_source_face),                \
      nb_faces(iter.nb_faces), premiere_face_interne(iter.premiere_face_interne) \
                                                                        \
  {}                                                                        \
                                                                        \
  inline void It_Sou_PolyMAC_old_Face(_TYPE_)::completer_(){                        \
    nb_faces=le_dom->nb_faces();                                        \
    premiere_face_interne=le_dom->premiere_face_int();                        \
  }                                                                        \
                                                                        \
  inline Evaluateur_Source_PolyMAC_old& It_Sou_PolyMAC_old_Face(_TYPE_)::evaluateur()        \
  {                                                                        \
    Evaluateur_Source_PolyMAC_old& eval = (Evaluateur_Source_PolyMAC_old&) evaluateur_source_face; \
    return eval;                                                        \
  }

#define implemente_It_Sou_PolyMAC_old_Face(_TYPE_)                                \
  Implemente_instanciable(It_Sou_PolyMAC_old_Face(_TYPE_),"Iterateur_Source_PolyMAC_old_Face",Iterateur_Source_PolyMAC_old_base); \
                                                                        \
  Sortie& It_Sou_PolyMAC_old_Face(_TYPE_)::printOn(Sortie& s ) const {                \
    return s << que_suis_je() ;                                                \
  }                                                                        \
  Entree& It_Sou_PolyMAC_old_Face(_TYPE_)::readOn(Entree& s ) {                        \
    return s ;                                                                \
  }                                                                        \
  DoubleTab& It_Sou_PolyMAC_old_Face(_TYPE_)::ajouter(DoubleTab& resu) const        \
  {                                                                        \
    ((_TYPE_&) (evaluateur_source_face)).mettre_a_jour( );                \
                                                                        \
    assert(resu.nb_dim() < 3);                                                \
    int ncomp=1;                                                        \
    if (resu.nb_dim() == 2)                                                \
      ncomp=resu.dimension(1);                                                \
                                                                        \
    DoubleVect& bilan = so_base->bilan();                                \
    bilan=0;                                                                \
    int nb_faces_tot = le_dom.valeur().nb_faces_tot();                \
    coef.resize(nb_faces_tot,Array_base::NOCOPY_NOINIT);                \
    coef=1;                                                                \
    if (equation_divisee_par_rho())                                        \
      {                                                                        \
        const Milieu_base& milieu = la_zcl->equation().milieu();        \
        const Champ_base& rho = milieu.masse_volumique().valeur();                \
        if (sub_type(Champ_Uniforme,rho))                        \
          coef = rho(0,0);                                                \
        else                                                                \
          {                                                                \
            const DoubleTab& val_rho = rho.valeurs();                \
            const IntTab& face_vois = le_dom.valeur().face_voisins();        \
            const DoubleVect& volumes = le_dom.valeur().volumes();        \
            coef = 0.;                                                        \
            for (int fac=0; fac<nb_faces_tot; fac++)                        \
              {                                                                \
                int elem1 = face_vois(fac,0);                                \
                int elem2 = face_vois(fac,1);                                \
                double vol = 0;                                                \
                if (elem1!=-1)                                                \
                  {                                                        \
                    coef(fac) += val_rho(elem1)*volumes(elem1);                \
                    vol += volumes(elem1);                                \
                  }                                                        \
                if (elem2!=-1)                                                \
                  {                                                        \
                    coef(fac) += val_rho(elem2)*volumes(elem2);                \
                    vol += volumes(elem2);                                \
                  }                                                        \
                coef(fac) /= vol;                                        \
              }                                                                \
          }                                                                \
      }                                                                        \
    if( ncomp == 1)                                                        \
      {                                                                        \
        ajouter_faces_bords(resu) ;                                        \
        ajouter_faces_internes(resu) ;                                        \
      }                                                                        \
    else                                                                \
      {                                                                        \
        ajouter_faces_bords(resu, ncomp) ;                                \
        ajouter_faces_internes(resu, ncomp) ;                                \
      }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_Sou_PolyMAC_old_Face(_TYPE_)::ajouter_faces_bords(DoubleTab& resu) const \
  {                                                                        \
    int num_cl;                                                                \
    int num_face;                                                        \
    DoubleVect& bilan = so_base->bilan();                                \
    for (num_cl =0; num_cl<le_dom->nb_front_Cl(); num_cl++)                \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis()); \
        int ndeb = le_bord.num_premiere_face();                                \
        int nfin = ndeb + le_bord.nb_faces();                                \
        if ( (sub_type(Dirichlet,la_cl.valeur()))                        \
             ||                                                                \
             (sub_type(Dirichlet_homogene,la_cl.valeur()))                \
             )                                                                \
          ;                                                                \
        else                                                                \
          for (num_face=ndeb; num_face<nfin; num_face++) {                \
            double source = evaluateur_source_face.calculer_terme_source_bord(num_face); \
            resu(num_face) += source;                                        \
            double contribution = (faces_doubles(num_face)==1) ? 0.5 : 1 ; \
            bilan(0) += contribution * coef(num_face) * source;                \
          }                                                                \
      }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_Sou_PolyMAC_old_Face(_TYPE_)::ajouter_faces_bords(DoubleTab& resu,int ncomp) const \
  {                                                                        \
    int num_cl;                                                                \
    int num_face;                                                        \
    DoubleVect source(ncomp);                                                \
    DoubleVect& bilan = so_base->bilan();                                \
    for (num_cl =0; num_cl<le_dom->nb_front_Cl(); num_cl++)                \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis()); \
        int ndeb = le_bord.num_premiere_face();                                \
        int nfin = ndeb + le_bord.nb_faces();                                \
        if ( (sub_type(Dirichlet,la_cl.valeur()))                        \
             || (sub_type(Dirichlet_homogene,la_cl.valeur())) )                \
          ;                                                                \
        else                                                                \
          for (num_face=ndeb; num_face<nfin; num_face++) {                \
            evaluateur_source_face.calculer_terme_source_bord(num_face,source);        \
            for (int k=0; k<ncomp; k++) {                                \
              resu(num_face,k) += source(k);                                \
              double contribution = (faces_doubles(num_face)==1) ? 0.5 : 1 ; \
              bilan(k) += contribution * coef(num_face) * source(k);        \
            }                                                                \
          }                                                                \
      }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_Sou_PolyMAC_old_Face(_TYPE_)::ajouter_faces_internes(DoubleTab& resu) const \
  {                                                                        \
    DoubleVect& bilan = so_base->bilan();                                \
    for (int num_face=premiere_face_interne; num_face<nb_faces; num_face++) { \
      double source = evaluateur_source_face.calculer_terme_source(num_face); \
      resu(num_face) += source;                                                \
      double contribution = (faces_doubles(num_face)==1) ? 0.5 : 1 ;        \
      bilan(0) += contribution * coef(num_face) * source;                \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_Sou_PolyMAC_old_Face(_TYPE_)::ajouter_faces_internes(DoubleTab& resu,int ncomp) const \
  {                                                                        \
    DoubleVect source(ncomp);                                                \
    DoubleVect& bilan = so_base->bilan();                                \
    for (int num_face=premiere_face_interne; num_face<nb_faces; num_face++) { \
      evaluateur_source_face.calculer_terme_source(num_face,source);        \
      for (int k=0; k<ncomp; k++) {                                        \
        resu(num_face,k) += source(k);                                        \
        double contribution = (faces_doubles(num_face)==1) ? 0.5 : 1 ;        \
        bilan(k) += contribution * coef(num_face) * source(k);                \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_Sou_PolyMAC_old_Face(_TYPE_)::calculer(DoubleTab& resu) const        \
  {                                                                        \
    resu=0;                                                                \
    return ajouter(resu);                                                \
  }
#endif
