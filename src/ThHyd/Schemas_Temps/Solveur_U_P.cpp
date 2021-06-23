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
// File:        Solveur_U_P.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/48
//
//////////////////////////////////////////////////////////////////////////////

#include <Solveur_U_P.h>
#include <Navier_Stokes_std.h>
#include <EChaine.h>
#include <Debog.h>
#include <Matrice_Bloc.h>
#include <Assembleur_base.h>
#include <Statistiques.h>
#include <Schema_Temps_base.h>
#include <DoubleTrav.h>
#include <Schema_Euler_Implicite.h>
#include <Probleme_base.h>
#include <Front_VF.h>
#include <MD_Vector_std.h>
#include <MD_Vector_composite.h>
#include <MD_Vector_tools.h>
#include <ConstDoubleTab_parts.h>
#include <Dirichlet.h>
#include <Matrice_Diagonale.h>
#include <Dirichlet_homogene.h>
#include <Neumann_sortie_libre.h>
#include <Symetrie.h>
#include <Zone_VF.h>


Implemente_instanciable(Solveur_U_P,"Solveur_U_P",Simple);

Sortie& Solveur_U_P::printOn(Sortie& os ) const
{
  return Simple::printOn(os);
}

Entree& Solveur_U_P::readOn(Entree& is )
{
  return Simple::readOn(is);
}

// ToDo: methode identique dans Solveur_UP.cpp
inline void modifier_pour_Cl_je_ne_sais_pas_ou_factoriser_cela(const Zone_dis_base& la_zone,
                                                               const Zone_Cl_dis_base& la_zone_cl,
                                                               Matrice_Morse& la_matrice, DoubleTab& inco, DoubleTab& secmem)
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();
  const IntVect& tab1=la_matrice.get_tab1();
  const IntVect& tab2=la_matrice.get_tab2();
  DoubleVect& coeff = la_matrice.get_set_coeff();
  int nb_comp = 1;
  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  if (champ_inconnue.nb_dim() == 2) nb_comp = champ_inconnue.dimension(1);
  ArrOfDouble normale(nb_comp);
  int size = les_cl.size();
  for (int i=0; i<size; i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet& la_cl_Dirichlet = ref_cast(Dirichlet,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          const Zone_VF& ma_zone_VF =  ref_cast(Zone_VF, la_zone);
          int nfaces = la_front_dis.nb_faces();
          int numdeb = la_front_dis.num_premiere_face();
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp=0; comp<nb_comp; comp++)
                {
                  for (int k = tab1[face*nb_comp+comp] - 1; k < tab1[face*nb_comp+1+comp] - 1; k++)
                    coeff[k] = (tab2[k] - 1 == face);
                  // pour le second membre
                  for (int j = secmem[face*nb_comp+comp] = 0; j < Objet_U::dimension; j++)
                    secmem[face*nb_comp+comp] += la_cl_Dirichlet.val_imp(face-numdeb,j) * ma_zone_VF.face_normales(face, j) / ma_zone_VF.face_surfaces(face);
                  secmem[face*nb_comp+comp] -= inco[face*nb_comp+comp];
                  // if (nb_comp == 1)
                  //   secmem(face) = la_cl_Dirichlet.val_imp(ind_face,0);
                  // else
                  //   secmem(face,comp)= la_cl_Dirichlet.val_imp(ind_face,comp);
                }
            }
        }
      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          const Dirichlet_homogene& la_cl_Dirichlet_homogene = ref_cast(Dirichlet_homogene,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          const Zone_VF& ma_zone_VF =  ref_cast(Zone_VF, la_zone);
          int nfaces = la_front_dis.nb_faces();
          int numdeb = la_front_dis.num_premiere_face();
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp=0; comp<nb_comp; comp++)
                {
                  for (int k = tab1[face*nb_comp+comp] - 1; k < tab1[face*nb_comp+1+comp] - 1; k++)
                    coeff[k] = (tab2[k] - 1 == face);
                  // pour le second membre
                  for (int j = secmem[face*nb_comp+comp] = 0; j < Objet_U::dimension; j++)
                    secmem[face*nb_comp+comp] += la_cl_Dirichlet_homogene.val_imp(face-numdeb,j) * ma_zone_VF.face_normales(face, j) / ma_zone_VF.face_surfaces(face);
                  secmem[face*nb_comp+comp] -= inco[face*nb_comp+comp];
                  // if (nb_comp == 1)
                  //   secmem(face) = la_cl_Dirichlet_homogene.val_imp(ind_face,0);
                  // else
                  //   secmem(face,comp)= la_cl_Dirichlet_homogene.val_imp(ind_face,comp);
                }
            }
        }
      if (sub_type(Symetrie,la_cl.valeur()))
        {
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nfaces = la_front_dis.nb_faces();
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp=0; comp<nb_comp; comp++)
                {
                  for (int k = tab1[face*nb_comp+comp] - 1; k < tab1[face*nb_comp+1+comp] - 1; k++)
                    coeff[k] = (tab2[k] - 1 == face);
                  // pour le second membre
                  secmem[face*nb_comp+comp] = 0;
                }
            }
        }
