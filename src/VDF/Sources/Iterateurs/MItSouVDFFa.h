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
// File:        MItSouVDFFa.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Iterateurs
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MItSouVDFFa_H
#define MItSouVDFFa_H

#include <Iterateur_Source_VDF_base.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Zone_VDF.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS Iterateur_Source_VDF_Face
//
//////////////////////////////////////////////////////////////////////////////

#define declare_It_Sou_VDF_Face(_TYPE_)                                        \
  class It_Sou_VDF_Face(_TYPE_) : public Iterateur_Source_VDF_base        \
  {                                                                        \
                                                                        \
    Declare_instanciable(It_Sou_VDF_Face(_TYPE_));                        \
                                                                        \
  public:                                                                \
                                                                        \
    inline It_Sou_VDF_Face(_TYPE_)(const It_Sou_VDF_Face(_TYPE_)&);        \
    inline Evaluateur_Source_VDF& evaluateur();                                \
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
    inline const int& faces_doubles(int num_face) const { return la_zone->faces_doubles()[num_face];}; \
                                                                        \
    int nb_faces;                                                        \
    int premiere_face_interne;                                                \
    mutable DoubleTab coef;                                                \
                                                                        \
  };                                                                        \
                                                                        \
  inline It_Sou_VDF_Face(_TYPE_)::                                        \
  It_Sou_VDF_Face(_TYPE_)(const It_Sou_VDF_Face(_TYPE_)& iter)                \
                                                                        \
    : Iterateur_Source_VDF_base(iter),                                        \
      evaluateur_source_face(iter.evaluateur_source_face),                \
      nb_faces(iter.nb_faces), premiere_face_interne(iter.premiere_face_interne) \
                                                                        \
  {}                                                                        \
                                                                        \
  inline void It_Sou_VDF_Face(_TYPE_)::completer_(){                        \
    nb_faces=la_zone->nb_faces();                                        \
    premiere_face_interne=la_zone->premiere_face_int();                        \
  }                                                                        \
                                                                        \
  inline Evaluateur_Source_VDF& It_Sou_VDF_Face(_TYPE_)::evaluateur()        \
  {                                                                        \
    Evaluateur_Source_VDF& eval = (Evaluateur_Source_VDF&) evaluateur_source_face; \
    return eval;                                                        \
  }

#define implemente_It_Sou_VDF_Face(_TYPE_)                                \
  Implemente_instanciable(It_Sou_VDF_Face(_TYPE_),"Iterateur_Source_VDF_Face",Iterateur_Source_VDF_base); \
                                                                        \
  Sortie& It_Sou_VDF_Face(_TYPE_)::printOn(Sortie& s ) const {                \
    return s << que_suis_je() ;                                                \
  }                                                                        \
  Entree& It_Sou_VDF_Face(_TYPE_)::readOn(Entree& s ) {                        \
    return s ;                                                                \
  }                                                                        \
  DoubleTab& It_Sou_VDF_Face(_TYPE_)::ajouter(DoubleTab& resu) const        \
  {                                                                        \
    ((_TYPE_&) (evaluateur_source_face)).mettre_a_jour( );                \
                                                                        \
    assert(resu.nb_dim() < 3);                                                \
    const int ncomp = resu.line_size();                                        \
                                                                        \
    DoubleVect& bilan = so_base->bilan();                                \
    bilan=0;                                                                \
    int nb_faces_tot = la_zone.valeur().nb_faces_tot();                \
    coef.resize(nb_faces_tot,Array_base::NOCOPY_NOINIT);                \
    coef=1;                                                                \
    if (equation_divisee_par_rho())                                        \
      {                                                                        \
        const Milieu_base& milieu = la_zcl->equation().milieu();        \
        const Champ_base& rho = milieu.masse_volumique();                \
        if (sub_type(Champ_Uniforme,rho))                        \
          coef = rho(0,0);                                                \
        else                                                                \
          {                                                                \
            const DoubleTab& val_rho = rho.valeurs();                \
            const IntTab& face_vois = la_zone.valeur().face_voisins();        \
            const DoubleVect& volumes = la_zone.valeur().volumes();        \
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
  DoubleTab& It_Sou_VDF_Face(_TYPE_)::ajouter_faces_bords(DoubleTab& resu) const \
  {                                                                        \
    int num_cl;                                                                \
    int num_face;                                                        \
    DoubleVect& bilan = so_base->bilan();                                \
    for (num_cl =0; num_cl<la_zone->nb_front_Cl(); num_cl++)                \
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
  DoubleTab& It_Sou_VDF_Face(_TYPE_)::ajouter_faces_bords(DoubleTab& resu,int ncomp) const \
  {                                                                        \
    int num_cl;                                                                \
    int num_face;                                                        \
    DoubleVect source(ncomp);                                                \
    DoubleVect& bilan = so_base->bilan();                                \
    for (num_cl =0; num_cl<la_zone->nb_front_Cl(); num_cl++)                \
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
  DoubleTab& It_Sou_VDF_Face(_TYPE_)::ajouter_faces_internes(DoubleTab& resu) const \
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
  DoubleTab& It_Sou_VDF_Face(_TYPE_)::ajouter_faces_internes(DoubleTab& resu,int ncomp) const \
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
  DoubleTab& It_Sou_VDF_Face(_TYPE_)::calculer(DoubleTab& resu) const        \
  {                                                                        \
    resu=0;                                                                \
    return ajouter(resu);                                                \
  }
#endif
