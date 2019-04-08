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
// File:        Masse_PolyMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_PolyMAC_Elem.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Debog.h>
#include <Champ_P0_PolyMAC.h>
#include <Equation_base.h>
#include <Conds_lim.h>
#include <Neumann_paroi.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Operateur.h>
#include <Op_Diff_negligeable.h>
#include <Echange_impose_base.h>
#include <Milieu_base.h>

Implemente_instanciable(Masse_PolyMAC_Elem,"Masse_PolyMAC_Elem",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_PolyMAC_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Masse_PolyMAC_Elem::readOn(Entree& s)
{
  return s ;
}


void Masse_PolyMAC_Elem::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC, la_zone_dis_base);
}

void Masse_PolyMAC_Elem::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, la_zone_Cl_dis_base);
}

//ne touche que la partie "elements"
DoubleTab& Masse_PolyMAC_Elem::appliquer_impl(DoubleTab& sm) const
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  const DoubleVect& volumes = zone_PolyMAC.volumes();
  const DoubleVect& porosite_elem = zone_PolyMAC.porosite_elem();
  int nb_elem = zone_PolyMAC.nb_elem();
  if(nb_elem==0)
    {
      sm.echange_espace_virtuel();
      return sm;
    }
  int nb_comp = sm.size()/nb_elem;
  int nb_dim=sm.nb_dim();
  assert((nb_comp*nb_elem == sm.size())||(nb_dim==3));
  const Champ_Don& rho = equation().milieu().masse_volumique(), Cp = equation().milieu().capacite_calorifique();
  int rCp = equation().que_suis_je() == "Conduction"
            || equation().que_suis_je() == "Convection_Diffusion_Temperature"
            || equation().que_suis_je() == "Convection_Diffusion_Temperature_Turbulent"
            || equation().que_suis_je().debute_par("Equation_") // equations F5
            || equation().que_suis_je().debute_par("Transport") ? 0 : 1;

  if (nb_dim == 1)
    for (int num_elem=0; num_elem<nb_elem; num_elem++)
      sm(num_elem) *= (rCp ? rho(num_elem, 0) * Cp(num_elem, 0) : 1) / (volumes(num_elem)*porosite_elem(num_elem));
  else if (nb_dim == 2)
    {
      for (int num_elem=0; num_elem<nb_elem; num_elem++)
        for (int k=0; k<nb_comp; k++)
          sm(num_elem,k) *= (rCp ? rho(num_elem, 0) * Cp(num_elem, 0) : 1) / (volumes(num_elem)*porosite_elem(num_elem));
    }
  else if (sm.nb_dim() == 3)
    {
      //int d0=sm.dimension(0);
      int d1=sm.dimension(1);
      int d2=sm.dimension(2);
      for (int num_elem=0; num_elem<nb_elem; num_elem++)
        for (int k=0; k<d1; k++)
          for (int d=0; d<d2; d++)
            sm(num_elem,k,d) *= (rCp ? rho(num_elem, 0) * Cp(num_elem, 0) : 1) / (volumes(num_elem)*porosite_elem(num_elem));
    }
  else
    {
      Cerr<< "Masse_PolyMAC_Elem::appliquer ne peut pas s'appliquer a un DoubleTab a "<<sm.nb_dim()<<" dimensions"<<endl;
      assert(0);
      exit();
    }
  sm.echange_espace_virtuel();
  return sm;
}

