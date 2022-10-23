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

#ifndef Champ_Elem_PolyMAC_included
#define Champ_Elem_PolyMAC_included

#include <Op_Diff_PolyMAC_base.h>
#include <Champ_Inc_P0_base.h>
#include <Operateur.h>

/*! @brief : class Champ_Elem_PolyMAC
 *
 *  Champ correspondant a une inconnue scalaire (type temperature ou pression)
 *  Degres de libertes : valeur aux elements + flux aux faces
 */
class Champ_Elem_PolyMAC : public Champ_Inc_P0_base
{
  Declare_instanciable(Champ_Elem_PolyMAC);
public :
  const Zone_PolyMAC& zone_PolyMAC() const;

  Champ_base& affecter_(const Champ_base& ch) override;
  int imprime(Sortie&, int) const override;

  int fixer_nb_valeurs_nodales(int n) override; //valeurs aux elements
  int nb_valeurs_nodales() const override;
  virtual void init_auxiliary_variables(); //pour demander en plus les inconnues auxiliaires (valeurs aux faces)
  int reprendre(Entree& fich) override;

  /* fonctions reconstruisant de maniere plus precise le champ aux faces */
  DoubleTab& valeur_aux_faces(DoubleTab& vals) const override;
  inline DoubleTab& trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const override;
};

inline DoubleTab& Champ_Elem_PolyMAC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  /* dimensionnement du tableau de destination x si necessaire */
  const DoubleTab& src = valeurs();
  const Front_VF& fvf = ref_cast(Front_VF, fr);
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis_base());
  const IntTab& f_e = zone.face_voisins();

  DoubleTrav dst; //reconstruction du champ aux faces (on ne le remplit que sur le bord concerne)
  int i, n, e, f, N = src.nb_dim() > 1 ? src.dimension(1): 1, has_faces = src.dimension_tot(0) > zone.nb_elem_tot();
  if (!x.dimension(0) && !x.get_md_vector().non_nul()) x.resize(fvf.nb_faces(), N);
  dst.resize(zone.nb_faces(), N);
  for (i = 0; i < fvf.nb_faces(); i++)
    for (n = 0, f = fvf.num_face(i), e = f_e(f, 0); n < N; n++)
      dst(f, n) = src(has_faces ? zone.nb_elem_tot() + f : e, n);

  if (distant) fr.frontiere().trace_face_distant(dst, x);
  else fr.frontiere().trace_face_local(dst, x);

  return x;
}

#endif
