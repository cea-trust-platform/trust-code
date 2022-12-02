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

#ifndef Iterateur_VDF_Elem_bis_TPP_included
#define Iterateur_VDF_Elem_bis_TPP_included

template <class _TYPE_>
void  Iterateur_VDF_Elem<_TYPE_>::modifier_flux() const
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
          if (sub_type(Op_Conv_VDF_base,op))
            if (ref_cast(Op_Conv_VDF_base,op).vitesse().le_nom()=="rho_u") is_rho_u = 1;
        }
      const int nb_faces_bords = la_zone_vdf.nb_faces_bord();
      for (int face = 0; face < nb_faces_bords; face++)
        for(int k = 0; k < flux_bords.dimension(1); k++)
          {
            int e = (face_voisins(face, 0) != -1) ? face_voisins(face, 0) : face_voisins(face, 1);
            const double rho_ = (is_rho_u) ? 1.0 : rho(!rho_uniforme * e, k);
            flux_bords(face, k) *= rho_ * Cp(!cp_uniforme * e, k);
          }
    }
}

template <class _TYPE_>
int Iterateur_VDF_Elem<_TYPE_>::impr(Sortie& os) const
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
  DoubleTrav flux_bords2( 3, nb_front_Cl , flux_bords.dimension(1));
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
                          Flux_face << " surface_face(m2)= " << la_zone_vdf.face_surfaces(face);
                          Flux_face << " flux_par_surface(W/m2)= " << flux_bords(face, k)/la_zone_vdf.face_surfaces(face);
                        }
                      Flux_face << " flux(W)= " << flux_bords(face, k);
                    }
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

#endif /* Iterateur_VDF_Elem_bis_TPP_included */
