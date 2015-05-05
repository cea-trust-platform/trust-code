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
// File:        Source_Transport_V2_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Transport_V2_VDF_Elem.h>
#include <Modele_turbulence_hyd_K_Eps_V2.h>
#include <Probleme_base.h>
#include <Zone_VDF.h>
//#include <Debog.h>

Implemente_instanciable_sans_constructeur(Source_Transport_V2_VDF_Elem,"Source_Transport_V2_VDF_P0_VDF",Source_base);



//// printOn
//

Sortie& Source_Transport_V2_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Source_Transport_V2_VDF_Elem::readOn(Entree& is)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_V2" << finl;
      exit();
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_V2" << finl;
      exit();

      is >> motlu;
    }

  return is ;
}



/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_V2_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

void Source_Transport_V2_VDF_Elem::associer_zones(const Zone_dis& zone_dis,
                                                  const Zone_Cl_dis& )
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
}

// remplit les references eq_hydraulique et mon_eq_transport_K_Eps
void Source_Transport_V2_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  eq_hydraulique = pb.equation(0);
  mon_eq_transport_V2 = ref_cast(Transport_V2,equation());
  mon_eq_transport_K_Eps = ref_cast(Transport_K_Eps_V2,mon_eq_transport_V2->modele_turbulence().eqn_transp_K_Eps());
}

DoubleTab& Source_Transport_V2_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& F22 = mon_eq_transport_V2->modele_turbulence().eqn_F22().inconnue().valeurs();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  const DoubleTab& v2 = mon_eq_transport_V2->inconnue().valeurs();

  int nb_elem = zone_VDF.nb_elem();
  //  int nb_elem_tot = zone_VDF.nb_elem_tot();

  for (int elem=0; elem<nb_elem; elem++)
    {
      resu(elem) += K_eps(elem,0)*F22(elem)*volumes(elem)*porosite_vol(elem);
      if (K_eps(elem,0) >= 1.e-10)
        {
          resu(elem) -= fac_n*(v2(elem)*K_eps(elem,1)/K_eps(elem,0)) * volumes(elem)*porosite_vol(elem);
        }
      else
        {
          resu(elem) -= 0. ;
        }

      //        resu(elem)*=volumes(elem)*porosite_vol(elem);
    }

  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Source_Transport_V2_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}



