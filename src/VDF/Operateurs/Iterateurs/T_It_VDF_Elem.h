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
// File:        T_It_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Iterateurs
// Version:     /main/43
//
//////////////////////////////////////////////////////////////////////////////

#ifndef T_It_VDF_Elem_included
#define T_It_VDF_Elem_included

#include <Operateur_Diff_base.h>
#include <Schema_Temps_base.h>
#include <Op_Conv_VDF_base.h>
#include <Champ_Uniforme.h>
#include <Operateur_base.h>
#include <communications.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Milieu_base.h>
#include <DoubleTrav.h>
#include <Debog.h>

template <class _TYPE_>
class T_It_VDF_Elem : public Iterateur_VDF_base
{
  inline unsigned taille_memoire() const { throw; }
  inline int duplique() const
  {
    T_It_VDF_Elem* xxx = new  T_It_VDF_Elem(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  };

public:
  inline T_It_VDF_Elem() { }
  inline T_It_VDF_Elem(const T_It_VDF_Elem<_TYPE_>& iter) :
    Iterateur_VDF_base(iter), flux_evaluateur(iter.flux_evaluateur) { elem.ref(iter.elem); }

  int impr(Sortie& os) const;
  void modifier_flux() const;
  void calculer_flux_bord(const DoubleTab&) const;
  void contribuer_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;

  inline Evaluateur_VDF& evaluateur() { return (Evaluateur_VDF&) flux_evaluateur; }
  inline const Evaluateur_VDF& evaluateur() const { return (Evaluateur_VDF&) flux_evaluateur; }
  inline void completer_() { elem.ref(la_zone->face_voisins()); }

protected:
  _TYPE_ flux_evaluateur;
  IntTab elem;
  mutable SFichier Flux, Flux_moment, Flux_sum;

  void contribuer_au_second_membre_bords(DoubleTab& ) const;
  void contribuer_au_second_membre_interne(DoubleTab& ) const;
  void ajouter_contribution_bords(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_interne(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_interne_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_bords_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>&) const;
  DoubleTab& ajouter_bords(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& ajouter_interne(const DoubleTab& , DoubleTab& ) const;
  inline const Milieu_base& milieu() const { return (la_zcl->equation()).milieu(); }

private:
  inline void fill_flux_tables_(int, int , const DoubleVect& , DoubleTab& , DoubleTab& ) const;

  /* Private methods called from ajouter_bords */
  template <bool should_calc_flux, typename BC>
  void ajouter_bords_(const BC& , int ,int ,const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  void ajouter_bords_(const Periodique& , int ,int ,const DoubleTab&,const Front_VF& ,DoubleTab& , DoubleTab& ) const;
  void ajouter_bords_(const Echange_externe_impose& , int ,int ,int, const DoubleTab&,const Front_VF& ,DoubleTab& , DoubleTab& ) const;

  /* Private methods called from calculer_flux_bord */
  template <bool should_calc_flux, typename BC>
  void calculer_flux_bord_(const BC& , int ,int ,const DoubleTab& ,DoubleTab& ) const;

  void calculer_flux_bord_(const Periodique& , int ,int ,const DoubleTab& ,const Front_VF& , DoubleTab& ) const;
  void calculer_flux_bord_(const Echange_externe_impose& , int ,int ,int, const DoubleTab&,const Front_VF& ,DoubleTab& ) const;

  /* Private methods called from contribuer_au_second_membre_bords */
  template <bool should_calc_flux, typename BC>
  void contribuer_au_second_membre_bords_(const BC& , int ,int ,DoubleTab& , DoubleTab& ) const;

  void contribuer_au_second_membre_bords_(const Periodique& , int ,int ,DoubleTab& , DoubleTab& ) const;
  void contribuer_au_second_membre_bords_(const Echange_externe_impose& , int ,int , int,const Front_VF& ,DoubleTab& , DoubleTab& ) const;

  /* Private methods called from ajouter_contribution_bords */
  template <bool should_calc_flux, typename BC>
  void ajouter_contribution_bords_(const BC& , int ,int ,const DoubleTab& , Matrice_Morse& ) const;

  void ajouter_contribution_bords_(const Periodique& , int ,int ,const DoubleTab& , Matrice_Morse& ) const;
  void ajouter_contribution_bords_(const Echange_externe_impose& , int ,int ,int,const DoubleTab& ,const Front_VF&, Matrice_Morse& ) const;

  /* Private methods called from ajouter_contribution_bords */
  template <bool should_calc_flux, typename BC>
  void ajouter_contribution_bords_vitesse_(const BC& , int ,int ,const DoubleTab& , Matrice_Morse& ) const;

  void ajouter_contribution_bords_vitesse_(const Periodique& , int ,int ,const DoubleTab& ,const Front_VF&, Matrice_Morse& ) const;
  void ajouter_contribution_bords_vitesse_(const Echange_externe_impose& , int ,int ,int ,const DoubleTab& ,const Front_VF&, Matrice_Morse& ) const;
};

template <class _TYPE_>
DoubleTab& T_It_VDF_Elem<_TYPE_>::ajouter(const DoubleTab& donne, DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(donne.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  const int ncomp = donne.line_size();
  DoubleTab& flux_bords=op_base->flux_bords();
  flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
  flux_bords=0;
  /* modif b.m.: on va faire += sur des items virtuels, initialiser les cases */
  /* sinon risque que les cases soient invalides ou non initialisees */
  {
    int n = resu.size_array() - resu.size();
    double *data = resu.addr() + resu.size();
    for (; n; n--, data++) *data = 0.;
  }
  ajouter_bords(donne, resu) ;
  ajouter_interne(donne, resu) ;
  modifier_flux() ;
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Elem<_TYPE_>::ajouter_bords(const DoubleTab& donnee, DoubleTab& resu) const
{
  int ndeb, nfin, num_cl=0, nb_front_Cl=la_zone->nb_front_Cl();
  DoubleTab& flux_bords=op_base->flux_bords();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      /* Test en bidim axi */
      if (bidim_axi && !sub_type(Symetrie,la_cl.valeur()))
        {
          if (nfin>ndeb && est_egal(la_zone.valeur().face_surfaces()[ndeb],0))
            {
              Cerr << "Error in the definition of the boundary conditions. The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry boundary condition (symetrie keyword) for the boundary " << frontiere_dis.le_nom() << finl;
              Process::exit();
            }
        }
      switch(type_cl(la_cl))
        {
        case symetrie :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_SYMM>((const Symetrie&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case sortie_libre :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB>((const Neumann_sortie_libre&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case entree_fluide :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL>((const Dirichlet_entree_fluide&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case paroi_fixe :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE>((const Dirichlet_paroi_fixe&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case paroi_defilante :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL>((const Dirichlet_paroi_defilante&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case paroi :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR>((const Neumann_paroi&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case echange_global_impose :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP>((const Echange_global_impose&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case paroi_adiabatique :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB>((const Neumann_paroi_adiabatique&) la_cl.valeur(), ndeb, nfin,donnee,flux_bords,resu);
          break;
        case echange_externe_impose :
          ajouter_bords_((const Echange_externe_impose&) la_cl.valeur(),ndeb,nfin,num_cl,donnee,frontiere_dis,flux_bords,resu);
          break;
        case periodique :
          ajouter_bords_((const Periodique&) la_cl.valeur(), ndeb, nfin, donnee, frontiere_dis, flux_bords, resu);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::ajouter_bords" << finl;
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord(const DoubleTab& donnee) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(donnee.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  const int ncomp = donnee.line_size();
  DoubleTab& flux_bords=op_base->flux_bords();
  flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
  flux_bords=0;

  if( ncomp != 1) Process::exit(); /* cas scalaire */

  int ndeb, nfin, num_cl=0, nb_front_Cl=la_zone->nb_front_Cl();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      /* Test en bidim axi */
      if (bidim_axi && !sub_type(Symetrie,la_cl.valeur()))
        {
          if (nfin>ndeb && est_egal(la_zone.valeur().face_surfaces()[ndeb],0))
            {
              Cerr << "Error in the definition of the boundary conditions. The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry boundary condition (symetrie keyword) for the boundary " << frontiere_dis.le_nom() << finl;
              Process::exit();
            }
        }
      switch(type_cl(la_cl))
        {
        case symetrie :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_SYMM> ((const Symetrie&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case sortie_libre :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB> ((const Neumann_sortie_libre&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case entree_fluide :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL> ((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case paroi_fixe :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE> ((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case paroi_defilante :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL> ((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case paroi :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR> ((const Neumann_paroi&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case echange_global_impose :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP> ((const Echange_global_impose&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case paroi_adiabatique :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb, nfin,donnee,flux_bords);
          break;
        case periodique :
          calculer_flux_bord_((const Periodique&) la_cl.valeur(),ndeb, nfin,donnee,frontiere_dis,flux_bords);
          break;
        case echange_externe_impose :
          calculer_flux_bord_((const Echange_externe_impose&) la_cl.valeur(),ndeb, nfin,num_cl,donnee,frontiere_dis,flux_bords);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::calculer_flux_bord" << finl;
          Process::exit();
          break;
        }
    }
  modifier_flux() ;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Elem<_TYPE_>::ajouter_interne(const DoubleTab& donnee, DoubleTab& resu) const
{
  const int ncomp = resu.line_size();
  const Zone_VDF& zone_VDF = la_zone.valeur();
  DoubleVect flux(ncomp);
  int face,k,elem0,elem1;
  const int ndeb = zone_VDF.premiere_face_int(), nfin = zone_VDF.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      flux_evaluateur.flux_faces_interne(donnee, face, flux);
      elem0 = elem(face,0);
      elem1 = elem(face,1);
      for (k=0; k<ncomp; k++)
        {
          resu(elem0,k)+=flux(k);
          resu(elem1,k)-=flux(k);
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Elem<_TYPE_>::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  operator_egal(resu, 0., VECT_REAL_ITEMS);
  return ajouter(inco,resu);
}

template <class _TYPE_>
void  T_It_VDF_Elem<_TYPE_>::modifier_flux() const
{
  if (op_base->equation().inconnue().le_nom().debute_par("temperature")
      && !( sub_type(Operateur_Diff_base,op_base.valeur()) && ref_cast(Operateur_Diff_base,op_base.valeur()).diffusivite().le_nom() == "conductivite" ) )
    {
      DoubleTab& flux_bords=op_base->flux_bords();
      const Zone_VDF& la_zone_vdf=ref_cast(Zone_VDF,op_base->equation().zone_dis().valeur());
      const Champ_base& rho = (op_base->equation()).milieu().masse_volumique();
      const Champ_Don& Cp = (op_base->equation()).milieu().capacite_calorifique();
      const IntTab& face_voisins=la_zone_vdf.face_voisins();
      int rho_uniforme = sub_type(Champ_Uniforme,rho) ? 1 : 0, cp_uniforme = sub_type(Champ_Uniforme,Cp.valeur()) ? 1 : 0;
      int is_rho_u=op_base->equation().probleme().is_dilatable();
      if (is_rho_u)
        {
          const Operateur_base& op=op_base.valeur();
          is_rho_u=0;
          if (sub_type(Op_Conv_VDF_base,op))
            if (ref_cast(Op_Conv_VDF_base,op).vitesse().le_nom()=="rho_u") is_rho_u=1;
        }
      double Cp_=0,rho_=0;
      const int& nb_faces_bords=la_zone_vdf.nb_faces_bord();
      for (int face=0; face<nb_faces_bords; face++)
        {
          int num_elem=face_voisins(face,0);
          if (num_elem == -1) num_elem = face_voisins(face,1);
          if (cp_uniforme) Cp_=Cp(0,0);
          else if (Cp.nb_comp()==1) Cp_=Cp(num_elem);
          else Cp_=Cp(num_elem,0);
          if (rho_uniforme) rho_=rho(0,0);
          else if (rho.nb_comp()==1) rho_=rho(num_elem);
          else rho_=rho(num_elem,0);
          /* si on est en QC temperature on a calcule div(rhou * T) */
          /* il ne faut pas remultiplier par rho */
          if (is_rho_u) rho_=1;
          flux_bords(face,0) *= (rho_*Cp_);
        }
    }
}

template <class _TYPE_>
int T_It_VDF_Elem<_TYPE_>::impr(Sortie& os) const
{
  const Zone_VDF& la_zone_vdf=ref_cast(Zone_VDF,op_base.valeur().equation().zone_dis().valeur());
  const Zone& mazone=la_zone->zone();
  const int impr_bord=(mazone.Bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = la_zcl->equation().probleme().schema_temps();
  double temps = sch.temps_courant();
  DoubleTab& flux_bords=op_base->flux_bords();
  DoubleVect bilan(flux_bords.dimension(1));
  int k,face;
  int nb_front_Cl=la_zone->nb_front_Cl();
  DoubleTrav flux_bords2( 3, nb_front_Cl , flux_bords.dimension(1)) ;
  flux_bords2=0;
  /*flux_bord(k)          ->   flux_bords2(0,num_cl,k) */
  /*flux_bord_perio1(k)   ->   flux_bords2(1,num_cl,k) */
  /*flux_bord_perio2(k)   ->   flux_bords2(2,num_cl,k) */
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      int periodicite = (type_cl(la_cl)==periodique?1:0);
      for (face=ndeb; face<nfin; face++)
        for(k=0; k<flux_bords.dimension(1); k++)
          {
            flux_bords2(0,num_cl,k)+=flux_bords(face, k);
            if(periodicite)
              {
                if( face < (ndeb+frontiere_dis.nb_faces()/2) ) flux_bords2(1,num_cl,k)+=flux_bords(face, k);
                else flux_bords2(2,num_cl,k)+=flux_bords(face, k);
              }
          }
    } /* fin for num_cl */
  mp_sum_for_each_item(flux_bords2);
  if (je_suis_maitre())
    {
      op_base->ouvrir_fichier(Flux,"",1);
      Flux.add_col(temps);
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          int periodicite = (type_cl(la_cl)==periodique?1:0);
          for(k=0; k<flux_bords.dimension(1); k++)
            {
              bilan(k)+=flux_bords2(0,num_cl,k);
              if(periodicite)
                {
                  Flux.add_col(flux_bords2(1,num_cl,k));
                  Flux.add_col(flux_bords2(2,num_cl,k));
                }
              else Flux.add_col(flux_bords2(0,num_cl,k));
            }
        }
      for(k=0; k<flux_bords.dimension(1); k++)
        Flux.add_col(bilan(k));
      Flux << finl;
    }
  const LIST(Nom)& Liste_Bords_a_imprimer = la_zone->zone().Bords_a_imprimer();
  if (!Liste_Bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_face;
      op_base->ouvrir_fichier_partage(Flux_face,"",impr_bord);
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = la_zcl->les_conditions_limites(num_cl).frontiere_dis();
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
          if (mazone.Bords_a_imprimer().contient(la_fr.le_nom()))
            {
              Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
              for (face=ndeb; face<nfin; face++)
                {
                  if (dimension == 2)
                    Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1);
                  else if (dimension == 3)
                    Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1) << " z= " << la_zone->xv(face,2);
                  for(k=0; k<flux_bords.dimension(1); k++)
                    {
                      if (!est_egal(la_zone_vdf.face_surfaces(face),0., 1.e-20))
                        {
                          Flux_face << " surface_face(m2)= " << la_zone_vdf.face_surfaces(face) ;
                          Flux_face << " flux_par_surface(W/m2)= "  << flux_bords(face, k)/la_zone_vdf.face_surfaces(face) ;
                        }
                      Flux_face << " flux(W)= " << flux_bords(face, k) ;
                    }
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre(DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(resu.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  assert(op_base->flux_bords().dimension(0)==la_zone->nb_faces_bord()); /* resize deja fait */
  contribuer_au_second_membre_bords(resu) ;
  contribuer_au_second_membre_interne(resu) ;
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords(DoubleTab& resu) const
{
  int ndeb, nfin, num_cl=0, nb_front_Cl=la_zone->nb_front_Cl();
  DoubleTab& flux_bords=op_base->flux_bords();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_SYMM>((const Symetrie&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case sortie_libre :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB>((const Neumann_sortie_libre&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case entree_fluide :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL>((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case paroi_fixe :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE>((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case paroi_defilante :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL>((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case paroi_adiabatique :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case paroi :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR>((const Neumann_paroi&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case echange_global_impose :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP>((const Echange_global_impose&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        case echange_externe_impose :
          contribuer_au_second_membre_bords_((const Echange_externe_impose&)la_cl.valeur(), ndeb,nfin, num_cl,frontiere_dis,flux_bords,resu);
          break;
        case periodique :
          contribuer_au_second_membre_bords_((const Periodique&) la_cl.valeur(),ndeb,nfin,flux_bords,resu);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_interne( DoubleTab& resu) const
{
  const int ncomp = resu.line_size();
  const Zone_VDF& zone_VDF = la_zone.valeur();
  DoubleVect flux(ncomp);
  int face,k,elem0,elem1;
  const int ndeb=zone_VDF.premiere_face_int(), nfin=zone_VDF.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      flux_evaluateur.secmem_faces_interne(face, flux);
      elem0 = elem(face,0);
      elem1 = elem(face,1);
      for (k=0; k<ncomp; k++)
        {
          resu(elem0,k)+=flux(k);
          resu(elem1,k)-=flux(k);
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  const int ncomp = inco.line_size();
  DoubleTab& flux_bords = op_base->flux_bords();
  flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
  flux_bords=0;
  ajouter_contribution_bords(inco, matrice) ;
  ajouter_contribution_interne(inco, matrice) ;
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  const int ncomp = inco.line_size();
  if( ncomp == 1) /* cas scalaire */
    {
      ajouter_contribution_bords_vitesse(inco, matrice);
      ajouter_contribution_interne_vitesse(inco, matrice);
    }
  else abort(); /* cas vectoriel */
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  int ndeb, nfin, num_cl=0, nb_front_Cl=la_zone->nb_front_Cl();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_SYMM>((const Symetrie&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case sortie_libre :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB>((const Neumann_sortie_libre&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case entree_fluide :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL>((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case paroi_fixe :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE>((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case paroi_defilante :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL>((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case paroi_adiabatique :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case paroi :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR>((const Neumann_paroi&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case echange_global_impose :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP>((const Echange_global_impose&) la_cl.valeur(),ndeb,nfin, inco, matrice);
          break;
        case echange_externe_impose :
          ajouter_contribution_bords_((const Echange_externe_impose&) la_cl.valeur(),ndeb,nfin, num_cl, inco, frontiere_dis, matrice);
          break;
        case periodique :
          ajouter_contribution_bords_((const Periodique&) la_cl.valeur(), ndeb, nfin, inco, matrice);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_interne(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  const int ncomp = inco.line_size();
  const Zone_VDF& zone_VDF = la_zone.valeur();
  DoubleVect aii(ncomp), ajj(ncomp);
  int face,i, elem1,elem2;
  const int ndeb = zone_VDF.premiere_face_int(), nfin = zone_VDF.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      elem1 = elem(face,0);
      elem2 = elem(face,1);
      flux_evaluateur.coeffs_faces_interne(face, aii, ajj);
      for (i=0; i<ncomp; i++)
        {
          int i0 = elem1*ncomp+i, j0 = elem2*ncomp+i;
          matrice(i0,i0)+=aii(i) ;
          matrice(i0,j0)-=ajj(i) ;
          matrice(j0,j0)+=ajj(i) ;
          matrice(j0,i0)-=aii(i) ;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_interne_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_VDF& zone_VDF = la_zone.valeur();
  double aef = 0;
  const int ndeb = zone_VDF.premiere_face_int(), nfin = zone_VDF.nb_faces();
  for (int f = ndeb; f < nfin; f++)
    {
      const int e1 = elem(f, 0), e2 = elem(f, 1);
      aef = flux_evaluateur.coeffs_faces_interne_bloc_vitesse(inco, f);
      matrice(e1, f) += aef;
      matrice(e2, f) -= aef;
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  int ndeb, nfin, nb_front_Cl=la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl<nb_front_Cl; num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_SYMM>((const Symetrie&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case sortie_libre :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB>((const Neumann_sortie_libre&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case entree_fluide :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL>((const Dirichlet_entree_fluide&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case paroi_fixe :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE>((const Dirichlet_paroi_fixe&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case paroi_defilante :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL>((const Dirichlet_paroi_defilante&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case paroi_adiabatique :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB>((const Neumann_paroi_adiabatique&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case paroi :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR>((const Neumann_paroi&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case echange_global_impose :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP>((const Echange_global_impose&) la_cl.valeur(), ndeb,nfin,inco,matrice);
          break;
        case echange_externe_impose :
          ajouter_contribution_bords_vitesse_((const Echange_externe_impose&) la_cl.valeur(), ndeb,nfin,num_cl,inco,frontiere_dis,matrice);
          break;
        case periodique :
          ajouter_contribution_bords_vitesse_((const Periodique&) la_cl.valeur(), ndeb,nfin,inco,frontiere_dis,matrice);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>& f2e) const
{
  double aii=0, ajj=0;
  const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
  const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
  if (_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP)
    {
      const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
      for (int f = ndeb; f < nfin; f++)
        {
          flux_evaluateur.coeffs_face(f, ndeb, cl, aii, ajj);
          const int e1 = f2e[f].first, e2 = f2e[f].second;
          matrice(e1, e2) = -(elem(f, 0) > -1 ? aii : ajj);
        }
    }
}

/* ************************ *
 * Internal private methods *
 * ************************ */

template <class _TYPE_>
inline void T_It_VDF_Elem<_TYPE_>::fill_flux_tables_(int ncomp, int face, const DoubleVect& flux, DoubleTab& flux_bords, DoubleTab& resu) const
{
  int elem1, elem2;
  if ( (elem1 = elem(face,0)) > -1 )
    for (int k=0; k<ncomp; k++)
      {
        resu(elem1,k) += flux(k);
        flux_bords(face,k) += flux(k);
      }
  if ( (elem2 = elem(face,1)) > -1 )
    for (int k=0; k<ncomp; k++)
      {
        resu(elem2,k) -= flux(k);
        flux_bords(face,k) -= flux(k);
      }
}

template <class _TYPE_> template <bool should_calc_flux, typename BC>
void T_It_VDF_Elem<_TYPE_>::ajouter_bords_(const BC& cl, int ndeb,int nfin,const DoubleTab& donnee,DoubleTab& flux_bords, DoubleTab& resu) const
{
  constexpr bool is_Neum_paroi_adiab = std::is_same<BC,Neumann_paroi_adiabatique>::value;
  const int ncomp = resu.line_size();
  DoubleVect flux(ncomp);
  int face;
  if (should_calc_flux)
    {
      for (face=ndeb; face<nfin; face++)
        {
          if (is_Neum_paroi_adiab)
            {
              assert(0); // On bloque ici :-)
              Process::exit();
            }
          else flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux); // Generic code

          fill_flux_tables_(ncomp, face, flux, flux_bords, resu);
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_bords_(const Periodique& cl, int ndeb,int nfin,const DoubleTab& donnee,const Front_VF& frontiere_dis,DoubleTab& flux_bords, DoubleTab& resu) const
{
  const int ncomp = resu.line_size();
  int face, elem1, elem2;
  DoubleVect flux(ncomp);
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      for (face=ndeb; face<nfin; face++)
        {
          flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
          if ( (elem1=elem(face,0)) > -1)
            for (int k=0; k<ncomp; k++)
              {
                resu(elem1,k) += 0.5*flux(k);
                if ( face < (ndeb+frontiere_dis.nb_faces()/2) ) flux_bords(face,k)+=flux(k);
              }
          if ( (elem2=elem(face,1)) > -1)
            for (int k=0; k<ncomp; k++)
              {
                resu(elem2,k) -= 0.5*flux(k);
                if ( (ndeb+frontiere_dis.nb_faces()/2) <= face ) flux_bords(face,k)-=flux(k);
              }
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_bords_(const Echange_externe_impose& cl, int ndeb,int nfin,int num_cl, const DoubleTab& donnee,const Front_VF& frontiere_dis,DoubleTab& flux_bords, DoubleTab& resu) const
{
  const int ncomp = resu.line_size();
  DoubleVect flux(ncomp);
  int face;
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      int boundary_index = -1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index=num_cl;

      for (face=ndeb; face<nfin; face++)
        {
          int local_face = la_zone.valeur().front_VF(boundary_index).num_local_face(face);
          flux_evaluateur.flux_face(donnee, boundary_index, face, local_face, cl, ndeb, flux);
          fill_flux_tables_(ncomp, face, flux, flux_bords, resu);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename BC>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord_(const BC& cl, int ndeb,int nfin,const DoubleTab& donnee,DoubleTab& flux_bords) const
{
  constexpr bool is_Neum_paroi_adiab = std::is_same<BC,Neumann_paroi_adiabatique>::value;
  int face;
  double flux;
  if (should_calc_flux)
    {
      for (face=ndeb; face<nfin; face++)
        {
          if (is_Neum_paroi_adiab)
            {
              assert(0);
              Process::exit(); // On bloque ici :-)
            }
          else flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb); //Generic

          if ( (elem(face,0)) > -1) flux_bords(face,0) += flux;
          if ( (elem(face,1)) > -1) flux_bords(face,0) -= flux;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord_(const Periodique& cl, int ndeb,int nfin,const DoubleTab& donnee,const Front_VF& frontiere_dis, DoubleTab& flux_bords) const
{
  int face;
  double flux;
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      for (face=ndeb; face<nfin; face++)
        {
          flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
          if ( (elem(face,0)) > -1 )
            if ( face < (ndeb+frontiere_dis.nb_faces()/2) ) flux_bords(face,0) += flux;

          if ( (elem(face,1)) > -1 )
            if ( (ndeb+frontiere_dis.nb_faces()/2) <= face ) flux_bords(face,0) -= flux;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord_(const Echange_externe_impose& cl, int ndeb,int nfin,int num_cl,const DoubleTab& donnee,const Front_VF& frontiere_dis,DoubleTab& flux_bords) const
{
  int face;
  double flux;
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      int boundary_index = -1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;
      for (face=ndeb; face<nfin; face++)
        {
          int local_face=la_zone.valeur().front_VF(boundary_index).num_local_face(face);
          flux = flux_evaluateur.flux_face(donnee, boundary_index,face,local_face, cl, ndeb);
          if ( (elem(face,0)) > -1) flux_bords(face,0) += flux;
          if ( (elem(face,1)) > -1) flux_bords(face,0) -= flux;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename BC>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords_(const BC& cl, int ndeb,int nfin,DoubleTab& flux_bords, DoubleTab& resu) const
{
  const int ncomp = resu.line_size();
  DoubleVect flux(ncomp);
  if (should_calc_flux)
    {
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.secmem_face(face, cl, ndeb, flux); // Generic code
          fill_flux_tables_(ncomp, face, flux, flux_bords, resu);
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords_(const Periodique& cl, int ndeb,int nfin,DoubleTab& flux_bords, DoubleTab& resu) const
{
  const int ncomp = resu.line_size();
  int face, elem1, elem2;
  DoubleVect flux(ncomp);
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      for (face=ndeb; face<nfin; face++)
        {
          flux_evaluateur.secmem_face(face, cl, ndeb, flux);
          if ( (elem1=elem(face,0)) > -1)
            for (int k=0; k<ncomp; k++)
              {
                resu(elem1,k) += 0.5*flux(k);
                flux_bords(face,k) += 0.5*flux(k);
              }
          if ( (elem2=elem(face,1)) > -1)
            for (int k=0; k<ncomp; k++)
              {
                resu(elem2,k) -= 0.5*flux(k);
                flux_bords(face,k) -= 0.5*flux(k);
              }
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords_(const Echange_externe_impose& cl, int ndeb,int nfin, int num_cl,const Front_VF& frontiere_dis,DoubleTab& flux_bords, DoubleTab& resu) const
{
  const int ncomp = resu.line_size();
  int face;
  DoubleVect flux(ncomp);
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      int boundary_index=-1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;
      for (face=ndeb; face<nfin; face++)
        {
          int local_face=la_zone.valeur().front_VF(boundary_index).num_local_face(face);
          flux_evaluateur.secmem_face(boundary_index,face,local_face, cl, ndeb, flux);
          fill_flux_tables_(ncomp, face, flux, flux_bords, resu);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename BC>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_(const BC& cl, int ndeb,int nfin,const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const int ncomp = inco.line_size();
  int elem1, elem2;
  DoubleVect aii(ncomp), ajj(ncomp);
  if (should_calc_flux)
    {
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj); // Generic code
          if ( (elem1 = elem(face,0)) > -1) for (int i = 0; i < ncomp; i++) matrice(elem1*ncomp+i,elem1*ncomp+i) += aii(i);
          if ( (elem2 = elem(face,1)) > -1) for (int i = 0; i < ncomp; i++) matrice(elem2*ncomp+i,elem2*ncomp+i) += ajj(i);
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_(const Periodique& cl, int ndeb,int nfin,const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const int ncomp = inco.line_size();
  int face,elem1, elem2;
  DoubleVect aii(ncomp), ajj(ncomp);
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      for (face=ndeb; face<nfin; face++)
        {
          flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
          elem1 = elem(face,0);
          elem2 = elem(face,1);
          for (int i = 0; i < ncomp; i++)
            {
              int n1 = elem1*ncomp+i, n2 = elem2*ncomp+i;
              matrice(n1,n1) += 0.5*aii(i);
              matrice(n1,n2) -= 0.5*ajj(i);
              matrice(n2,n2) += 0.5*ajj(i);
              matrice(n2,n1) -= 0.5*aii(i);
            }
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_(const Echange_externe_impose& cl, int ndeb,int nfin,int num_cl,const DoubleTab& inco,const Front_VF& frontiere_dis, Matrice_Morse& matrice) const
{
  const int ncomp = inco.line_size();
  int face,elem1, elem2;
  DoubleVect aii(ncomp), ajj(ncomp);
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      int boundary_index=-1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;

      for (face=ndeb; face<nfin; face++)
        {
          int local_face=la_zone.valeur().front_VF(boundary_index).num_local_face(face);
          flux_evaluateur.coeffs_face(boundary_index,face,local_face,ndeb, cl, aii, ajj);
          if ( (elem1=elem(face,0)) > -1)
            for (int i = 0; i < ncomp; i++) matrice(elem1*ncomp+i,elem1*ncomp+i) += aii(i);
          if ( (elem2=elem(face,1)) > -1)
            for (int i = 0; i < ncomp; i++) matrice(elem2*ncomp+i,elem2*ncomp+i) += ajj(i);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename BC>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse_(const BC& cl, int ndeb,int nfin,const DoubleTab& inco, Matrice_Morse& matrice) const
{
  int e1, e2;
  double aef=0;
  if (should_calc_flux)
    {
      for (int f = ndeb; f < nfin; f++)
        {
          aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
          if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
          if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse_(const Periodique& cl, int ndeb,int nfin,const DoubleTab& inco,const Front_VF& frontiere_dis, Matrice_Morse& matrice) const
{
  int e1, e2;
  double aef=0;
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      for (int f = ndeb; f < nfin; f++)
        {
          aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
          if ( (e1 = elem(f, 0)) > -1)
            if ( f < (ndeb+frontiere_dis.nb_faces()/2) ) matrice(e1, f) += aef;
          if ( (e2 = elem(f, 1)) > -1)
            if ( (ndeb+frontiere_dis.nb_faces()/2) <= f ) matrice(e2, f) -= aef;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse_(const Echange_externe_impose& cl, int ndeb,int nfin,int num_cl,const DoubleTab& inco,const Front_VF& frontiere_dis, Matrice_Morse& matrice) const
{
  int e1, e2;
  double aef=0;
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      int boundary_index=-1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;
      for (int f = ndeb; f < nfin; f++)
        {
          int local_face=la_zone.valeur().front_VF(boundary_index).num_local_face(f);
          aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, boundary_index, f, local_face, cl, ndeb);
          if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
          if ( (e2 = elem(f, 1)) > -1) matrice(e1, f) -= aef;
        }
    }
}

#endif /* T_It_VDF_Elem_included */
