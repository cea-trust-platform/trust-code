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
// File:        Op_Dift_VDF_base2.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_op
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_base2.h>
#include <Eval_Dift_VDF_const2.h>
#include <Eval_Dift_VDF_var2.h>
#include <Champ_Fonc.h>
#include <Motcle.h>
#include <DoubleTrav.h>
#include <Mod_turb_hyd_base.h>

Implemente_base(Op_Dift_VDF_base2,"Op_Dift_VDF_base2",Op_Diff_VDF_base);

Sortie& Op_Dift_VDF_base2::printOn(Sortie& is) const
{
  return Op_Diff_VDF_base::printOn(is);
}

Entree& Op_Dift_VDF_base2::readOn(Entree& is)
{
  Op_Diff_VDF_base::readOn(is);
  return is;
}

//On utilise plus cette methode mais on la conserve en commentaire
//pour en garder une trace (elle sera aussi tracee dans RoundUp demande DE243)
/*
  int sub_type_by_name(const char * const type_name, const Objet_U & objet)
  {
  const Type_info * base_info = Type_info::type_info_from_name(type_name);
  if (!base_info) {
  Cerr << "Erreur sub_type : " << type_name << " n'est pas un type" << finl;
  Process::exit();
  }
  const Type_info * objet_info = objet.get_info();

  int resu = 0;
  if (objet_info == base_info)
  resu = 1;
  else if (objet_info->has_base(base_info))
  resu = 1;
  return resu;
  }
*/

DoubleTab& Op_Dift_VDF_base2::ajouter(const DoubleTab& inco,  DoubleTab& resu) const
{
  iter.ajouter(inco, resu);
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

          const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
          if (sub_type(Mod_turb_hyd_base,modele_turbulence.valeur()))
            {
              const Eval_Diff_VDF2& eval=dynamic_cast<const Eval_Diff_VDF2&> (iter.evaluateur());
              const Champ_base& ch=eval.get_diffusivite();
              const DoubleVect& tab_diffusivite=ch.valeurs();
              int size=diffu_tot.size_totale();
              if (tab_diffusivite.size() == 1)
                {
                  db_diffusivite = tab_diffusivite[0];
                  const Eval_Dift_VDF_const2& eval_dift = dynamic_cast<const Eval_Dift_VDF_const2&> (eval);
                  const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                  const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                  for (int i=0; i<size; i++)
                    diffu_tot[i] = db_diffusivite+diffusivite_turb[i];
                }
              else
                {
                  const Eval_Dift_VDF_var2& eval_dift = dynamic_cast<const Eval_Dift_VDF_var2&> (eval);
                  const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                  const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                  for (int i=0; i<size; i++)
                    diffu_tot[i] = tab_diffusivite[i]+diffusivite_turb[i];
                }
            }
          else
            {
              Cerr << "Method Op_Dift_VDF_base2::ajouter" << finl;
              Cerr << "The type "<<equation().que_suis_je()<<" of the equation associated "<< finl;
              Cerr << "with the current operator "<< que_suis_je() << "is not coherent."<<finl;
              Cerr << "It must be sub typing of Navier_Stokes_Turbulent" << finl;
              exit();
            }

          for (face=0; face<nb_faces; face++)
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
  return resu;
}

//Description:
//on assemble la matrice.

void Op_Dift_VDF_base2::contribuer_a_avec(const DoubleTab& inco,
                                          Matrice_Morse& matrice) const
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
          if (equation().que_suis_je() == "Navier_Stokes_Turbulent")
            {
              const Eval_Diff_VDF2& eval=dynamic_cast<const Eval_Diff_VDF2&> (iter.evaluateur());
              const Champ_base& ch=eval.get_diffusivite();
              const DoubleVect& tab_diffusivite=ch.valeurs();
              if (tab_diffusivite.size() == 1)
                {
                  db_diffusivite = tab_diffusivite[0];
                  const Eval_Dift_VDF_const2& eval_dift = dynamic_cast<const Eval_Dift_VDF_const2&> (eval);
                  const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                  const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                  for (int i=0; i<diffusivite_turb.size(); i++)
                    diffu_tot[i] = db_diffusivite+diffusivite_turb[i];
                }
              else
                {
                  const Eval_Dift_VDF_var2& eval_dift = dynamic_cast<const Eval_Dift_VDF_var2&> (eval);
                  const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                  const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                  for (int i=0; i<diffusivite_turb.size(); i++)
                    diffu_tot[i] = tab_diffusivite[i]+diffusivite_turb[i];
                }
            }
          else
            {
              Cerr << "Probleme dans Op_Dift_VDF_base2::ajouter avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_Turbulent" << finl;
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
void Op_Dift_VDF_base2::contribuer_au_second_membre(DoubleTab& resu) const
{
  iter.contribuer_au_second_membre(resu);
  if (equation().domaine_application() == Motcle("Hydraulique"))
    // On est dans le cas des equations de Navier_Stokes
    {
      // Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
      if(Objet_U::bidim_axi == 1)
        {
          const Zone_VDF& zvdf=iter.zone();
          const Zone_Cl_VDF& zclvdf = iter.zone_Cl();
          const DoubleTab& xv=zvdf.xv();
          const IntVect& ori=zvdf.orientation();
          const IntTab& face_voisins=zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces=zvdf.volumes_entrelaces();
          //int face, nb_faces=zvdf.nb_faces(), cst;
          DoubleTrav diffu_tot(zvdf.nb_elem_tot());
          double db_diffusivite;
          if (equation().que_suis_je() == "Navier_Stokes_Turbulent")
            {
              const Eval_Diff_VDF2& eval=dynamic_cast<const Eval_Diff_VDF2&> (iter.evaluateur());
              const Champ_base& ch=eval.get_diffusivite();
              const DoubleVect& tab_diffusivite=ch.valeurs();
              if (tab_diffusivite.size() == 1)
                {
                  db_diffusivite = tab_diffusivite[0];
                  const Eval_Dift_VDF_const2& eval_dift = dynamic_cast<const Eval_Dift_VDF_const2&> (eval);
                  const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                  const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                  for (int i=0; i<diffusivite_turb.size(); i++)
                    diffu_tot[i] = db_diffusivite+diffusivite_turb[i];
                }
              else
                {
                  const Eval_Dift_VDF_var2& eval_dift = dynamic_cast<const Eval_Dift_VDF_var2&> (eval);
                  const Champ_Fonc& ch_diff_turb = eval_dift.diffusivite_turbulente();
                  const DoubleVect& diffusivite_turb = ch_diff_turb.valeurs();
                  for (int i=0; i<diffusivite_turb.size(); i++)
                    diffu_tot[i] = tab_diffusivite[i]+diffusivite_turb[i];
                }
            }
          else
            {
              Cerr << "Probleme dans Op_Dift_VDF_base2::ajouter avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_Turbulent" << finl;
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
                          //                          resu(num_face) -=0.;
                          resu(num_face) -= (la_cl_diri.val_imp(num_face-ndeb)*coef)*db_diffusivite;
                        }
                    }
                }
            }
        }
    }
}
