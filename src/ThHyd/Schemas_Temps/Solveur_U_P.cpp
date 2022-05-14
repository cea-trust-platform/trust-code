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
#include <Schema_Temps_base.h>
#include <TRUSTTrav.h>
#include <Probleme_base.h>
#include <Front_VF.h>
#include <MD_Vector_std.h>
#include <MD_Vector_composite.h>
#include <MD_Vector_tools.h>
#include <TRUSTTab_parts.h>
#include <Dirichlet.h>
#include <Matrice_Diagonale.h>
#include <Neumann_sortie_libre.h>



Implemente_instanciable(Solveur_U_P,"Solveur_U_P",Simple);

Sortie& Solveur_U_P::printOn(Sortie& os ) const
{
  return Simple::printOn(os);
}

Entree& Solveur_U_P::readOn(Entree& is )
{
  return Simple::readOn(is);
}

//Entree : Uk-1 ; Pk-1
//Sortie Uk ; Pk
//k designe une iteration
void Solveur_U_P::iterer_NS(Equation_base& eqn,DoubleTab& current,DoubleTab& pression,double dt,Matrice_Morse& matrice_inut,double seuil_resol,DoubleTrav& secmem,int nb_ite,int& converge, int& ok)
{
  if (eqn.probleme().is_dilatable())
    {
      Cerr<<" Solveur_U_P cannot be used with a quasi-compressible fluid."<<finl;
      Cerr<<__FILE__<<(int)__LINE__<<" non code" <<finl;
      Process::exit();
    }

  Parametre_implicite& param = get_and_set_parametre_implicite(eqn);
  SolveurSys& le_solveur_ = param.solveur();

  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std,eqn);

  /* MD_Vector (vitesse, pression) */
  MD_Vector_composite mds;
  mds.add_part(current.get_md_vector(), current.line_size());
  mds.add_part(pression.get_md_vector(), pression.line_size());
  MD_Vector md_UP;
  md_UP.copy(mds);

  DoubleTab Inconnues,residu;
  MD_Vector_tools::creer_tableau_distribue(md_UP, Inconnues);
  MD_Vector_tools::creer_tableau_distribue(md_UP, residu);

  DoubleTab_parts residu_parts(residu);
  DoubleTab_parts Inconnues_parts(Inconnues);

  Inconnues_parts[0] = current, Inconnues_parts[1] = pression;

  Matrice_Bloc Matrice_global(2,2) ; //matrice M.(du, dp) = (Navier-Stokes, divergence)

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

  /* doit-on fixer P(elem 0) = 0 ? */
  int has_P_ref=0;
  const Conds_lim& cls = eqnNS.zone_Cl_dis().les_conditions_limites();
  for (int n_bord=0; n_bord < cls.size(); n_bord++)
    if (sub_type(Neumann_sortie_libre,cls[n_bord].valeur())) has_P_ref=1;

  /* ligne de masse : (div, 0) */
  Operateur_Div_base& divergence = eqnNS.operateur_divergence().valeur();
  Matrice_global.get_bloc(1,0).typer("Matrice_Morse");
  Matrice_Morse& mat_div_v = ref_cast(Matrice_Morse, Matrice_global.get_bloc(1,0).valeur());
  if (divergence.has_interface_blocs()) /* si interface_blocs : direct */
    {
      Matrice_global.get_bloc(1,1).typer("Matrice_Morse");
      Matrice_Morse& mat_div_p = ref_cast(Matrice_Morse, Matrice_global.get_bloc(1,1).valeur());
      divergence.dimensionner_blocs({ { "vitesse", &mat_div_v } , { "pression", &mat_div_p}});
      divergence.ajouter_blocs({ { "vitesse", &mat_div_v } , { "pression", &mat_div_p}}, residu_parts[1]);
      if (!has_P_ref && !Process::me()) mat_div_p(0, 0) += 1; //revient a imposer P(0) = 0
    }
  else /* sinon : l'operateur remplit mat_div_v, on construit a la main mat_div_p = 0 */
    {
      divergence.dimensionner(mat_div_v);
      divergence.contribuer_a_avec(current, mat_div_v);
      divergence.calculer(current, residu_parts[1]);
      Matrice_global.get_bloc(1,1).typer("Matrice_Diagonale");
      Matrice_Diagonale& mat_div_p = ref_cast(Matrice_Diagonale,Matrice_global.get_bloc(1,1).valeur());
      mat_div_p.dimensionner(mat_div_v.nb_lignes());
      if (!has_P_ref && !Process::me()) mat_div_p.coeff(0, 0) += 1; //revient a imposer P(0) = 0
    }

  le_solveur_.valeur().reinit();
  le_solveur_.valeur().resoudre_systeme(Matrice_global,residu,Inconnues);

  //Calcul de Uk = U*_k + U'k
  current  += Inconnues_parts[0];
  pression += Inconnues_parts[1];
  //current.echange_espace_virtuel();
  Debog::verifier("Solveur_U_P::iterer_NS current",current);
  eqn.solv_masse().corriger_solution(current, current);    //PolyMAC_P0 : mise en coherence de ve avec vf
  eqnNS.assembleur_pression().modifier_solution(pression);

  if (1)
    {
      //  DoubleTrav secmem(current);
      divergence.calculer(current, secmem);
      Cerr<<" apresdiv "<<mp_max_abs_vect(secmem)<<finl;;
    }
}
