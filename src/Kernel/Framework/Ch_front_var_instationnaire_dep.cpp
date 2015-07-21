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
// File:        Ch_front_var_instationnaire_dep.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Ch_front_var_instationnaire_dep.h>
#include <Champ_Inc_base.h>
#include <Frontiere_dis_base.h>
#include <Zone_dis_base.h>
#include <Zone_VF.h>

Implemente_instanciable(Ch_front_var_instationnaire_dep,"Ch_front_var_instationnaire_dep",Champ_front_var_instationnaire);

Sortie& Ch_front_var_instationnaire_dep::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Ch_front_var_instationnaire_dep::readOn(Entree& s)
{
  int dim;
  s >> dim;
  fixer_nb_comp(dim);

  return s;
}

int Ch_front_var_instationnaire_dep::initialiser(double temps, const Champ_Inc_base& inco)
{

  if (!Champ_front_var_instationnaire::initialiser(temps,inco))
    return 0;

  // Initialisation par defaut a la valeur de l'inconnue au bord
  // Ca veut dire prendre la trace de son propre bord
  // Attention, ne pas passer par les raccords !
  DoubleTab& tab=les_valeurs->valeurs();
  assert(temps==inco.temps());
  if(nb_comp()!=inco.nb_comp())
    {
      // Used by ICoCo see U_in_var_impl test case
      // Different field so we initialize to 0:
      tab = 0;
    }
  else
    {
      const Frontiere& MaFrontiere = frontiere_dis().frontiere();
      if (inco.valeurs().dimension(0)==inco.zone_dis_base().nb_elem())
        MaFrontiere.Frontiere::trace_elem(inco.valeurs(),tab);
      else if (inco.valeurs().dimension(0)==ref_cast(Zone_VF,inco.zone_dis_base()).nb_faces())
        MaFrontiere.Frontiere::trace_face(inco.valeurs(),tab);
      else
        {
          Cerr << "Warning: Field " << inco.que_suis_je() << " is not supported yet in Ch_front_var_instationnaire_dep::initialiser" << finl;
          //Process::exit();
        }
    }
  tab.echange_espace_virtuel();

  return 1;
}

Champ_front_base& Ch_front_var_instationnaire_dep::affecter_(const Champ_front_base& ch)
{
  return *this;
}
