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
// File:        ParoiVDF_TBLE_LRM.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ParoiVDF_TBLE_LRM_included
#define ParoiVDF_TBLE_LRM_included

#include <Vect.h>
#include <Paroi_hyd_base_VDF.h>
#include <Zone_dis.h>
#include <Zone_Cl_dis.h>
#include <Vect_Eq_couch_lim.h>
#include <Eq_couch_lim.h>
#include <Ref_Milieu_base.h>
#include <Milieu_base.h>
#include <Mod_echelle_LRM.h>


class Champ_Fonc_base;
class Zone_dis;
class Zone_Cl_dis;
class Mod_echelle_LRM_base;

//.DESCRIPTION
//
// CLASS: ParoiVDF_TBLE_LRM
//
//.SECTION  voir aussi
// Turbulence_paroi_base

class ParoiVDF_TBLE_LRM : public Paroi_hyd_base_VDF
{
  Declare_instanciable_sans_constructeur(ParoiVDF_TBLE_LRM);

public:

  virtual int init_lois_paroi();
  int calculer_hyd(DoubleTab& );
  int calculer_hyd(DoubleTab& , DoubleTab&);
  int calculer_sous_couche_log(DoubleTab& ,double ,int ,int );
  Eq_couch_lim& get_eq_couch_lim(int i)
  {
    return eq_k_U_W[i];
  };
  void imprimer_ustar(Sortie& os) const;

protected:

  int nb_pts, nb_comp, oui_stats, max_it;
  double fac, epsilon;
  Motcle modele_visco;

  VECT(Eq_couch_lim) eq_k_U_W ;
  int avec_boussi;
  double k_init;
  int nb_post_pts;
  DoubleTab sonde_tble;
  Noms nom_pts;
  IntTab num_faces_post;
  IntTab num_faces_post2;


private:

  Mod_echelle_LRM mod_ech;


};

#endif
