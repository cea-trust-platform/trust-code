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
// File:        Source_Robin.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     Version:
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Robin.h>
#include <Zone_dis.h>
#include <Zone_Cl_dis.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Equation_base.h>
#include <Fluide_Incompressible.h>
#include <distances_VEF.h>
#include <Navier_Stokes_Turbulent.h>
#include <SFichier.h>
#include <Paroi_decalee_Robin.h>
#include <Paroi_std_hyd_VEF.h>

Implemente_instanciable(Source_Robin,"Source_Robin_VEF_P1NC",Source_base);


// printOn
Sortie& Source_Robin::printOn(Sortie& s) const
{
  return s << que_suis_je();
}


// readOn
Entree& Source_Robin::readOn(Entree& s)
{
//  s >> noms_parois >> dt_post;
  s >> noms_parois ;
  return s;
}

// associer_pb
void Source_Robin::associer_pb(const Probleme_base& pb)
{
}


// completer
void Source_Robin::completer()
{
  Source_base::completer();
  Cerr << "Dans Source_Robin::completer()" << finl;
  const int nb_faces_bord = la_zone_VEF->nb_faces_bord();
  tab_u_star_.resize(nb_faces_bord);
  Cerr << "nb_faces_bord = " << nb_faces_bord << finl;
}


// ajouter
DoubleTab& Source_Robin::ajouter(DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF             = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF       = la_zone_Cl_VEF.valeur();
  const Navier_Stokes_Turbulent& eq_ns = ref_cast(Navier_Stokes_Turbulent,equation());
  const DoubleTab& cisaillement        = eq_ns.modele_turbulence().loi_paroi().valeur().Cisaillement_paroi();
//  const DoubleVect& u_star             = eq_ns.modele_turbulence().loi_paroi().valeur().tab_u_star();
//  double temps = mon_equation->inconnue().temps();
//  static double temps_dernier_post = -1;
//  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,equation().milieu());
//  double nu = fluide.viscosite_cinematique().valeurs()(0,0);
//  double utaumoy = 0.;
//  int compt = 0;

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Paroi_decalee_Robin,la_cl.valeur()))
        {
          ArrOfDouble acc_loc_tot(dimension);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();

          for (int face=ndeb; face<nfin; face++)
            {
              int elem = zone_VEF.face_voisins(face,0);
              if (elem==-1) elem = zone_VEF.face_voisins(face,1);

              for (int compo=0; compo<dimension; compo++)
                {
                  resu(face,compo) -= cisaillement(face,compo)*zone_VEF.face_surfaces(face);
                }

//              utaumoy += u_star(face);
//              compt += 1;
            }
        }
    }

//  utaumoy = mp_sum(utaumoy);
//  compt = mp_sum(compt);

// NB: desormais fait dans src/ThHyd/Turbulence/Traitement_particulier_NS_canal.cpp avec la bonne valeur de h
//  if (je_suis_maitre())
//    {
//      if (dabs(temps-temps_dernier_post)>=dt_post)
//        {
//          SFichier fic1("u_tau_robin_old.dat",ios::app);
//          fic1 << temps << "\t" << utaumoy/compt << finl;
//          fic1<<flush;
//          fic1.close();
//          SFichier fic2("reynolds_tau_robin_old.dat",ios::app);
//          fic2 << temps << "\t" << utaumoy/compt/nu << finl;	// Retau valable pour h=1
//          fic2<<flush;
//          fic2.close();
//          temps_dernier_post = temps;
//        }
//    }

  return resu;
}


// calculer
DoubleTab& Source_Robin::calculer(DoubleTab& resu) const
{
  resu = 0;
  ajouter(resu);
  return resu;
}


// associer_zones
void Source_Robin::associer_zones(const Zone_dis& z, const Zone_Cl_dis& zcl)
{
  la_zone_VEF = ref_cast(Zone_VEF,z.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF,zcl.valeur());
}
