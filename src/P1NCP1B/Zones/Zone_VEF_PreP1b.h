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
// File:        Zone_VEF_PreP1b.h
// Directory:   $TRUST_ROOT/src/P1NCP1B/Zones
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_VEF_PreP1b_included
#define Zone_VEF_PreP1b_included

#include <Zone_VEF.h>
#include <Zone.h>

class VEFPreP1B;

class Zone_VEF_PreP1b : public Zone_VEF
{

  Declare_instanciable(Zone_VEF_PreP1b);

public :
  void discretiser() override;
  virtual void discretiser_suite(const VEFPreP1B&);
  void modifier_pour_Cl(const Conds_lim& ) override;
  int nb_som() const
  {
    return zone().nb_som();
  };
  int nb_elem() const
  {
    return zone().nb_elem();
  };
  double volume_au_sommet(int som) const
  {
    return volumes_som[som];
  };
  const DoubleVect& volume_aux_sommets() const
  {
    return volumes_som;
  };
  int get_P1Bulle() const
  {
    assert(P1Bulle!=-1);
    return P1Bulle;
  };
  int get_alphaE() const
  {
    assert(alphaE!=-1);
    return alphaE;
  };
  int get_alphaS() const
  {
    assert(alphaS!=-1);
    return alphaS;
  };
  int get_alphaA() const
  {
    assert(alphaA!=-1);
    return alphaA;
  };
  int get_modif_div_face_dirichlet() const
  {
    assert(modif_div_face_dirichlet!=-1);
    return modif_div_face_dirichlet;
  };
  int get_cl_pression_sommet_faible() const
  {
    assert(cl_pression_sommet_faible!=-1);
    return cl_pression_sommet_faible;
  };
  const ArrOfInt& get_renum_arete_perio() const
  {
    return renum_arete_perio;
  };
  const IntVect& get_ok_arete() const
  {
    return ok_arete;
  };
  const DoubleVect& get_volumes_aretes() const
  {
    return volumes_aretes;
  };
  void discretiser_arete();
  void construire_ok_arete();
  int lecture_ok_arete();
  void verifie_ok_arete(int) const;
  void construire_renum_arete_perio(const Conds_lim&);
  inline int numero_premier_element() const;
  inline int numero_premier_sommet() const;
  inline int numero_premiere_arete() const;

  virtual void creer_tableau_p1bulle(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual const MD_Vector& md_vector_p1b() const
  {
    assert(md_vector_p1b_.non_nul());
    return md_vector_p1b_;
  }
protected:
  DoubleVect volumes_som;
  DoubleVect volumes_aretes;
  ArrOfInt renum_arete_perio;
  IntVect ok_arete;
  int P1Bulle;
  int alphaE;
  int alphaS;
  int alphaA;
  int modif_div_face_dirichlet ;
  int cl_pression_sommet_faible; // determine si les cl de pression sont imposees de facon faible ou forte -> voir divergence et assembleur
  // Descripteur pour les tableaux p1b (selon alphaE, alphaS et alphaA)
  // (construit dans Zone_VEF_PreP1b::discretiser())
  MD_Vector md_vector_p1b_;
};


inline int Zone_VEF_PreP1b::numero_premier_element() const
{
  if (!alphaE)
    return -1;
  else
    return 0;
}
inline int Zone_VEF_PreP1b::numero_premier_sommet() const
{
  if (!alphaS)
    return -1;
  else if (!alphaE)
    return 0;
  else
    return nb_elem_tot();
}
inline int Zone_VEF_PreP1b::numero_premiere_arete() const
{
  if (!alphaA)
    return -1;
  else if (!alphaE && !alphaS)
    return 0;
  else if (!alphaE && alphaS)
    return nb_som_tot();
  else if (!alphaS && alphaE)
    return nb_elem_tot();
  else
    return nb_elem_tot()+nb_som_tot();
}

// Methode pour tester:
void exemple_champ_non_homogene(const Zone_VEF_PreP1b&, DoubleTab&);

#endif
