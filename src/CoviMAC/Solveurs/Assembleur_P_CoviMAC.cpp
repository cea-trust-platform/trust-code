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

int Assembleur_P_CoviMAC::assembler_rho_variable(Matrice& la_matrice, const Champ_Don_base& rho)
{
  abort();
  return 0;
  /*
  // On multiplie par la masse volumique aux sommets
  if (!sub_type(Champ_Fonc_Q1_CoviMAC, rho))
    {
      Cerr << "La masse volumique n'est pas aux sommets dans Assembleur_P_CoviMAC::assembler_rho_variable." << finl;
      Process::exit();
    }
  const DoubleVect& volumes_som=ref_cast(Zone_CoviMAC, la_zone_CoviMAC.valeur()).volumes_sommets_thilde();
  const DoubleVect& masse_volumique=rho.valeurs();
  DoubleVect quantitee_som(volumes_som);
  int size=quantitee_som.size_array();
  for (int i=0; i<size; i++)
    quantitee_som(i)=(volumes_som(i)*masse_volumique(i));

  // On assemble la matrice
  return assembler_mat(la_matrice,quantitee_som,1,1);
  */
}

int  Assembleur_P_CoviMAC::assembler_mat(Matrice& la_matrice,const DoubleVect& diag,int incr_pression,int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur());

  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, la_zone_CoviMAC.valeur());
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face(), &vf = zone.volumes_entrelaces();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, mon_equation->inconnue().valeur());
  int i, j, k, e, f, fb, ne = zone.nb_elem(), ne_tot = zone.nb_elem_tot(), nfb = zone.premiere_face_int();
  zone.init_w1(), ch.init_cl();

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
      for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2; i++)
          if ((e = f_e(f, i)) < ne || (e >= ne_tot && e < ne_tot + nfb && ch.icl(e - ne_tot, 0) > 1)) //elem reel ou CL de Dirichlet
            for (j = zone.w1d(f); j < zone.w1d(f + 1); j++) for (fb = zone.w1j(j), k = 0; k < 2; k++)
                stencil.append_line(e, f_e(fb, k));
          else if (e >= ne_tot && e < ne_tot + nfb && ch.icl(e - ne_tot, 0) == 1) //CL de Neumann -> ligne P = P_imp
            stencil.append_line(e, e);

      tableau_trier_retirer_doublons(stencil);
      Matrix_tools::allocate_morse_matrix(zone.nb_elems_faces_bord_tot(), zone.nb_elems_faces_bord_tot(), stencil, mat);
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
  double coeff, coeff_b;
  for (f = 0; f < zone.nb_faces(); f++)
    for (coeff = pf(f) * fs(f) * (diag.size() ? pf(f) * vf(f) / diag(f) : 1), i = 0; i < 2; i++)
      if ((e = f_e(f, i)) < ne || (e >= ne_tot && e < ne_tot + nfb && ch.icl(e - ne_tot, 0) > 1)) //elem reel ou CL de Dirichlet
        for (j = zone.w1d(f); j < zone.w1d(f + 1); j++)
          for (fb = zone.w1j(j), coeff_b = coeff * zone.w1c(j) * fs(fb) / vf(fb), k = 0; k < 2; k++)
            mat(e, f_e(fb, k)) += (i ? -1 : 1) * (k ? -1 : 1) * coeff_b;
      else if (e >= ne_tot && e < ne_tot + nfb && ch.icl(e - ne_tot, 0) == 1) //CL de Neumann
        mat(e, e) = 1;

  if (!has_P_ref && !Process::me()) mat(0, 0) *= 2;

  return 1;
}

