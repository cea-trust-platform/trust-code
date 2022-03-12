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
// File:        Op_Diff_VDF_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift/Op_Diff_Dift_base
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Check_espace_virtuel.h>
#include <Op_Diff_VDF_base.h>
#include <Eval_Diff_VDF.h>
#include <TRUSTTrav.h>
#include <Champ_Don.h>
#include <Motcle.h>

Implemente_base(Op_Diff_VDF_base,"Op_Diff_VDF_base",Operateur_Diff_base);

Sortie& Op_Diff_VDF_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Diff_VDF_base::readOn(Entree& s ) { return s ; }

void Op_Diff_VDF_base::completer()
{
  Operateur_base::completer();
  // Certains operateurs (Axi) n'ont pas d'iterateurs en VDF...
  // Encore une anomalie dans la conception a corriger un jour !
  if (iter.non_nul()) iter.completer_();
}

int Op_Diff_VDF_base::impr(Sortie& os) const
{
  // Certains operateurs (Axi) n'ont pas d'iterateurs en VDF...
  // Encore une anomalie dans la conception a corriger un jour !
  return (iter.non_nul()) ? iter.impr(os) : 0;
}

void Op_Diff_VDF_base::calculer_flux_bord(const DoubleTab& inco) const
{
  iter.valeur().calculer_flux_bord(inco);
}

// Description:
// ajoute la contribution de la diffusion au second membre resu
DoubleTab& Op_Diff_VDF_base::ajouter(const DoubleTab& inco,  DoubleTab& resu) const
{
  // on fait += sur les espaces virtuels, pas grave s'ils contiennent n'importe quoi
  assert_invalide_items_non_calcules(resu, 0.);
  iter.ajouter(inco,resu);
  if (equation().domaine_application() == Motcle("Hydraulique"))
    {
      // On est dans le cas des equations de Navier_Stokes
      // Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
      if(Objet_U::bidim_axi == 1)
        {
          const Zone_VDF& zvdf=iter.zone();
          const DoubleTab& xv=zvdf.xv();
          const IntVect& ori=zvdf.orientation();
          const IntTab& face_voisins=zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces=zvdf.volumes_entrelaces();
          int face, nb_faces=zvdf.nb_faces(); //, cst;         // unused
          DoubleTrav diffu_tot(zvdf.nb_elem_tot());
          double db_diffusivite;
          Nom nom_eq=equation().que_suis_je();
          if ((nom_eq == "Navier_Stokes_standard")||(nom_eq == "Navier_Stokes_QC")||(nom_eq == "Navier_Stokes_FT_Disc"))
            {
              const Eval_Diff_VDF& eval=dynamic_cast<const Eval_Diff_VDF&> (iter.evaluateur());
              const Champ_base& ch_diff=eval.get_diffusivite();
              const DoubleTab& tab_diffusivite=ch_diff.valeurs();
              if (tab_diffusivite.size() == 1)
                diffu_tot = tab_diffusivite(0,0);
              else
                diffu_tot = tab_diffusivite;
              for (face=0; face<nb_faces; face++)
                {
                  if(ori(face)==0)
                    {
                      int elem1=face_voisins(face,0);
                      int elem2=face_voisins(face,1);
                      if(elem1==-1)
                        db_diffusivite=diffu_tot(elem2);
                      else if (elem2==-1)
                        db_diffusivite=diffu_tot(elem1);
                      else
                        db_diffusivite=0.5*(diffu_tot(elem2)+diffu_tot(elem1));
                      double r= xv(face,0);
                      if(r>=1.e-24)
                        resu(face) -=inco(face)*db_diffusivite*volumes_entrelaces(face)/(r*r);
                    }
                }
            }
          else if (equation().que_suis_je() == "Navier_Stokes_Interface_avec_trans_masse" ||
                   equation().que_suis_je() == "Navier_Stokes_Interface_sans_trans_masse" ||
                   equation().que_suis_je() == "Navier_Stokes_Front_Tracking" ||
                   equation().que_suis_je() == "Navier_Stokes_Front_Tracking_BMOL")
            {
              // Voir le terme source axi dans Interfaces/VDF
            }
          else
            {
              Cerr << "Probleme dans Op_Diff_VDF_base::ajouter avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_std" << finl;
              exit();
            }
        }
    }
  return resu;
}

