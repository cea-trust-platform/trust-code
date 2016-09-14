/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        MItVDFFa.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Iterateurs
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////


#ifndef MIt_VDFFa_H
#define MIt_VDFFa_H

#include <Equation_base.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Iterateur_VDF_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Operateur_base.h>
#include <EcrFicPartage.h>
#include <communications.h>
#include <DoubleTrav.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS Iterateur_VDF_Face
//
//////////////////////////////////////////////////////////////////////////////

#define declare_It_VDF_Face(_TYPE_)                                        \
  class It_VDF_Face(_TYPE_) : public Iterateur_VDF_base                        \
  {                                                                        \
    Declare_instanciable(It_VDF_Face(_TYPE_));                                \
  public:                                                                \
    inline It_VDF_Face(_TYPE_)(const It_VDF_Face(_TYPE_)&);                \
    inline Evaluateur_VDF& evaluateur();                                \
    inline const Evaluateur_VDF& evaluateur() const;                        \
    DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;                \
    DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;                \
    void contribuer_au_second_membre(DoubleTab& ) const;                \
    void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;        \
    inline void completer_();                                                \
    int impr(Sortie& os) const;                                        \
  protected:                                                                \
    _TYPE_ flux_evaluateur;                                                \
    DoubleTab& ajouter_fa7_elem(const DoubleTab&, DoubleTab& ) const;        \
    DoubleTab& ajouter_fa7_elem(const DoubleTab&, DoubleTab& , int ) const; \
    DoubleTab& corriger_flux_fa7_elem_periodicite(const DoubleTab&, DoubleTab& ) const;        \
    DoubleTab& corriger_flux_fa7_elem_periodicite(const DoubleTab&, DoubleTab& , int ) const; \
    DoubleTab& ajouter_fa7_sortie_libre(const DoubleTab&, DoubleTab& ) const; \
    DoubleTab& ajouter_fa7_sortie_libre(const DoubleTab&,                \
                                        DoubleTab& , int ) const;        \
    DoubleTab& ajouter_aretes_internes(const DoubleTab&, DoubleTab& ) const; \
    DoubleTab& ajouter_aretes_internes(const DoubleTab&,                \
                                       DoubleTab& , int ) const;        \
    DoubleTab& ajouter_aretes_mixtes(const DoubleTab&, DoubleTab& ) const; \
    DoubleTab& ajouter_aretes_mixtes(const DoubleTab&,                        \
                                     DoubleTab& , int ) const;        \
    DoubleTab& ajouter_aretes_bords(const DoubleTab&, DoubleTab& ) const; \
    DoubleTab& ajouter_aretes_bords(const DoubleTab&,                        \
                                    DoubleTab& , int ) const;        \
    DoubleTab& ajouter_aretes_coins(const DoubleTab&, DoubleTab& ) const; \
    DoubleTab& ajouter_aretes_coins(const DoubleTab&,                        \
                                    DoubleTab& , int ) const;        \
    void contribuer_au_second_membre_fa7_elem(DoubleTab& ) const;        \
    void contribuer_au_second_membre_fa7_elem(DoubleTab& , int ) const; \
    void corriger_secmem_fa7_elem_periodicite(DoubleTab& ) const;        \
    void corriger_secmem_fa7_elem_periodicite(DoubleTab& , int ) const; \
    void contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& ) const; \
    void contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& , int ) const; \
    void contribuer_au_second_membre_aretes_internes(DoubleTab& ) const; \
    void contribuer_au_second_membre_aretes_internes(DoubleTab& , int ) const; \
    void contribuer_au_second_membre_aretes_mixtes(DoubleTab& ) const;        \
    void contribuer_au_second_membre_aretes_mixtes(DoubleTab& , int ) const;        \
    void contribuer_au_second_membre_aretes_bords(DoubleTab& ) const;        \
    void contribuer_au_second_membre_aretes_bords(DoubleTab& , int ) const; \
    void contribuer_au_second_membre_aretes_coins(DoubleTab& ) const;        \
    void contribuer_au_second_membre_aretes_coins(DoubleTab&, int) const; \
    void ajouter_contribution_fa7_elem(const DoubleTab&, Matrice_Morse& ) const; \
    void ajouter_contribution_fa7_elem(const DoubleTab&, Matrice_Morse& , int ) const; \
    void corriger_coeffs_fa7_elem_periodicite(const DoubleTab&, Matrice_Morse& ) const;        \
    void corriger_coeffs_fa7_elem_periodicite(const DoubleTab&, Matrice_Morse& , int ) const; \
    void ajouter_contribution_fa7_sortie_libre(const DoubleTab&, Matrice_Morse& ) const; \
    void ajouter_contribution_fa7_sortie_libre(const DoubleTab&, Matrice_Morse& , int ) const; \
    void ajouter_contribution_aretes_internes(const DoubleTab&, Matrice_Morse& ) const;        \
    void ajouter_contribution_aretes_internes(const DoubleTab&, Matrice_Morse& , int ) const; \
    void ajouter_contribution_aretes_mixtes(const DoubleTab&, Matrice_Morse& ) const; \
    void ajouter_contribution_aretes_mixtes(const DoubleTab&, Matrice_Morse& , int ) const; \
    void ajouter_contribution_aretes_bords(const DoubleTab&, Matrice_Morse& ) const; \
    void ajouter_contribution_aretes_bords(const DoubleTab&, Matrice_Morse& , int ) const; \
    void  ajouter_contribution_aretes_coins(const DoubleTab&, Matrice_Morse& ) const ; \
    void  ajouter_contribution_aretes_coins(const DoubleTab&, Matrice_Morse& , int) const ; \
    int nb_elem;                                                        \
    IntVect orientation;                                                \
    IntTab Qdm;                                                                \
    IntTab elem;                                                        \
    IntTab elem_faces;                                                        \
    IntVect type_arete_bord;                                                \
    IntVect type_arete_coin;                                                \
    int premiere_arete_interne;                                        \
    int derniere_arete_interne;                                        \
    int premiere_arete_mixte;                                        \
    int derniere_arete_mixte;                                        \
    int premiere_arete_bord;                                                \
    int derniere_arete_bord;                                                \
    int premiere_arete_coin;                                                \
    int derniere_arete_coin;                                                \
  };                                                                        \
  inline It_VDF_Face(_TYPE_)::It_VDF_Face(_TYPE_)(const It_VDF_Face(_TYPE_)& iter) : \
    Iterateur_VDF_base(iter),                                                \
    flux_evaluateur(iter.flux_evaluateur),                                \
    nb_elem(iter.nb_elem), premiere_arete_interne(iter.premiere_arete_interne),        \
    derniere_arete_interne(iter.derniere_arete_interne),                \
    premiere_arete_mixte(iter.premiere_arete_mixte),                        \
    derniere_arete_mixte(iter.derniere_arete_mixte),                        \
    premiere_arete_bord(iter.premiere_arete_bord),                        \
    derniere_arete_bord(iter.derniere_arete_bord),                        \
    premiere_arete_coin(iter.premiere_arete_coin),                        \
    derniere_arete_coin(iter.derniere_arete_coin)                        \
  {                                                                        \
    orientation.ref(iter.orientation);                                        \
    Qdm.ref(iter.Qdm);                                                        \
    elem.ref(iter.elem);                                                \
    elem_faces.ref(iter.elem_faces);                                        \
    type_arete_bord.ref(iter.type_arete_bord);                                \
    type_arete_coin.ref(iter.type_arete_coin);                                \
  }                                                                        \
                                                                        \
  inline Evaluateur_VDF& It_VDF_Face(_TYPE_)::evaluateur()                \
  {                                                                        \
    Evaluateur_VDF& eval = (Evaluateur_VDF&) flux_evaluateur;                \
    return eval;                                                        \
  }                                                                        \
  inline const Evaluateur_VDF& It_VDF_Face(_TYPE_)::evaluateur() const        \
  {                                                                        \
    Evaluateur_VDF& eval = (Evaluateur_VDF&) flux_evaluateur;                \
    return eval;                                                        \
  }


