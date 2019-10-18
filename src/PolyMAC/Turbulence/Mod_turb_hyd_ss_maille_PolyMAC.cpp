/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Mod_turb_hyd_ss_maille_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Turbulence
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Mod_turb_hyd_ss_maille_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_dis.h>
#include <Zone_Cl_PolyMAC.h>

Implemente_base(Mod_turb_hyd_ss_maille_PolyMAC,"Mod_turb_hyd_ss_maille_PolyMAC",Mod_turb_hyd_ss_maille);

//// printOn
//

Sortie& Mod_turb_hyd_ss_maille_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Mod_turb_hyd_ss_maille_PolyMAC::readOn(Entree& is )
{
  return Mod_turb_hyd_ss_maille::readOn(is);
}

void Mod_turb_hyd_ss_maille_PolyMAC::associer(const Zone_dis& zone_dis,
                                              const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC,zone_dis.valeur());
  la_zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC,zone_Cl_dis.valeur());
}

void Mod_turb_hyd_ss_maille_PolyMAC::calculer_longueurs_caracteristiques()
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  int nb_elem = zone_PolyMAC.zone().nb_elem();

  l_.resize(nb_elem);

  ArrOfDouble h(dimension);
  double dist_tot,dist_min,dist_max,dist_moy;
  double a1,a2,f_scotti;

  const int modele_scotti = (methode ==  Motcle("Scotti"));
  if (modele_scotti && (dimension==2))
    {
      Cerr << "The Scotti correction can be used only for dimension 3." << finl;
      exit();
    }

  for(int elem=0 ; elem<nb_elem ; elem ++)
    {
      for (int i=0 ; i<dimension ; i++)
        h(i)= std::pow(zone_PolyMAC.volumes(elem), 1. / dimension);

      if (dimension==2)
        l_(elem)=exp((log(h(0)*h(1)))/2);
      else
        l_(elem)=exp((log(h(0)*h(1)*h(2)))/3);

      if (modele_scotti)
        {
          dist_tot=h(0)+h(1)+h(2);

          dist_min=min_array(h);
          dist_max=max_array(h);
          dist_moy=dist_tot-dist_min-dist_max;

          a1=dist_min/dist_max;
          a2=dist_moy/dist_max;

          f_scotti=cosh(sqrt((4./27.)*( log(a1)*log(a1) - log(a1)*log(a2) + log(a2)*log(a2) )));

          l_(elem) *=f_scotti;
        }
    }
}
