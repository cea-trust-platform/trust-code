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

#ifndef Op_Diff_EF_base_included
#define Op_Diff_EF_base_included

#include <Neumann_sortie_libre.h>
#include <Operateur_Diff_base.h>
#include <Ref_Champ_Inc_base.h>
#include <Ref_Zone_Cl_EF.h>
#include <Ref_Zone_EF.h>
#include <Op_EF_base.h>
#include <Zone_Cl_EF.h>
#include <Zone_EF.h>
class Zone_dis;
class Zone_Cl_dis;
class Champ_Inc_base;
class Sortie;

enum class AJOUTE_SCAL { GEN , D3_8 , D2_4 };
enum class AJOUTE_VECT { GEN , D3_8 , D2_4 };

// .DESCRIPTION class Op_Diff_EF_base
// Classe de base des operateurs de diffusion EF
class Op_Diff_EF_base : public Operateur_Diff_base, public Op_EF_base
{

  Declare_base(Op_Diff_EF_base);

public:

  int impr(Sortie& os) const override;
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& ) override;
  double calculer_dt_stab() const override;
  virtual void remplir_nu(DoubleTab&) const=0;

protected:

  REF(Zone_EF) la_zone_EF;
  REF(Zone_Cl_EF) la_zcl_EF;
  REF(Champ_Inc_base) inconnue;
  mutable DoubleTab nu_;

};

inline void remplir_marqueur_sommet_neumann(ArrOfInt& marqueur,const Zone_EF& zone_EF,const Zone_Cl_EF& zone_Cl_EF , int transpose_partout)
{
  marqueur.resize_array(zone_EF.nb_som_tot());
  if ( transpose_partout) return;
  // Neumann :
  int n_bord;
  int nb_bords=zone_EF.nb_front_Cl();
  const IntTab& face_sommets=zone_EF.face_sommets();
  int nb_som_face=zone_EF.nb_som_face();
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nfin =le_bord.nb_faces_tot();
          for (int ind_face=0; ind_face<nfin; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  marqueur[glob2]=1;

                }
            }
        }
    }
}

#endif
