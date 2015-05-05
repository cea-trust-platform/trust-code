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
// File:        Modele_Launder_Sharma_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_Launder_Sharma_VEF.h>
#include <Zone_VEF.h>

Implemente_instanciable(Modele_Launder_Sharma_VEF,"Modele_Launder_Sharma_VEF",Modele_Jones_Launder_VEF);



///////////////////////////////////////////////////////////////
//   Implementation des fonctions de la classe
///////////////////////////////////////////////////////////////
// printOn et readOn

Sortie& Modele_Launder_Sharma_VEF::printOn(Sortie& s ) const
{
  Modele_Jones_Launder_VEF::printOn(s);
  return s;
}

Entree& Modele_Launder_Sharma_VEF::readOn(Entree& is )
{
  Modele_Jones_Launder_VEF::readOn(is);
  return is;
}

Entree& Modele_Launder_Sharma_VEF::lire(const Motcle& , Entree& is)
{
  return is;
}

void  Modele_Launder_Sharma_VEF::associer(const Zone_dis& zone_dis,
                                          const Zone_Cl_dis& zone_Cl_dis)
{
  //  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  //  const Zone_Cl_VEF& la_zone_Cl = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
}

DoubleTab&  Modele_Launder_Sharma_VEF::Calcul_Fmu( DoubleTab& Fmu,const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,double visco ) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  int nb_faces = la_zone.nb_faces();
  int num_face;
  double Re;
  Fmu = 0;

  for (num_face=0; num_face<nb_faces  ; num_face++)
    {
      if (K_eps_Bas_Re(num_face,1)>0)
        {
          Re = (K_eps_Bas_Re(num_face,0)*K_eps_Bas_Re(num_face,0))/(visco*K_eps_Bas_Re(num_face,1));
          Fmu[num_face] = exp(-3.4/((1.+Re/50.)*(1.+Re/50.)));
        }
      else
        {
          Fmu[num_face] = 0.;
        }
    }
  return Fmu;
}

DoubleTab&  Modele_Launder_Sharma_VEF::Calcul_Fmu( DoubleTab& Fmu,const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,const DoubleTab& tab_visco ) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  int nb_faces = la_zone.nb_faces();
  int num_face,elem0,elem1;
  double Re,nulam;
  Fmu = 0;

  for (num_face=0; num_face<nb_faces  ; num_face++)
    {
      elem0 = la_zone.face_voisins(num_face,0);
      elem1 = la_zone.face_voisins(num_face,1);
      if (elem1!=-1)
        {
          nulam = tab_visco(elem0)*la_zone.volumes(elem0)+tab_visco(elem1)*la_zone.volumes(elem1);
          nulam /= la_zone.volumes(elem0) + la_zone.volumes(elem1);
        }
      else
        nulam =  tab_visco(elem0);

      if (K_eps_Bas_Re(num_face,1)>0)
        {
          Re = (K_eps_Bas_Re(num_face,0)*K_eps_Bas_Re(num_face,0))/(nulam*K_eps_Bas_Re(num_face,1));
          Fmu[num_face] = exp(-3.4/((1.+Re/50.)*(1.+Re/50.)));
        }
      else
        {
          Fmu[num_face] = 0.;
        }
    }

  return Fmu;
}


