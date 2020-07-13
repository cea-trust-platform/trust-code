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
// File:        Assembleur_P_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur_P_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Champ_Face_CoviMAC.h>
#include <Op_Grad_CoviMAC_Face.h>
#include <Masse_CoviMAC_Face.h>

#include <Champ_front_instationnaire_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Matrice_Bloc_Sym.h>
#include <Matrice_Diagonale.h>

#include <Array_tools.h>
#include <Debog.h>
#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>
#include <Champ_Fonc_Q1_CoviMAC.h>
#include <Operateur_Grad.h>
#include <Schema_Temps_base.h>
#include <Piso.h>

#include <Navier_Stokes_std.h>
#include <Matrice_Morse_Sym.h>
#include <Matrix_tools.h>

Implemente_instanciable(Assembleur_P_CoviMAC,"Assembleur_P_CoviMAC",Assembleur_base);

Sortie& Assembleur_P_CoviMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom() ;
}

Entree& Assembleur_P_CoviMAC::readOn(Entree& s )
{
  return Assembleur_base::readOn(s);
}

int Assembleur_P_CoviMAC::assembler(Matrice& la_matrice)
{
  DoubleVect rien;
  return assembler_mat(la_matrice,rien,1,1);
}

int  Assembleur_P_CoviMAC::assembler_mat(Matrice& la_matrice,const DoubleVect& diag,int incr_pression,int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur());

  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, la_zone_CoviMAC.valeur());
  const Op_Grad_CoviMAC_Face& grad = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur());
  const IntTab& f_e = zone.face_voisins(), &gradp_d = grad.gradp_d, &gradp_j = grad.gradp_j;
  const DoubleVect& pe = zone.porosite_elem();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, mon_equation->inconnue().valeur());
  const DoubleTab& gradp_c = grad.gradp_c, &W_e = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).W_e, &nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv();
  int i, j, e, eb, f, ne = zone.nb_elem(), ne_tot = zone.nb_elem_tot(), piso = sub_type(Piso, equation().schema_temps()) && !sub_type(Implicite, equation().schema_temps());

  ch.init_cl(), grad.update_gradp();

  //en l'absence de CLs en pression, on ajoute P(0) = 0 sur le process 0
  has_P_ref=0;
  for (int n_bord=0; n_bord<la_zone_CoviMAC->nb_front_Cl(); n_bord++)
    if (sub_type(Neumann_sortie_libre, la_zone_Cl_CoviMAC->les_conditions_limites(n_bord).valeur()) )
      has_P_ref=1;

  /* 1. stencil de la matrice en pression : seulement au premier passage */
  if (!stencil_done)
    {
      IntTrav stencil(0, 2);
      stencil.set_smart_resize(1);
      const IntTab& feb_d = piso ? zone.feb_d : gradp_d, &feb_j = piso ? zone.feb_j : gradp_j;
      for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0)) stencil.append_line(f_e(f, 0), f_e(f, 0)); //faces de bord -> flux a deux points
        else for (i = 0; i < 2; i++) if ((e = f_e(f, i)) < ne) for (j = feb_d(f); j < feb_d(f + 1); j++) //faces internes -> flux multipoints
                eb = feb_j(j), stencil.append_line(e, eb < ne_tot ? eb : f_e(eb - ne_tot, 0));

      tableau_trier_retirer_doublons(stencil);
      Matrix_tools::allocate_morse_matrix(zone.nb_elem_tot(), zone.nb_elem_tot(), stencil, mat);
      tab1.ref_array(mat.get_set_tab1()), tab2.ref_array(mat.get_set_tab2());
      stencil_done = 1;
    }
  else //sinon, on recycle
    {
      mat.get_set_tab1().ref_array(tab1);
      mat.get_set_tab2().ref_array(tab2);
      mat.get_set_coeff().resize(tab2.size());
      mat.set_nb_columns(ne_tot);
    }

  /* 2. remplissage des coefficients : style Op_Diff_PolyMAC_Elem */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0) == 1) //bord de Neumann -> flux a deux points
      e = f_e(f, 0), mat(e, e) += pe(e) * zone.nu_dot(W_e, e, 0, 1, &nf(f, 0), &nf(f, 0)) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0));
    else if (!ch.fcl(f, 0)) //face interne -> flux multipoints
      for (i = 0; i < 2; i++)for (e = f_e(f, i), j = gradp_d(f); j < gradp_d(f + 1); j++)
          if ((eb = gradp_j(j)) < ne_tot || ch.fcl(eb - ne_tot, 0) > 1) /* les valeurs au bords de Dirichlet varient comme l'element voisin */
            mat(e, eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) += (i ? 1 : -1) * gradp_c(j, 0);

  if (!has_P_ref && !Process::me()) mat(0, 0) *= 2;

  return 1;
}

int Assembleur_P_CoviMAC::modifier_solution(DoubleTab& pression)
{
  Debog::verifier("pression dans modifier solution in",pression);
  // Projection :
  double press_0;
  if(!has_P_ref)
    {
      //abort();
      // On prend la pression minimale comme pression de reference
      // afin d'avoir la meme pression de reference en sequentiel et parallele
      press_0=DMAXFLOAT;
      int n,nb_elem=la_zone_CoviMAC.valeur().zone().nb_elem();
      for(n=0; n<nb_elem; n++)
        if (pression[n] < press_0)
          press_0 = pression[n];
      press_0 = Process::mp_min(press_0);

      for(n=0; n<nb_elem; n++)
        pression[n] -=press_0;

      pression.echange_espace_virtuel();
    }
  return 1;
}

const Zone_dis_base& Assembleur_P_CoviMAC::zone_dis_base() const
{
  return la_zone_CoviMAC.valeur();
}

const Zone_Cl_dis_base& Assembleur_P_CoviMAC::zone_Cl_dis_base() const
{
  return la_zone_Cl_CoviMAC.valeur();
}

void Assembleur_P_CoviMAC::associer_zone_dis_base(const Zone_dis_base& la_zone_dis)
{
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC,la_zone_dis);
}

void Assembleur_P_CoviMAC::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis)
{
  la_zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, la_zone_Cl_dis);
}

void Assembleur_P_CoviMAC::completer(const Equation_base& Eqn)
{
  mon_equation=Eqn;
  stencil_done = 0;
}
