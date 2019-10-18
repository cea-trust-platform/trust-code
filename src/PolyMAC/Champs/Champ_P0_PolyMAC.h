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
// File:        Champ_P0_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_P0_PolyMAC_included
#define Champ_P0_PolyMAC_included

#include <Champ_Inc_P0_base.h>
#include <Ref_Zone_VF.h>
#include <Zone_PolyMAC.h>
#include <Operateur.h>
#include <Op_Diff_PolyMAC_base.h>

class Zone_PolyMAC;

/////////////////////////////////////////////////////////////////////////////
// .NAME        : Champ_P0_PolyMAC
// .DESCRIPTION : class Champ_P0_PolyMAC
//
// Champ correspondant a une inconnue scalaire (type temperature ou pression)
// Degres de libertes : valeur aux elements + flux aux faces
/////////////////////////////////////////////////////////////////////////////

class Champ_P0_PolyMAC : public Champ_Inc_P0_base
{
  Declare_instanciable(Champ_P0_PolyMAC);

public :

  const Zone_PolyMAC&        zone_PolyMAC() const;
  void                         associer_zone_dis_base(const Zone_dis_base&);
  virtual const Zone_dis_base& zone_dis_base() const;
  Champ_base& affecter_(const Champ_base& ch);
  int                       imprime(Sortie& , int ) const;

  virtual int fixer_nb_valeurs_nodales(int n);

  /* fonctions reconstruisant de maniere plus precise le champ aux faces */
  DoubleTab& valeur_aux_faces(DoubleTab& vals) const;
  inline DoubleTab& trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const;

  //tableaux utilitaires sur les CLs
  //types de CL : 0 -> pas de CL
  //              1 -> Echange_externe_impose
  //              2 -> Echange_global_impose
  //              3 -> Neumann_paroi
  //              4 -> Neumann_sortie_libre
  //              5 -> Dirichlet
  //              6 -> Dirichlet_homogene
  void init_cl() const;
  mutable IntTab icl; //icl(f, .) = (type de la CL, no de la CL, indice dans la CL)

protected :

  REF(Zone_VF) la_zone_VF;


};

inline DoubleTab& Champ_P0_PolyMAC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  /* dimensionnement du tableau de destination x si necessaire */
  const DoubleTab& src = valeurs();
  const Front_VF& fvf = ref_cast(Front_VF, fr);
  int N = src.nb_dim() > 1 ? src.dimension(1): 1;
  if (!x.dimension(0) && !x.get_md_vector().non_nul()) x.resize(fvf.nb_faces(), N);
  const Operateur_base& op_diff = equation().operateur(0).l_op_base(); //pour acceder a la diffusivite
  const DoubleTab *nu_f = (&equation().inconnue().valeur() == this && sub_type(Op_Diff_PolyMAC_base, op_diff)) ? &ref_cast(Op_Diff_PolyMAC_base, op_diff).get_nu_faces() : NULL;
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC, zone_dis_base());
  const IntTab& f_e = zone.face_voisins();
  zone.init_m2();

  DoubleTrav dst; //reconstruction du champ aux faces (on ne le remplit que sur le bord concerne)
  N > 1 ? dst.resize(zone.nb_faces(), N) : dst.resize(zone.nb_faces()); //aargh
  for (int i = 0; i < fvf.nb_faces(); i++) for (int n = 0, f = fvf.num_face(i); n < N; n++)
      dst.addr()[N * f + n] = src.addr()[N * f_e(f, 0) + n]; //on part de la valeur en l'element voisin

  if (src.dimension_tot(0) > zone.zone().nb_elem_tot()) //si on a les flux aux faces, on corrige avec m2
    {
      for (int i = 0; i < fvf.nb_faces(); i++) for (int n = 0, f = fvf.num_face(i); n < N; n++) for (int j = zone.m2deb(f); j < zone.m2deb(f + 1); j++)
            dst.addr()[N * f + n] -= src.addr()[N * (zone.nb_elem_tot() + zone.m2ji(j, 0)) + n] * zone.m2ci(j) / zone.face_surfaces(f) * (nu_f && nu_f->addr()[N * f + n] > 1e-12 ? 1. / nu_f->addr()[N * f + n] : 0);
    }

  if (distant)
    fr.frontiere().trace_face_distant(dst, x);
  else
    fr.frontiere().trace_face_local(dst, x);
  // useless ? x.echange_espace_virtuel();
  return x;
}

#endif