//Masse_PolyMAC_Elem est responsable des parties de la matrice n'impliquant pas la diffusion
void Masse_PolyMAC_Elem::dimensionner(Matrice_Morse& matrix) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const IntTab& e_f = zone.elem_faces();
  int i, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = ch.valeurs().line_size();

  zone.init_m2(), ch.init_cl();
  IntTab indice(0,2);
  indice.set_smart_resize(1);
  //partie superieure : diagonale (matrice de masse classique) et divergence
  for (e = 0; e < zone.nb_elem(); e++) for (i = 0, indice.append_line(e, e); i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      for (n = 0; n < N; n++) indice.append_line(N * e + n, N * (ne_tot + f) + n);
  //partie inferieure : partie diagonale pour les flux de faces imposes par CLs
  for (f = 0; f < zone.nb_faces(); f++)
    if (sub_type(Op_Diff_negligeable, equation().operateur(0).l_op_base()) || (ch.icl(f, 0) > 2 && ch.icl(f, 0) < 5))
      for (n = 0; n < N; n++) indice.append_line(N * (ne_tot + f) + n, N * (ne_tot + f) + n);

  tableau_trier_retirer_doublons(indice);
  Matrix_tools::allocate_morse_matrix(N * (ne_tot + nf_tot), N * (ne_tot + nf_tot), indice, matrix);
}
DoubleTab& Masse_PolyMAC_Elem::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zone_Cl_PolyMAC->les_conditions_limites();
  const DoubleVect& ve = zone.volumes(), &pe = zone.porosite_elem();
  int e, f, ne_tot = zone.nb_elem_tot(), n, N = inco.line_size();

  const Champ_Don& rho = equation().milieu().masse_volumique(), Cp = equation().milieu().capacite_calorifique();
  int rCp = equation().que_suis_je() == "Conduction"
            || equation().que_suis_je() == "Convection_Diffusion_Temperature"
            || equation().que_suis_je() == "Convection_Diffusion_Temperature_Turbulent"
            || equation().que_suis_je().debute_par("Transport") ? 0 : 1;

  ch.init_cl();
  //partie superieure : diagonale
  for (e = 0; e < zone.nb_elem(); e++) for (n = 0; n < N; n++)
      secmem.addr()[N * e + n] += (rCp ? rho(e, 0) * Cp(e, 0) : 1) * pe(e) * ve(e) * inco.addr()[N * e + n] / dt;

  //partie inferieure : valeur imposee pour les CLs de Neumann, 0
  for (f = 0; f < zone.nb_faces(); f++)
    if (ch.icl(f, 0) == 3) for (n = 0; n < N; n++)
        secmem(N * (ne_tot + f) + n) = ref_cast(Neumann_paroi, cls[ch.icl(f, 1)].valeur()).flux_impose(ch.icl(f, 2), n);
    else if (sub_type(Op_Diff_negligeable, equation().operateur(0).l_op_base()) || ch.icl(f, 0) == 4)
      for (n = 0; n < N; n++) secmem(N * (ne_tot + f) + n) = 0;
  return secmem;
}

Matrice_Base& Masse_PolyMAC_Elem::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleVect& ve = zone.volumes(), &pe = zone.porosite_elem(), &fs = zone.face_surfaces();
  int i, e, f, ne_tot = zone.nb_elem_tot(), n, N = ch.valeurs().line_size();
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);

  const Champ_Don& rho = equation().milieu().masse_volumique(), Cp = equation().milieu().capacite_calorifique();
  int rCp = equation().que_suis_je() == "Conduction"
            || equation().que_suis_je() == "Convection_Diffusion_Temperature"
            || equation().que_suis_je() == "Convection_Diffusion_Temperature_Turbulent"
            || equation().que_suis_je().debute_par("Transport") ? 0 : 1;

  zone.init_m2(), ch.init_cl();
  //partie superieure : diagonale et divergence
  for (e = 0; e < zone.nb_elem(); e++)
    {
      for (n = 0; n < N; n++) mat(N * e + n, N * e + n) += (rCp ? rho(e, 0) * Cp(e, 0) : 1) * pe(e) * ve(e) / dt; //diagonale
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (n = 0; n < N; n++)
          mat(N * e + n, N * (ne_tot + f) + n) += fs(f) * (e == f_e(f, 0) ? 1 : -1);
    }
  //partie inferieure : 1 pour les flux imposes par CLs aux faces (si diffusion) ou pour toutes les faces (sinon)
  for (f = 0; f < zone.nb_faces(); f++)
    if (sub_type(Op_Diff_negligeable, equation().operateur(0).l_op_base()) || ch.icl(f, 0) == 3 || ch.icl(f, 0) == 4)
      for (n = 0; n < N; n++) mat(N * (ne_tot + f) + n, N * (ne_tot + f) + n) = 1;

  return matrice;
}