// Description:
//    Assemble la matrice de pression pour un fluide quasi compressible
//    laplacein(P) est remplace par div(grad(P)/rho).
// Precondition:
// Parametre: DoubleTab& tab_rho
//    Signification: mass volumique
//    Valeurs par dCoviMACaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// CoviMACfets de bord:
// Postcondition:
int Assembleur_P_CoviMAC::assembler_QC(const DoubleTab& tab_rho, Matrice& matrice)
{
  Cerr << "Assemblage de la matrice de pression pour Quasi Compressible en cours..." << finl;
  assembler(matrice);
  set_resoudre_increment_pression(1);
  set_resoudre_en_u(0);
  abort();
  Matrice_Bloc& matrice_bloc= ref_cast(Matrice_Bloc,matrice.valeur());
  Matrice_Morse_Sym& la_matrice =ref_cast(Matrice_Morse_Sym,matrice_bloc.get_bloc(0,0).valeur());
  if ((la_matrice.get_est_definie()!=1)&&(1))
    {
      Cerr<<"Pas de pression imposee  --> P(0)=0"<<finl;
      if (je_suis_maitre())
        la_matrice(0,0) *= 2;
      la_matrice.set_est_definie(1);
    }

  Cerr << "Fin de l'assemblage de la matrice de pression" << finl;
  return 1;
}

int Assembleur_P_CoviMAC::modifier_secmem(DoubleTab& secmem)
{
  Debog::verifier("secmem dans modifier secmem",secmem);

  const Zone_CoviMAC& la_zone = la_zone_CoviMAC.valeur();
  const Zone_Cl_CoviMAC& la_zone_cl = la_zone_Cl_CoviMAC.valeur();
  int nb_cond_lim = la_zone_cl.nb_cond_lim();
  const IntTab& face_voisins = la_zone.face_voisins();

  // Modification du second membre :
  int i;
  for (i=0; i<nb_cond_lim; i++)
    {
      const Cond_lim_base& la_cl_base = la_zone_cl.les_conditions_limites(i).valeur();
      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
      int ndeb = la_front_dis.num_premiere_face();
      int nfin = ndeb + la_front_dis.nb_faces();


      // GF on est passe en increment de pression
      if ((sub_type(Neumann_sortie_libre,la_cl_base)) && (!get_resoudre_increment_pression()))
        {
          double Pimp, coCoviMAC;
          const Neumann_sortie_libre& la_cl_Neumann = ref_cast(Neumann_sortie_libre, la_cl_base);
          // const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
          //int ndeb = la_front_dis.num_premiere_face();
          //int nfin = ndeb + la_front_dis.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              Pimp = la_cl_Neumann.flux_impose(num_face-ndeb);
              coCoviMAC = les_coeff_pression[num_face]*Pimp;
              secmem[face_voisins(num_face,0)] += coCoviMAC;
            }
        }
      else if (sub_type(Champ_front_instationnaire_base,
                        la_cl_base.champ_front().valeur())&&(get_resoudre_en_u()))
        {
          if (sub_type(Dirichlet,la_cl_base))
            {
              // Cas Dirichlet variable dans le temps
              // N'est utile que pour des champs front. variables dans le temps
              const Champ_front_instationnaire_base& le_ch_front =
                ref_cast( Champ_front_instationnaire_base,
                          la_cl_base.champ_front().valeur());
              const DoubleTab& Gpt = le_ch_front.Gpoint();

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  //for num_face
                  double Stt = 0.;
                  for (int k=0; k<dimension; k++)
                    Stt -= Gpt(k) * la_zone.face_normales(num_face, k);
                  secmem(face_voisins(num_face,0)) += Stt;
                }
            }
        }
      else if (sub_type(Champ_front_var_instationnaire,
                        la_cl_base.champ_front().valeur())&&(get_resoudre_en_u()))
        {
          if (sub_type(Dirichlet,la_cl_base))
            {
              //cas instationaire et variable
              const Champ_front_var_instationnaire& le_ch_front =
                ref_cast( Champ_front_var_instationnaire, la_cl_base.champ_front().valeur());
              const DoubleTab& Gpt = le_ch_front.Gpoint();

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  double Stt = 0.;
                  for (int k=0; k<dimension; k++)
                    Stt -= Gpt(num_face - ndeb, k) *
                           la_zone.face_normales(num_face, k);
                  secmem(face_voisins(num_face,0)) += Stt;
                }
            }
        }
    }

  secmem.echange_espace_virtuel();
  Debog::verifier("secmem dans modifier secmem fin",secmem);
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
