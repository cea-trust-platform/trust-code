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
// File:        Source_Qdm_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Qdm_EF.h>
#include <Champ_Uniforme.h>
#include <Domaine.h>
#include <Zone_Cl_dis.h>
#include <Zone_EF.h>
#include <Zone_Cl_EF.h>

#include <Equation_base.h>
#include <Champ_Inc.h>
#include <Debog.h>

Implemente_instanciable(Source_Qdm_EF,"Source_Qdm_EF",Source_base);



//// printOn
//

Sortie& Source_Qdm_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Source_Qdm_EF::readOn(Entree& s )
{
  s >> la_source;
  if (la_source->nb_comp() != dimension)
    {
      Cerr << "Erreur a la lecture du terme source de type " << que_suis_je() << finl;
      Cerr << "le champ source doit avoir " << dimension << " composantes" << finl;
      exit();
    }
  return s ;
}

void Source_Qdm_EF::associer_pb(const Probleme_base& )
{
  ;
}

void Source_Qdm_EF::associer_zones(const Zone_dis& zone_dis,
                                   const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_EF = ref_cast(Zone_EF, zone_dis.valeur());
  la_zone_Cl_EF = ref_cast(Zone_Cl_EF, zone_Cl_dis.valeur());
}


DoubleTab& Source_Qdm_EF::ajouter(DoubleTab& resu) const
{
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  int ncomp=dimension;
  const IntTab& elems= zone_EF.zone().les_elems() ;
  int nb_som_elem=zone_EF.zone().nb_som_elem();
  int nb_elems=zone_EF.zone().nb_elem_tot();
  const DoubleTab& IPhi_thilde=zone_EF.IPhi_thilde();
  int is_source_unif=0;

  if (sub_type(Champ_Uniforme,la_source.valeur()))
    is_source_unif=1;
  const DoubleTab& tab_source=la_source.valeur().valeurs();
  for (int num_elem=0; num_elem<nb_elems; num_elem++)
    for (int comp=0; comp<ncomp; comp++)
      {
        double sourcel ;
        int cc=0;
        if (!is_source_unif) cc=num_elem;
        sourcel=tab_source(cc,comp);
        for (int i=0; i<nb_som_elem; i++)
          {
            // assert faux si on a des porosites
            //	assert(est_egal(zone_EF.volumes(num_elem)/nb_som_elem,IPhi_thilde(num_elem,i)));
            resu(elems(num_elem,i),comp)+=sourcel*IPhi_thilde(num_elem,i);
          }
      }

  return resu;
}

DoubleTab& Source_Qdm_EF::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Source_Qdm_EF::mettre_a_jour(double temps)
{
  la_source->mettre_a_jour(temps);
}

