/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Terme_Source_Decroissance_Radioactive_P0_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Decroissance_Radioactive_P0_PolyMAC.h>
#include <Equation_base.h>
#include <Zone_Cl_dis.h>
#include <Zone_PolyMAC.h>
#include <Synonyme_info.h>

Implemente_instanciable_sans_constructeur(Terme_Source_Decroissance_Radioactive_P0_PolyMAC,"Decroissance_Radioactive_P0_PolyMAC",Source_base);
Add_synonym(Terme_Source_Decroissance_Radioactive_P0_PolyMAC,"radioactive_decay_P0_PolyMAC");

//// printOn
//

Sortie& Terme_Source_Decroissance_Radioactive_P0_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}

//// readOn
//

Entree& Terme_Source_Decroissance_Radioactive_P0_PolyMAC::readOn(Entree& s )
{
  double lambda_tmp;
  s >> nb_groupes;
  Cerr << "Nombre de groupes a lire : " << nb_groupes << finl;
  for (int i = 0; i < nb_groupes; i++)
    {
      s >> lambda_tmp;
      Cerr << "lambda lu : " << lambda_tmp << finl;
      lambda.push_back(lambda_tmp);
    }

  return s ;
}

void Terme_Source_Decroissance_Radioactive_P0_PolyMAC::completer()
{
  Source_base::completer();
  const int N = equation().inconnue().valeurs().line_size();
  if (N != nb_groupes)
    {
      Cerr << "Terme_Source_Decroissance_Radioactive_P0_PolyMAC : inconsistency between the number of radioactive decay constants ( " << nb_groupes << " ) and the number of components of the unknown of the equation ( " << N << " )" << finl;
      Process::exit();
    }
}

void Terme_Source_Decroissance_Radioactive_P0_PolyMAC::associer_zones(const Zone_dis& zone_dis,
                                                                      const Zone_Cl_dis& zone_Cl_dis)
{
  Cerr << " Terme_Source_Decroissance_Radioactive_P0_PolyMAC::associer_zones " << finl ;
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC, zone_dis.valeur());
}

DoubleTab& Terme_Source_Decroissance_Radioactive_P0_PolyMAC::ajouter(DoubleTab& resu)  const
{
  int nb_elem = la_zone_PolyMAC.valeur().nb_elem();
  const Zone_VF& zone = la_zone_PolyMAC.valeur();
  const DoubleVect& ve = zone.volumes();
  const DoubleTab& c = equation().inconnue().valeurs();

  for (int f = 0; f < nb_elem; f++)
    for (int l = 0; l < nb_groupes; l++)
      resu.addr()[nb_groupes * f + l] -= lambda[l] * c.addr()[nb_groupes * f + l] * ve(f);

  return resu;
}

DoubleTab& Terme_Source_Decroissance_Radioactive_P0_PolyMAC::calculer(DoubleTab& resu) const
{
  resu=0;
  ajouter(resu);
  return resu;
}

void Terme_Source_Decroissance_Radioactive_P0_PolyMAC::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  int nb_elem = la_zone_PolyMAC.valeur().nb_elem();
  const Zone_VF& zone = la_zone_PolyMAC.valeur();
  const DoubleVect& ve = zone.volumes();

  for (int f = 0; f < nb_elem; f++)
    for (int l = 0; l < nb_groupes; l++)
      {
        const int k = f * nb_groupes + l;
        matrice(k, k) += lambda[l] * ve(f);
      }
}