//Description:
//on assemble la matrice.
void Op_Diff_VDF_base::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  iter.ajouter_contribution(inco, matrice);
  if (equation().domaine_application() == Motcle("Hydraulique"))
    // On est dans le cas des equations de Navier_Stokes
    {
      // Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
      if(Objet_U::bidim_axi == 1)
        {
          const Zone_VDF& zvdf=iter.zone();
          const DoubleTab& xv=zvdf.xv();
          const IntVect& ori=zvdf.orientation();
          const IntTab& face_voisins=zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces=zvdf.volumes_entrelaces();
          int face, nb_faces=zvdf.nb_faces();//, cst;
          DoubleTrav diffu_tot(zvdf.nb_elem_tot());
          double db_diffusivite;

          if (equation().que_suis_je() == "Navier_Stokes_standard")
            {
              const Eval_Diff_VDF& eval=dynamic_cast<const Eval_Diff_VDF&> (iter.evaluateur());
              const Champ_base& ch_diff=eval.get_diffusivite();
              const DoubleTab& tab_diffusivite=ch_diff.valeurs();
              if (tab_diffusivite.size() == 1)
                diffu_tot = tab_diffusivite(0,0);
              else
                diffu_tot = tab_diffusivite;
            }
          else
            {
              Cerr << "Probleme dans Op_Diff_VDF_base::contribuer_a_avec  avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_std" << finl;
              exit();
            }

          for(face=0; face<nb_faces; face++)
            if(ori(face)==0)
              {
                int elem1=face_voisins(face,0);
                int elem2=face_voisins(face,1);
                if(elem1==-1)
                  db_diffusivite=diffu_tot(elem2);
                else if (elem2==-1)
                  db_diffusivite=diffu_tot(elem1);
                else
                  db_diffusivite=0.5*(diffu_tot(elem2)+diffu_tot(elem1));
                double r= xv(face,0);
                if(r>=1.e-24)
                  matrice(face,face)+=db_diffusivite*volumes_entrelaces(face)/(r*r);
              }
        }
    }
}

//Description:
//on ajoute la contribution du second membre.
void Op_Diff_VDF_base::contribuer_au_second_membre(DoubleTab& resu) const
{
  iter.contribuer_au_second_membre(resu);
  if (equation().domaine_application() == Motcle("Hydraulique"))
    // On est dans le cas des equations de Navier_Stokes
    {
      // Ajout du terme du au terme supplementaire en V/(R*R)
      // dans le cas des coordonnees axisymetriques
      if(Objet_U::bidim_axi == 1)
        {
          const Zone_VDF& zvdf=iter.zone();
          const Zone_Cl_VDF& zclvdf = iter.zone_Cl();
          const DoubleTab& xv=zvdf.xv();
          const IntVect& ori=zvdf.orientation();
          const IntTab& face_voisins=zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces=zvdf.volumes_entrelaces();
          //          int nb_faces=zvdf.nb_faces();
          DoubleTrav diffu_tot(zvdf.nb_elem_tot());
          double db_diffusivite;

          if (equation().que_suis_je() == "Navier_Stokes_standard")
            {
              const Eval_Diff_VDF& eval=dynamic_cast<const Eval_Diff_VDF&> (iter.evaluateur());
              const Champ_base& ch_diff=eval.get_diffusivite();
              const DoubleTab& tab_diffusivite=ch_diff.valeurs();
              if (tab_diffusivite.size() == 1)
                diffu_tot = tab_diffusivite(0,0);
              else
                diffu_tot = tab_diffusivite;
            }
          else
            {
              Cerr << "Probleme dans Op_Diff_VDF_base::contribuer_au_second_membre  avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_std" << finl;
              exit();
            }

          // Boucle sur les conditions limites pour traiter les faces de bord

          int ndeb,nfin,num_face;

          for (int n_bord=0; n_bord<zvdf.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);

              if (sub_type(Dirichlet,la_cl.valeur()))
                {
                  const Dirichlet& la_cl_diri = ref_cast(Dirichlet,la_cl.valeur());
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  double r_2,coef;
                  for (num_face=ndeb; num_face<nfin; num_face++)
                    {
                      if (ori(num_face) == 0)
                        {
                          int elem1=face_voisins(num_face,0);
                          int elem2=face_voisins(num_face,1);
                          if(elem1==-1)
                            db_diffusivite=diffu_tot(elem2);
                          else
                            {
                              assert (elem2==-1);
                              db_diffusivite=diffu_tot(elem1);
                            }
                          r_2 = xv(num_face,0)*xv(num_face,0);
                          coef = volumes_entrelaces(num_face)/r_2;
                          resu(num_face) -= (la_cl_diri.val_imp(num_face-ndeb)*coef)*db_diffusivite;
                        }
                    }
                }
            }
        }
    }
}

// Description:
// calcule la contribution de la diffusion, la range dans resu
DoubleTab& Op_Diff_VDF_base::calculer(const DoubleTab& inco,  DoubleTab& resu) const
{
  resu =0;
  return ajouter(inco, resu);
}
