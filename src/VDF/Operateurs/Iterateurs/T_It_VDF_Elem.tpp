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
// File:        T_It_VDF_Elem.tpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Iterateurs
// Version:     /main/43
//
//////////////////////////////////////////////////////////////////////////////

#ifndef T_It_VDF_Elem_TPP_included
#define T_It_VDF_Elem_TPP_included

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
DoubleTab& T_It_VDF_Elem<_TYPE_>::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  operator_egal(resu, 0., VECT_REAL_ITEMS);
  return ajouter(inco,resu);
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Elem<_TYPE_>::ajouter(const DoubleTab& donne, DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(donne.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  const int ncomp = donne.line_size();
  DoubleTab& flux_bords=op_base->flux_bords();
  flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
  flux_bords=0;
  // modif b.m.: on va faire += sur des items virtuels, initialiser les cases : sinon risque que les cases soient invalides ou non initialisees
  {
    int n = resu.size_array() - resu.size();
    double *data = resu.addr() + resu.size();
    for (; n; n--, data++) *data = 0.;
  }
  if (ncomp == 1)
    {
      ajouter_bords<SingleDouble>(ncomp,donne,resu);
      ajouter_interne<SingleDouble>(ncomp,donne,resu);
    }
  else
    {
      ajouter_bords<ArrOfDouble>(ncomp,donne,resu);
      ajouter_interne<ArrOfDouble>(ncomp,donne,resu);
    }
  modifier_flux();
  return resu;
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord(const DoubleTab& donnee) const // XXX : On entre jamais dedans :-) :-)
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(donnee.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  const int ncomp = donnee.line_size();
  DoubleTab& flux_bords=op_base->flux_bords();
  flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
  flux_bords=0;
  if( ncomp != 1) Process::exit(); /* cas scalaire */
  (ncomp == 1) ? calculer_flux_bord2<SingleDouble>(ncomp,donnee) : calculer_flux_bord2<ArrOfDouble>(ncomp,donnee);
  modifier_flux() ;
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
  if (ncomp == 1)
    {
      ajouter_contribution_bords<SingleDouble>(ncomp,matrice);
      ajouter_contribution_interne<SingleDouble>(ncomp,matrice);
    }
  else
    {
      ajouter_contribution_bords<ArrOfDouble>(ncomp,matrice);
      ajouter_contribution_interne<ArrOfDouble>(ncomp,matrice);
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  const int ncomp = inco.line_size();
  if( ncomp != 1) Process::exit(); /* cas scalaire */ // TODO : FIXME : Yannick c'est vrai ? :-)
  if (ncomp == 1)
    {
      ajouter_contribution_bords_vitesse<SingleDouble>(ncomp,inco,matrice);
      ajouter_contribution_interne_vitesse<SingleDouble>(ncomp,inco,matrice);
    }
  else
    {
      ajouter_contribution_bords_vitesse<ArrOfDouble>(ncomp,inco,matrice);
      ajouter_contribution_interne_vitesse<ArrOfDouble>(ncomp,inco,matrice);
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>& f2e) const
{
  const int ncomp = inco.line_size();
  ArrOfDouble aii(ncomp), ajj(ncomp);
  const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
  const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
  if (_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP)
    {
      const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
      for (int f = ndeb; f < nfin; f++)
        {
          const int e1 = f2e[f].first, e2 = f2e[f].second;
          flux_evaluateur.coeffs_face(f, ndeb, cl, aii, ajj);
          for (int i = 0; i < ncomp; i++) matrice(e1*ncomp+i,e2*ncomp+i) = -(elem(f, 0) > -1 ? aii[i] : ajj[i]);
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre(DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  const int ncomp = resu.line_size();
  assert(resu.nb_dim() < 3 && la_zcl.non_nul() && la_zone.non_nul());
  assert(op_base->flux_bords().dimension(0)==la_zone->nb_faces_bord()); /* resize deja fait */
  if (ncomp == 1)
    {
      contribuer_au_second_membre_bords<SingleDouble>(ncomp,resu);
      contribuer_au_second_membre_interne<SingleDouble>(ncomp,resu);
    }
  else
    {
      contribuer_au_second_membre_bords<ArrOfDouble>(ncomp,resu);
      contribuer_au_second_membre_interne<ArrOfDouble>(ncomp,resu);
    }
}

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <class _TYPE_> template <typename Type_Double>
DoubleTab& T_It_VDF_Elem<_TYPE_>::ajouter_bords(const int ncomp, const DoubleTab& donnee, DoubleTab& resu) const
{
  for (int num_cl = 0; num_cl < la_zone->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      /* Test en bidim axi */
      if (bidim_axi && !sub_type(Symetrie,la_cl.valeur()))
        {
          if (nfin > ndeb && est_egal(la_zone.valeur().face_surfaces()[ndeb],0))
            {
              Cerr << "Error in the definition of the boundary conditions. The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry boundary condition (symetrie keyword) for the boundary " << frontiere_dis.le_nom() << finl;
              Process::exit();
            }
        }
      switch(type_cl(la_cl))
        {
        case symetrie :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_SYMM,Type_Double>((const Symetrie&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case sortie_libre :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB,Type_Double>((const Neumann_sortie_libre&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case entree_fluide :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL,Type_Double>((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case paroi_fixe :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE,Type_Double>((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case paroi_defilante :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL,Type_Double>((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case paroi :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR,Type_Double>((const Neumann_paroi&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case echange_global_impose :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP,Type_Double>((const Echange_global_impose&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case paroi_adiabatique :
          ajouter_bords_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB,Type_Double>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,resu);
          break;
        case echange_externe_impose :
          ajouter_bords_<Type_Double>((const Echange_externe_impose&) la_cl.valeur(),ndeb,nfin,num_cl,ncomp,donnee,frontiere_dis,resu);
          break;
        case periodique :
          ajouter_bords_<Type_Double>((const Periodique&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,frontiere_dis,resu);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::ajouter_bords" << finl;
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_> template <typename Type_Double>
DoubleTab& T_It_VDF_Elem<_TYPE_>::ajouter_interne(const int ncomp, const DoubleTab& donnee, DoubleTab& resu) const
{
  Type_Double flux(ncomp);
  const Zone_VDF& zone_VDF = la_zone.valeur();
  const int ndeb = zone_VDF.premiere_face_int(), nfin = zone_VDF.nb_faces();
  for (int face = ndeb; face<nfin; face++)
    {
      flux_evaluateur.flux_faces_interne(donnee,face,flux);
      const int elem0 = elem(face,0), elem1 = elem(face,1);
      for (int k = 0; k < ncomp; k++)
        {
          resu(elem0,k) += flux[k];
          resu(elem1,k) -= flux[k];
        }
    }
  return resu;
}

template <class _TYPE_> template <bool should_calc_flux, typename Type_Double,typename BC>
void T_It_VDF_Elem<_TYPE_>::ajouter_bords_(const BC& cl, const int ndeb, const int nfin, const int ncomp, const DoubleTab& donnee, DoubleTab& resu) const
{
  constexpr bool is_Neum_paroi_adiab = std::is_same<BC,Neumann_paroi_adiabatique>::value;
  if (should_calc_flux)
    {
      if (is_Neum_paroi_adiab) Process::exit(); // On bloque ici :-)
      Type_Double flux(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.flux_face(donnee,face,cl,ndeb,flux); // Generic code
          fill_flux_tables_(face,ncomp,1.0 /* coeff */,flux,resu);
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_bords_(const Periodique& cl, const int ndeb, const int nfin, const int ncomp, const DoubleTab& donnee, const Front_VF& frontiere_dis,  DoubleTab& resu) const
{
  DoubleTab& flux_bords = op_base->flux_bords();
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      Type_Double flux(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          const int elem1 = elem(face,0), elem2 = elem(face,1);
          flux_evaluateur.flux_face(donnee,face,cl,ndeb,flux);
          if ( elem1 > -1)
            for (int k = 0; k < ncomp; k++)
              {
                resu(elem1,k) += 0.5*flux[k];
                if ( face < (ndeb + frontiere_dis.nb_faces()/2) ) flux_bords(face,k) += flux[k];
              }
          if ( elem2 > -1)
            for (int k = 0; k < ncomp; k++)
              {
                resu(elem2,k) -= 0.5*flux[k];
                if ( (ndeb + frontiere_dis.nb_faces()/2) <= face ) flux_bords(face,k) -= flux[k];
              }
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_bords_(const Echange_externe_impose& cl, const int ndeb, const int nfin, const int num_cl, const int ncomp, const DoubleTab& donnee,const Front_VF& frontiere_dis,DoubleTab& resu) const
{
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      Type_Double flux(ncomp);
      int boundary_index = -1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;

      for (int face = ndeb; face < nfin; face++)
        {
          int local_face = la_zone.valeur().front_VF(boundary_index).num_local_face(face);
          flux_evaluateur.flux_face(donnee, boundary_index, face, local_face, cl, ndeb, flux);
          fill_flux_tables_(face,ncomp,1.0 /* coeff */,flux,resu);
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
inline void T_It_VDF_Elem<_TYPE_>::fill_flux_tables_(const int face, const int ncomp, const double coeff , const Type_Double& flux, DoubleTab& resu) const
{
  DoubleTab& flux_bords = op_base->flux_bords();
  const int elem1 = elem(face,0), elem2 = elem(face,1);
  if ( elem1 > -1 )
    for (int k = 0; k < ncomp; k++)
      {
        resu(elem1,k) += coeff*flux[k];
        flux_bords(face,k) += coeff*flux[k];
      }
  if ( elem2 > -1 )
    for (int k = 0; k < ncomp; k++)
      {
        resu(elem2,k) -= coeff*flux[k];
        flux_bords(face,k) -= coeff*flux[k];
      }
}

template <class _TYPE_>
void  T_It_VDF_Elem<_TYPE_>::modifier_flux() const
{
  if (op_base->equation().inconnue().le_nom().debute_par("temperature")
      && !( sub_type(Operateur_Diff_base,op_base.valeur()) && ref_cast(Operateur_Diff_base,op_base.valeur()).diffusivite().le_nom() == "conductivite" ) )
    {
      DoubleTab& flux_bords=op_base->flux_bords();
      const Zone_VDF& la_zone_vdf=ref_cast(Zone_VDF,op_base->equation().zone_dis().valeur());
      const Champ_base& rho = (op_base->equation()).milieu().masse_volumique().valeur();
      const Champ_Don& Cp = (op_base->equation()).milieu().capacite_calorifique();
      const IntTab& face_voisins=la_zone_vdf.face_voisins();
      int rho_uniforme = sub_type(Champ_Uniforme,rho) ? 1 : 0, cp_uniforme = sub_type(Champ_Uniforme,Cp.valeur()) ? 1 : 0;
      int is_rho_u=op_base->equation().probleme().is_dilatable();
      if (is_rho_u)
        {
          const Operateur_base& op=op_base.valeur();
          is_rho_u=0;
          if (sub_type(Op_Conv_VDF_base,op)) if (ref_cast(Op_Conv_VDF_base,op).vitesse().le_nom()=="rho_u") is_rho_u = 1;
        }
      double Cp_=0,rho_=0;
      const int nb_faces_bords = la_zone_vdf.nb_faces_bord();
      for (int face = 0; face < nb_faces_bords; face++)
        {
          int num_elem = face_voisins(face,0);
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

// Debut : Inutile, jamais utilise ...
template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord2(const int ncomp, const DoubleTab& donnee) const
{
  for (int num_cl = 0; num_cl < la_zone->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
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
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_SYMM,Type_Double>((const Symetrie&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case sortie_libre :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB,Type_Double>((const Neumann_sortie_libre&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case entree_fluide :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL,Type_Double>((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case paroi_fixe :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE,Type_Double>((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case paroi_defilante :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL,Type_Double>((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case paroi :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR,Type_Double>((const Neumann_paroi&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case echange_global_impose :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP,Type_Double>((const Echange_global_impose&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case paroi_adiabatique :
          calculer_flux_bord_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB,Type_Double>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb,nfin,ncomp,donnee);
          break;
        case periodique :
          calculer_flux_bord_<Type_Double>((const Periodique&) la_cl.valeur(),ndeb,nfin,ncomp,donnee,frontiere_dis);
          break;
        case echange_externe_impose :
          calculer_flux_bord_<Type_Double>((const Echange_externe_impose&) la_cl.valeur(),ndeb,nfin,num_cl,ncomp,donnee,frontiere_dis);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::calculer_flux_bord" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename Type_Double, typename BC>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord_(const BC& cl, const int ndeb, const int nfin, const int ncomp, const DoubleTab& donnee ) const
{
  constexpr bool is_Neum_paroi_adiab = std::is_same<BC,Neumann_paroi_adiabatique>::value;
  DoubleTab& flux_bords = op_base->flux_bords();
  if (should_calc_flux)
    {
      if (is_Neum_paroi_adiab) Process::exit(); // On bloque ici :-)
      Type_Double flux(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          const int elem1 = elem(face,0), elem2 = elem(face,1);
          flux_evaluateur.flux_face(donnee, face, cl, ndeb,flux); //Generic

          if ( elem1 > -1) for (int k = 0; k < ncomp; k++) flux_bords(face,k) += flux[k];
          if ( elem2 > -1) for (int k = 0; k < ncomp; k++) flux_bords(face,k) -= flux[k];
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord_(const Periodique& cl, const int ndeb, const int nfin, const int ncomp, const DoubleTab& donnee, const Front_VF& frontiere_dis) const
{
  DoubleTab& flux_bords = op_base->flux_bords();
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      Type_Double flux(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          const int elem1 = elem(face,0), elem2 = elem(face,1);
          flux_evaluateur.flux_face(donnee, face, cl, ndeb,flux);
          if ( elem1 > -1 ) for (int k = 0; k < ncomp; k++) if ( face < (ndeb + frontiere_dis.nb_faces()/2) ) flux_bords(face,k) += flux[k];
          if ( elem2 > -1 ) for (int k = 0; k < ncomp; k++) if ( (ndeb + frontiere_dis.nb_faces()/2) <= face ) flux_bords(face,k) -= flux[k];
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::calculer_flux_bord_(const Echange_externe_impose& cl, const int ndeb, const int nfin, const int num_cl, const int ncomp, const DoubleTab& donnee, const Front_VF& frontiere_dis) const
{
  DoubleTab& flux_bords = op_base->flux_bords();
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      Type_Double flux(ncomp);
      int boundary_index = -1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;
      for (int face = ndeb; face < nfin; face++)
        {
          int local_face = la_zone.valeur().front_VF(boundary_index).num_local_face(face);
          const int elem1 = elem(face,0), elem2 = elem(face,1);
          flux_evaluateur.flux_face(donnee, boundary_index,face,local_face, cl, ndeb,flux);
          if ( elem1 > -1) for (int k = 0; k < ncomp; k++) flux_bords(face,k) += flux[k];
          if ( elem2 > -1) for (int k = 0; k < ncomp; k++) flux_bords(face,k) -= flux[k];
        }
    }
}
// Fin : Inutile, jamais utilise ...

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords(const int ncomp, Matrice_Morse& matrice ) const
{
  for (int num_cl = 0; num_cl < la_zone->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_SYMM,Type_Double>((const Symetrie&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case sortie_libre :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB,Type_Double>((const Neumann_sortie_libre&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case entree_fluide :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL,Type_Double>((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case paroi_fixe :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE,Type_Double>((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case paroi_defilante :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL,Type_Double>((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case paroi_adiabatique :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB,Type_Double>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case paroi :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_PAR,Type_Double>((const Neumann_paroi&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case echange_global_impose :
          ajouter_contribution_bords_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP,Type_Double>((const Echange_global_impose&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        case echange_externe_impose :
          ajouter_contribution_bords_<Type_Double>((const Echange_externe_impose&) la_cl.valeur(),ndeb,nfin,num_cl,ncomp,frontiere_dis,matrice);
          break;
        case periodique :
          ajouter_contribution_bords_<Type_Double>((const Periodique&) la_cl.valeur(),ndeb,nfin,ncomp,matrice);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_interne(const int ncomp, Matrice_Morse& matrice ) const
{
  Type_Double aii(ncomp), ajj(ncomp);
  const Zone_VDF& zone_VDF = la_zone.valeur();
  const int ndeb = zone_VDF.premiere_face_int(), nfin = zone_VDF.nb_faces();
  for (int face = ndeb; face < nfin; face++)
    {
      const int elem1 = elem(face,0), elem2 = elem(face,1);
      flux_evaluateur.coeffs_faces_interne(face, aii, ajj);
      for (int i = 0; i < ncomp; i++)
        {
          const int i0 = elem1*ncomp+i, j0 = elem2*ncomp+i;
          matrice(i0,i0) += aii[i];
          matrice(i0,j0) -= ajj[i];
          matrice(j0,j0) += ajj[i];
          matrice(j0,i0) -= aii[i];
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename Type_Double, typename BC>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_(const BC& cl, const int ndeb, const int nfin, const int ncomp, Matrice_Morse& matrice) const
{
  Type_Double aii(ncomp), ajj(ncomp);
  if (should_calc_flux)
    {
      for (int face = ndeb; face < nfin; face++)
        {
          const int elem1 = elem(face,0), elem2 = elem(face,1);
          flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj); // Generic code
          if ( elem1 > -1) for (int i = 0; i < ncomp; i++) matrice(elem1*ncomp+i,elem1*ncomp+i) += aii[i];
          if ( elem2 > -1) for (int i = 0; i < ncomp; i++) matrice(elem2*ncomp+i,elem2*ncomp+i) += ajj[i];
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_(const Periodique& cl, const int ndeb, const int nfin, const int ncomp, Matrice_Morse& matrice) const
{
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      Type_Double aii(ncomp), ajj(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          const int elem1 = elem(face,0), elem2 = elem(face,1);
          flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
          for (int i = 0; i < ncomp; i++)
            {
              const int n1 = elem1*ncomp+i, n2 = elem2*ncomp+i;
              matrice(n1,n1) += 0.5*aii[i];
              matrice(n1,n2) -= 0.5*ajj[i];
              matrice(n2,n2) += 0.5*ajj[i];
              matrice(n2,n1) -= 0.5*aii[i];
            }
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_(const Echange_externe_impose& cl, const int ndeb, const int nfin, const int num_cl, const int ncomp, const Front_VF& frontiere_dis, Matrice_Morse& matrice) const
{
  Type_Double aii(ncomp), ajj(ncomp);
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      int boundary_index = -1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;

      for (int face = ndeb; face < nfin; face++)
        {
          const int local_face = la_zone.valeur().front_VF(boundary_index).num_local_face(face), elem1 = elem(face,0), elem2 = elem(face,1);
          flux_evaluateur.coeffs_face(boundary_index,face,local_face,ndeb, cl, aii, ajj);
          if ( elem1 > -1) for (int i = 0; i < ncomp; i++) matrice(elem1*ncomp+i,elem1*ncomp+i) += aii[i];
          if ( elem2 > -1) for (int i = 0; i < ncomp; i++) matrice(elem2*ncomp+i,elem2*ncomp+i) += ajj[i];
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_interne_vitesse(const int ncomp, const DoubleTab& inco, Matrice_Morse& matrice) const
{
  Type_Double aef(ncomp);
  const Zone_VDF& zone_VDF = la_zone.valeur();
  const int ndeb = zone_VDF.premiere_face_int(), nfin = zone_VDF.nb_faces();
  for (int face = ndeb; face < nfin; face++)
    {
      const int elem1 = elem(face, 0), elem2 = elem(face,1);
      flux_evaluateur.coeffs_faces_interne_bloc_vitesse(inco, face,aef);
      for (int i = 0; i < ncomp; i++)
        {
          matrice(elem1*ncomp+i,face*ncomp+i) += aef[i];
          matrice(elem2*ncomp+i,face*ncomp+i) -= aef[i];
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse(const int ncomp, const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  for (int num_cl = 0; num_cl < la_zone->nb_front_Cl(); num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_SYMM,Type_Double>((const Symetrie&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case sortie_libre :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB,Type_Double>((const Neumann_sortie_libre&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case entree_fluide :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL,Type_Double>((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case paroi_fixe :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE,Type_Double>((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case paroi_defilante :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL,Type_Double>((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case paroi_adiabatique :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB,Type_Double>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case paroi :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_PAR,Type_Double>((const Neumann_paroi&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case echange_global_impose :
          ajouter_contribution_bords_vitesse_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP,Type_Double>((const Echange_global_impose&) la_cl.valeur(),ndeb,nfin,ncomp,inco,matrice);
          break;
        case echange_externe_impose :
          ajouter_contribution_bords_vitesse_<Type_Double>((const Echange_externe_impose&) la_cl.valeur(),ndeb,nfin,num_cl,ncomp,inco,frontiere_dis,matrice);
          break;
        case periodique :
          ajouter_contribution_bords_vitesse_<Type_Double>((const Periodique&) la_cl.valeur(),ndeb,nfin,ncomp,inco,frontiere_dis,matrice);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename Type_Double, typename BC>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse_(const BC& cl, const int ndeb, const int nfin, const int ncomp, const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (should_calc_flux)
    {
      Type_Double aef(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          const int elem1 = elem(face, 0), elem2 = elem(face, 1);
          flux_evaluateur.coeffs_face_bloc_vitesse(inco, face, cl, ndeb,aef);
          if ( elem1 > -1) for (int i = 0; i < ncomp; i++) matrice(elem1*ncomp+i,face*ncomp+i) += aef[i];
          if ( elem2 > -1) for (int i = 0; i < ncomp; i++) matrice(elem2*ncomp+i,face*ncomp+i) -= aef[i];
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse_(const Periodique& cl, const int ndeb, const int nfin, const int ncomp, const DoubleTab& inco, const Front_VF& frontiere_dis, Matrice_Morse& matrice) const
{
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      Type_Double aef(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          const int elem1 = elem(face, 0), elem2 = elem(face, 1);
          flux_evaluateur.coeffs_face_bloc_vitesse(inco, face, cl, ndeb,aef);
          if ( elem1 > -1) for (int i = 0; i < ncomp; i++) if ( face < (ndeb + frontiere_dis.nb_faces()/2) ) matrice(elem1*ncomp+i,face*ncomp+i) += aef[i];
          if ( elem2 > -1) for (int i = 0; i < ncomp; i++) if ( (ndeb + frontiere_dis.nb_faces()/2) <= face ) matrice(elem2*ncomp+i,face*ncomp+i) -= aef[i];
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::ajouter_contribution_bords_vitesse_(const Echange_externe_impose& cl, const int ndeb, const int nfin, const int num_cl, const int ncomp, const DoubleTab& inco,const Front_VF& frontiere_dis, Matrice_Morse& matrice) const
{
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      Type_Double aef(ncomp);
      int boundary_index = -1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;
      for (int face = ndeb; face < nfin; face++)
        {
          const int local_face = la_zone.valeur().front_VF(boundary_index).num_local_face(face), elem1 = elem(face, 0), elem2 = elem(face, 1);
          flux_evaluateur.coeffs_face_bloc_vitesse(inco, boundary_index, face, local_face, cl, ndeb,aef);
          if ( elem1 > -1) for (int i = 0; i < ncomp; i++) matrice(elem1*ncomp+i,face*ncomp+i) += aef[i];
          if ( elem2 > -1) for (int i = 0; i < ncomp; i++) matrice(elem2*ncomp+i,face*ncomp+i) -= aef[i]; // BUG FIX !! c'etait elem1 et pas elem 2!!
        }
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords(const int ncomp, DoubleTab& resu) const
{
  for (int num_cl = 0; num_cl < la_zone->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_SYMM,Type_Double>((const Symetrie&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case sortie_libre :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB,Type_Double>((const Neumann_sortie_libre&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case entree_fluide :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL,Type_Double>((const Dirichlet_entree_fluide&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case paroi_fixe :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE,Type_Double>((const Dirichlet_paroi_fixe&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case paroi_defilante :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL,Type_Double>((const Dirichlet_paroi_defilante&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case paroi_adiabatique :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB,Type_Double>((const Neumann_paroi_adiabatique&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case paroi :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PAR,Type_Double>((const Neumann_paroi&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case echange_global_impose :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP,Type_Double>((const Echange_global_impose&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case periodique :
          contribuer_au_second_membre_bords_<_TYPE_::CALC_FLUX_FACES_PERIO,Type_Double>((const Periodique&) la_cl.valeur(),ndeb,nfin,ncomp,resu);
          break;
        case echange_externe_impose :
          contribuer_au_second_membre_bords_<Type_Double>((const Echange_externe_impose&)la_cl.valeur(), ndeb,nfin,num_cl,ncomp,frontiere_dis,resu);
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_interne(const int ncomp, DoubleTab& resu) const
{
  Type_Double flux(ncomp);
  const Zone_VDF& zone_VDF = la_zone.valeur();
  const int ndeb = zone_VDF.premiere_face_int(), nfin = zone_VDF.nb_faces();
  for (int face = ndeb; face < nfin; face++)
    {
      const int elem0 = elem(face,0), elem1 = elem(face,1);
      flux_evaluateur.secmem_faces_interne(face, flux);
      for (int k = 0; k < ncomp; k++)
        {
          resu(elem0,k) += flux[k];
          resu(elem1,k) -= flux[k];
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, typename Type_Double, typename BC>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords_(const BC& cl, const int ndeb, const int nfin, const int ncomp, DoubleTab& resu) const
{
  constexpr bool is_Periodique = std::is_same<BC,Periodique>::value;
  if (should_calc_flux)
    {
      Type_Double flux(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.secmem_face(face, cl, ndeb, flux); // Generic code
          is_Periodique ? fill_flux_tables_(face,ncomp,0.5 /* coeff */,flux,resu) : fill_flux_tables_(face,ncomp,1.0 /* coeff */,flux,resu);
        }
    }
}

template <class _TYPE_> template <typename Type_Double>
void T_It_VDF_Elem<_TYPE_>::contribuer_au_second_membre_bords_(const Echange_externe_impose& cl, const int ndeb, const int nfin, const int num_cl, const int ncomp, const Front_VF& frontiere_dis, DoubleTab& resu) const
{
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      Type_Double flux(ncomp);
      int boundary_index=-1;
      if (la_zone.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom()) boundary_index = num_cl;
      for (int face = ndeb; face < nfin; face++)
        {
          int local_face=la_zone.valeur().front_VF(boundary_index).num_local_face(face);
          flux_evaluateur.secmem_face(boundary_index,face,local_face, cl, ndeb, flux);
          fill_flux_tables_(face,ncomp,1.0 /* coeff */,flux,resu);
        }
    }
}

#endif /* T_It_VDF_Elem_TPP_included */
