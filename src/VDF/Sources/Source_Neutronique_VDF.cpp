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
// File:        Source_Neutronique_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Neutronique_VDF.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ_Inc.h>

Implemente_instanciable_sans_constructeur(Source_Neutronique_VDF,"Source_Neutronique_VDF_P0_VDF",Source_Neutronique);
implemente_It_Sou_VDF_Elem(Eval_Puiss_Neutr_VDF_Elem)


Sortie& Source_Neutronique_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Source_Neutronique_VDF::readOn(Entree& s )
{
  Source_Neutronique::readOn(s);
  return s;
}

void Source_Neutronique_VDF::completer()
{
  Source_Neutronique::completer();
  iter.completer_();
}

double Source_Neutronique_VDF::calculer_Tmoyenne()
{
  double T=0.;
  const int nb_elem = la_zone->nb_elem();
  const DoubleTab& temp = equation().inconnue().valeurs();
  for (int i=0; i<nb_elem; i++)
    T += temp(i);

  T=T/nb_elem;
  return T;
}

void Source_Neutronique_VDF::associer_zones(const Zone_dis& zone_dis,
                                            const Zone_Cl_dis& zone_cl_dis)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  la_zone = zvdf;
  iter->associer_zones(zvdf, zclvdf);

  Eval_Puiss_Neutr_VDF_Elem& eval_puis = (Eval_Puiss_Neutr_VDF_Elem&) iter.evaluateur();
  eval_puis.associer_zones(zvdf, zclvdf );
}

void Source_Neutronique_VDF::associer_pb(const Probleme_base& pb)
{
  preparer_source(pb);
  Eval_Puiss_Neutr_VDF_Elem& eval_puis = (Eval_Puiss_Neutr_VDF_Elem&) iter.evaluateur();
  eval_puis.associer_champs(rho_ref.valeur(),Cp.valeur(),la_puissance);
  eval_puis.associer_repartition(repartition(),nom_ssz());
}


