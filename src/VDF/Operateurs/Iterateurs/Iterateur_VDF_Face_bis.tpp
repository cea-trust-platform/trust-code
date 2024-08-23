/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Iterateur_VDF_Face_bis_TPP_included
#define Iterateur_VDF_Face_bis_TPP_included

template<class _TYPE_>
inline Iterateur_VDF_Face<_TYPE_>::Iterateur_VDF_Face(const Iterateur_VDF_Face<_TYPE_>& iter) :
  Iterateur_VDF_base(iter), flux_evaluateur(iter.flux_evaluateur), nb_elem(iter.nb_elem), premiere_arete_interne(iter.premiere_arete_interne), derniere_arete_interne(iter.derniere_arete_interne),
  premiere_arete_mixte(iter.premiere_arete_mixte), derniere_arete_mixte(iter.derniere_arete_mixte), premiere_arete_bord(iter.premiere_arete_bord), derniere_arete_bord(iter.derniere_arete_bord),
  premiere_arete_coin(iter.premiere_arete_coin), derniere_arete_coin(iter.derniere_arete_coin)
{
  orientation.ref(iter.orientation);
  Qdm.ref(iter.Qdm);
  elem.ref(iter.elem);
  elem_faces.ref(iter.elem_faces);
  type_arete_bord.ref(iter.type_arete_bord);
  type_arete_coin.ref(iter.type_arete_coin);
}

template<class _TYPE_>
void Iterateur_VDF_Face<_TYPE_>::completer_()
{
  nb_elem = le_dom->nb_elem_tot();
  orientation.ref(le_dom->orientation());
  Qdm.ref(le_dom->Qdm());
  elem.ref(le_dom->face_voisins());
  elem_faces.ref(le_dom->elem_faces());
  type_arete_bord.ref(la_zcl->type_arete_bord());
  type_arete_coin.ref(la_zcl->type_arete_coin());
  premiere_arete_interne = le_dom->premiere_arete_interne();
  derniere_arete_interne = premiere_arete_interne + le_dom->nb_aretes_internes();
  premiere_arete_mixte = le_dom->premiere_arete_mixte();
  derniere_arete_mixte = premiere_arete_mixte + le_dom->nb_aretes_mixtes();
  premiere_arete_bord = le_dom->premiere_arete_bord();
  derniere_arete_bord = premiere_arete_bord + le_dom->nb_aretes_bord();
  premiere_arete_coin = le_dom->premiere_arete_coin();
  derniere_arete_coin = premiere_arete_coin + le_dom->nb_aretes_coin();
}

template<class _TYPE_>
inline void Iterateur_VDF_Face<_TYPE_>::multiply_by_rho_if_hydraulique(DoubleTab& tab_flux_bords) const
{
  Nom nom_eqn = la_zcl->equation().que_suis_je();
  /* Modif B.Mathieu pour front-tracking: masse_volumique() invalide en f.t.*/
  if (nom_eqn.debute_par("Navier_Stokes") && nom_eqn != "Navier_Stokes_Melange" && nom_eqn != "Navier_Stokes_FT_Disc")
    {
      const Champ_base& rho = la_zcl->equation().milieu().masse_volumique().valeur();
      if (sub_type(Champ_Uniforme, rho))
        {
          const double coef = rho.valeurs()(0, 0);
          const int nb_faces_bord = le_dom->nb_faces_bord();
          for (int face = 0; face < nb_faces_bord; face++)
            for (int k = 0; k < tab_flux_bords.line_size(); k++) tab_flux_bords(face, k) *= coef;
        }
    }
}