#if 0
      if (sub_type(Symetrie,la_cl.valeur()))
        if (la_zone_cl.equation().inconnue().valeur().nature_du_champ()==vectoriel)
          {
            abort();
            const IntVect& tab2=la_matrice.tab2_;
            const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
            const DoubleTab& face_normales = la_zone.face_normales();
            int nfaces = la_front_dis.nb_faces_tot();
            ArrOfDouble somme(la_matrice.nb_colonnes()); // On dimensionne au plus grand
            for (int ind_face=0; ind_face < nfaces; ind_face++)
              {
                int face = la_front_dis.num_face(ind_face);
                double max_coef=0;
                int ind_max=-1;
                double n2=0;
                for (int comp=0; comp<nb_comp; comp++)
                  {
                    normale[comp]=face_normales(face,comp);
                    if (dabs(normale[comp])>dabs(max_coef))
                      {
                        max_coef=normale[comp];
                        ind_max=comp;
                      }
                    n2+=normale[comp]*normale[comp];
                  }
                normale/=sqrt(n2);
                max_coef=normale[ind_max];

                // On commence par recalculer secmem=secmem-A *present pour pouvoir modifier A (on en profite pour projeter)
                int nb_coeff_ligne=tab1[face*nb_comp+1] - tab1[face*nb_comp];
                for (int k=0; k<nb_coeff_ligne; k++)
                  {
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        int j=tab2[tab1[face*nb_comp+comp]-1+k]-1;
                        //assert(j!=(face*nb_comp+comp));
                        //if ((j>=(face*nb_comp))&&(j<(face*nb_comp+nb_comp)))
                        const    double& coef_ij=la_matrice(face*nb_comp+comp,j);
                        int face2=j/nb_comp;
                        int comp2=j-face2*nb_comp;
                        secmem(face,comp)-=coef_ij*champ_inconnue(face2,comp2);
                      }
                  }
                double somme_b=0;

                for (int comp=0; comp<nb_comp; comp++)
                  somme_b+=secmem(face,comp)*normale[comp];

                // on retire secmem.n n
                for (int comp=0; comp<nb_comp; comp++)
                  secmem(face,comp)-=somme_b*normale[comp];

                // on doit remettre la meme diagonale partout on prend la moyenne
                double ref=0;
                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int j0=face*nb_comp+comp;
                    ref+=la_matrice(j0,j0);
                  }
                ref/=nb_comp;

                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int j0=face*nb_comp+comp;
                    double rap=ref/la_matrice(j0,j0);
                    for (int k=0; k<nb_coeff_ligne; k++)
                      {
                        int j=tab2[tab1[j0]-1+k]-1;
                        la_matrice(j0,j)*=rap;
                      }
                    assert(est_egal(la_matrice(j0,j0),ref));
                  }
                // on annule tous les coef extra diagonaux du bloc
                //
                for (int k=1; k<nb_coeff_ligne; k++)
                  {
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        int j=tab2[tab1[face*nb_comp+comp]-1+k]-1;
                        assert(j!=(face*nb_comp+comp));
                        if ((j>=(face*nb_comp))&&(j<(face*nb_comp+nb_comp)))
                          la_matrice(face*nb_comp+comp,j)=0;
                      }
                  }

                // pour les blocs extra diagonaux on assure que Aij.ni=0
                //ArrOfDouble somme(nb_coeff_ligne);
                for (int k=0; k<nb_coeff_ligne; k++)
                  {
                    somme(k)=0;
                    int j=tab2[tab1[face*nb_comp]-1+k]-1;

                    // le coeff j doit exister sur les nb_comp lignes
                    double dsomme=0;
                    for (int comp=0; comp<nb_comp; comp++)
                      dsomme+=la_matrice(face*nb_comp+comp,j)*normale[comp];

                    // on retire somme ni

                    for (int comp=0; comp<nb_comp; comp++)
                      // on modifie que les coefficients ne faisant pas intervenir u(face,comp)
                      if ((j<(face*nb_comp))||(j>=(face*nb_comp+nb_comp)))
                        la_matrice(face*nb_comp+comp,j)-=(dsomme)*normale[comp];
                  }
                // Finalement on recalcule secmem=secmem+A*champ_inconnue (A a ete beaucoup modiife)
                for (int k=0; k<nb_coeff_ligne; k++)
                  {
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        int j=tab2[tab1[face*nb_comp+comp]-1+k]-1;
                        int face2=j/nb_comp;
                        int comp2=j-face2*nb_comp;
                        const double& coef_ij=la_matrice(face*nb_comp+comp,j);
                        secmem(face,comp)+=coef_ij*champ_inconnue(face2,comp2);
                      }
                  }
                {
                  // verification
                  double somme_c=0;
                  for (int comp=0; comp<nb_comp; comp++)
                    somme_c+=secmem(face,comp)*normale[comp];
                  // on retire secmem.n n
                  for (int comp=0; comp<nb_comp; comp++)
                    secmem(face,comp)-=somme_c*normale[comp];
                }
              }
          }
