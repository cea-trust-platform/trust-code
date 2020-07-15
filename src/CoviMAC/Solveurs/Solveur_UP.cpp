/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Solveur_UP.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     /main/48
//
//////////////////////////////////////////////////////////////////////////////

#include <Solveur_UP.h>
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

#include <MD_Vector_std.h>
#include <MD_Vector_composite.h>
#include <MD_Vector_tools.h>
#include <ConstDoubleTab_parts.h>

#include <Matrice_Diagonale.h>

#include <Zone_Cl_CoviMAC.h>
#include <Neumann_sortie_libre.h>


Implemente_instanciable(Solveur_UP,"Solveur_UP",Simple);

Sortie& Solveur_UP::printOn(Sortie& os ) const
{
  return Simple::printOn(os);
}

Entree& Solveur_UP::readOn(Entree& is )
{
  return Simple::readOn(is);
}





void modifier_pour_Cl_je_ne_sais_pas_ou_factoriser_cela(const Zone_dis_base& la_zone,
                                                        const Zone_Cl_dis_base& la_zone_cl,
                                                        Matrice_Morse& la_matrice, DoubleTab& inco, DoubleTab& secmem) ;



//Entree : Uk-1 ; Pk-1
//Sortie Uk ; Pk
//k designe une iteration

void Solveur_UP::iterer_NS(Equation_base& eqn,DoubleTab& current,DoubleTab& pression,double dt,Matrice_Morse& matrice_inut,double seuil_resol,DoubleTrav& secmem,int nb_ite,int& converge)
{
  if (eqn.probleme().is_QC())
    {
      Cerr<<" Solveur_UP cannot be used with a quasi-compressible fluid."<<finl;
      Cerr<<__FILE__<<(int)__LINE__<<" non code" <<finl;
      exit();
    }

  Parametre_implicite& param = get_and_set_parametre_implicite(eqn);
  SolveurSys& le_solveur_ = param.solveur();

  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std,eqn);

  MD_Vector md_UP;
  {
    MD_Vector_composite mds;
    mds.add_part(current.get_md_vector());
    mds.add_part(pression.get_md_vector());
    md_UP.copy(mds);
  }

  DoubleTab Inconnues,residu;
  MD_Vector_tools::creer_tableau_distribue(md_UP, Inconnues);
  MD_Vector_tools::creer_tableau_distribue(md_UP, residu);

  DoubleTab_parts residu_parts(residu);
  DoubleTab_parts Inconnues_parts(Inconnues);

  Inconnues_parts[0]=current;
  Inconnues_parts[1]=pression;

  Operateur_Grad& gradient = eqnNS.operateur_gradient();
  Operateur_Div& divergence = eqnNS.operateur_divergence();
  Matrice_Bloc Matrice_global(2,2) ; //  Div 0 puis 1/dt+A   +grapdP

  Matrice_global.get_bloc(1,0).typer("Matrice_Morse");
  Matrice_Morse& mat_div=ref_cast(Matrice_Morse, Matrice_global.get_bloc(1,0).valeur());
  divergence.valeur().dimensionner(mat_div);
  divergence.valeur().contribuer_a_avec( current,mat_div);

  Matrice_global.get_bloc(0,1).typer("Matrice_Morse");
  Matrice_Morse& mat_grad=ref_cast(Matrice_Morse, Matrice_global.get_bloc(0,1).valeur());
  gradient.valeur().dimensionner(mat_grad);
  gradient.valeur().contribuer_a_avec(pression, mat_grad);
  mat_grad.get_set_coeff()*=-1;

  Matrice_global.get_bloc(1,1).typer("Matrice_Diagonale");
  Matrice_Diagonale& mat_diag = ref_cast(Matrice_Diagonale,Matrice_global.get_bloc(1,1).valeur());
  mat_diag.dimensionner(mat_div.nb_lignes());
  int has_P_ref=0;
  const Conds_lim& cls = eqnNS.zone_Cl_dis().les_conditions_limites();
  for (int n_bord=0; n_bord < cls.size(); n_bord++)
    if (sub_type(Neumann_sortie_libre,cls[n_bord].valeur())) has_P_ref=1;
  if (!has_P_ref && !Process::me()) mat_diag.coeff(0, 0) = 1; //revient a imposer P(0) = 0

  Matrice_global.get_bloc(0,0).typer("Matrice_Morse");
  Matrice_Morse& matrice=ref_cast(Matrice_Morse, Matrice_global.get_bloc(0,0).valeur());
  eqnNS.dimensionner_matrice(matrice);
  eqnNS.assembler_avec_inertie(matrice,current,residu_parts[0]);

  DoubleTrav secmem_grad(residu_parts[0]);
  /* pour que ajouter_NS voie le bon second membre */
  gradient.valeur().ajouter_NS(pression, secmem_grad, &matrice, &current, &residu_parts[0]);
  // gradient.valeur().ajouter(pression, secmem_grad);
  residu_parts[0] -= secmem_grad;
  divergence.calculer(current, residu_parts[1]);
  /*
      modifier_pour_Cl_je_ne_sais_pas_ou_factoriser_cela(eqnNS.zone_dis().valeur(),
                                                       eqnNS.zone_Cl_dis(),
                                                       matrice, current, residu_parts[0]) ;
  */
  /* int nb_f=current.dimension(0);
  int nnz=matrice.tab1_[nb_f]-1;
  matrice.dimensionner(nb_f,matrice.nb_colonnes(),nnz);
  */
  le_solveur_.valeur().reinit();
  le_solveur_.valeur().resoudre_systeme(Matrice_global,residu,Inconnues);

  //Calcul de Uk = U*_k + U'k
  current   = Inconnues_parts[0];
  pression += Inconnues_parts[1];
  eqn.solv_masse().corriger_solution(current, current); //mise en coherence de ve avec vf
  //current.echange_espace_virtuel();
  Debog::verifier("Solveur_UP::iterer_NS current",current);

  if (1)
    {
      //  DoubleTrav secmem(current);
      divergence.calculer(current, secmem);
      Cerr<<" apresdiv "<<mp_max_abs_vect(secmem)<<finl;;
    }

}

