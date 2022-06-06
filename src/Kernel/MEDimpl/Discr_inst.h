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

#ifndef Discr_inst_included
#define Discr_inst_included

//
// .DESCRIPTION class Discr_inst
//  Cette classe ne sait rien faire, on en a besoin pour le Pb_MED
// on a besoin d'avoir associe une discretisation


#include <Discretisation_base.h>


class Discr_inst : public Discretisation_base
{
  Declare_instanciable(Discr_inst);

public:
  //
  // Methodes surchargees de Discretisation_base
  //
  void zone_Cl_dis(Zone_dis& , Zone_Cl_dis& ) const override
  {
    Cerr<<__FILE__<<(int)__LINE__<<" not coded"<<finl;
    exit();
  };
  void volume_maille(const Schema_Temps_base& sch, const Zone_dis& z,Champ_Fonc& ch) const
  {
    Cerr<<__FILE__<<(int)__LINE__<<" not coded"<<finl;
    exit();
  };
  void modifier_champ_tabule(const Zone_dis_base& zone_vdf,Champ_Fonc_Tabule& lambda_tab,const VECT(REF(Champ_base))&  ch_temper) const override
  {
    Cerr<<__FILE__<<(int)__LINE__<<" not coded"<<finl;
    Process::exit();
  };

  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, int nb_pas_dt, double temps,
                         Champ_Inc& champ, const Nom& sous_type = nom_vide) const override;
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, double temps,
                         Champ_Fonc& champ) const override;
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, double temps,
                         Champ_Don& champ) const override;
private:
  void discretiser_champ_fonc_don(const Motcle& directive, const Zone_dis_base& z,
                                  Nature_du_champ nature, const Noms& noms, const Noms& unites,
                                  int nb_comp, double temps, Objet_U& champ) const;

};

#endif
