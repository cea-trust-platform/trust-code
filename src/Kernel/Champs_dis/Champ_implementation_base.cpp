/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_implementation_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_implementation_base.h>
#include <Champ_Inc_base.h>
#include <Champ_Fonc_base.h>
#include <Zone_VF.h>
#include <Domaine.h>

DoubleTab& Champ_implementation_base::valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const
{
  Nom domain_name  = domain.le_nom();
  Nom support_name = get_zone_geom().domaine().le_nom();

  if(domain_name == support_name)
    {
      valeur_aux_sommets_impl(result);
    }
  else
    {
      const DoubleTab& positions = domain.coord_sommets();
      le_champ().valeur_aux(positions, result);
    }

  return result;
}

DoubleVect& Champ_implementation_base::valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const
{
  Nom domain_name  = domain.le_nom();
  Nom support_name = get_zone_geom().domaine().le_nom();

  if(domain_name == support_name)
    {
      valeur_aux_sommets_compo_impl(result,ncomp);
    }
  else
    {
      const DoubleTab& positions = domain.coord_sommets();
      le_champ().valeur_aux_compo(positions, result, ncomp);
    }

  return result;
}

const Zone_VF& Champ_implementation_base::get_zone_dis(void)  const
{
  const Champ_base& ch_base = le_champ();

  if(sub_type(Champ_Inc_base, ch_base))
    {
      const Champ_Inc_base& ch_inc = ref_cast(Champ_Inc_base, ch_base);
      return ref_cast(Zone_VF,ch_inc.zone_dis_base());
    }
  else if(sub_type(Champ_Fonc_base, ch_base))
    {
      const Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base, ch_base);
      return ref_cast(Zone_VF,ch_fonc.zone_dis_base());
    }
  else
    {
      Cerr << le_champ().que_suis_je() << "do not know refer a Zone_VF" << finl;
      Process::exit();
      return get_zone_dis();
    }
}

const Zone& Champ_implementation_base::get_zone_geom(void) const
{
  return get_zone_dis().zone();
}