template<class _TYPE_>
int Iterateur_VDF_Face<_TYPE_>::impr(Sortie& os) const
{
  const Domaine& mon_dom = le_dom->domaine();
  const int impr_mom = mon_dom.moments_a_imprimer();
  const int impr_sum = (mon_dom.bords_a_imprimer_sum().est_vide() ? 0 : 1), impr_bord = (mon_dom.bords_a_imprimer().est_vide() ? 0 : 1);
  const Schema_Temps_base& sch = la_zcl->equation().probleme().schema_temps();
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  DoubleVect bilan(tab_flux_bords.dimension(1));
  DoubleTab xgr;
  if (impr_mom) xgr = le_dom->calculer_xgr();
  int k, face, nb_front_Cl = le_dom->nb_front_Cl();
  DoubleTrav flux_bords2(5, nb_front_Cl, tab_flux_bords.dimension(1));
  flux_bords2 = 0;
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl->frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces(), periodicite = (type_cl(la_cl) == periodique ? 1 : 0);
      for (face = ndeb; face < nfin; face++)
        {
          for (k = 0; k < tab_flux_bords.dimension(1); k++)
            {
              flux_bords2(0, num_cl, k) += tab_flux_bords(face, k);
              if (periodicite)
                {
                  if (face < (ndeb + frontiere_dis.nb_faces() / 2))
                    flux_bords2(1, num_cl, k) += tab_flux_bords(face, k);
                  else
                    flux_bords2(2, num_cl, k) += tab_flux_bords(face, k);
                }
              if (mon_dom.bords_a_imprimer_sum().contient(frontiere_dis.le_nom()))
                flux_bords2(3, num_cl, k) += tab_flux_bords(face, k);
            } /* fin for k */
          if (impr_mom)
            {
              if (dimension == 2)
                flux_bords2(4, num_cl, 0) += tab_flux_bords(face, 1) * xgr(face, 0) - tab_flux_bords(face, 0) * xgr(face, 1);
              else
                {
                  flux_bords2(4, num_cl, 0) += tab_flux_bords(face, 2) * xgr(face, 1) - tab_flux_bords(face, 1) * xgr(face, 2);
                  flux_bords2(4, num_cl, 1) += tab_flux_bords(face, 0) * xgr(face, 2) - tab_flux_bords(face, 2) * xgr(face, 0);
                  flux_bords2(4, num_cl, 2) += tab_flux_bords(face, 1) * xgr(face, 0) - tab_flux_bords(face, 0) * xgr(face, 1);
                }
            }
        } /* fin for face */
    }
  mp_sum_for_each_item(flux_bords2);
  if (je_suis_maitre())
    {
      //SFichier Flux;
      if (!Flux.is_open())
        op_base->ouvrir_fichier(Flux, "", 1);
      //SFichier Flux_moment;
      if (!Flux_moment.is_open())
        op_base->ouvrir_fichier(Flux_moment, "moment", impr_mom);
      //SFichier Flux_sum;
      if (!Flux_sum.is_open())
        op_base->ouvrir_fichier(Flux_sum, "sum", impr_sum);
      Flux.add_col(sch.temps_courant());
      if (impr_mom)
        Flux_moment.add_col(sch.temps_courant());
      if (impr_sum)
        Flux_sum.add_col(sch.temps_courant());
      for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
        {
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          int periodicite = (type_cl(la_cl) == periodique ? 1 : 0);
          for (k = 0; k < tab_flux_bords.dimension(1); k++)
            {
              if (periodicite)
                {
                  Flux.add_col(flux_bords2(1, num_cl, k));
                  Flux.add_col(flux_bords2(2, num_cl, k));
                }
              else
                Flux.add_col(flux_bords2(0, num_cl, k));

              if (impr_sum)
                Flux_sum.add_col(flux_bords2(3, num_cl, k));
              bilan(k) += flux_bords2(0, num_cl, k);
            }
          if (dimension == 3)
            {
              for (k = 0; k < tab_flux_bords.dimension(1); k++)
                if (impr_mom)
                  Flux_moment.add_col(flux_bords2(4, num_cl, k));
            }
          else if (impr_mom)
            Flux_moment.add_col(flux_bords2(4, num_cl, 0));
        } /* fin for num_cl */
      for (k = 0; k < tab_flux_bords.dimension(1); k++)
        Flux.add_col(bilan(k));
      Flux << finl;
      if (impr_sum)
        Flux_sum << finl;
      if (impr_mom)
        Flux_moment << finl;
    }
  const LIST (Nom)
  &Liste_bords_a_imprimer = le_dom->domaine().bords_a_imprimer();
  if (!Liste_bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_face;
      op_base->ouvrir_fichier_partage(Flux_face, "", impr_bord);
      for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = la_zcl->les_conditions_limites(num_cl)->frontiere_dis();
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl->frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
          if (mon_dom.bords_a_imprimer().contient(la_fr.le_nom()))
            {
              if (je_suis_maitre())
                {
                  Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_face);
                  Flux_face << " : " << finl;
                }
              for (face = ndeb; face < nfin; face++)
                {
                  if (dimension == 2)
                    Flux_face << "# Face a x= " << le_dom->xv(face, 0) << " y= " << le_dom->xv(face, 1) << " : ";
                  else if (dimension == 3)
                    Flux_face << "# Face a x= " << le_dom->xv(face, 0) << " y= " << le_dom->xv(face, 1) << " z= " << le_dom->xv(face, 2) << " : ";
                  for (k = 0; k < tab_flux_bords.dimension(1); k++)
                    Flux_face << tab_flux_bords(face, k) << " ";
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

#endif /* Iterateur_VDF_Face_bis_TPP_included */
