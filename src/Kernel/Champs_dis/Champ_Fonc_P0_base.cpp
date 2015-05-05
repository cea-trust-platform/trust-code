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
// File:        Champ_Fonc_P0_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_P0_base.h>
#include <Domaine.h>
#include <Zone_dis_base.h>
#include <Scatter.h>
#include <Frontiere_dis_base.h>

Implemente_base(Champ_Fonc_P0_base,"Champ_Fonc_P0_base",Champ_Fonc_base);

Sortie& Champ_Fonc_P0_base::printOn(Sortie& os) const
{
  os << que_suis_je() << " " << le_nom();
  return os;
}

Entree& Champ_Fonc_P0_base::readOn(Entree& is)
{
  return is;
}

Champ_base& Champ_Fonc_P0_base::le_champ(void)
{
  return *this;
}

const Champ_base& Champ_Fonc_P0_base::le_champ(void) const
{
  return *this;
}

int Champ_Fonc_P0_base::fixer_nb_valeurs_nodales(int n)
{
  // Encore une syntaxe a la con, sinon on ne sait pas s'il faut appeler
  // zone_dis_base() de champ_inc_base ou de champ_impl...
  const Zone_dis_base& zonedis = ref_cast(Champ_Fonc_base, *this).zone_dis_base();
  assert(n == zonedis.zone().nb_elem());
  const MD_Vector& md = zonedis.zone().md_vector_elements();
  creer_tableau_distribue(md);
  return 1;
}

Champ_base& Champ_Fonc_P0_base::affecter_(const Champ_base& ch)
{
  if (Champ_implementation_P0::affecter_(ch))
    {
      return *this;
    }
  else
    {
      return Champ_Fonc_base::affecter_(ch);
    }
}

// Description :
// Trace du champ P0 sur la frontiere
DoubleTab& Champ_Fonc_P0_base::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps) const
{
  return Champ_implementation_P0::trace(fr, valeurs(), x);
}