#endif
    }
}

//Entree : Uk-1 ; Pk-1
//Sortie Uk ; Pk
//k designe une iteration

void Solveur_U_P::iterer_NS(Equation_base& eqn,DoubleTab& current,DoubleTab& pression,double dt,Matrice_Morse& matrice_inut,double seuil_resol,DoubleTrav& secmem,int nb_ite,int& converge)
{
  if (eqn.probleme().is_QC())
    {
      Cerr<<" Solveur_U_P cannot be used with a quasi-compressible fluid."<<finl;
      Cerr<<__FILE__<<(int)__LINE__<<" non code" <<finl;
      exit();
    }

  Parametre_implicite& param = get_and_set_parametre_implicite(eqn);
  SolveurSys& le_solveur_ = param.solveur();

  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std,eqn);

  DoubleTab_parts ppart(pression); //dans PolyMAC, pression contient (p, v) -> on doit ignorer la 2e partie...
  MD_Vector md_UP;
  {
    MD_Vector_composite mds;
    mds.add_part(current.get_md_vector(), current.line_size());
    mds.add_part(ppart[0].get_md_vector(), ppart[0].line_size());
    md_UP.copy(mds);
  }

  DoubleTab Inconnues,residu;
  MD_Vector_tools::creer_tableau_distribue(md_UP, Inconnues);


  MD_Vector_tools::creer_tableau_distribue(md_UP, residu);

  DoubleTab_parts residu_parts(residu);
  DoubleTab_parts Inconnues_parts(Inconnues);

  Inconnues_parts[0]=current;
  Inconnues_parts[1]=ppart[0];

  Matrice_Bloc Matrice_global(2,2) ; //  Div 0 puis 1/dt+A   +grapdP

  /* ligne Navier-Stokes : blocs N-S, bloc gradient */
  Matrice_global.get_bloc(0,0).typer("Matrice_Morse");
  Matrice_Morse& matrice=ref_cast(Matrice_Morse, Matrice_global.get_bloc(0,0).valeur());
  Matrice_global.get_bloc(0,1).typer("Matrice_Morse");
  Matrice_Morse& mat_grad=ref_cast(Matrice_Morse, Matrice_global.get_bloc(0,1).valeur());

  if (eqnNS.has_interface_blocs()) /* si interface_blocs : on peut remplir les deux d'un coup */
    {
      eqnNS.dimensionner_blocs({{ "vitesse", &matrice }, { "pression", &mat_grad }});
      eqnNS.assembler_blocs_avec_inertie({{ "vitesse", &matrice }, { "pression", &mat_grad }}, residu_parts[0]);
    }
  else /* sinon : moins elegant */
    {
      Operateur_Grad& gradient = eqnNS.operateur_gradient();

      eqnNS.dimensionner_matrice(matrice);
      eqnNS.assembler_avec_inertie(matrice,current,residu_parts[0]);
      gradient.valeur().dimensionner( mat_grad);
      gradient.valeur().contribuer_a_avec(pression, mat_grad);
      mat_grad.get_set_coeff()*=-1;
      /* pour repasser en increments */
      residu_parts[0]*=-1;
      matrice.ajouter_multvect(current, residu_parts[0]);
      gradient.valeur().ajouter(pression,residu_parts[0]);
      residu_parts[0]*=-1;
    }

  /* ligne de masse : (div, 0) */
  Operateur_Div& divergence = eqnNS.operateur_divergence();
  Matrice_global.get_bloc(1,0).typer("Matrice_Morse");
  Matrice_Morse& mat_div=ref_cast(Matrice_Morse, Matrice_global.get_bloc(1,0).valeur());
  divergence.valeur().dimensionner(mat_div);
  divergence.valeur().contribuer_a_avec( current,mat_div);
  divergence.calculer(current, residu_parts[1]);

  Matrice_global.get_bloc(1,1).typer("Matrice_Diagonale");
  Matrice_Diagonale& mat_diag = ref_cast(Matrice_Diagonale,Matrice_global.get_bloc(1,1).valeur());
  mat_diag.dimensionner(mat_div.nb_lignes());
  int has_P_ref=0;
  const Conds_lim& cls = eqnNS.zone_Cl_dis().les_conditions_limites();
  for (int n_bord=0; n_bord < cls.size(); n_bord++)
    if (sub_type(Neumann_sortie_libre,cls[n_bord].valeur())) has_P_ref=1;
  if (!has_P_ref && !Process::me()) mat_diag.coeff(0, 0) = 1; //revient a imposer P(0) = 0


  //en PolyMAC, on doit ajouter des lignes vides a grad et des colonnes vides a div
  int n = matrice.get_tab1().size(), i;
  for (i = mat_grad.get_tab1().size(), mat_grad.get_set_tab1().resize(n); i < n; i++)
    mat_grad.get_set_tab1()(i) = mat_grad.get_tab1()(i - 1);
  mat_div.set_nb_columns(n - 1);

  le_solveur_.valeur().reinit();
  le_solveur_.valeur().resoudre_systeme(Matrice_global,residu,Inconnues);

  //Calcul de Uk = U*_k + U'k
  current  += Inconnues_parts[0];
  ppart[0] += Inconnues_parts[1];
  //current.echange_espace_virtuel();
  Debog::verifier("Solveur_U_P::iterer_NS current",current);
  eqn.solv_masse().corriger_solution(current, current);    //CoviMAC : mise en coherence de ve avec vf
  eqnNS.assembleur_pression().modifier_solution(pression);

  if (1)
    {
      //  DoubleTrav secmem(current);
      divergence.calculer(current, secmem);
      Cerr<<" apresdiv "<<mp_max_abs_vect(secmem)<<finl;;
    }

}
