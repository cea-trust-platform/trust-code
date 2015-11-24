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
// File:        Echange_contact_ODVM_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_contact_ODVM_VDF.h>
#include <Pb_Conduction.h>
#include <Equation_base.h>
#include <Modele_turbulence_scal_base.h>
#include <Paroi_ODVM_scal_VDF.h>


Implemente_instanciable(Echange_contact_ODVM_VDF,"Paroi_echange_contact_ODVM_VDF",Echange_contact_VDF);



Sortie& Echange_contact_ODVM_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_ODVM_VDF::readOn(Entree& s )
{
  return  Echange_contact_VDF::readOn( s );
}

void Echange_contact_ODVM_VDF::calculer_Teta_equiv(DoubleTab& La_T_ext,const DoubleTab& mon_h,const DoubleTab& lautre_h,int i,double temps)
{
  const Equation_base& mon_eqn = zone_Cl_dis().equation();
  const DoubleTab& mon_inco=mon_eqn.inconnue().valeurs();
  const Zone_VDF& ma_zvdf = ref_cast(Zone_VDF,zone_Cl_dis().zone_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  DoubleTab& t_autre=T_autre_pb()->valeurs_au_temps(temps);
  int ndeb = ma_front_vf.num_premiere_face();
  int nb_faces_bord = ma_front_vf.nb_faces();
  int ind_fac,elem, isfluide=0;
  //La_T_ext.resize(nb_faces_bord,1);
  try
    {
      mon_eqn.probleme().equation(0).get_champ("vitesse");
      isfluide=1;
    }
  catch (Champs_compris_erreur)
    {
    }

  const Champ_front_calc& chcal=ref_cast(Champ_front_calc,T_autre_pb().valeur());
  const Equation_base& eq = (isfluide==1?mon_eqn:chcal.inconnue().equation());
  const Modele_turbulence_scal_base& modele_turbulence = ref_cast(Modele_turbulence_scal_base,eq.get_modele(TURBULENCE).valeur());
  const Turbulence_paroi_scal& loipar = modele_turbulence.loi_paroi();

  const Paroi_ODVM_scal_VDF& paroi_vdf = ref_cast(Paroi_ODVM_scal_VDF,loipar.valeur());
  // Initialise la loi de paroi si necessaire:
// GF non pas ici	// paroi_vdf.init_lois_paroi();
  if (paroi_vdf.get_Tf0().size_array()==0)
    {
      Cerr<<"pb !!!!!!!!"<<finl;
      exit() ;
    }

  for (int numfa=0; numfa<nb_faces_bord; numfa++)
    {
      ind_fac = numfa+ndeb;
      if (ma_zvdf.face_voisins(ind_fac,0)!= -1)
        elem = ma_zvdf.face_voisins(ind_fac,0);
      else
        elem = ma_zvdf.face_voisins(ind_fac,1);
      //      double Delta_T= (mon_h(numfa,0)*mon_inco(elem)+autre_h(numfa,0)*T_autre_pb(numfa) - paroi_vdf.get_Tf0(numfa)*(mon_h(numfa,0)+autre_h(numfa,0)) );

      double Delta_T= (mon_h(numfa,0)*mon_inco(elem)+lautre_h(numfa,0)*t_autre(numfa,0) - paroi_vdf.get_Tf0(numfa)*(mon_h(numfa,0)+lautre_h(numfa,0)));

      // SI PB FLUIDE :
      if(isfluide) La_T_ext(numfa,0) = t_autre(numfa,0)+Delta_T/mon_h(numfa,0);
      // SINON (Solide)
      else         La_T_ext(numfa,0) = t_autre(numfa,0)-Delta_T/lautre_h(numfa,0);
    }
}