#define implemente1_It_VDF_Face(_TYPE_)                                        \
  Implemente_instanciable(It_VDF_Face(_TYPE_),"Iterateur_VDF_Face",Iterateur_VDF_base);        \
  Sortie& It_VDF_Face(_TYPE_)::printOn(Sortie& s ) const {                \
    return s << que_suis_je() ;                                                \
  }                                                                        \
  Entree& It_VDF_Face(_TYPE_)::readOn(Entree& s ) {                        \
    return s ;                                                                \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter(const DoubleTab& inco, DoubleTab& resu) const        \
  {                                                                        \
    ((_TYPE_&) flux_evaluateur).mettre_a_jour();                        \
    assert(inco.nb_dim() < 3);                                                \
    int ncomp=1;                                                        \
    if (inco.nb_dim() == 2)                                                \
      ncomp=inco.dimension(1);                                                \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    tab_flux_bords.resize(la_zone->nb_faces_bord(),dimension);                \
    tab_flux_bords=0;                                                        \
    if( ncomp == 1)                                                        \
      {                                                                        \
        ajouter_aretes_bords(inco, resu) ;                                \
        ajouter_aretes_coins(inco, resu) ;                                \
        ajouter_aretes_internes(inco,resu);                                \
        ajouter_aretes_mixtes(inco,resu);                                \
        ajouter_fa7_sortie_libre(inco,resu);                                \
        ajouter_fa7_elem(inco,resu);                                        \
      }                                                                        \
    else                                                                \
      {                                                                        \
        ajouter_aretes_bords(inco, resu, ncomp) ;                        \
        ajouter_aretes_coins(inco, resu, ncomp) ;                        \
        ajouter_aretes_internes(inco, resu, ncomp) ;                        \
        ajouter_aretes_mixtes(inco,resu, ncomp);                        \
        ajouter_fa7_sortie_libre(inco,resu, ncomp);                        \
        ajouter_fa7_elem(inco,resu, ncomp);                                \
      }                                                                        \
    /* On multiplie les flux au bord par rho en hydraulique */                \
    /*  (sert uniquement a la sortie) */                                \
    Nom nom_eqn=la_zcl->equation().que_suis_je();                        \
    int face,k;                                                        \
    /* Modif B.Mathieu pour front-tracking: masse_volumique() invalide en f.t.*/ \
    if (nom_eqn.debute_par("Navier_Stokes")                                \
        &&nom_eqn!="Navier_Stokes_Melange"                                \
        &&nom_eqn!="Navier_Stokes_FT_Disc")                                \
      {                                                                        \
        const Champ_Don& rho = la_zcl->equation().milieu().masse_volumique(); \
        if (sub_type(Champ_Uniforme,rho.valeur()))                        \
          {                                                                \
            double coef = rho(0,0);                                        \
            int nb_faces_bord=la_zone->nb_faces_bord();                \
            for (face=0; face<nb_faces_bord; face++)                        \
              for(k=0; k<tab_flux_bords.dimension(1); k++)                        \
                tab_flux_bords(face,k) *= coef;                                \
          }                                                                \
      }                                                                        \
    return resu;                                                        \
  }                                                                        \
                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_bords(const DoubleTab& inco, DoubleTab& resu) const \
  {                                                                        \
    if (!flux_evaluateur.calculer_arete_bord())                                \
      return resu;                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    double flux,flux3=0,flux3_4=0,flux1_2=0;                                \
    int fac1, fac2, fac3, fac4;                                        \
    int n=la_zone->nb_faces_bord();                                        \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for (n_arete=premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++) {        \
      n_type=type_arete_bord(n_arete-premiere_arete_bord);                \
      switch(n_type){                                                        \
      case TypeAreteBordVDF::PAROI_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe);        \
          resu[fac3]+=signe*flux;                                        \
          if (fac1<n)                                                        \
            tab_flux_bords(fac1,orientation(fac3))-=0.5*signe*flux;                \
          if (fac2<n)                                                        \
            tab_flux_bords(fac2,orientation(fac3))-=0.5*signe*flux;                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_fluide()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_fluide(inco, fac1, fac2, fac3, signe, flux3, flux1_2); \
          resu[fac3]+=signe*flux3;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
          if (fac1<n)                                                        \
            tab_flux_bords(fac1,orientation(fac3))-=0.5*signe*flux3;        \
          if (fac2<n)                                                        \
            tab_flux_bords(fac2,orientation(fac3))-=0.5*signe*flux3;        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_paroi_fluide(inco, fac1, fac2, fac3, signe, flux3, flux1_2); \
          resu[fac3]+=signe*flux3;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
          if (fac1<n)                                                        \
            tab_flux_bords(fac1,orientation(fac3))-=0.5*signe*flux3;        \
          if (fac2<n)                                                        \
            tab_flux_bords(fac2,orientation(fac3))-=0.5*signe*flux3;        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::SYM_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.flux_arete_symetrie(inco, fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_periodicite(inco, fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          resu[fac3]+=0.5*flux3_4;                                        \
          resu[fac4]-=0.5*flux3_4;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie_paroi()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.flux_arete_symetrie_paroi(inco, fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_SYM:                                \
        if (flux_evaluateur.calculer_arete_symetrie_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_symetrie_fluide(inco, fac1, fac2, fac3, signe, flux3, flux1_2); \
          resu[fac3]+=signe*flux3;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
        }                                                                \
        break;                                                                \
      default :                                                                \
        Cerr << "On a rencontre un type d'arete non prevu\n";                \
        Cerr << "num arete : " << n_arete;                                \
        Cerr << " type : " << n_type;                                        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_bords(const DoubleTab& inco, \
                                                       DoubleTab& resu,int ncomp) const \
  {                                                                        \
    if (!flux_evaluateur.calculer_arete_bord())                                \
      return resu;                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    DoubleVect flux(ncomp), flux3(ncomp), flux3_4(ncomp), flux1_2(ncomp); \
    int fac1, fac2, fac3, fac4;                                        \
    int k;                                                                \
    int n=la_zone->nb_faces_bord();                                        \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for (n_arete=premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++) {        \
      n_type=type_arete_bord(n_arete-premiere_arete_bord);                \
      switch(n_type){                                                        \
      case TypeAreteBordVDF::PAROI_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe, flux); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=signe*flux(k);                                \
            if (fac1<n)                                                        \
              tab_flux_bords(fac1,k)-=0.5*signe*flux(k);                        \
            if (fac2<n)                                                        \
              tab_flux_bords(fac2,k)-=0.5*signe*flux(k);                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_fluide()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_fluide(inco, fac1, fac2, fac3, signe, flux3, flux1_2); \
          for (k=0; k<ncomp; k++){                                        \
            resu(fac3,k)+=signe*flux3(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
            if (fac1<n)                                                        \
              tab_flux_bords(fac1,k)-=0.5*signe*flux3(k);                        \
            if (fac2<n)                                                        \
              tab_flux_bords(fac2,k)-=0.5*signe*flux3(k);                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_paroi_fluide(inco, fac1, fac2, fac3, signe, flux3, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=signe*flux3(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
            if (fac1<n)                                                        \
              tab_flux_bords(fac1,k)-=0.5*signe*flux3(k);                        \
            if (fac2<n)                                                        \
              tab_flux_bords(fac2,k)-=0.5*signe*flux3(k);                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::SYM_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_symetrie(inco, fac1, fac2, fac3, signe, flux); \
          for (k=0; k<ncomp; k++)                                        \
            resu(fac3,k)+=signe*flux(k);                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_periodicite(inco, fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=0.5*flux3_4(k);                                \
            resu(fac4,k)-=0.5*flux3_4(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie_paroi()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_symetrie_paroi(inco, fac1, fac2, fac3, signe, flux); \
          for (k=0; k<ncomp; k++)                                        \
            resu(fac3,k)+=signe*flux(k);                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_SYM:                                \
        if (flux_evaluateur.calculer_arete_symetrie_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_symetrie_fluide(inco, fac1, fac2, fac3, signe, flux3, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=signe*flux3(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      default :                                                                \
        Cerr << "On a rencontre un type d'arete non prevu\n";                \
        Cerr << "num arete : " << n_arete;                                \
        Cerr << " type : " << n_type;                                        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_coins(const DoubleTab& inco, DoubleTab& resu) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    double flux,flux3_4=0,flux1_2=0;                                        \
    int fac1, fac2, fac3, fac4;                                        \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for (n_arete=premiere_arete_coin; n_arete<derniere_arete_coin; n_arete++) {        \
      n_type=type_arete_coin(n_arete-premiere_arete_coin);                \
      switch(n_type){                                                        \
      case TypeAreteCoinVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_periodicite(inco, fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          resu[fac3]+=0.5*flux3_4;                                        \
          resu[fac4]-=0.5*flux3_4;                                        \
          resu[fac1]+=0.5*flux1_2;                                        \
          resu[fac2]-=0.5*flux1_2;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PERIO_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe);        \
          resu[fac3]+=signe*flux;                                        \
          /* on ajoute la contribution du coin paroi-perio au flux_bord */ \
          /* classiquement -0.5*signe*flux */                                \
          /* on met 0.25 sur les deux faces (car on  ajoutera deux fois la contrib) */ \
          tab_flux_bords(fac1,orientation(fac3))-=0.25*signe*flux;                \
          tab_flux_bords(fac2,orientation(fac3))-=0.25*signe*flux;                \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          /*               int i=0;  while ( Qdm(n_arete,i)==-1) i++;        \
                           fac1=Qdm(n_arete,i);i++;                        \
                           while ( Qdm(n_arete,i)==-1) i++;                \
                           fac3 = Qdm(n_arete,i);                        \
                           signe = 1;                                        \
                           flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac1,fac3, signe);\*/ \
                                                                                                 /*cerr<<"coin "<<fac1<<" "<<fac3<<" "<<signe<<endl;*/ \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe);        \
          resu[fac3]+=signe*flux;                                        \
          /*cerr<<"coin "<<fac1<<" "<<fac2<<" "<<fac3<<" "<<signe << " " << flux<<endl;*/ \
          ((DoubleTab&)(tab_flux_bords))(fac1,orientation(fac3))-=0.5*signe*flux; \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::FLUIDE_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          /*int i=0;  while ( Qdm(n_arete,i)==-1) i++;                        \
            fac3=Qdm(n_arete,i);i++;                                        \
            while ( Qdm(n_arete,i)==-1) i++;                                \
            fac1 = Qdm(n_arete,i);                                        \
            signe = 1;                                                        \
            flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac1,fac3, signe);*/ \
          /*cerr<<"coin2 "<<fac1<<" "<<fac3<<" "<<signe<<endl;*/        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe);        \
          resu[fac3]+=signe*flux;                                        \
          ((DoubleTab&)(tab_flux_bords))(fac1,orientation(fac3))-=0.5*signe*flux; \
        }                                                                \
        break;                                                                \
      default :                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_coins(const DoubleTab& inco, \
                                                       DoubleTab& resu,int ncomp) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    DoubleVect flux(ncomp), flux3(ncomp), flux3_4(ncomp), flux1_2(ncomp); \
    int fac1, fac2, fac3, fac4;                                        \
    int k;                                                                \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for (n_arete=premiere_arete_coin; n_arete<derniere_arete_coin; n_arete++) {        \
      /* OC 01/2005 : bug dans la ligne suivante ? type_arete_bord->type_arete_coin*/ \
      /* n_type=type_arete_bord(n_arete-premiere_arete_coin);*/                \
      n_type=type_arete_coin(n_arete-premiere_arete_coin);                \
      switch(n_type){                                                        \
      case TypeAreteCoinVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_periodicite(inco, fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=0.5*flux3_4(k);                                \
            resu(fac4,k)-=0.5*flux3_4(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PERIO_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe, flux); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=signe*flux(k);                                \
            /* on ajoute la contribution du coin paroi-perio au flux_bord */ \
            /* classiquement -0.5*signe*flux */                                \
            /* on met 0.25 sur les deux faces (car on  ajoutera deux fois la contrib) */ \
            tab_flux_bords(fac1,k)-=0.25*signe*flux(k);                        \
            tab_flux_bords(fac2,k)-=0.25*signe*flux(k);                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      default :                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }

#define implemente2_It_VDF_Face(_TYPE_)                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_internes(const DoubleTab& inco, \
                                                          DoubleTab& resu) const \
  {                                                                        \
    if(!flux_evaluateur.calculer_arete_interne())                        \
      return resu;                                                        \
    double flux;                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete;                                                        \
    for (n_arete=premiere_arete_interne; n_arete<derniere_arete_interne; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux=flux_evaluateur.flux_arete_interne(inco, fac1, fac2, fac3, fac4); \
      resu[fac3] += flux;                                                \
      resu[fac4] -= flux;                                                \
      flux=flux_evaluateur.flux_arete_interne(inco, fac3, fac4, fac1, fac2); \
      resu[fac1] += flux;                                                \
      resu[fac2] -= flux;                                                \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_internes(const DoubleTab& inco, \
                                                          DoubleTab& resu,int ncomp) const \
  {                                                                        \
    if(!flux_evaluateur.calculer_arete_interne())                        \
      return resu;                                                        \
    DoubleVect flux(ncomp);                                                \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete,k;                                                        \
    for (n_arete=premiere_arete_interne; n_arete<derniere_arete_interne; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.flux_arete_interne(inco, fac1, fac2, fac3, fac4,flux); \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac3,k) += flux(k);                                        \
        resu(fac4,k) -= flux(k);                                        \
      }                                                                        \
      flux_evaluateur.flux_arete_interne(inco, fac3, fac4, fac1, fac2, flux); \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac1,k) += flux(k);                                        \
        resu(fac2,k) -= flux(k);                                        \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_mixtes(const DoubleTab& inco, \
                                                        DoubleTab& resu) const \
  {                                                                        \
    if(!flux_evaluateur.calculer_arete_mixte())                                \
      return resu;                                                        \
    double flux;                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete;                                                        \
    int n=la_zone->nb_faces_bord();					\
    int n2=la_zone->nb_faces_tot();  /* GF pour assurer bilan seq= para */ \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for (n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux=flux_evaluateur.flux_arete_mixte(inco, fac1, fac2, fac3, fac4); \
      resu[fac3] += flux;                                                \
      resu[fac4] -= flux;                                                \
      if (fac4<n2) {							\
        if (fac1<n)                                                        \
          tab_flux_bords(fac1,orientation(fac3))-=flux;                        \
        if (fac2<n)                                                        \
          tab_flux_bords(fac2,orientation(fac4))-=flux;                        \
      }                                                                        \
      if (fac3<n2) {							\
        if (fac1<n)                                                        \
          tab_flux_bords(fac1,orientation(fac3))+=flux;                        \
        if (fac2<n)                                                        \
          tab_flux_bords(fac2,orientation(fac4))+=flux;                        \
      }                                                                        \
      flux=flux_evaluateur.flux_arete_mixte(inco, fac3, fac4, fac1, fac2); \
      resu[fac1] += flux;                                                \
      resu[fac2] -= flux;                                                \
       if (fac2<n2)  {						\
        if (fac3<n)                                                        \
          tab_flux_bords(fac3,orientation(fac1))-=flux;                        \
        if (fac4<n)                                                        \
          tab_flux_bords(fac4,orientation(fac2))-=flux;                        \
      }                                                                        \
       if (fac1<n2)  {							\
        if (fac3<n)                                                        \
          tab_flux_bords(fac3,orientation(fac1))+=flux;                        \
        if (fac4<n)                                                        \
          tab_flux_bords(fac4,orientation(fac2))+=flux;                        \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_aretes_mixtes(const DoubleTab& inco, \
                                                        DoubleTab& resu,int ncomp) const \
  {                                                                        \
    if(!flux_evaluateur.calculer_arete_mixte())                                \
      return resu;                                                        \
    DoubleVect flux(ncomp);                                                \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete,k;                                                        \
    for (n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.flux_arete_mixte(inco, fac1, fac2, fac3, fac4, flux); \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac3,k) += flux(k);                                        \
        resu(fac4,k) -= flux(k);                                        \
      }                                                                        \
      flux_evaluateur.flux_arete_mixte(inco, fac3, fac4, fac1, fac2, flux); \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac1,k) += flux(k);                                        \
        resu(fac2,k) -= flux(k);                                        \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_fa7_sortie_libre(const DoubleTab& inco, DoubleTab& resu) const \
  {                                                                        \
    int num_cl;                                                        \
    int ndeb, nfin;                                                        \
    int face;                                                        \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    for (num_cl=0; num_cl<nb_front_Cl; num_cl++) {                        \
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());        \
      ndeb = frontiere_dis.num_premiere_face();                                \
      nfin = ndeb + frontiere_dis.nb_faces();                                \
      switch(type_cl(la_cl)){                                                \
      case sortie_libre :                                                \
        if (flux_evaluateur.calculer_fa7_sortie_libre()){                \
          const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur()); \
          for (face=ndeb; face<nfin; face++) {                                \
            double flux=flux_evaluateur.flux_fa7_sortie_libre(inco, face, cl, ndeb); \
            if ( (elem(face,0)) > -1)                                        \
              resu[face]+=flux;                                                \
            if ( (elem(face,1)) > -1)                                        \
              resu[face]-=flux;                                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      case symetrie :                                                        \
        break;                                                                \
      case entree_fluide :                                                \
        break;                                                                \
      case paroi_fixe :                                                        \
        break;                                                                \
      case paroi_defilante :                                                \
        break;                                                                \
      case paroi_adiabatique :                                                \
        break;                                                                \
      case paroi:                                                        \
        break;                                                                \
      case echange_externe_impose :                                        \
        break;                                                                \
      case echange_global_impose :                                        \
        break;                                                                \
      case periodique :                                                        \
        break;                                                                \
      default :                                                                \
        Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }

#define implemente3_It_VDF_Face(_TYPE_)                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_fa7_sortie_libre(const DoubleTab& inco,DoubleTab& resu,int ncomp) const \
  {                                                                        \
    int num_cl;                                                        \
    int ndeb, nfin;                                                        \
    int face,k;                                                        \
    DoubleVect flux(ncomp);                                                \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    int n=la_zone->nb_faces_bord();                                        \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for (num_cl=0; num_cl<nb_front_Cl; num_cl++) {                        \
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());        \
      ndeb = frontiere_dis.num_premiere_face();                                \
      nfin = ndeb + frontiere_dis.nb_faces();                                \
      switch(type_cl(la_cl)){                                                \
      case sortie_libre :                                                \
        if (flux_evaluateur.calculer_fa7_sortie_libre()){                \
          const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur()); \
          for (face=ndeb; face<nfin; face++) {                                \
            flux_evaluateur.flux_fa7_sortie_libre(inco, face, cl, ndeb, flux); \
            if ( (elem(face,0)) > -1)                                        \
              for (k=0; k<ncomp; k++)                                        \
                resu(face,k) += flux(k);                                \
            if ( (elem(face,1)) > -1)                                        \
              for (k=0; k<ncomp; k++)                                        \
                resu(face,k) -= flux(k);                                \
            for (k=0; k<ncomp; k++)                                        \
              if (face<n)                                                \
                tab_flux_bords(face,k)-=flux(k);                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      case symetrie :                                                        \
        break;                                                                \
      case entree_fluide :                                                \
        break;                                                                \
      case paroi_fixe :                                                        \
        break;                                                                \
      case paroi_defilante :                                                \
        break;                                                                \
      case paroi_adiabatique :                                                \
        break;                                                                \
      case paroi:                                                        \
        break;                                                                \
      case echange_externe_impose :                                        \
        break;                                                                \
      case echange_global_impose :                                        \
        break;                                                                \
      case periodique :                                                        \
        break;                                                                \
      default :                                                                \
        Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_fa7_elem(const DoubleTab& inco, DoubleTab& resu) const \
  {                                                                        \
    double flux;                                                        \
    int fa7, fac1, fac2;                                                \
    int num_elem=0;                                                        \
    int n=la_zone->nb_faces_bord();                                        \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for( ; num_elem<nb_elem; num_elem++){                                \
      for (fa7=0; fa7<dimension; fa7++) {                                \
        fac1=elem_faces(num_elem,fa7);                                        \
        fac2=elem_faces(num_elem,fa7+dimension);                        \
        flux=flux_evaluateur.flux_fa7_elem(inco, num_elem, fac1, fac2);        \
        resu[fac1] += flux;                                                \
        resu[fac2] -= flux;                                                \
        if (fac1<n)                                                        \
          tab_flux_bords(fac1,orientation(fac1))+=flux;                        \
        if (fac2<n)                                                        \
          tab_flux_bords(fac2,orientation(fac2))-=flux;                        \
      }                                                                        \
    }                                                                        \
    corriger_flux_fa7_elem_periodicite(inco,resu);                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::ajouter_fa7_elem(const DoubleTab& inco, \
                                                   DoubleTab& resu,int ncomp) const \
  {                                                                        \
    int fa7, fac1, fac2;                                                \
    int num_elem;                                                        \
    int k;                                                                \
    DoubleVect flux(ncomp);                                                \
    for(num_elem=0; num_elem<nb_elem; num_elem++){                        \
      for (fa7=0; fa7<dimension; fa7++) {                                \
        fac1=elem_faces(num_elem,fa7);                                        \
        fac2=elem_faces(num_elem,fa7+dimension);                        \
        flux_evaluateur.flux_fa7_elem(inco, num_elem, fac1, fac2, flux); \
        for (k=0; k<ncomp; k++) {                                        \
          resu(fac1,k) += flux(k);                                        \
          resu(fac2,k) -= flux(k);                                        \
        }                                                                \
      }                                                                        \
    }                                                                        \
    corriger_flux_fa7_elem_periodicite(inco,resu,ncomp);                \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::corriger_flux_fa7_elem_periodicite(const DoubleTab& inco, DoubleTab& resu, int ncomp) const \
  {                                                                        \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)                        \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
        if (sub_type(Periodique,la_cl.valeur()))                        \
          {                                                                \
            int ndeb,nfin,elem1,elem2,num_elem,ori;                        \
            int face,fac1,fac2,signe;                                \
            DoubleVect flux(ncomp);                                        \
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur()); \
            const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis()); \
            ndeb = le_bord.num_premiere_face();                                \
            nfin = ndeb + le_bord.nb_faces();                                \
            for (face=ndeb; face<nfin; face++)                                \
              {                                                                \
                elem1 = elem(face,0);                                        \
                elem2 = elem(face,1);                                        \
                ori = orientation(face);                                \
                if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) ) \
                  {                                                        \
                    num_elem = elem2;                                        \
                    signe = 1;                                                \
                  }                                                        \
                else                                                        \
                  {                                                        \
                    num_elem = elem1;                                        \
                    signe = -1;                                                \
                  }                                                        \
                fac1 = elem_faces(num_elem,ori);                        \
                fac2 = elem_faces(num_elem,ori+dimension);                \
                flux_evaluateur.flux_fa7_elem(inco,num_elem,fac1,fac2,flux); \
                for (int k=0; k<ncomp; k++)                                \
                  resu(face,k) += signe*flux(k);                        \
              }                                                                \
          }                                                                \
      }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::corriger_flux_fa7_elem_periodicite(const DoubleTab& inco, DoubleTab& resu) const \
  {                                                                        \
    const Zone_Cl_VDF& la_zcl_vdf = ref_cast(Zone_Cl_VDF,la_zcl.valeur()); \
    int nb_cond_lim = la_zcl_vdf.nb_cond_lim();                        \
    for (int num_cl=0; num_cl<nb_cond_lim; num_cl++)                        \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl_vdf.les_conditions_limites(num_cl); \
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis()); \
        if (sub_type(Periodique,la_cl.valeur()))                        \
          {                                                                \
            int ndeb,nfin,elem1,elem2,num_elem,ori;                        \
            int face,fac1,fac2,signe;                                \
            double flux;                                                \
            /* const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur()); */ \
            ndeb = le_bord.num_premiere_face();                                \
            nfin = ndeb + le_bord.nb_faces();                                \
            for (face=ndeb; face<nfin; face++)                                \
              {                                                                \
                elem1 = elem(face,0);                                        \
                elem2 = elem(face,1);                                        \
                ori = orientation(face);                                \
                if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) ) \
                  {                                                        \
                    num_elem = elem2;                                        \
                    signe = 1;                                                \
                  }                                                        \
                else                                                        \
                  {                                                        \
                    num_elem = elem1;                                        \
                    signe = -1;                                                \
                  }                                                        \
                fac1 = elem_faces(num_elem,ori);                        \
                fac2 = elem_faces(num_elem,ori+dimension);                \
                flux=flux_evaluateur.flux_fa7_elem(inco,num_elem,fac1,fac2); \
                resu(face) += signe*flux;                                \
              }                                                                \
          }                                                                \
      }                                                                        \
    return resu;                                                        \
  }                                                                        \
  DoubleTab& It_VDF_Face(_TYPE_)::calculer(const DoubleTab& inco, DoubleTab& resu) const \
  {                                                                        \
    resu=0;                                                                \
    return ajouter(inco,resu);                                                \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::completer_(){                                \
    nb_elem=la_zone->nb_elem_tot();                                        \
    orientation.ref(la_zone->orientation());                                \
    Qdm.ref(la_zone->Qdm());                                                \
    elem.ref(la_zone->face_voisins());                                        \
    elem_faces.ref(la_zone->elem_faces());                                \
    type_arete_bord.ref(la_zcl->type_arete_bord());                        \
    type_arete_coin.ref(la_zcl->type_arete_coin());                        \
    premiere_arete_interne=la_zone->premiere_arete_interne();                \
    derniere_arete_interne=premiere_arete_interne+la_zone->nb_aretes_internes(); \
    premiere_arete_mixte=la_zone->premiere_arete_mixte();                \
    derniere_arete_mixte=premiere_arete_mixte+la_zone->nb_aretes_mixtes(); \
    premiere_arete_bord=la_zone->premiere_arete_bord();                        \
    derniere_arete_bord=premiere_arete_bord+la_zone->nb_aretes_bord();        \
    premiere_arete_coin=la_zone->premiere_arete_coin();                        \
    derniere_arete_coin=premiere_arete_coin+la_zone->nb_aretes_coin();        \
  }                                                                        \
  int It_VDF_Face(_TYPE_)::impr(Sortie& os) const{                        \
    const Zone& ma_zone=la_zone->zone();                                        \
    const int& impr_mom=ma_zone.Moments_a_imprimer();                        \
    const int impr_sum=(ma_zone.Bords_a_imprimer_sum().est_vide() ? 0:1); \
    const int impr_bord=(ma_zone.Bords_a_imprimer().est_vide() ? 0:1);        \
    const Schema_Temps_base& sch = la_zcl->equation().probleme().schema_temps(); \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    DoubleVect bilan(tab_flux_bords.dimension(1));                                \
    const int nb_faces = la_zone->nb_faces_tot();                        \
    DoubleTab xgr(nb_faces,dimension);                                        \
    xgr=0.;                                                                \
    if (impr_mom) {                                                        \
      const DoubleTab& xgrav = la_zone->xv();                                \
      const ArrOfDouble& c_grav=ma_zone.cg_moments();                        \
      for (int num_face=0;num_face <nb_faces;num_face++)                \
        for (int i=0;i<dimension;i++)                                        \
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);                        \
    }                                                                        \
    Nom espace=" \t";                                                        \
    int k,face;                                                                \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    DoubleTrav flux_bords2( 5, nb_front_Cl , tab_flux_bords.dimension(1)) ;           \
    flux_bords2=0;                                                                \
    for (int num_cl=0; num_cl<nb_front_Cl; num_cl++) {                         \
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);             \
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());   \
      int ndeb = frontiere_dis.num_premiere_face();                               \
      int nfin = ndeb + frontiere_dis.nb_faces();                                 \
      int periodicite = (type_cl(la_cl)==periodique?1:0);                         \
      for (face=ndeb; face<nfin; face++) {                                        \
        for(k=0; k<tab_flux_bords.dimension(1); k++)  {                               \
            flux_bords2(0,num_cl,k)+=tab_flux_bords(face, k);                         \
            if(periodicite) {                                                     \
              if( face < (ndeb+frontiere_dis.nb_faces()/2) )                      \
                flux_bords2(1,num_cl,k)+=tab_flux_bords(face, k);                     \
              else                                                                \
                flux_bords2(2,num_cl,k)+=tab_flux_bords(face, k);                      \
            }                                                                     \
            if (ma_zone.Bords_a_imprimer_sum().contient(frontiere_dis.le_nom()))     \
              flux_bords2(3,num_cl,k)+=tab_flux_bords(face, k);                       \
        }  /* fin for k */                                                        \
        if (impr_mom) {                                                           \
          if (dimension==2) {                                                     \
            flux_bords2(4,num_cl,0)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1); \
          }                                                                       \
          else {                                                                  \
              flux_bords2(4,num_cl,0)+=tab_flux_bords(face,2)*xgr(face,1)-tab_flux_bords(face,1)*xgr(face,2); \
              flux_bords2(4,num_cl,1)+=tab_flux_bords(face,0)*xgr(face,2)-tab_flux_bords(face,2)*xgr(face,0); \
              flux_bords2(4,num_cl,2)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1); \
          }                                                                    \
        }                                                                      \
      } /* fin for face */                                                     \
    }                                                                              \
    mp_sum_for_each_item(flux_bords2); \
    if (je_suis_maitre())\
    {\
       SFichier Flux;op_base->ouvrir_fichier(Flux,"",1);                        \
       SFichier Flux_moment;op_base->ouvrir_fichier(Flux_moment,"moment",impr_mom); \
       SFichier Flux_sum;op_base->ouvrir_fichier(Flux_sum,"sum",impr_sum);        \
       sch.imprimer_temps_courant(Flux);                                        \
       if (impr_mom) sch.imprimer_temps_courant(Flux_moment);                \
       if (impr_sum) sch.imprimer_temps_courant(Flux_sum);                \
       for (int num_cl=0; num_cl<nb_front_Cl; num_cl++) {                          \
	 const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);              \
	 int periodicite = (type_cl(la_cl)==periodique?1:0);                          \
	 for(k=0; k<tab_flux_bords.dimension(1); k++) {                                   \
            if(periodicite)                                            \
          	Flux<< espace << flux_bords2(1,num_cl,k) << espace << flux_bords2(2,num_cl,k); \
            else                                                       \
                Flux<< espace << flux_bords2(0,num_cl,k);          \
            if (impr_sum) Flux_sum << espace << flux_bords2(3,num_cl,k);           \
            bilan(k)+=flux_bords2(0,num_cl,k);                                    \
	 }                                                                            \
	 if (dimension==3) {                                                          \
            for (k=0; k<tab_flux_bords.dimension(1); k++)                            \
                if (impr_mom) Flux_moment << espace << flux_bords2(4,num_cl,k);     \
	 }                                                                            \
	 else {                                                                        \
            if (impr_mom) Flux_moment << espace << flux_bords2(4,num_cl,0);     \
	 }                                                                            \
       } /* fin for num_cl */                                                  \
       for(k=0; k<tab_flux_bords.dimension(1); k++)                             \
           Flux<< espace << bilan(k);            \
       Flux << finl;                                                        \
       if (impr_sum) Flux_sum << finl;                                        \
       if (impr_mom) Flux_moment << finl;                                \
    }\
    const LIST(Nom)& Liste_Bords_a_imprimer = la_zone->zone().Bords_a_imprimer();\
    if (!Liste_Bords_a_imprimer.est_vide())\
    { \
       EcrFicPartage Flux_face;op_base->ouvrir_fichier_partage(Flux_face,"",impr_bord); \
       for (int num_cl=0; num_cl<nb_front_Cl; num_cl++) {                \
	 const Frontiere_dis_base& la_fr = la_zcl->les_conditions_limites(num_cl).frontiere_dis();        \
	 const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
	 const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());        \
	 int ndeb = frontiere_dis.num_premiere_face();                        \
	 int nfin = ndeb + frontiere_dis.nb_faces();                        \
	 if (ma_zone.Bords_a_imprimer().contient(la_fr.le_nom())) {                \
           if(je_suis_maitre())                                                \
             {                                                                \
               Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps "; \
               sch.imprimer_temps_courant(Flux_face);                        \
               Flux_face << " : " << finl;                                        \
             }                                                                \
           for (face=ndeb; face<nfin; face++){                                \
             if (dimension == 2)                                                \
               Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1) << " : "; \
             else if (dimension == 3)                                        \
               Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1) << " z= " << la_zone->xv(face,2) << " : "; \
             for(k=0; k<tab_flux_bords.dimension(1); k++)                        \
               Flux_face << tab_flux_bords(face, k) << " ";                        \
             Flux_face << finl;                                                \
           }                                                                \
           Flux_face.syncfile();                                                \
	 }                                                                        \
       }                                                                        \
    }\
    return 1;                                                                \
  }

#define implemente4_It_VDF_Face(_TYPE_)                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre(DoubleTab& resu) const \
  {                                                                        \
    ((_TYPE_&) flux_evaluateur).mettre_a_jour();                        \
    assert(resu.nb_dim() < 3);                                                \
    int ncomp=1;                                                        \
    if (resu.nb_dim() == 2)                                                \
      ncomp=resu.dimension(1);                                                \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    if (la_zone->nb_faces_bord() >0){                                        \
      tab_flux_bords.resize(la_zone->nb_faces_bord(),ncomp);                \
      tab_flux_bords=0;                                                        \
    }                                                                        \
    if( ncomp == 1)                                                        \
      {                                                                        \
        contribuer_au_second_membre_aretes_bords(resu) ;                \
        contribuer_au_second_membre_aretes_coins(resu) ;                \
        contribuer_au_second_membre_aretes_internes(resu);                \
        contribuer_au_second_membre_aretes_mixtes(resu);                \
        contribuer_au_second_membre_fa7_sortie_libre(resu);                \
        contribuer_au_second_membre_fa7_elem(resu);                        \
      }                                                                        \
    else                                                                \
      {                                                                        \
        contribuer_au_second_membre_aretes_bords(resu, ncomp) ;                \
        contribuer_au_second_membre_aretes_coins(resu,ncomp) ;                \
        contribuer_au_second_membre_aretes_internes(resu, ncomp) ;        \
        contribuer_au_second_membre_aretes_mixtes(resu, ncomp);                \
        contribuer_au_second_membre_fa7_sortie_libre(resu, ncomp);        \
        contribuer_au_second_membre_fa7_elem(resu, ncomp);                \
      }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_bords(DoubleTab& resu) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    double flux,flux3=0,flux3_4=0,flux1_2=0;                                \
    int fac1, fac2, fac3, fac4;                                        \
    for (n_arete=premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++) {        \
      n_type=type_arete_bord(n_arete-premiere_arete_bord);                \
      switch(n_type){                                                        \
      case TypeAreteBordVDF::PAROI_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.secmem_arete_paroi(fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_fluide()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_fluide(fac1, fac2, fac3, signe, flux3, flux1_2);        \
          resu[fac3]+=signe*flux3;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_paroi_fluide(fac1, fac2, fac3, signe, flux3, flux1_2); \
          resu[fac3]+=signe*flux3;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::SYM_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.secmem_arete_symetrie(fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_periodicite(fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          resu[fac3]+=0.5*flux3_4;                                        \
          resu[fac4]-=0.5*flux3_4;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie_paroi()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.secmem_arete_symetrie_paroi(fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_SYM:                                \
        if (flux_evaluateur.calculer_arete_symetrie_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_symetrie_fluide(fac1, fac2, fac3, signe, flux3, flux1_2); \
          resu[fac3]+=signe*flux3;                                        \
          resu[fac1]+=flux1_2;                                                \
          resu[fac2]-=flux1_2;                                                \
        }                                                                \
        break;                                                                \
      default :                                                                \
        Cerr << "On a rencontre un type d'arete non prevu\n";                \
        Cerr << "num arete : " << n_arete;                                \
        Cerr << " type : " << n_type;                                        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_bords(DoubleTab& resu,int ncomp) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    DoubleVect flux(ncomp), flux3(ncomp), flux3_4(ncomp), flux1_2(ncomp); \
    int fac1, fac2, fac3, fac4;                                        \
    int k;                                                                \
    for (n_arete=premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++) {        \
      n_type=type_arete_bord(n_arete-premiere_arete_bord);                \
      switch(n_type){                                                        \
      case TypeAreteBordVDF::PAROI_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_paroi(fac1, fac2, fac3, signe, flux); \
          for (k=0; k<ncomp; k++)                                        \
            resu(fac3,k)+=signe*flux(k);                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_fluide()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_fluide(fac1, fac2, fac3, signe, flux3, flux1_2);        \
          for (k=0; k<ncomp; k++){                                        \
            resu(fac3,k)+=signe*flux3(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_paroi_fluide(fac1, fac2, fac3, signe, flux3, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=signe*flux3(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::SYM_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_symetrie(fac1, fac2, fac3, signe, flux);        \
          for (k=0; k<ncomp; k++)                                        \
            resu(fac3,k)+=signe*flux(k);                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_periodicite(fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=0.5*flux3_4(k);                                \
            resu(fac4,k)-=0.5*flux3_4(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie_paroi()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_symetrie_paroi(fac1, fac2, fac3, signe, flux); \
          for (k=0; k<ncomp; k++)                                        \
            resu(fac3,k)+=signe*flux(k);                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_SYM:                                \
        if (flux_evaluateur.calculer_arete_symetrie_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_symetrie_fluide(fac1, fac2, fac3, signe, flux3, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=signe*flux3(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      default :                                                                \
        Cerr << "On a rencontre un type d'arete non prevu\n";                \
        Cerr << "num arete : " << n_arete;                                \
        Cerr << " type : " << n_type;                                        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_coins( DoubleTab& resu) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    double flux,flux3_4=0,flux1_2=0;                                        \
    int fac1, fac2, fac3, fac4;                                        \
    for (n_arete=premiere_arete_coin; n_arete<derniere_arete_coin; n_arete++) {        \
      n_type=type_arete_coin(n_arete-premiere_arete_coin);                \
      switch(n_type){                                                        \
      case TypeAreteCoinVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_periodicite(fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          resu[fac3]+=0.5*flux3_4;                                        \
          resu[fac4]-=0.5*flux3_4;                                        \
          resu[fac1]+=0.5*flux1_2;                                        \
          resu[fac2]-=0.5*flux1_2;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PERIO_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.secmem_arete_paroi( fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          /*               int i=0;  while ( Qdm(n_arete,i)==-1) i++;        \
                           fac1=Qdm(n_arete,i);i++;                        \
                           while ( Qdm(n_arete,i)==-1) i++;                \
                           fac3 = Qdm(n_arete,i);                        \
                           signe = 1;                                        \
                           flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac1,fac3, signe);\*/ \
                                                                                                 /*cerr<<"coin "<<fac1<<" "<<fac3<<" "<<signe<<endl;*/ \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.secmem_arete_paroi( fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
          /*cerr<<"coin "<<fac1<<" "<<fac2<<" "<<fac3<<" "<<signe << " " << flux<<endl;*/ \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::FLUIDE_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          /*int i=0;  while ( Qdm(n_arete,i)==-1) i++;                        \
            fac3=Qdm(n_arete,i);i++;                                        \
            while ( Qdm(n_arete,i)==-1) i++;                                \
            fac1 = Qdm(n_arete,i);                                        \
            signe = 1;                                                        \
            flux=flux_evaluateur.flux_arete_paroi( fac1, fac1,fac3, signe);*/ \
          /*cerr<<"coin2 "<<fac1<<" "<<fac3<<" "<<signe<<endl;*/        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux=flux_evaluateur.secmem_arete_paroi( fac1, fac2, fac3, signe); \
          resu[fac3]+=signe*flux;                                        \
        }                                                                \
        break;                                                                \
      default :                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_coins(        \
                                                                     DoubleTab& resu,int ncomp) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    DoubleVect flux(ncomp), flux3(ncomp), flux3_4(ncomp), flux1_2(ncomp); \
    int fac1, fac2, fac3, fac4;                                        \
    int k;                                                                \
    for (n_arete=premiere_arete_coin; n_arete<derniere_arete_coin; n_arete++) {        \
      /* OC 01/2005 : bug dans la ligne suivante ? type_arete_bord->type_arete_coin*/ \
      /* n_type=type_arete_bord(n_arete-premiere_arete_coin);*/                \
      n_type=type_arete_coin(n_arete-premiere_arete_coin);                \
      switch(n_type){                                                        \
      case TypeAreteCoinVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_periodicite( fac1, fac2, fac3, fac4, flux3_4, flux1_2); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=0.5*flux3_4(k);                                \
            resu(fac4,k)-=0.5*flux3_4(k);                                \
            resu(fac1,k)+=flux1_2(k);                                        \
            resu(fac2,k)-=flux1_2(k);                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PERIO_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.secmem_arete_paroi(fac1, fac2, fac3, signe, flux); \
          for (k=0; k<ncomp; k++) {                                        \
            resu(fac3,k)+=signe*flux(k);                                \
            /* on ajoute la contribution du coin paroi-perio au flux_bord */ \
            /* classiquement -0.5*signe*flux */                                \
            /* on met 0.25 sur les deux faces (car on  ajoutera deux fois la contrib) */ \
          }                                                                \
        }                                                                \
        break;                                                                \
      default :                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }

#define implemente5_It_VDF_Face(_TYPE_)                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_internes(DoubleTab& resu) const \
  {                                                                        \
    double flux;                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete;                                                        \
    for (n_arete=premiere_arete_interne; n_arete<derniere_arete_interne; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux=flux_evaluateur.secmem_arete_interne(fac1, fac2, fac3, fac4); \
      resu[fac3] += flux;                                                \
      resu[fac4] -= flux;                                                \
      flux=flux_evaluateur.secmem_arete_interne(fac3, fac4, fac1, fac2); \
      resu[fac1] += flux;                                                \
      resu[fac2] -= flux;                                                \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_internes(DoubleTab& resu,int ncomp) const \
  {                                                                        \
    DoubleVect flux(ncomp);                                                \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete,k;                                                        \
    for (n_arete=premiere_arete_interne; n_arete<derniere_arete_interne; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.secmem_arete_interne(fac1, fac2, fac3, fac4,flux); \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac3,k) += flux(k);                                        \
        resu(fac4,k) -= flux(k);                                        \
      }                                                                        \
      flux_evaluateur.secmem_arete_interne(fac3, fac4, fac1, fac2, flux); \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac1,k) += flux(k);                                        \
        resu(fac2,k) -= flux(k);                                        \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_mixtes(DoubleTab& resu) const \
  {                                                                        \
    double flux;                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete;                                                        \
    for (n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux=flux_evaluateur.secmem_arete_mixte(fac1, fac2, fac3, fac4);        \
      resu[fac3] += flux;                                                \
      resu[fac4] -= flux;                                                \
      flux=flux_evaluateur.secmem_arete_mixte(fac3, fac4, fac1, fac2);        \
      resu[fac1] += flux;                                                \
      resu[fac2] -= flux;                                                \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_aretes_mixtes(DoubleTab& resu,int ncomp) const \
  {                                                                        \
    DoubleVect flux(ncomp);                                                \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete,k;                                                        \
    for (n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.secmem_arete_mixte(fac1, fac2, fac3, fac4, flux);        \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac3,k) += flux(k);                                        \
        resu(fac4,k) -= flux(k);                                        \
      }                                                                        \
      flux_evaluateur.secmem_arete_mixte(fac3, fac4, fac1, fac2, flux);        \
      for (k=0; k<ncomp; k++) {                                                \
        resu(fac1,k) += flux(k);                                        \
        resu(fac2,k) -= flux(k);                                        \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& resu) const        \
  {                                                                        \
    int num_cl;                                                        \
    int ndeb, nfin;                                                        \
    int face;                                                        \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    for (num_cl=0; num_cl<nb_front_Cl; num_cl++) {                        \
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());        \
      ndeb = frontiere_dis.num_premiere_face();                                \
      nfin = ndeb + frontiere_dis.nb_faces();                                \
      switch(type_cl(la_cl)){                                                \
      case sortie_libre :                                                \
        if (flux_evaluateur.calculer_fa7_sortie_libre()){                \
          const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur()); \
          for (face=ndeb; face<nfin; face++) {                                \
            double flux=flux_evaluateur.secmem_fa7_sortie_libre(face, cl, ndeb); \
            if ( (elem(face,0)) > -1)                                        \
              resu[face]+=flux;                                                \
            if ( (elem(face,1)) > -1)                                        \
              resu[face]-=flux;                                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      case symetrie :                                                        \
        break;                                                                \
      case entree_fluide :                                                \
        break;                                                                \
      case paroi_fixe :                                                        \
        break;                                                                \
      case paroi_defilante :                                                \
        break;                                                                \
      case paroi_adiabatique :                                                \
        break;                                                                \
      case paroi:                                                        \
        break;                                                                \
      case echange_externe_impose :                                        \
        break;                                                                \
      case echange_global_impose :                                        \
        break;                                                                \
      case periodique :                                                        \
        break;                                                                \
      default :                                                                \
        Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }

#define implemente6_It_VDF_Face(_TYPE_)                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& resu, int ncomp) const \
  {                                                                        \
    int num_cl;                                                        \
    int ndeb, nfin;                                                        \
    int face,k;                                                        \
    DoubleVect flux(ncomp);                                                \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    for (num_cl=0; num_cl<nb_front_Cl; num_cl++) {                        \
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());        \
      ndeb = frontiere_dis.num_premiere_face();                                \
      nfin = ndeb + frontiere_dis.nb_faces();                                \
      switch(type_cl(la_cl)){                                                \
      case sortie_libre :                                                \
        if (flux_evaluateur.calculer_fa7_sortie_libre()){                \
          const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur()); \
          for (face=ndeb; face<nfin; face++) {                                \
            flux_evaluateur.secmem_fa7_sortie_libre(face, cl, ndeb, flux); \
            if ( (elem(face,0)) > -1)                                        \
              for (k=0; k<ncomp; k++)                                        \
                resu(face,k) += flux(k);                                \
            if ( (elem(face,1)) > -1)                                        \
              for (k=0; k<ncomp; k++)                                        \
                resu(face,k) -= flux(k);                                \
            for (k=0; k<ncomp; k++)                                        \
              tab_flux_bords(face,k)-=flux(k);                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      case symetrie :                                                        \
        break;                                                                \
      case entree_fluide :                                                \
        break;                                                                \
      case paroi_fixe :                                                        \
        break;                                                                \
      case paroi_defilante :                                                \
        break;                                                                \
      case paroi_adiabatique :                                                \
        break;                                                                \
      case paroi:                                                        \
        break;                                                                \
      case echange_externe_impose :                                        \
        break;                                                                \
      case echange_global_impose :                                        \
        break;                                                                \
      case periodique :                                                        \
        break;                                                                \
      default :                                                                \
        Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_fa7_elem(DoubleTab& resu) const        \
  {                                                                        \
    double flux;                                                        \
    int fa7, fac1, fac2;                                                \
    int num_elem=0;                                                        \
    for( ; num_elem<nb_elem; num_elem++){                                \
      for (fa7=0; fa7<dimension; fa7++) {                                \
        fac1=elem_faces(num_elem,fa7);                                        \
        fac2=elem_faces(num_elem,fa7+dimension);                        \
        flux=flux_evaluateur.secmem_fa7_elem(num_elem, fac1, fac2);        \
        resu[fac1] += flux;                                                \
        resu[fac2] -= flux;                                                \
      }                                                                        \
    }                                                                        \
    corriger_secmem_fa7_elem_periodicite(resu);                                \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::contribuer_au_second_membre_fa7_elem(DoubleTab& resu, int ncomp) const \
  {                                                                        \
    int fa7, fac1, fac2;                                                \
    int num_elem;                                                        \
    int k;                                                                \
    DoubleVect flux(ncomp);                                                \
    for(num_elem=0; num_elem<nb_elem; num_elem++){                        \
      for (fa7=0; fa7<dimension; fa7++) {                                \
        fac1=elem_faces(num_elem,fa7);                                        \
        fac2=elem_faces(num_elem,fa7+dimension);                        \
        flux_evaluateur.secmem_fa7_elem(num_elem, fac1, fac2, flux);        \
        for (k=0; k<ncomp; k++) {                                        \
          resu(fac1,k) += flux(k);                                        \
          resu(fac2,k) -= flux(k);                                        \
        }                                                                \
      }                                                                        \
    }                                                                        \
    corriger_secmem_fa7_elem_periodicite(resu,ncomp);                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::corriger_secmem_fa7_elem_periodicite(DoubleTab& resu, int ncomp) const \
  {                                                                        \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)                        \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
        if (sub_type(Periodique,la_cl.valeur()))                        \
          {                                                                \
            int ndeb,nfin,elem1,elem2,num_elem,ori;                        \
            int face,fac1,fac2,signe;                                \
            DoubleVect flux(ncomp);                                        \
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur()); \
            const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis()); \
            ndeb = le_bord.num_premiere_face();                                \
            nfin = ndeb + le_bord.nb_faces();                                \
            for (face=ndeb; face<nfin; face++)                                \
              {                                                                \
                elem1 = elem(face,0);                                        \
                elem2 = elem(face,1);                                        \
                ori = orientation(face);                                \
                if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) ) \
                  {                                                        \
                    num_elem = elem2;                                        \
                    signe = 1;                                                \
                  }                                                        \
                else                                                        \
                  {                                                        \
                    num_elem = elem1;                                        \
                    signe = -1;                                                \
                  }                                                        \
                fac1 = elem_faces(num_elem,ori);                        \
                fac2 = elem_faces(num_elem,ori+dimension);                \
                flux_evaluateur.secmem_fa7_elem(num_elem,fac1,fac2,flux); \
                for (int k=0; k<ncomp; k++)                                \
                  resu(face,k) += signe*flux(k);                        \
              }                                                                \
          }                                                                \
      }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::corriger_secmem_fa7_elem_periodicite(DoubleTab& resu) const        \
  {                                                                        \
    const Zone_Cl_VDF& la_zcl_vdf = ref_cast(Zone_Cl_VDF,la_zcl.valeur()); \
    int nb_cond_lim = la_zcl_vdf.nb_cond_lim();                        \
    for (int num_cl=0; num_cl<nb_cond_lim; num_cl++)                        \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl_vdf.les_conditions_limites(num_cl); \
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis()); \
        if (sub_type(Periodique,la_cl.valeur()))                        \
          {                                                                \
            int ndeb,nfin,elem1,elem2,num_elem,ori;                        \
            int face,fac1,fac2,signe;                                \
            double flux;                                                \
            /* const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur()); */ \
            ndeb = le_bord.num_premiere_face();                                \
            nfin = ndeb + le_bord.nb_faces();                                \
            for (face=ndeb; face<nfin; face++)                                \
              {                                                                \
                elem1 = elem(face,0);                                        \
                elem2 = elem(face,1);                                        \
                ori = orientation(face);                                \
                if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) ) \
                  {                                                        \
                    num_elem = elem2;                                        \
                    signe = 1;                                                \
                  }                                                        \
                else                                                        \
                  {                                                        \
                    num_elem = elem1;                                        \
                    signe = -1;                                                \
                  }                                                        \
                fac1 = elem_faces(num_elem,ori);                        \
                fac2 = elem_faces(num_elem,ori+dimension);                \
                flux=flux_evaluateur.secmem_fa7_elem(num_elem,fac1,fac2); \
                resu(face) += signe*flux;                                \
              }                                                                \
          }                                                                \
      }                                                                        \
  }

#define implemente7_It_VDF_Face(_TYPE_)                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const \
  {                                                                        \
    ((_TYPE_&) flux_evaluateur).mettre_a_jour();                        \
    assert(inco.nb_dim() < 3);                                                \
    int ncomp=1;                                                        \
    if (inco.nb_dim() == 2)                                                \
      ncomp=inco.dimension(1);                                                \
    DoubleTab& tab_flux_bords=op_base->flux_bords();                        \
    tab_flux_bords.resize(la_zone->nb_faces_bord(),ncomp);                        \
    tab_flux_bords=0;                                                        \
    if( ncomp == 1)                                                        \
      {                                                                        \
        ajouter_contribution_aretes_bords(inco, matrice) ;                \
        ajouter_contribution_aretes_coins(inco, matrice) ;                \
        ajouter_contribution_aretes_internes(inco, matrice );                \
        ajouter_contribution_aretes_mixtes(inco, matrice);                \
        ajouter_contribution_fa7_sortie_libre(inco, matrice);                \
        ajouter_contribution_fa7_elem(inco, matrice);                        \
      }                                                                        \
    else                                                                \
      {                                                                        \
        ajouter_contribution_aretes_bords(inco, matrice, ncomp) ;        \
        ajouter_contribution_aretes_coins(inco, matrice, ncomp) ;        \
        ajouter_contribution_aretes_internes(inco, matrice, ncomp) ;        \
        ajouter_contribution_aretes_mixtes(inco, matrice, ncomp);        \
        ajouter_contribution_fa7_sortie_libre(inco, matrice, ncomp);        \
        ajouter_contribution_fa7_elem(inco, matrice, ncomp);                \
      }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_bords(const DoubleTab& inco, Matrice_Morse& matrice ) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int nb_face_reelle=la_zone->nb_faces();                                \
    double aii=0, ajj=0, aii1_2=0, aii3_4=0, ajj1_2=0;                        \
    int n_type;                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    for (n_arete=premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++) {        \
      n_type=type_arete_bord(n_arete-premiere_arete_bord);                \
      switch(n_type){                                                        \
      case TypeAreteBordVDF::PAROI_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_paroi(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          matrice(fac3,fac3)+=signe*aii3_4;                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_fluide()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_fluide(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);        \
          matrice(fac3,fac3)+=signe*aii3_4;                                \
          matrice(fac1,fac1)+=aii1_2;                                        \
          matrice(fac1,fac2)-=ajj1_2;                                        \
          matrice(fac2,fac1)-=aii1_2;                                        \
          matrice(fac2,fac2)+=ajj1_2;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_paroi_fluide(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          matrice(fac3,fac3)+=signe*aii3_4;                                \
          matrice(fac1,fac1)+=aii1_2;                                        \
          matrice(fac1,fac2)-=ajj1_2;                                        \
          matrice(fac2,fac1)-=aii1_2;                                        \
          matrice(fac2,fac2)+=ajj1_2;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::SYM_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_symetrie(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          matrice(fac3,fac3)+=signe*aii3_4;                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_periodicite(fac3, fac4, fac1, fac2, aii, ajj); \
          if (fac1<nb_face_reelle) {                                        \
            matrice(fac1,fac1)+=aii;                                        \
            matrice(fac1,fac2)-=ajj;                                        \
          }                                                                \
          if (fac2<nb_face_reelle) {                                        \
            matrice(fac2,fac1)-=aii;                                        \
            matrice(fac2,fac2)+=ajj;                                        \
          }                                                                \
          flux_evaluateur.coeffs_arete_periodicite(fac1, fac2, fac3, fac4, aii, ajj); \
          aii*=0.5;ajj*=0.5;                                                \
          if (fac3<nb_face_reelle) {                                        \
            matrice(fac3,fac3)+=aii;                                        \
            matrice(fac3,fac4)-=ajj;                                        \
          }                                                                \
          if (fac4<nb_face_reelle) {                                        \
            matrice(fac4,fac3)-=aii;                                        \
            matrice(fac4,fac4)+=ajj;                                        \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie_paroi()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_symetrie_paroi(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);        \
          matrice(fac3,fac3)+=signe*aii3_4;                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_SYM:                                \
        if (flux_evaluateur.calculer_arete_symetrie_fluide()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_symetrie_fluide(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          matrice(fac3,fac3)+=signe*aii3_4;                                \
          matrice(fac1,fac1)+=aii1_2;                                        \
          matrice(fac1,fac2)-=ajj1_2;                                        \
          matrice(fac2,fac1)-=aii1_2;                                        \
          matrice(fac2,fac2)+=ajj1_2;                                        \
        }                                                                \
        break;                                                                \
      default :                                                                \
        Cerr << "On a rencontre un type d'arete non prevu\n";                \
        Cerr << "num arete : " << n_arete;                                \
        Cerr << " type : " << n_type;                                        \
        exit();                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_bords(const DoubleTab& inco, Matrice_Morse& matrice ,int ncomp) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    DoubleVect aii(ncomp), ajj(ncomp), aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp), ajj3_4(ncomp); \
    int n_type;                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    int k,i;                                                                \
    IntVect& tab1 = matrice.get_set_tab1();				\
    IntVect& tab2 = matrice.get_set_tab2();				\
    DoubleVect& coeff = matrice.get_set_coeff();			\
    for (n_arete=premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++) {        \
      n_type=type_arete_bord(n_arete-premiere_arete_bord);                \
      switch(n_type){                                                        \
      case TypeAreteBordVDF::PAROI_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_paroi(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          for (i=0; i<ncomp; i++)                                        \
            {                                                                \
              for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac3*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=signe*aii3_4(i);                        \
                    }                                                        \
                }                                                        \
            }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_fluide()) {                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_fluide(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);        \
          for (i=0; i< ncomp; i++ ){                                        \
            for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac3*ncomp+i)                                \
                  {                                                        \
                    coeff[k]+=signe*aii3_4(i);                                \
                  }                                                        \
              }                                                                \
            for (k=tab1[fac1*ncomp+i]-1; k<tab1[fac1*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac1*ncomp+i)                                \
                  {                                                        \
                    /* correction de coef[k,i] on fait planter */        \
                    assert(0);exit();                                        \
                    coeff[k]+=aii1_2(i);                                \
                  }                                                        \
                if (tab2[k]-1==fac2*ncomp+i)                                \
                  {                                                        \
                    coeff[k]-=ajj1_2(i);                                \
                  }                                                        \
              }                                                                \
            for (k=tab1[fac2*ncomp+i]-1; k<tab1[fac2*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac1*ncomp+i)                                \
                  {                                                        \
                    coeff[k]-=aii1_2(i);                                \
                  }                                                        \
                if (tab2[k]-1==fac2*ncomp+i)                                \
                  {                                                        \
                    coeff[k]+=ajj1_2(i);                                \
                  }                                                        \
              }                                                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_FLUIDE:                                \
        if (flux_evaluateur.calculer_arete_paroi_fluide()) {                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_paroi_fluide(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          for (i=0; i< ncomp; i++ )                                        \
            {                                                                \
              for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac3*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=signe*aii3_4(i);                        \
                    }                                                        \
                }                                                        \
              for (k=tab1[fac1*ncomp+i]-1; k<tab1[fac1*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac1*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=aii1_2(i);                                \
                    }                                                        \
                  if (tab2[k]-1==fac2*ncomp+i)                                \
                    {                                                        \
                      coeff[k]-=ajj1_2(i);                                \
                    }                                                        \
                }                                                        \
              for (k=tab1[fac2*ncomp+i]-1; k<tab1[fac2*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac1*ncomp+i)                                \
                    {                                                        \
                      coeff[k]-=aii1_2(i);                                \
                    }                                                        \
                  if (tab2[k]-1==fac2*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=ajj1_2(i);                                \
                    }                                                        \
                }                                                        \
            }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::SYM_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie()) {                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_symetrie(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          for (i=0; i< ncomp; i++ )                                        \
            {                                                                \
              for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac3*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=signe*aii3_4(i);                        \
                    }                                                        \
                }                                                        \
            }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()) {                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_periodicite(fac3, fac4, fac1, fac2, aii, ajj); \
          for (i=0; i< ncomp; i++ )                                        \
            {                                                                \
              for (k=tab1[fac1*ncomp+i]-1; k<tab1[fac1*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac1*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=aii(i);                                        \
                    }                                                        \
                  if (tab2[k]-1==fac2*ncomp+i)                                \
                    {                                                        \
                      coeff[k]-=ajj(i);                                        \
                    }                                                        \
                }                                                        \
              for (k=tab1[fac2*ncomp+i]-1; k<tab1[fac2*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac1*ncomp+i)                                \
                    {                                                        \
                      coeff[k]-=aii(i);                                        \
                    }                                                        \
                  if (tab2[k]-1==fac2*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=ajj(i);                                        \
                    }                                                        \
                }                                                        \
            }                                                                \
          flux_evaluateur.coeffs_arete_periodicite(fac1, fac2, fac3, fac4, aii, ajj); \
          for (i=0; i< ncomp; i++ )                                        \
            {                                                                \
              for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+i+1]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac3*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=aii(i);                                        \
                    }                                                        \
                  if (tab2[k]-1==fac4*ncomp+i)                                \
                    {                                                        \
                      coeff[k]-=ajj(i);                                        \
                    }                                                        \
                }                                                        \
              for (k=tab1[fac4*ncomp+i]-1; k<tab1[fac4*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac3*ncomp+i)                                \
                    {                                                        \
                      coeff[k]-=aii(i);                                        \
                    }                                                        \
                  if (tab2[k]-1==fac4*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=ajj(i);                                        \
                    }                                                        \
                }                                                        \
            }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::PAROI_SYM:                                        \
        if (flux_evaluateur.calculer_arete_symetrie_paroi()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_symetrie_paroi(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);        \
          for (i=0; i<ncomp; i++)                                        \
            {                                                                \
              for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++) \
                {                                                        \
                  if (tab2[k]-1==fac3*ncomp+i)                                \
                    {                                                        \
                      coeff[k]+=signe*aii3_4(i);                        \
                    }                                                        \
                }                                                        \
            }                                                                \
        }                                                                \
        break;                                                                \
      case TypeAreteBordVDF::FLUIDE_SYM:                                \
        if (flux_evaluateur.calculer_arete_symetrie_fluide()) {                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_symetrie_fluide(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          for (i=0; i< ncomp; i++ ){                                        \
            for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac3*ncomp+i)                                \
                  {                                                        \
                    coeff[k]+=signe*aii3_4(i);                                \
                  }                                                        \
              }                                                                \
            for (k=tab1[fac1*ncomp+i]-1; k<tab1[fac1*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac1*ncomp+i)                                \
                  {                                                        \
                    /* correction de coef[k,i] on fait planter */        \
                    assert(0);exit();                                        \
                    coeff[k]+=aii1_2(i);                                \
                  }                                                        \
                if (tab2[k]-1==fac2*ncomp+i)                                \
                  {                                                        \
                    coeff[k]-=ajj1_2(i);                                \
                  }                                                        \
              }                                                                \
            for (k=tab1[fac2*ncomp+i]-1; k<tab1[fac2*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac1*ncomp+i)                                \
                  {                                                        \
                    coeff[k]-=aii1_2(i);                                \
                  }                                                        \
                if (tab2[k]-1==fac2*ncomp+i)                                \
                  {                                                        \
                    coeff[k]+=ajj1_2(i);                                \
                  }                                                        \
              }                                                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      default :                                                                \
        {                                                                \
          Cerr << "On a rencontre un type d'arete non prevu\n";                \
          Cerr << "num arete : " << n_arete;                                \
          Cerr << " type : " << n_type;                                        \
          exit();                                                        \
        }                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_coins(const DoubleTab&inco , Matrice_Morse& matrice ) const \
  {                                                                        \
    int n_arete;                                                        \
    int signe;                                                        \
    int n_type;                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    double aii=0, ajj=0, aii1_2, aii3_4=0, ajj1_2;                        \
    for (n_arete=premiere_arete_coin; n_arete<derniere_arete_coin; n_arete++) {        \
      n_type=type_arete_coin(n_arete-premiere_arete_coin);                \
      switch(n_type){                                                        \
      case TypeAreteCoinVDF::PERIO_PERIO:                                \
        if (flux_evaluateur.calculer_arete_periodicite()){                \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          fac4 = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_periodicite(fac3, fac4, fac1, fac2, aii, ajj); \
          matrice(fac1,fac1)+=aii;                                        \
          matrice(fac1,fac2)-=ajj;                                        \
          matrice(fac2,fac1)-=aii;                                        \
          matrice(fac2,fac2)+=ajj;                                        \
          flux_evaluateur.coeffs_arete_periodicite(fac1, fac2, fac3, fac4, aii, ajj); \
          matrice(fac3,fac3)+=aii;                                        \
          matrice(fac3,fac4)-=ajj;                                        \
          matrice(fac4,fac3)-=aii;                                        \
          matrice(fac4,fac4)+=ajj;                                        \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PERIO_PAROI:                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          flux_evaluateur.coeffs_arete_paroi(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2); \
          matrice(fac3,fac3)+=signe*aii3_4;                                \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::PAROI_FLUIDE:                                \
        exit(); /* pas fini */                                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          /* flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe);  */ \
          /*               resu[fac3]+=signe*flux;        */                \
        }                                                                \
        break;                                                                \
      case TypeAreteCoinVDF::FLUIDE_PAROI:                                \
        exit(); /* pas fini */                                                \
        if (flux_evaluateur.calculer_arete_paroi()){                        \
          fac1 = Qdm(n_arete,0);                                        \
          fac2 = Qdm(n_arete,1);                                        \
          fac3 = Qdm(n_arete,2);                                        \
          signe = Qdm(n_arete,3);                                        \
          /* flux=flux_evaluateur.flux_arete_paroi(inco, fac1, fac2, fac3, signe); \
             resu[fac3]+=signe*flux; */                                        \
        }                                                                \
        break;                                                                \
      default :                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void   It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_coins(const DoubleTab&, Matrice_Morse&,int ) const \
  { cerr<<"non code "<<endl;exit(); }
#define implemente8_It_VDF_Face(_TYPE_)                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_internes(const DoubleTab& inco,        \
                                                                 Matrice_Morse& matrice) const \
  {                                                                        \
    if(!flux_evaluateur.calculer_arete_interne())                        \
      return;                                                                \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete;                                                        \
    double aii=0, ajj=0;                                                \
    for (n_arete=premiere_arete_interne; n_arete<derniere_arete_interne; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.coeffs_arete_interne(fac3, fac4, fac1, fac2, aii, ajj); \
      matrice(fac1,fac1)+=aii;                                                \
      matrice(fac1,fac2)-=ajj;                                                \
      matrice(fac2,fac1)-=aii;                                                \
      matrice(fac2,fac2)+=ajj;                                                \
      flux_evaluateur.coeffs_arete_interne(fac1, fac2, fac3, fac4, aii, ajj); \
      matrice(fac3,fac3)+=aii;                                                \
      matrice(fac3,fac4)-=ajj;                                                \
      matrice(fac4,fac3)-=aii;                                                \
      matrice(fac4,fac4)+=ajj;                                                \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_internes(const DoubleTab& inco,        \
                                                                 Matrice_Morse& matrice, int ncomp) const \
  {                                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    DoubleVect aii(ncomp), ajj(ncomp);                                        \
    IntVect& tab1 = matrice.get_set_tab1();				\
    IntVect& tab2 = matrice.get_set_tab2();					\
    DoubleVect& coeff = matrice.get_set_coeff();				\
    int n_arete,k,i;                                                        \
    for (n_arete=premiere_arete_interne; n_arete<derniere_arete_interne; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.coeffs_arete_interne(fac3, fac4, fac1, fac2, aii, ajj); \
      for (i=0; i< ncomp; i++ )                                                \
        {                                                                \
          for (k=tab1[fac1*ncomp+i]-1; k<tab1[fac1*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac1*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac2*ncomp+i)                                \
                {                                                        \
                  coeff[k]-=ajj(i);                                        \
                }                                                        \
            }                                                                \
          for (k=tab1[fac2*ncomp+i]-1; k<tab1[fac2*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac1*ncomp+i)                                \
                {                                                        \
                  coeff[k]-=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac2*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=ajj(i);                                        \
                }                                                        \
            }                                                                \
        }                                                                \
      flux_evaluateur.coeffs_arete_interne(fac1, fac2, fac3, fac4, aii, ajj); \
      for (i=0; i< ncomp; i++ )                                                \
        {                                                                \
          for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac3*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac4*ncomp+i)                                \
                {                                                        \
                  coeff[k]-=ajj(i);                                        \
                }                                                        \
            }                                                                \
          for (k=tab1[fac4*ncomp+i]-1; k<tab1[fac4*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac3*ncomp+i)                                \
                {                                                        \
                  coeff[k]-=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac4*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=ajj(i);                                        \
                }                                                        \
            }                                                                \
        }                                                                \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_mixtes(const DoubleTab& inco, \
                                                               Matrice_Morse& matrice) const \
  {                                                                        \
    if(!flux_evaluateur.calculer_arete_mixte())                                \
      return ;                                                                \
    int fac1, fac2, fac3, fac4;                                        \
    double aii=0, ajj=0;                                                \
    int n_arete;                                                        \
    for (n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.coeffs_arete_mixte(fac1, fac2, fac3, fac4, aii, ajj); \
      matrice(fac3,fac3)+=aii;                                                \
      matrice(fac3,fac4)-=ajj;                                                \
      matrice(fac4,fac3)-=aii;                                                \
      matrice(fac4,fac4)+=ajj;                                                \
      flux_evaluateur.coeffs_arete_mixte(fac3, fac4, fac1, fac2, aii, ajj); \
      matrice(fac1,fac1)+=aii;                                                \
      matrice(fac1,fac2)-=ajj;                                                \
      matrice(fac2,fac1)-=aii;                                                \
      matrice(fac2,fac2)+=ajj;                                                \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_aretes_mixtes(const DoubleTab& inco, \
                                                               Matrice_Morse& matrice, int ncomp) const \
  {                                                                        \
    int fac1, fac2, fac3, fac4;                                        \
    int n_arete,k,i;                                                        \
    IntVect& tab1 = matrice.get_set_tab1();				\
    IntVect& tab2 = matrice.get_set_tab2();					\
    DoubleVect& coeff = matrice.get_set_coeff();			\
    DoubleVect aii(ncomp), ajj(ncomp);                                        \
    for (n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++) { \
      fac1=Qdm(n_arete,0);                                                \
      fac2=Qdm(n_arete,1);                                                \
      fac3=Qdm(n_arete,2);                                                \
      fac4=Qdm(n_arete,3);                                                \
      flux_evaluateur.coeffs_arete_mixte(fac1, fac2, fac3, fac4, aii, ajj); \
      for (i=0; i< ncomp; i++ )                                                \
        {                                                                \
          for (k=tab1[fac1*ncomp+i]-1; k<tab1[fac1*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac1*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac2*ncomp+i)                                \
                {                                                        \
                  coeff[k]-=ajj(i);                                        \
                }                                                        \
            }                                                                \
          for (k=tab1[fac2*ncomp+i]-1; k<tab1[fac2*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac1*ncomp+i)                                \
                {                                                        \
                  coeff[k]-=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac2*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=ajj(i);                                        \
                }                                                        \
            }                                                                \
        }                                                                \
      flux_evaluateur.coeffs_arete_mixte(fac3, fac4, fac1, fac2, aii, ajj); \
      for (i=0; i< ncomp; i++ )                                                \
        {                                                                \
          for (k=tab1[fac3*ncomp+i]-1; k<tab1[fac3*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac3*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac4*ncomp+i)                                \
                {                                                        \
                  coeff[k*i]-=ajj(i);                                        \
                }                                                        \
            }                                                                \
          for (k=tab1[fac4*ncomp+i]-1; k<tab1[fac4*ncomp+1+i]-1; k++)        \
            {                                                                \
              if (tab2[k]-1==fac3*ncomp+i)                                \
                {                                                        \
                  coeff[k]-=aii(i);                                        \
                }                                                        \
              if (tab2[k]-1==fac4*ncomp+i)                                \
                {                                                        \
                  coeff[k]+=ajj(i);                                        \
                }                                                        \
            }                                                                \
        }                                                                \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_fa7_sortie_libre(const DoubleTab& inco, Matrice_Morse& matrice ) const        \
  {                                                                        \
    int num_cl;                                                        \
    int ndeb, nfin,k;                                                \
    double aii=0, ajj=0;                                                \
    IntVect& tab1 = matrice.get_set_tab1();				\
    IntVect& tab2 = matrice.get_set_tab2();					\
    DoubleVect& coeff = matrice.get_set_coeff();			\
    int face;                                                        \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    for (num_cl=0; num_cl<nb_front_Cl; num_cl++) {                        \
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());        \
      ndeb = frontiere_dis.num_premiere_face();                                \
      nfin = ndeb + frontiere_dis.nb_faces();                                \
      switch(type_cl(la_cl)){                                                \
      case sortie_libre :                                                \
        if (flux_evaluateur.calculer_fa7_sortie_libre()){                \
          const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur()); \
          for (face=ndeb; face<nfin; face++) {                                \
            flux_evaluateur.coeffs_fa7_sortie_libre(face, cl, aii, ajj); \
            if ( (elem(face,0)) > -1)                                        \
              {                                                                \
                for (k=tab1[face]-1; k<tab1[face+1]-1; k++){                \
                  if (tab2[k]-1==face)                                        \
                    coeff[k]+=aii;                                        \
                }                                                        \
              }                                                                \
            if ( (elem(face,1)) > -1)                                        \
              {                                                                \
                for (k=tab1[face]-1; k<tab1[face+1]-1; k++){                \
                  if (tab2[k]-1==face)                                        \
                    coeff[k]+=ajj;                                        \
                }                                                        \
              }                                                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      case symetrie :                                                        \
        break;                                                                \
      case entree_fluide :                                                \
        break;                                                                \
      case paroi_fixe :                                                        \
        break;                                                                \
      case paroi_defilante :                                                \
        break;                                                                \
      case paroi_adiabatique :                                                \
        break;                                                                \
      case paroi:                                                        \
        break;                                                                \
      case echange_externe_impose :                                        \
        break;                                                                \
      case echange_global_impose :                                        \
        break;                                                                \
      case periodique :                                                        \
        break;                                                                \
      default :                                                                \
        Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }

#define implemente9_It_VDF_Face(_TYPE_)                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_fa7_sortie_libre(const DoubleTab& inco, Matrice_Morse& matrice , int ncomp) const \
  {                                                                        \
    int num_cl;                                                        \
    int ndeb, nfin;                                                        \
    IntVect& tab1 = matrice.get_set_tab1();				\
    IntVect& tab2 = matrice.get_set_tab2();					\
    DoubleVect& coeff = matrice.get_set_coeff();			\
    DoubleVect aii(ncomp), ajj(ncomp);                                        \
    int face,k,i;                                                        \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    for (num_cl=0; num_cl<nb_front_Cl; num_cl++) {                        \
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());        \
      ndeb = frontiere_dis.num_premiere_face();                                \
      nfin = ndeb + frontiere_dis.nb_faces();                                \
      switch(type_cl(la_cl)){                                                \
      case sortie_libre :                                                \
        if (flux_evaluateur.calculer_fa7_sortie_libre()){                \
          const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur()); \
          for (face=ndeb; face<nfin; face++) {                                \
            flux_evaluateur.coeffs_fa7_sortie_libre(face, cl, aii, ajj); \
            if ( (elem(face,0)) > -1)                                        \
              {                                                                \
                for (i=0; i< ncomp; i++ )                                \
                  {                                                        \
                    for (k=tab1[face*ncomp+i]-1; k<tab1[face*ncomp+1+i]-1; k++)        \
                      {                                                        \
                        if (tab2[k]-1==face*ncomp+i)                        \
                          {                                                \
                            coeff[k]+=aii(i);                                \
                          }                                                \
                      }                                                        \
                  }                                                        \
              }                                                                \
            if ( (elem(face,1)) > -1)                                        \
              {                                                                \
                for (i=0; i< ncomp; i++ )                                \
                  {                                                        \
                    for (k=tab1[face*ncomp+i]-1; k<tab1[face*ncomp+1+i]-1; k++)        \
                      {                                                        \
                        if (tab2[k]-1==face*ncomp+i)                        \
                          {                                                \
                            coeff[k]+=ajj(i);                                \
                          }                                                \
                      }                                                        \
                  }                                                        \
              }                                                                \
          }                                                                \
        }                                                                \
        break;                                                                \
      case symetrie :                                                        \
        break;                                                                \
      case entree_fluide :                                                \
        break;                                                                \
      case paroi_fixe :                                                        \
        break;                                                                \
      case paroi_defilante :                                                \
        break;                                                                \
      case paroi_adiabatique :                                                \
        break;                                                                \
      case paroi :                                                        \
        break;                                                                \
      case echange_externe_impose :                                        \
        break;                                                                \
      case echange_global_impose :                                        \
        break;                                                                \
      case periodique :                                                        \
        break;                                                                \
      default :                                                                \
        Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();        \
        exit();                                                                \
        break;                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_fa7_elem(const DoubleTab& inco, Matrice_Morse& matrice) const \
  {                                                                        \
    int fa7, fac1, fac2;                                                \
    int num_elem=0;                                                        \
    double aii=0, ajj=0;                                                \
    for( ; num_elem<nb_elem; num_elem++){                                \
      for (fa7=0; fa7<dimension; fa7++) {                                \
        fac1=elem_faces(num_elem,fa7);                                        \
        fac2=elem_faces(num_elem,fa7+dimension);                        \
        flux_evaluateur.coeffs_fa7_elem(num_elem, fac1, fac2, aii, ajj); \
        matrice(fac1,fac1)+=aii;                                        \
        matrice(fac1,fac2)-=ajj;                                        \
        matrice(fac2,fac1)-=aii;                                        \
        matrice(fac2,fac2)+=ajj;                                        \
      }                                                                        \
    }                                                                        \
    corriger_coeffs_fa7_elem_periodicite(inco, matrice);                \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::ajouter_contribution_fa7_elem(const DoubleTab& inco, Matrice_Morse& matrice, int ncomp ) const \
  {                                                                        \
    int fa7, fac1, fac2;                                                \
    int num_elem;                                                        \
    IntVect& tab1 = matrice.get_set_tab1();				\
    IntVect& tab2 = matrice.get_set_tab2();					\
    DoubleVect& coeff = matrice.get_set_coeff();				\
    DoubleVect aii(ncomp), ajj(ncomp);                                        \
    int k,i;                                                                \
    for(num_elem=0; num_elem<nb_elem; num_elem++){                        \
      for (fa7=0; fa7<dimension; fa7++) {                                \
        fac1=elem_faces(num_elem,fa7);                                        \
        fac2=elem_faces(num_elem,fa7+dimension);                        \
        flux_evaluateur.coeffs_fa7_elem(num_elem, fac1, fac2, aii, ajj); \
        for (i=0; i< ncomp; i++ )                                        \
          {                                                                \
            for (k=tab1[fac1*ncomp+i]-1; k<tab1[fac1*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac1*ncomp+i)                                \
                  {                                                        \
                    coeff[k]+=aii(i);                                        \
                  }                                                        \
                if (tab2[k]-1==fac2*ncomp+i)                                \
                  {                                                        \
                    coeff[k]-=ajj(i);                                        \
                  }                                                        \
              }                                                                \
            for (k=tab1[fac2*ncomp+i]-1; k<tab1[fac2*ncomp+1+i]-1; k++) \
              {                                                                \
                if (tab2[k]-1==fac1*ncomp+i)                                \
                  {                                                        \
                    coeff[k]-=aii(i);                                        \
                  }                                                        \
                if (tab2[k]-1==fac2*ncomp+i)                                \
                  {                                                        \
                    coeff[k]+=ajj(i);                                        \
                  }                                                        \
              }                                                                \
          }                                                                \
      }                                                                        \
    }                                                                        \
    corriger_coeffs_fa7_elem_periodicite(inco, matrice, ncomp);                \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::corriger_coeffs_fa7_elem_periodicite(const DoubleTab& inco, Matrice_Morse& matrice) const \
  {                                                                        \
    int nb_front_Cl=la_zone->nb_front_Cl();                                \
    for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)                        \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);        \
        if (sub_type(Periodique,la_cl.valeur()))                        \
          {                                                                \
            int ndeb,nfin,elem1,elem2,num_elem,ori;                        \
            int face,fac1,fac2,signe;                                \
            double aii=0, ajj=0;                                        \
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur()); \
            const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis()); \
            ndeb = le_bord.num_premiere_face();                                \
            nfin = ndeb + le_bord.nb_faces();                                \
            for (face=ndeb; face<nfin; face++)                                \
              {                                                                \
                elem1 = elem(face,0);                                        \
                elem2 = elem(face,1);                                        \
                ori = orientation(face);                                \
                if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) ) \
                  {                                                        \
                    num_elem = elem2;                                        \
                    signe = 1;                                                \
                  }                                                        \
                else                                                        \
                  {                                                        \
                    num_elem = elem1;                                        \
                    signe = -1;                                                \
                  }                                                        \
                fac1 = elem_faces(num_elem,ori);                        \
                fac2 = elem_faces(num_elem,ori+dimension);                \
                flux_evaluateur.coeffs_fa7_elem(num_elem, fac1, fac2, aii, ajj); \
                {                                                        \
                  if (signe>0)                                                \
                    { /* on a oublie a droite  la contribution de la gche */ \
                      matrice(face,face)+=aii ;                                \
                      matrice(face,fac2)-=ajj;                                \
                    }  else { /* on a oublie a gauche  la contribution de la droite */ \
                    matrice(face,fac1)-=aii; ;                                \
                    matrice(face,face)+=ajj; }                                \
                }                                                        \
              }                                                                \
          }                                                                \
      }                                                                        \
  }                                                                        \
  void It_VDF_Face(_TYPE_)::corriger_coeffs_fa7_elem_periodicite(const DoubleTab& inco, Matrice_Morse& matrice, int ncomp) const \
  {                                                                        \
    const Zone_Cl_VDF& la_zcl_vdf = ref_cast(Zone_Cl_VDF,la_zcl.valeur()); \
    int nb_cond_lim = la_zcl_vdf.nb_cond_lim();                        \
    for (int num_cl=0; num_cl<nb_cond_lim; num_cl++)                        \
      {                                                                        \
        const Cond_lim& la_cl = la_zcl_vdf.les_conditions_limites(num_cl); \
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis()); \
        if (sub_type(Periodique,la_cl.valeur()))                        \
          {                                                                \
            int ndeb,nfin,elem1,elem2,num_elem,ori,k,i;                \
            int face,fac1,fac2,signe;                                \
            IntVect& tab1 = matrice.get_set_tab1();			\
            IntVect& tab2 = matrice.get_set_tab2();				\
            DoubleVect& coeff = matrice.get_set_coeff();		\
            DoubleVect aii(ncomp), ajj(ncomp);                                \
            /* const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur()); */ \
            ndeb = le_bord.num_premiere_face();                                \
            nfin = ndeb + le_bord.nb_faces();                                \
            for (face=ndeb; face<nfin; face++)                                \
              {                                                                \
                elem1 = elem(face,0);                                        \
                elem2 = elem(face,1);                                        \
                ori = orientation(face);                                \
                if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) ) \
                  {                                                        \
                    num_elem = elem2;                                        \
                    signe = 1;                                                \
                  }                                                        \
                else                                                        \
                  {                                                        \
                    num_elem = elem1;                                        \
                    signe = -1;                                                \
                  }                                                        \
                fac1 = elem_faces(num_elem,ori);                        \
                fac2 = elem_faces(num_elem,ori+dimension);                \
                flux_evaluateur.coeffs_fa7_elem(num_elem, fac1, fac2, aii, ajj); \
                for (i=0; i< ncomp; i++ )                                \
                  {                                                        \
                    for (k=tab1[face*ncomp+i]-1; k<tab1[face*ncomp+1+i]-1; k++) \
                      {                                                        \
                        if (tab2[k]-1==face*ncomp+i) {                        \
                          coeff[k]+=signe*aii(i);                        \
                        }                                                \
                      }                                                        \
                  }                                                        \
              }                                                                \
          }                                                                \
      }                                                                        \
  }
#define implemente_It_VDF_Face(_TYPE_)                \
  implemente1_It_VDF_Face(_TYPE_)                \
    implemente2_It_VDF_Face(_TYPE_)                \
    implemente3_It_VDF_Face(_TYPE_)                \
    implemente4_It_VDF_Face(_TYPE_)                \
    implemente5_It_VDF_Face(_TYPE_)                \
    implemente6_It_VDF_Face(_TYPE_)                \
    implemente7_It_VDF_Face(_TYPE_)                \
    implemente8_It_VDF_Face(_TYPE_)                \
    implemente9_It_VDF_Face(_TYPE_)
#endif

