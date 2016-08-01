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
// File:        Traitement_particulier_NS_temperature_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_temperature_VEF.h>
#include <Zone_VEF.h>
#include <Champ_P1NC.h>
#include <Champ_Uniforme.h>
#include <IntTrav.h>
#include <SFichier.h>
#include <EcrFicPartage.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Chaleur_QC.h>
#include <Probleme_base.h>
#include <Fluide_Incompressible.h>
#include <Fluide_Quasi_Compressible.h>
#include <Navier_Stokes_std.h>

Implemente_instanciable(Traitement_particulier_NS_temperature_VEF,"Traitement_particulier_NS_temperature_VEF",Traitement_particulier_NS_temperature);


// Description:
//
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Traitement_particulier_NS_temperature_VEF::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Traitement_particulier_NS_temperature_VEF::readOn(Entree& is)
{
  return is;
}

void Traitement_particulier_NS_temperature_VEF::calcul_temperature()
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,mon_equation->zone_Cl_dis().valeur() );
  //  const DoubleTab& xv = zone_VEF.xv();    // centre de gravite des faces
  int nb_front=zone_VEF.nb_front_Cl();


  const Probleme_base& pb = mon_equation.valeur().probleme();
  int flag=0;

  for(int i=0; i<pb.nombre_d_equations(); i++)
    {
      if(sub_type(Convection_Diffusion_Temperature,pb.equation(i)))
        {
          mon_equation_NRJ = ref_cast(Convection_Diffusion_Temperature,pb.equation(i));
          flag=1;
        }
      else if(sub_type(Convection_Diffusion_Chaleur_QC,pb.equation(i)))
        {
          mon_equation_NRJ = ref_cast(Convection_Diffusion_Chaleur_QC,pb.equation(i));
          flag=1;
        }
    }

  if (flag==0)
    {
      Cerr << "Error : none equation for temperature was  found...!" << finl;
      Cerr << "User can not ask for Traitement_particulier_NS_temperature_VEF" << finl;
      Cerr << "if code does not solve a heat equation." << finl;
      exit();
    }

  const DoubleTab& tab_temperature = mon_equation_NRJ.valeur().inconnue().valeurs();

  const DoubleTab& vitesse = pb.get_champ(Motcle("vitesse")).valeurs();
  const Champ_Don& rho_ = pb.milieu().masse_volumique();
  const DoubleTab& rho = rho_.valeurs();
  int taille_rho=rho.dimension(0);
  const double temps = mon_equation->inconnue().temps();


  for (int n_bord=0; n_bord<nb_front; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (le_bord.le_nom()==nom_cl )
        {
          double Tmoyen=0.;
          double Tmax=-10e6;
          double Tmin=10e6;
          double rhoUS=0.;
          double tmp=0.;
          for (int iface=num1; iface<num2; iface++)
            {
              if (taille_rho==1)  tmp = rho(0,0)  *vitesse(iface,dir)*zone_VEF.face_surfaces(iface);
              else                     tmp = rho(iface)*vitesse(iface,dir)*zone_VEF.face_surfaces(iface);
              Tmoyen += tmp*tab_temperature(iface);
              rhoUS += tmp;
              if(tab_temperature(iface)>Tmax) Tmax=tab_temperature(iface);
              if(tab_temperature(iface)<Tmin) Tmin=tab_temperature(iface);
            }

          // PQ : 06/04/09 : devlpts pour le parallele
          rhoUS  = mp_sum(rhoUS);
          Tmoyen = mp_sum(Tmoyen);
          Tmin   = mp_min(Tmin);
          Tmax   = mp_max(Tmax);


          if (Process::je_suis_maitre())
            {
              if (rhoUS==0)
                {
                  Cerr << "rhoUS=0..." << finl;
                  Cerr << "May be the direction of Traitement_particulier { temperature is wrong..." << finl;
                  exit();
                }
              Tmoyen /= rhoUS;
              Nom nom_fic1("Tmoyen_");
              nom_fic1 += le_bord.le_nom();
              SFichier fic1(nom_fic1, ios::app);
              fic1 << temps << " " << Tmoyen << " " << Tmin << " " << Tmax << finl;

              Nom nom_fic2("RhoU_");
              nom_fic2 += le_bord.le_nom();
              SFichier fic2(nom_fic2, ios::app);
              fic2 << temps << " " << rhoUS << finl;
            }
        }

      /*
      /////////////////////////////////////////////////////////////////////////
      // PQ : 04/08/08 anciennement utilise pour la thermalisation des solides
      // a conserver en cas de besoin futur
      /////////////////////////////////////////////////////////////////////////

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
      {
      Nom fichier1 = "temp_face_paroi_";
      fichier1+=le_bord.le_nom();
      fichier1+=".xyz";
      EcrFicPartage fic1 (fichier1);

      fic1<<le_bord.nb_faces()<<finl;

      if(dimension==2)
      for (int num_face=num1; num_face<num2; num_face++)
      {
      fic1<<xv(num_face,0)<<" "<<xv(num_face,1)<<" "<<temperature(num_face)<<finl;
      }
      else if(dimension==3)
      for (int num_face=num1; num_face<num2; num_face++)
      {
      fic1<<xv(num_face,0)<<" "<<xv(num_face,1)<<" "<<xv(num_face,2)<<" "<<temperature(num_face)<<finl;
      }

      fic1.flush();
      fic1.syncfile();
      fic1.close();

      }
      */
    }
}
