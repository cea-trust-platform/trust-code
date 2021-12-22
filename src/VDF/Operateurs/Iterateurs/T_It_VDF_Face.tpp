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
// File:        T_It_VDF_Face.tpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Iterateurs
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////

#ifndef T_It_VDF_Face_TPP_included
#define T_It_VDF_Face_TPP_included

template <class _TYPE_>
inline T_It_VDF_Face<_TYPE_>::T_It_VDF_Face(const T_It_VDF_Face<_TYPE_>& iter) :
  Iterateur_VDF_base(iter), flux_evaluateur(iter.flux_evaluateur), nb_elem(iter.nb_elem),
  premiere_arete_interne(iter.premiere_arete_interne), derniere_arete_interne(iter.derniere_arete_interne),
  premiere_arete_mixte(iter.premiere_arete_mixte), derniere_arete_mixte(iter.derniere_arete_mixte),
  premiere_arete_bord(iter.premiere_arete_bord), derniere_arete_bord(iter.derniere_arete_bord),
  premiere_arete_coin(iter.premiere_arete_coin), derniere_arete_coin(iter.derniere_arete_coin)
{
  orientation.ref(iter.orientation);
  Qdm.ref(iter.Qdm);
  elem.ref(iter.elem);
  elem_faces.ref(iter.elem_faces);
  type_arete_bord.ref(iter.type_arete_bord);
  type_arete_coin.ref(iter.type_arete_coin);
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::completer_()
{
  nb_elem=la_zone->nb_elem_tot();
  orientation.ref(la_zone->orientation());
  Qdm.ref(la_zone->Qdm());
  elem.ref(la_zone->face_voisins());
  elem_faces.ref(la_zone->elem_faces());
  type_arete_bord.ref(la_zcl->type_arete_bord());
  type_arete_coin.ref(la_zcl->type_arete_coin());
  premiere_arete_interne=la_zone->premiere_arete_interne();
  derniere_arete_interne=premiere_arete_interne+la_zone->nb_aretes_internes();
  premiere_arete_mixte=la_zone->premiere_arete_mixte();
  derniere_arete_mixte=premiere_arete_mixte+la_zone->nb_aretes_mixtes();
  premiere_arete_bord=la_zone->premiere_arete_bord();
  derniere_arete_bord=premiere_arete_bord+la_zone->nb_aretes_bord();
  premiere_arete_coin=la_zone->premiere_arete_coin();
  derniere_arete_coin=premiere_arete_coin+la_zone->nb_aretes_coin();
}

template <class _TYPE_>
int T_It_VDF_Face<_TYPE_>::impr(Sortie& os) const
{
  const Zone& ma_zone=la_zone->zone();
  const int& impr_mom=ma_zone.Moments_a_imprimer();
  const int impr_sum=(ma_zone.Bords_a_imprimer_sum().est_vide() ? 0:1), impr_bord=(ma_zone.Bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = la_zcl->equation().probleme().schema_temps();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  DoubleVect bilan(tab_flux_bords.dimension(1));
  const int nb_faces = la_zone->nb_faces_tot();
  DoubleTab xgr(nb_faces,dimension);
  xgr=0.;
  if (impr_mom)
    {
      const DoubleTab& xgrav = la_zone->xv();
      const ArrOfDouble& c_grav=ma_zone.cg_moments();
      for (int num_face=0; num_face <nb_faces; num_face++)
        for (int i=0; i<dimension; i++)
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);
    }
  int k,face, nb_front_Cl=la_zone->nb_front_Cl();
  DoubleTrav flux_bords2( 5, nb_front_Cl , tab_flux_bords.dimension(1)) ;
  flux_bords2=0;
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces(), periodicite = (type_cl(la_cl)==periodique?1:0);
      for (face=ndeb; face<nfin; face++)
        {
          for(k=0; k<tab_flux_bords.dimension(1); k++)
            {
              flux_bords2(0,num_cl,k)+=tab_flux_bords(face, k);
              if(periodicite)
                {
                  if( face < (ndeb+frontiere_dis.nb_faces()/2) ) flux_bords2(1,num_cl,k)+=tab_flux_bords(face, k);
                  else flux_bords2(2,num_cl,k)+=tab_flux_bords(face, k);
                }
              if (ma_zone.Bords_a_imprimer_sum().contient(frontiere_dis.le_nom())) flux_bords2(3,num_cl,k)+=tab_flux_bords(face, k);
            }  /* fin for k */
          if (impr_mom)
            {
              if (dimension==2) flux_bords2(4,num_cl,0)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1);
              else
                {
                  flux_bords2(4,num_cl,0)+=tab_flux_bords(face,2)*xgr(face,1)-tab_flux_bords(face,1)*xgr(face,2);
                  flux_bords2(4,num_cl,1)+=tab_flux_bords(face,0)*xgr(face,2)-tab_flux_bords(face,2)*xgr(face,0);
                  flux_bords2(4,num_cl,2)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1);
                }
            }
        } /* fin for face */
    }
  mp_sum_for_each_item(flux_bords2);
  if (je_suis_maitre())
    {
      //SFichier Flux;
      if (!Flux.is_open()) op_base->ouvrir_fichier(Flux,"",1);
      //SFichier Flux_moment;
      if (!Flux_moment.is_open()) op_base->ouvrir_fichier(Flux_moment,"moment",impr_mom);
      //SFichier Flux_sum;
      if (!Flux_sum.is_open()) op_base->ouvrir_fichier(Flux_sum,"sum",impr_sum);
      Flux.add_col(sch.temps_courant());
      if (impr_mom) Flux_moment.add_col(sch.temps_courant());
      if (impr_sum) Flux_sum.add_col(sch.temps_courant());
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          int periodicite = (type_cl(la_cl)==periodique?1:0);
          for(k=0; k<tab_flux_bords.dimension(1); k++)
            {
              if(periodicite)
                {
                  Flux.add_col(flux_bords2(1,num_cl,k));
                  Flux.add_col(flux_bords2(2,num_cl,k));
                }
              else Flux.add_col(flux_bords2(0,num_cl,k));

              if (impr_sum) Flux_sum.add_col(flux_bords2(3,num_cl,k));
              bilan(k)+=flux_bords2(0,num_cl,k);
            }
          if (dimension==3)
            {
              for (k=0; k<tab_flux_bords.dimension(1); k++) if (impr_mom) Flux_moment.add_col(flux_bords2(4,num_cl,k));
            }
          else if (impr_mom) Flux_moment.add_col(flux_bords2(4,num_cl,0));
        } /* fin for num_cl */
      for(k=0; k<tab_flux_bords.dimension(1); k++) Flux.add_col(bilan(k));
      Flux << finl;
      if (impr_sum) Flux_sum << finl;
      if (impr_mom) Flux_moment << finl;
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
          if (ma_zone.Bords_a_imprimer().contient(la_fr.le_nom()))
            {
              if(je_suis_maitre())
                {
                  Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_face);
                  Flux_face << " : " << finl;
                }
              for (face=ndeb; face<nfin; face++)
                {
                  if (dimension == 2) Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1) << " : ";
                  else if (dimension == 3) Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1) << " z= " << la_zone->xv(face,2) << " : ";
                  for(k=0; k<tab_flux_bords.dimension(1); k++) Flux_face << tab_flux_bords(face, k) << " ";
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3);
  const int ncomp = inco.line_size() ;
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  tab_flux_bords.resize(la_zone->nb_faces_bord(),dimension);
  tab_flux_bords=0;
  if( ncomp == 1) // TODO : FIXME : a virer ! plus besoin de cas scalire !
    {
      ajouter_aretes_bords<Type_Champ::SCALAIRE>(ncomp,inco,resu);
      ajouter_aretes_coins<Type_Champ::SCALAIRE>(ncomp,inco,resu);
      ajouter_aretes_internes<Type_Champ::SCALAIRE>(ncomp,inco,resu);
      ajouter_aretes_mixtes<Type_Champ::SCALAIRE>(ncomp,inco,resu);
      ajouter_fa7_sortie_libre<Type_Champ::SCALAIRE>(ncomp,inco,resu);
      ajouter_fa7_elem<Type_Champ::SCALAIRE>(ncomp,inco,resu);
    }
  else
    {
      ajouter_aretes_bords<Type_Champ::VECTORIEL>(ncomp,inco,resu);
      ajouter_aretes_coins<Type_Champ::VECTORIEL>(ncomp,inco,resu);
      ajouter_aretes_internes<Type_Champ::VECTORIEL>(ncomp,inco,resu);
      ajouter_aretes_mixtes<Type_Champ::VECTORIEL>(ncomp,inco,resu);
      ajouter_fa7_sortie_libre<Type_Champ::VECTORIEL>(ncomp,inco,resu);
      ajouter_fa7_elem<Type_Champ::VECTORIEL>(ncomp,inco,resu);
    }
  /* On multiplie les flux au bord par rho en hydraulique (sert uniquement a la sortie) */
  Nom nom_eqn=la_zcl->equation().que_suis_je();
  /* Modif B.Mathieu pour front-tracking: masse_volumique() invalide en f.t.*/
  if (nom_eqn.debute_par("Navier_Stokes") && nom_eqn!="Navier_Stokes_Melange" && nom_eqn!="Navier_Stokes_FT_Disc")
    {
      const Champ_base& rho = la_zcl->equation().milieu().masse_volumique();
      if (sub_type(Champ_Uniforme,rho))
        {
          double coef = rho(0,0);
          int nb_faces_bord=la_zone->nb_faces_bord();
          for (int face = 0; face < nb_faces_bord; face++) for(int k = 0; k < tab_flux_bords.line_size(); k++) tab_flux_bords(face,k) *= coef;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre(DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(resu.nb_dim() < 3);
  const int ncomp = resu.line_size();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  if (la_zone->nb_faces_bord() >0)
    {
      tab_flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
      tab_flux_bords=0;
    }
  if( ncomp == 1) // TODO : FIXME : a virer :-)
    {
      contribuer_au_second_membre_aretes_bords<Type_Champ::SCALAIRE>(ncomp,resu);
      contribuer_au_second_membre_aretes_coins<Type_Champ::SCALAIRE>(ncomp,resu);
      contribuer_au_second_membre_aretes_internes<Type_Champ::SCALAIRE>(ncomp,resu);
      contribuer_au_second_membre_aretes_mixtes<Type_Champ::SCALAIRE>(ncomp,resu);
      contribuer_au_second_membre_fa7_sortie_libre<Type_Champ::SCALAIRE>(ncomp,resu);
      contribuer_au_second_membre_fa7_elem<Type_Champ::SCALAIRE>(ncomp,resu);
    }
  else
    {
      contribuer_au_second_membre_aretes_bords<Type_Champ::VECTORIEL>(ncomp,resu);
      contribuer_au_second_membre_aretes_coins<Type_Champ::VECTORIEL>(ncomp,resu);
      contribuer_au_second_membre_aretes_internes<Type_Champ::VECTORIEL>(ncomp,resu);
      contribuer_au_second_membre_aretes_mixtes<Type_Champ::VECTORIEL>(ncomp,resu);
      contribuer_au_second_membre_fa7_sortie_libre<Type_Champ::VECTORIEL>(ncomp,resu);
      contribuer_au_second_membre_fa7_elem<Type_Champ::VECTORIEL>(ncomp,resu);
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3);
  const int ncomp = inco.line_size();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  tab_flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
  tab_flux_bords = 0;
  if( ncomp == 1)
    {
      ajouter_contribution_aretes_bords<Type_Champ::SCALAIRE>(ncomp,inco,matrice);
      ajouter_contribution_aretes_coins<Type_Champ::SCALAIRE>(ncomp,inco,matrice);
      ajouter_contribution_aretes_internes<Type_Champ::SCALAIRE>(ncomp,inco,matrice);
      ajouter_contribution_aretes_mixtes<Type_Champ::SCALAIRE>(ncomp,inco,matrice);
      ajouter_contribution_fa7_sortie_libre<Type_Champ::SCALAIRE>(ncomp,inco,matrice);
      ajouter_contribution_fa7_elem<Type_Champ::SCALAIRE>(ncomp,inco,matrice);
    }
  else
    {
      ajouter_contribution_aretes_bords<Type_Champ::VECTORIEL>(ncomp,inco,matrice);
      ajouter_contribution_aretes_coins<Type_Champ::VECTORIEL>(ncomp,inco,matrice);
      ajouter_contribution_aretes_internes<Type_Champ::VECTORIEL>(ncomp,inco,matrice);
      ajouter_contribution_aretes_mixtes<Type_Champ::VECTORIEL>(ncomp,inco,matrice);
      ajouter_contribution_fa7_sortie_libre<Type_Champ::VECTORIEL>(ncomp,inco,matrice);
      ajouter_contribution_fa7_elem<Type_Champ::VECTORIEL>(ncomp,inco,matrice);
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_vitesse(const DoubleTab& , Matrice_Morse& ) const
{
  Cerr << "Yannick dit : ajouter_contribution_vitesse should not be called from " <<  op_base.que_suis_je() << finl;
  abort();
}

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <class _TYPE_> template<Type_Champ Field_Type>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords(const int ncomp, const DoubleTab& inco, DoubleTab& resu) const
{
  if (!_TYPE_::CALC_ARR_BORD) return resu;
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  for (int n_arete = premiere_arete_bord; n_arete < derniere_arete_bord; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_> template<Type_Champ Field_Type>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins(const int ncomp, const DoubleTab& inco, DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  for (int n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      const int n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PAROI_FLUIDE:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,TypeAreteCoinVDF::PAROI_FLUIDE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::FLUIDE_PAROI:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,TypeAreteCoinVDF::FLUIDE_PAROI,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::PERIO_PAROI:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,TypeAreteCoinVDF::PERIO_PAROI,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::FLUIDE_FLUIDE:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_COIN_FL,Type_Flux_Arete::COIN_FLUIDE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::PERIO_PERIO:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        default :
          break;
        }
    }
  return resu;
}

template <class _TYPE_> template<Type_Champ Field_Type>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_internes(const int ncomp, const DoubleTab& inco, DoubleTab& resu) const
{
  if(!_TYPE_::CALC_ARR_INT) return resu;
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++) ajouter_aretes_internes_<true,Type_Flux_Arete::INTERNE,Field_Type>(n_arete,ncomp,inco,resu);
  return resu;
}

template <class _TYPE_> template<Type_Champ Field_Type>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_mixtes(const int ncomp, const DoubleTab& inco, DoubleTab& resu) const
{
  if(!_TYPE_::CALC_ARR_MIXTE) return resu;
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  for (int n_arete = premiere_arete_mixte; n_arete < derniere_arete_mixte; n_arete++) ajouter_aretes_mixtes_<true,Type_Flux_Arete::MIXTE,Field_Type>(n_arete,ncomp,inco,resu,tab_flux_bords);
  return resu;
}

template <class _TYPE_> template<Type_Champ Field_Type>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_fa7_sortie_libre(const int ncomp, const DoubleTab& inco,DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  const int nb_front_Cl = la_zone->nb_front_Cl();
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          ajouter_fa7_sortie_libre_<_TYPE_::CALC_FA7_SORTIE_LIB,Type_Flux_Fa7::SORTIE_LIBRE,Field_Type>(ndeb,nfin,ncomp,(const Neumann_sortie_libre&) la_cl.valeur(),inco,resu,tab_flux_bords);
          break;
        case symetrie : /* fall through */
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_> template<Type_Champ Field_Type>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_fa7_elem(const int ncomp, const DoubleTab& inco, DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  for(int num_elem = 0 ; num_elem < nb_elem; num_elem++) ajouter_fa7_elem_<Type_Flux_Fa7::ELEM,Field_Type>(num_elem,ncomp,inco,resu,tab_flux_bords);
  corriger_flux_fa7_elem_periodicite<Field_Type>(ncomp,inco,resu);
  return resu;
}

template <class _TYPE_> template<Type_Champ Field_Type>
DoubleTab& T_It_VDF_Face<_TYPE_>::corriger_flux_fa7_elem_periodicite(const int ncomp, const DoubleTab& inco, DoubleTab& resu) const
{
  const int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis());
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          int num_elem, signe;
          for (int face=ndeb; face < nfin; face++)
            {
              int elem1 = elem(face,0), elem2 = elem(face,1), ori = orientation(face);
              if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) )
                {
                  num_elem = elem2;
                  signe = 1;
                }
              else
                {
                  num_elem = elem1;
                  signe = -1;
                }
              const int fac1 = elem_faces(num_elem,ori), fac2 = elem_faces(num_elem,ori+dimension);
              corriger_flux_fa7_elem_periodicite_<Field_Type>(ncomp,num_elem,fac1,fac2,face,signe,inco,resu);
            }
        }
    }
  return resu;
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::corriger_flux_fa7_elem_periodicite_(const int ncomp, const int num_elem, const int fac1, const int fac2, const int face, const int signe, const DoubleTab& inco, DoubleTab& resu) const
{
      ArrOfDouble flux(ncomp);
      flux_evaluateur.template flux_fa7<Type_Flux_Fa7::ELEM,Field_Type>(inco,num_elem,fac1,fac2,flux);
      for (int k=0; k<ncomp; k++) resu(face,k) += signe*flux(k);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords_(const int n_arete, const int ncomp, const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
      const int n = la_zone->nb_faces_bord(), fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      ArrOfDouble flux(ncomp);
      flux_evaluateur.template flux_arete<Arete_Type,Field_Type>(inco, fac1, fac2, fac3, signe, flux);
      for (int k=0; k < ncomp; k++)
        {
          resu(fac3,k) += signe*flux(k);
          if (is_PAROI) // TODO : FIXME : Yannick help :/ j'ai fait comme le cas scalaire
            {
              if (fac1 < n) tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux(k);
              if (fac2 < n) tab_flux_bords(fac2,orientation(fac3)) -= 0.5*signe*flux(k);
            }
        }
    }
}

template <class _TYPE_>  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords_(const int n_arete, const int ncomp, const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE), is_FLUIDE = (Arete_Type == Type_Flux_Arete::FLUIDE), is_PAROI_FL = (Arete_Type == Type_Flux_Arete::PAROI_FLUIDE);
      ArrOfDouble flux3(ncomp), flux1_2(ncomp);
      const int n = la_zone->nb_faces_bord(), fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Arete_Type,Field_Type>(inco, fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          int fac4 = signe;
          ArrOfDouble flux3_4 = flux3;
          for (int k=0; k<ncomp; k++)
            {
              resu(fac3,k) += 0.5*flux3_4(k);
              resu(fac4,k) -= 0.5*flux3_4(k);
            }
        }
      else for (int k=0; k<ncomp; k++) resu(fac3,k) += signe*flux3(k); // les autres Type_Flux_Arete ...

      fill_resu_tab(fac1,fac2,ncomp,flux1_2,resu); // pour tous les types !

      if (is_FLUIDE || is_PAROI_FL) // TODO : FIXME : Yannick help :/ j'ai fait comme le cas scalaire
        {
          if (fac1 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux3(k);
          if (fac2 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac2,orientation(fac3)) -= 0.5*signe*flux3(k);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, TypeAreteCoinVDF::type_arete Arete_Type_Coin, Type_Champ Field_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins_(const int n_arete, const int ncomp, const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PERIO_PAROI = (Arete_Type_Coin == TypeAreteCoinVDF::PERIO_PAROI);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      ArrOfDouble flux(ncomp);
      flux_evaluateur.template flux_arete<Arete_Type,Field_Type>(inco, fac1, fac2, fac3, signe,flux);
      for (int k = 0; k < ncomp; k++)
        {
          resu(fac3,k) += signe*flux(k);
          if (is_PERIO_PAROI) /* on met 0.25 sur les deux faces (car on  ajoutera deux fois la contrib) */
            {
              tab_flux_bords(fac1,orientation(fac3)) -= 0.25*signe*flux(k);
              tab_flux_bords(fac2,orientation(fac3)) -= 0.25*signe*flux(k);
            }
          else tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux(k);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins_(const int n_arete, const int ncomp, const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE);
      ArrOfDouble flux3(ncomp), flux1_2(ncomp);
      const int n = la_zone->nb_faces_bord(), fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Arete_Type,Field_Type>(inco, fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          int fac4 = signe;
          ArrOfDouble flux3_4 = flux3;
          for (int k=0; k<ncomp; k++)
            {
              resu(fac3,k) += 0.5*flux3_4(k);
              resu(fac4,k) -= 0.5*flux3_4(k);
              resu(fac1,k) += 0.5*flux1_2(k);
              resu(fac2,k) -= 0.5*flux1_2(k);
            }
        }
      else
        {
          for (int k=0; k<ncomp; k++)
            {
              resu(fac3,k) += signe*flux3(k);
              resu(fac1,k) += flux1_2(k);
              if (fac1 < n) tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux3(k); // TODO : FIXME : Yannick help :/ j'ai fait comme le cas scalaire (pas code a la base pour vect)
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_aretes_internes_(const int n_arete, const int ncomp, const DoubleTab& inco, DoubleTab& resu) const
{
  if(should_calc_flux) ajouter_aretes_mixtes_<should_calc_flux,Arete_Type,Field_Type>(n_arete,ncomp,inco,resu,op_base->flux_bords() /* inutile */);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_aretes_mixtes_(const int n_arete, const int ncomp, const DoubleTab& inco, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  if (should_calc_flux)
    {
      constexpr bool is_MIXTE = (Arete_Type == Type_Flux_Arete::MIXTE);
      ArrOfDouble flux(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      const int n = la_zone->nb_faces_bord(), n2 = la_zone->nb_faces_tot(); /* GF pour assurer bilan seq = para */
      flux_evaluateur.template flux_arete<Arete_Type,Field_Type>(inco, fac1, fac2, fac3, fac4, flux);
      fill_resu_tab(fac3,fac4,ncomp,flux,resu);
      if (is_MIXTE) // TODO : FIXME : Yannick help :/ j'ai fait comme le cas scalaire (pas code a la base pour vect)
        {
          if (fac4 < n2)
            {
              if (fac1 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac1,orientation(fac3)) -= flux(k);
              if (fac2 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac2,orientation(fac4)) -= flux(k);
            }
          if (fac3 < n2)
            {
              if (fac1 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac1,orientation(fac3)) += flux(k);
              if (fac2 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac2,orientation(fac4)) += flux(k);
            }
        }

      flux_evaluateur.template flux_arete<Arete_Type,Field_Type>(inco, fac3, fac4, fac1, fac2, flux);
      fill_resu_tab(fac1,fac2,ncomp,flux,resu);
      if (is_MIXTE) // TODO : FIXME : Yannick help :/ j'ai fait comme le cas scalaire (pas code a la base pour vect)
        {
          if (fac2 < n2)
            {
              if (fac3 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac3,orientation(fac1)) -= flux(k);
              if (fac4 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac4,orientation(fac2)) -= flux(k);
            }
          if (fac1 < n2)
            {
              if (fac3 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac3,orientation(fac1)) += flux(k);
              if (fac4 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac4,orientation(fac2)) += flux(k);
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_fa7_sortie_libre_(const int ndeb, const int nfin, const int ncomp , const Neumann_sortie_libre& cl, const DoubleTab& inco, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  if (should_calc_flux)
    {
      ArrOfDouble flux(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.template flux_fa7<Fa7_Type,Field_Type>(inco,face,cl,ndeb,flux);
          if ( (elem(face,0)) > -1) for (int k=0; k<ncomp; k++) resu(face,k) += flux(k);
          if ( (elem(face,1)) > -1) for (int k=0; k<ncomp; k++) resu(face,k) -= flux(k);
          // if (!is_SCALAIRE) for (int k=0; k<ncomp; k++) if (face<n) tab_flux_bords(face,k) -= flux(k); // TODO : FIXME : Yannick help :/
        }
    }
}

template <class _TYPE_> template <Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_fa7_elem_(const int num_elem, const int ncomp, const DoubleTab& inco, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  ArrOfDouble flux(ncomp);
  const int n = la_zone->nb_faces_bord();
  for (int fa7 = 0; fa7 < dimension; fa7++)
    {
      int fac1 = elem_faces(num_elem,fa7), fac2 = elem_faces(num_elem,fa7+dimension);
      flux_evaluateur.template flux_fa7<Fa7_Type,Field_Type>(inco, num_elem, fac1, fac2,flux);
      fill_resu_tab(fac1,fac2,ncomp,flux,resu);
      if (fac1 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac1,orientation(fac1)) += flux(k); // TODO : FIXME : Yannick help :/ j'ai fait comme le cas scalaire (pas code a la base pour vect)
      if (fac2 < n) for (int k=0; k<ncomp; k++) tab_flux_bords(fac2,orientation(fac2)) -= flux(k);
    }
}

template <class _TYPE_>
inline void T_It_VDF_Face<_TYPE_>::fill_resu_tab(const int fac1, const int fac2, const int ncomp, const ArrOfDouble& flux, DoubleTab& resu) const
{
  for (int k = 0; k < ncomp; k++)
    {
      resu(fac1,k) += flux(k);
      resu(fac2,k) -= flux(k);
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords(const int ncomp, const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete = premiere_arete_bord; n_arete < derniere_arete_bord; n_arete++)
    {
      int n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE,Field_Type>(n_arete,ncomp,matrice);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
        }
    }
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_coins(const int ncomp, const DoubleTab& inco , Matrice_Morse& matrice) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      int n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PERIO_PERIO:
          ajouter_contribution_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteCoinVDF::PERIO_PAROI:
          ajouter_contribution_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteCoinVDF::FLUIDE_FLUIDE:
          ajouter_contribution_aretes_coins_<true,Type_Flux_Arete::COIN_FLUIDE,Field_Type>(n_arete,ncomp,matrice);
          break;
        case TypeAreteCoinVDF::PAROI_FLUIDE: /* fall through */
        case TypeAreteCoinVDF::FLUIDE_PAROI:
          Process::exit(); /* non code */
          break;
        default :
          break;
        }
    }
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_internes(const int ncomp, const DoubleTab& inco , Matrice_Morse& matrice) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++) ajouter_contribution_aretes_internes_<Type_Flux_Arete::INTERNE,Field_Type>(n_arete,ncomp,matrice);
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_mixtes(const int ncomp, const DoubleTab& inco , Matrice_Morse& matrice) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  if (!_TYPE_::CALC_ARR_MIXTE) return;
  for (int n_arete = premiere_arete_mixte; n_arete < derniere_arete_mixte; n_arete++) ajouter_contribution_aretes_mixtes_<Type_Flux_Arete::MIXTE,Field_Type>(n_arete,ncomp,matrice);
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_sortie_libre(const int ncomp, const DoubleTab& inco , Matrice_Morse& matrice) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  const int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          ajouter_contribution_fa7_sortie_libre_<_TYPE_::CALC_FA7_SORTIE_LIB,Type_Flux_Fa7::SORTIE_LIBRE,Field_Type>(ndeb,nfin,ncomp,(const Neumann_sortie_libre&) la_cl.valeur(),matrice);
          break;
        case symetrie : /* fall through */
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>  template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_elem(const int ncomp, const DoubleTab& inco , Matrice_Morse& matrice) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for(int num_elem=0; num_elem<nb_elem; num_elem++) ajouter_contribution_fa7_elem_<Type_Flux_Fa7::ELEM,Field_Type>(num_elem,ncomp,matrice);
  corriger_coeffs_fa7_elem_periodicite<Field_Type>(ncomp,inco,matrice);
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::corriger_coeffs_fa7_elem_periodicite(const int ncomp, const DoubleTab& inco , Matrice_Morse& matrice) const
{
  int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis());
          int num_elem, signe, ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            {
              const int elem1 = elem(face,0), elem2 = elem(face,1), ori = orientation(face);
              if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) )
                {
                  num_elem = elem2;
                  signe = 1;
                }
              else
                {
                  num_elem = elem1;
                  signe = -1;
                }
              const int fac1 = elem_faces(num_elem,ori), fac2 = elem_faces(num_elem,ori+dimension);
              corriger_coeffs_fa7_elem_periodicite_<Field_Type>(ncomp,num_elem,fac1,fac2,face,signe,matrice);
            }
        }
    }
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::corriger_coeffs_fa7_elem_periodicite_(const int ncomp, const int num_elem, const int fac1, const int fac2, const int face, const int signe, Matrice_Morse& matrice) const
{
  ArrOfDouble aii(ncomp), ajj(ncomp);
  flux_evaluateur.template coeffs_fa7<Type_Flux_Fa7::ELEM,Field_Type>(num_elem, fac1, fac2, aii, ajj);
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  // XXX : Elie Saikali : j'ai fait comme le codage initial pour le cas Scalaire...
  if (signe>0) /* on a oublie a droite  la contribution de la gche */
    {
      for (int i = 0; i < ncomp; i++ ) for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1==face*ncomp+i) coeff[k] += aii(i);
      for (int i = 0; i < ncomp; i++ ) for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1==fac2*ncomp+i) coeff[k] -= ajj(i);
    }
  else /* on a oublie a gauche  la contribution de la droite */
    {
      for (int i = 0; i < ncomp; i++ ) for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1==fac1*ncomp+i) coeff[k] -= aii(i);
      for (int i = 0; i < ncomp; i++ ) for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1==face*ncomp+i) coeff[k] += ajj(i);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, const int ncomp, Matrice_Morse& matrice) const
{
  if (should_calc_flux)
    {
      ArrOfDouble aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
      for (int i = 0; i < ncomp; i++) fill_coeff_matrice_morse(fac3,i,ncomp,signe,aii3_4,matrice);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, const int ncomp, Matrice_Morse& matrice) const
{
  if (should_calc_flux)
    {
      ArrOfDouble aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
      for (int i = 0; i < ncomp; i++ )
        {
          fill_coeff_matrice_morse(fac3,i,ncomp,signe,aii3_4,matrice);
          fill_coeff_matrice_morse(fac1,fac2,i,ncomp,aii1_2,ajj1_2,matrice);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, const int ncomp, Matrice_Morse& matrice) const
{
  if (should_calc_flux)
    {
      ArrOfDouble aii(ncomp), ajj(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type,Field_Type>(fac3, fac4, fac1, fac2, aii, ajj);
      for (int i = 0; i < ncomp; i++ ) fill_coeff_matrice_morse(fac1,fac2,i,ncomp,aii,ajj,matrice);

      flux_evaluateur.template coeffs_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, fac4, aii, ajj);
      for (int i = 0; i < ncomp; i++ )
        {
          // XXX : Elie Saikali : j'ai fait comme le codage initial pour le cas Scalaire... par contre j'ai vire le test if (facxxx < nb_face_reelle).. inutile je pense
          aii(i) *= 0.5;
          ajj(i) *= 0.5;
          fill_coeff_matrice_morse(fac3,fac4,i,ncomp,aii,ajj,matrice);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_coins_(const int n_arete, const int ncomp, Matrice_Morse& matrice) const
{
  if (should_calc_flux)
    {
      constexpr bool is_COIN_FL = (Arete_Type == Type_Flux_Arete::COIN_FLUIDE);
      ArrOfDouble aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);

      for (int i = 0; i < ncomp; i++) fill_coeff_matrice_morse(fac3,i,ncomp,signe,aii3_4,matrice);
      if(is_COIN_FL) for (int i = 0; i < ncomp; i++) fill_coeff_matrice_morse(fac1,i,ncomp,1,aii1_2,matrice);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_coins_(const int n_arete, const int ncomp, Matrice_Morse& matrice) const
{
  if (should_calc_flux)
    {
      ArrOfDouble aii(ncomp), ajj(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type,Field_Type>(fac3, fac4, fac1, fac2, aii, ajj);
      for (int i = 0; i < ncomp; i++ ) fill_coeff_matrice_morse(fac1,fac2,i,ncomp,aii,ajj,matrice);

      flux_evaluateur.template coeffs_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, fac4, aii, ajj);
      for (int i = 0; i < ncomp; i++ ) fill_coeff_matrice_morse(fac3,fac4,i,ncomp,aii,ajj,matrice);
    }
}

template <class _TYPE_> template <Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_internes_(const int n_arete, const int ncomp, Matrice_Morse& matrice) const
{
  // XXX : Elie Saikali : j'ai fait comme le codage initial pour le cas Scalaire...
  ajouter_contribution_aretes_coins_<true,Arete_Type,Field_Type>(n_arete,ncomp,matrice); /* meme codage que le cas PERIODICITE */
}

template <class _TYPE_> template <Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_mixtes_(const int n_arete, const int ncomp, Matrice_Morse& matrice) const
{
  ajouter_contribution_aretes_coins_<true,Arete_Type,Field_Type>(n_arete,ncomp,matrice); /* meme codage que le cas PERIODICITE */
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_sortie_libre_(const int ndeb, const int nfin, const int ncomp, const Neumann_sortie_libre& cl, Matrice_Morse& matrice) const
{
  if (should_calc_flux)
    {
      ArrOfDouble aii(ncomp), ajj(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.template coeffs_fa7<Fa7_Type,Field_Type>(face,cl, aii, ajj);
          if ( (elem(face,0)) > -1) for (int i = 0; i < ncomp; i++ ) fill_coeff_matrice_morse(face,i,ncomp,1,aii,matrice);
          if ( (elem(face,1)) > -1) for (int i = 0; i < ncomp; i++ ) fill_coeff_matrice_morse(face,i,ncomp,1,ajj,matrice);
        }
    }
}

template <class _TYPE_> template <Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_elem_(const int num_elem, const int ncomp, Matrice_Morse& matrice) const
{
  ArrOfDouble aii(ncomp), ajj(ncomp);
  for (int fa7 = 0; fa7 < dimension; fa7++)
    {
      const int fac1 = elem_faces(num_elem,fa7), fac2 = elem_faces(num_elem,fa7+dimension);
      flux_evaluateur.template coeffs_fa7<Fa7_Type,Field_Type>(num_elem, fac1, fac2, aii, ajj);
      for (int i = 0; i < ncomp; i++ ) fill_coeff_matrice_morse(fac1,fac2,i,ncomp,aii,ajj,matrice);
    }
}

template <class _TYPE_>
inline void T_It_VDF_Face<_TYPE_>::fill_coeff_matrice_morse(const int face, const int i, const int ncomp, const int signe, const ArrOfDouble& A, Matrice_Morse& matrice) const
{
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1 == face*ncomp+i) coeff[k] += signe*A(i); // equivalent a matrice(face,face) += A(i)
}

template <class _TYPE_>
inline void T_It_VDF_Face<_TYPE_>::fill_coeff_matrice_morse(const int fac1, const int fac2, const int i, const int ncomp, const ArrOfDouble& A, const ArrOfDouble& B, Matrice_Morse& matrice) const
{
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int k = tab1[fac1*ncomp+i]-1; k < tab1[fac1*ncomp+1+i]-1; k++)
    {
      if (tab2[k]-1 == fac1*ncomp+i) coeff[k] += A(i); // equivalent a matrice(fac1,fac1) += A(i)
      if (tab2[k]-1 == fac2*ncomp+i) coeff[k] -= B(i); // equivalent a matrice(fac1,fac2) -= B(i)
    }
  for (int k = tab1[fac2*ncomp+i]-1; k < tab1[fac2*ncomp+1+i]-1; k++)
    {
      if (tab2[k]-1 == fac1*ncomp+i) coeff[k] -= A(i); // equivalent a matrice(fac2,fac1) -= A(i)
      if (tab2[k]-1 == fac2*ncomp+i) coeff[k] += B(i); // equivalent a matrice(fac2,fac2) += B(i)
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords(const int ncomp,DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete = premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++)
    {
      int n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE,Field_Type>(n_arete,ncomp,resu);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins(const int ncomp,DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      int n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PAROI_FLUIDE:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteCoinVDF::FLUIDE_PAROI:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteCoinVDF::PERIO_PAROI:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteCoinVDF::FLUIDE_FLUIDE:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_COIN_FL,Type_Flux_Arete::COIN_FLUIDE,Field_Type>(n_arete,ncomp,resu);
          break;
        case TypeAreteCoinVDF::PERIO_PERIO:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE,Field_Type>(n_arete,ncomp,resu);
          break;
        default :
          break;
        }
    }
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_internes(const int ncomp,DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++) contribuer_au_second_membre_aretes_internes_<true,Type_Flux_Arete::INTERNE,Field_Type>(n_arete,ncomp,resu);
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_mixtes(const int ncomp,DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for (int n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++) contribuer_au_second_membre_aretes_mixtes_<true,Type_Flux_Arete::MIXTE,Field_Type>(n_arete,ncomp,resu);
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_sortie_libre(const int ncomp,DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          contribuer_au_second_membre_fa7_sortie_libre_<_TYPE_::CALC_FA7_SORTIE_LIB,Type_Flux_Fa7::SORTIE_LIBRE,Field_Type>(ndeb,nfin,ncomp,(const Neumann_sortie_libre&) la_cl.valeur(),resu);
          break;
        case symetrie : /* fall through */
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_elem(const int ncomp,DoubleTab& resu) const
{
  if (Field_Type == Type_Champ::SCALAIRE && ncomp != 1) throw;
  for(int num_elem = 0; num_elem < nb_elem; num_elem++) contribuer_au_second_membre_fa7_elem_<Type_Flux_Fa7::ELEM,Field_Type>(num_elem,ncomp,resu);
  corriger_secmem_fa7_elem_periodicite<Field_Type>(ncomp,resu);
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::corriger_secmem_fa7_elem_periodicite(const int ncomp,DoubleTab& resu) const // TODO : FIXME : a factoriser avec corriger_flux_fa7_elem_periodicite !!
{
  const int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis());
          int num_elem, signe, ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            {
              int elem1 = elem(face,0), elem2 = elem(face,1), ori = orientation(face);
              if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) )
                {
                  num_elem = elem2;
                  signe = 1;
                }
              else
                {
                  num_elem = elem1;
                  signe = -1;
                }
              const int fac1 = elem_faces(num_elem,ori), fac2 = elem_faces(num_elem,ori+dimension);
              corriger_secmem_fa7_elem_periodicite_<Field_Type>(ncomp,num_elem,fac1,fac2,face,signe,resu);
            }
        }
    }
}

template <class _TYPE_> template<Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::corriger_secmem_fa7_elem_periodicite_(const int ncomp, const int num_elem, const int fac1, const int fac2, const int face, const int signe, DoubleTab& resu) const
{
  ArrOfDouble flux(ncomp);
  flux_evaluateur.template secmem_fa7<Type_Flux_Fa7::ELEM,Field_Type>(num_elem,fac1,fac2,flux);
  for (int k = 0; k < ncomp; k++) resu(face,k) += signe*flux(k);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  if (should_calc_flux)
    {
      ArrOfDouble flux(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, signe, flux);
      for (int k = 0; k < ncomp; k++) resu(fac3,k) += signe*flux(k);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE);
      ArrOfDouble flux3(ncomp), flux1_2(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          ArrOfDouble flux3_4 = flux3;
          const int fac4 = signe;
          for (int k=0; k<ncomp; k++)
            {
              resu(fac3,k) += 0.5*flux3_4(k);
              resu(fac4,k) -= 0.5*flux3_4(k);
            }
        }
      else for (int k=0; k<ncomp; k++) resu(fac3,k) += signe*flux3(k);
      fill_resu_tab(fac1,fac2,ncomp,flux1_2,resu);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  // XXX : Elie Saikali : j'ai fait comme le codage initial pour le cas Scalaire...
  contribuer_au_second_membre_aretes_bords_<should_calc_flux,Arete_Type,Field_Type>(n_arete,ncomp,resu);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE);
      ArrOfDouble flux3(ncomp), flux1_2(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type,Field_Type>(fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          ArrOfDouble flux3_4 = flux3;
          const int fac4 = signe;
          for (int k=0; k<ncomp; k++)
            {
              resu(fac3,k) += 0.5 * flux3_4(k);
              resu(fac4,k) -= 0.5 * flux3_4(k);
              resu(fac1,k) += 0.5 * flux1_2(k);
              resu(fac2,k) -= 0.5 * flux1_2(k);
            }
        }
      else
        for (int k=0; k<ncomp; k++)
          {
            resu(fac3,k) += signe * flux3(k);
            resu(fac1,k) += flux1_2(k);
          }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_internes_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  if (should_calc_flux)
    {
      ArrOfDouble flux(ncomp);
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type,Field_Type>(fac1,fac2,fac3,fac4,flux);
      fill_resu_tab(fac3,fac4,ncomp,flux,resu);

      flux_evaluateur.template secmem_arete<Arete_Type,Field_Type>(fac3,fac4,fac1,fac2,flux);
      fill_resu_tab(fac1,fac2,ncomp,flux,resu);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_mixtes_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  contribuer_au_second_membre_aretes_internes_<should_calc_flux,Arete_Type,Field_Type>(n_arete,ncomp,resu);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_sortie_libre_(const int ndeb, const int nfin, const int ncomp, const Neumann_sortie_libre& cl, DoubleTab& resu) const
{
  if (should_calc_flux)
    {
      ArrOfDouble flux(ncomp);
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.template secmem_fa7<Fa7_Type,Field_Type>(face,cl, ndeb, flux);
          if ( (elem(face,0)) > -1) for (int k = 0; k < ncomp; k++) resu(face,k) += flux(k);
          if ( (elem(face,1)) > -1) for (int k = 0; k < ncomp; k++) resu(face,k) -= flux(k);
        }
    }
}

template <class _TYPE_> template <Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_elem_(const int num_elem, const int ncomp, DoubleTab& resu) const
{
  ArrOfDouble flux(ncomp);
  for (int fa7 = 0; fa7 < dimension; fa7++)
    {
      const int fac1 = elem_faces(num_elem,fa7), fac2 = elem_faces(num_elem,fa7+dimension);
      flux_evaluateur.template secmem_fa7<Fa7_Type,Field_Type>(num_elem,fac1,fac2,flux);
      fill_resu_tab(fac1,fac2,ncomp,flux,resu);
    }
}

#endif /* T_It_VDF_Face_TPP_included */
