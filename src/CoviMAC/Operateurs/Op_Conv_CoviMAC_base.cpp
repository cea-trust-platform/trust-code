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
// File:        Op_Conv_CoviMAC_base.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_CoviMAC_base.h>
#include <TRUSTTrav.h>
#include <Discretisation_base.h>
#include <Champ.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <SFichier.h>
#include <EcrFicPartage.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>

Implemente_base(Op_Conv_CoviMAC_base,"Op_Conv_CoviMAC_base",Operateur_Conv_base);

//// printOn
//

Sortie& Op_Conv_CoviMAC_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_CoviMAC_base::readOn(Entree& s )
{
  return s ;
}



///////////////////////////////////////////////////////////////////////////////////
//
//    Implementation de fonctions de la classe Op_Conv_CoviMAC_base
//
///////////////////////////////////////////////////////////////////////////////////

/*
// cf Op_Conv_CoviMAC_base::calculer_dt_stab() pour choix de calcul de dt_stab
void Op_Conv_CoviMAC_base::calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const
{
  if (Motcle(option)=="stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage->valeurs();
      es_valeurs = 1.e30;

      const Zone_VDF& zone_VDF = iter.zone();
      const Zone_Cl_VDF& zone_Cl_VDF = iter.zone_Cl();
      const IntTab& face_voisins = zone_VDF.face_voisins();
      const DoubleVect& volumes = zone_VDF.volumes();
      const DoubleVect& face_surfaces = zone_VDF.face_surfaces();
      const DoubleVect& vit = vitesse().valeurs();
      DoubleTrav fluent(zone_VDF.zone().nb_elem_tot());

      // Remplissage du tableau fluent

      fluent = 0;
      double psc;
      int num1,num2,face;
      int elem1;

      // On traite les bords

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

          if ( sub_type(Dirichlet_entree_fluide,la_cl.valeur())
               || sub_type(Neumann_sortie_libre,la_cl.valeur())  )

            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              num1 = le_bord.num_premiere_face();
              num2 = num1 + le_bord.nb_faces();
              for (face=num1; face<num2; face++)
                {
                  psc = vit[face]*face_surfaces(face);
                  if ( (elem1 = face_voisins(face,0)) != -1)
                    {
                      if (psc < 0)
                        fluent[elem1] -= psc;
                    }
                  else // (elem2 != -1)
                    if (psc > 0)
                      fluent[face_voisins(face,1)] += psc;
                }
            }
        }



      // Boucle sur les faces internes pour remplir fluent
      int zone_VDF_nb_faces=zone_VDF.nb_faces();
      for (face=zone_VDF.premiere_face_int(); face<zone_VDF_nb_faces; face++)
        {
          psc = vit[face]*face_surfaces(face);
          eval_fluent(psc,face_voisins(face,0),face_voisins(face,1),fluent);
        }
      //fluent.echange_espace_virtuel();
      if (vitesse().le_nom()=="rho_u")
        diviser_par_rho_si_dilatable(fluent,equation().milieu());

      int zone_VDF_nb_elem=zone_VDF.nb_elem();
      for (int num_poly=0; num_poly<zone_VDF_nb_elem; num_poly++)
        {
          es_valeurs(num_poly) = volumes(num_poly)/(fluent[num_poly]+1.e-30);
        }

      //double dt_min = mp_min_vect(es_valeurs);
      //assert(dt_min==calculer_dt_stab());
    }
  else
    Operateur_Conv_base::calculer_pour_post(espace_stockage,option,comp);
}

Motcle Op_Conv_CoviMAC_base::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="stabilite")
    loc = "elem";
  else
    return Operateur_Conv_base::get_localisation_pour_post(option);
  return loc;
}
*/
void Op_Conv_CoviMAC_base::completer()
{
  Operateur_base::completer();
}

void Op_Conv_CoviMAC_base::associer_zone_cl_dis(const Zone_Cl_dis_base& zcl)
{
  la_zcl_poly_ = ref_cast(Zone_Cl_CoviMAC,zcl);
}

void Op_Conv_CoviMAC_base::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zcl,const Champ_Inc& )
{
  la_zone_poly_ = ref_cast(Zone_CoviMAC,zone_dis.valeur());
  la_zcl_poly_ = ref_cast(Zone_Cl_CoviMAC,zcl.valeur());

}
int Op_Conv_CoviMAC_base::impr(Sortie& os) const
{
  if (flux_bords_.nb_dim() != 2) return 0;
  const int impr_bord=(la_zone_poly_->zone().Bords_a_imprimer().est_vide() ? 0:1);
  SFichier Flux_div;
  ouvrir_fichier(Flux_div,"",je_suis_maitre());
  EcrFicPartage Flux_face;
  ouvrir_fichier_partage(Flux_face,"",impr_bord);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  double temps = sch.temps_courant();
  if (je_suis_maitre()) Flux_div.add_col(temps);

  int nb_compo=flux_bords_.dimension(1);
  // On parcours les frontieres pour sommer les flux par frontiere dans le tableau flux_bord
  DoubleVect flux_bord(nb_compo);
  DoubleVect bilan(nb_compo);
  bilan = 0;
  for (int num_cl=0; num_cl<la_zone_poly_->nb_front_Cl(); num_cl++)
    {
      flux_bord=0;
      const Cond_lim& la_cl = la_zcl_poly_->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      for (int face=ndeb; face<nfin; face++)
        for(int k=0; k<nb_compo; k++)
          flux_bord(k)+=flux_bords_(face, k);
      for(int k=0; k<nb_compo; k++)
        flux_bord(k)=Process::mp_sum(flux_bord(k));

      if(je_suis_maitre())
        {
          for(int k=0; k<nb_compo; k++)
            {
              //Ajout pour impression sur fichiers separes
              Flux_div.add_col(flux_bord(k));
              bilan(k)+=flux_bord(k);
            }
        }
    }

  if(je_suis_maitre())
    {
      for(int k=0; k<nb_compo; k++)
        {
          Flux_div.add_col(bilan(k));
        }
      Flux_div << finl;
    }

  for (int num_cl=0; num_cl<la_zone_poly_->nb_front_Cl(); num_cl++)
    {
      const Frontiere_dis_base& la_fr = la_zcl_poly_->les_conditions_limites(num_cl).frontiere_dis();
      const Cond_lim& la_cl = la_zcl_poly_->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      if (la_zone_poly_->zone().Bords_a_imprimer().contient(la_fr.le_nom()))
        {
          Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
          for (int face=ndeb; face<nfin; face++)
            {
              if (dimension==2)
                Flux_face << "# Face a x= " << la_zone_poly_->xv(face,0) << " y= " << la_zone_poly_->xv(face,1) << " flux=" ;
              else if (dimension==3)
                Flux_face << "# Face a x= " << la_zone_poly_->xv(face,0) << " y= " << la_zone_poly_->xv(face,1) << " z= " << la_zone_poly_->xv(face,2) << " flux=" ;
              for(int k=0; k<nb_compo; k++)
                Flux_face << " " << flux_bords_(face, k);
              Flux_face << finl;
            }
          Flux_face.syncfile();
        }
    }

  return 1;
}

void Op_Conv_CoviMAC_base::associer_vitesse(const Champ_base& ch )
{
  vitesse_= ref_cast(Champ_Face_CoviMAC, ch);
}
