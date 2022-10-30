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

#include <Masse_VDF_Elem.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Milieu_base.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>

Implemente_instanciable(Masse_VDF_Elem,"Masse_VDF_P0_VDF",Masse_VDF_base);

Sortie& Masse_VDF_Elem::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_VDF_Elem::readOn(Entree& s) { return s ; }

void Masse_VDF_Elem::preparer_calcul()
{
  associer_masse_proto(*this, la_zone_VDF.valeur());
  if (sub_type(Pb_Multiphase,equation().probleme())) preparer_calcul_proto();
  equation().init_champ_conserve();

}

DoubleTab& Masse_VDF_Elem::appliquer_impl(DoubleTab& sm) const
{
  if (sub_type(Pb_Multiphase, equation().probleme())) return appliquer_impl_proto(sm);
  else
    {
      const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
      const DoubleVect& volumes = zone_VDF.volumes(), &porosite_elem = equation().milieu().porosite_elem();
      int nb_elem = zone_VDF.nb_elem();
      if (nb_elem == 0)
        {
          sm.echange_espace_virtuel();
          return sm;
        }
      int nb_comp = sm.size() / nb_elem;
      int nb_dim = sm.nb_dim();
      assert((nb_comp * nb_elem == sm.size()) || (nb_dim == 3));
      if (nb_dim == 1)
        for (int num_elem = 0; num_elem < nb_elem; num_elem++)
          sm(num_elem) /= (volumes(num_elem) * porosite_elem(num_elem));
      else if (nb_dim == 2)
        {
          for (int num_elem = 0; num_elem < nb_elem; num_elem++)
            for (int k = 0; k < nb_comp; k++)
              sm(num_elem, k) /= (volumes(num_elem) * porosite_elem(num_elem));
        }
      else if (sm.nb_dim() == 3)
        {
          //int d0=sm.dimension(0);
          int d1 = sm.dimension(1);
          int d2 = sm.dimension(2);
          for (int num_elem = 0; num_elem < nb_elem; num_elem++)
            for (int k = 0; k < d1; k++)
              for (int d = 0; d < d2; d++)
                sm(num_elem, k, d) /= (volumes(num_elem) * porosite_elem(num_elem));
        }
      else
        {
          Cerr << "Masse_VDF_Elem::appliquer ne peut pas s'appliquer a un DoubleTab a " << sm.nb_dim() << " dimensions" << finl;
          Process::exit();
        }
      sm.echange_espace_virtuel();
      return sm;
    }
}

void Masse_VDF_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  if (sub_type(Pb_Multiphase, equation().probleme()))
    dimensionner_blocs_proto(matrices, semi_impl);
  else
    Masse_VDF_base::dimensionner_blocs(matrices, semi_impl);
}

void Masse_VDF_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  if (sub_type(Pb_Multiphase, equation().probleme()))
    ajouter_blocs_proto(matrices, secmem, dt, semi_impl, resoudre_en_increments);
  else
    Masse_VDF_base::ajouter_blocs(matrices, secmem, dt, semi_impl, resoudre_en_increments);
}
