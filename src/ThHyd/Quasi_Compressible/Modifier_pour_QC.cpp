/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Modifier_pour_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Modifier_pour_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Zone_VF.h>

static void multiplier_ou_diviser(DoubleVect& x, const DoubleVect& y, int diviser)
{
  x.echange_espace_virtuel(); // Fixed bug by MR
  if (!diviser)
    tab_multiply_any_shape(x, y); //,VECT_REAL_ITEMS);
  else
    tab_divide_any_shape(x, y); //,VECT_REAL_ITEMS);
}

// Modif B.M: on ne remplit que la partie reelle du tableau.
void multiplier_diviser_rho(DoubleVect& tab,const Fluide_Quasi_Compressible& le_fluide,int diviser)
{
  const Zone_VF& zvf = ref_cast(Zone_VF,le_fluide.vitesse().valeur().zone_dis_base());
  // Descripteurs des tableaux aux elements et aux faces:
  const Zone& zone = zvf.zone();
  const MD_Vector& md_elem = zone.les_elems().get_md_vector();
  const MD_Vector& md_faces = zvf.md_vector_faces();
  const MD_Vector& md_faces_bord = zvf.md_vector_faces_bord();

  if (tab.get_md_vector() == md_faces_bord)
    {
      const DoubleTab& rho = ref_cast(Fluide_Quasi_Compressible,le_fluide).rho_discvit();
      DoubleVect rho_bord;
      // B.M. je cree une copie sinon il faut truander les tests sur les tailles dans multiply_any_shape
      // ou creer un DoubleTab qui pointe sur rho...
      zvf.creer_tableau_faces_bord(rho_bord,Array_base::NOCOPY_NOINIT);
      rho_bord.inject_array(rho,rho_bord.size());
      multiplier_ou_diviser(tab,rho_bord,diviser);
      return;
    }

  if (tab.get_md_vector() == md_faces)
    {
      const DoubleTab& rho_faces = ref_cast(Fluide_Quasi_Compressible,le_fluide).rho_discvit();
      multiplier_ou_diviser(tab,rho_faces,diviser);
      return;
    }

  const DoubleTab& rho = le_fluide.masse_volumique().valeurs();
  if (tab.get_md_vector() == md_elem && rho.get_md_vector() == md_elem)
    {
      multiplier_ou_diviser(tab,rho,diviser);
      return;
    }

  if (tab.get_md_vector() == md_elem && rho.get_md_vector() == md_faces)
    {
      // Il faut calculer rho aux elements
      const DoubleTab& tab_rho = le_fluide.masse_volumique().valeurs();
      DoubleVect rho_elem;
      zone.creer_tableau_elements(rho_elem, Array_base::NOCOPY_NOINIT);
      const int nb_elem_tot = zone.nb_elem_tot();
      const IntTab& elem_faces = zvf.elem_faces();
      const int nfe = elem_faces.dimension(1);
      const double facteur = 1. / nfe;
      for (int elem = 0; elem < nb_elem_tot; elem++)
        {
          double x = 0.;
          for (int face=0; face < nfe; face++)
            {
              int f = elem_faces(elem, face);
              x += tab_rho[f];
            }
          rho_elem[elem] = x * facteur;
        }
      multiplier_ou_diviser(tab,rho_elem,diviser);
      return;
    }

  Cerr << "Error in Modifier_pour_QC.cpp: multiplier_rho. Invalid discretization of tab and rho." << finl;
  Process::exit();
}

// Description: multiplie le tableau val par la masse volumique si le fluide est QC.
//  Le tableau val peut avoir diverses localisations (determinees a partir de get_md_vector())
//  et peut etre de type DoubleTab avec des dimension(i>0) quelconques (plusieurs composantes)
void multiplier_par_rho_si_qc(DoubleVect& val,const Milieu_base& mil)
{
  if (sub_type(Fluide_Quasi_Compressible,mil))
    {
      multiplier_diviser_rho(val,ref_cast(Fluide_Quasi_Compressible,mil), 0 /* multiplier */);
    }
}

// Description: Idem que multiplier_par_rho_si_qc mais on divise par rho.
void diviser_par_rho_si_qc(DoubleVect& val,const Milieu_base& mil)
{
  if (sub_type(Fluide_Quasi_Compressible,mil))
    {
      multiplier_diviser_rho(val,ref_cast(Fluide_Quasi_Compressible,mil), 1 /* diviser */);
    }
}
