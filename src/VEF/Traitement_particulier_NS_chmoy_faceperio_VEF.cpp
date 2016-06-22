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
// File:        Traitement_particulier_NS_chmoy_faceperio_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_chmoy_faceperio_VEF.h>
#include <Zone_VEF.h>
#include <Periodique.h>
#include <Champ_P1NC.h>
#include <Champ_Uniforme.h>
#include <IntTrav.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Chaleur_QC.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Navier_Stokes_std.h>

Implemente_instanciable(Traitement_particulier_NS_chmoy_faceperio_VEF,"Traitement_particulier_NS_chmoy_faceperio_VEF",Traitement_particulier_NS_chmoy_faceperio);


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
Sortie& Traitement_particulier_NS_chmoy_faceperio_VEF::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_chmoy_faceperio_VEF::readOn(Entree& is)
{
  return is;
}

void Traitement_particulier_NS_chmoy_faceperio_VEF::init_calcul_stats(void)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,mon_equation->zone_Cl_dis().valeur() );
  const DoubleTab& xv = zone_VEF.xv();    // centre de gravite des faces
  // Imprime dans un fichier les faces periodiques (numero+coordonnees du centre)
  int nb_front=zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_front; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          if (le_bord.nb_faces())
            {
              chmoy_faceperio.resize(le_bord.nb_faces(),Objet_U::dimension);
              chmoy_faceperio=0.;
              EcrFicPartage fic("geom_face_perio");
              fic.setf(ios::scientific);
              fic.precision(format_precision_geom);
              int nb_faces_bord = mp_sum(le_bord.nb_faces());
              if (Process::je_suis_maitre()) fic<<nb_faces_bord<<finl;
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              for (int num_face=num1; num_face<num2; num_face++)
                fic<<num_face<<" "<<xv(num_face,0)<<" "<<xv(num_face,1)<<" "<<xv(num_face,2)<<finl;
              fic.syncfile();
              fic.close();
            }
        }
    }
}

void Traitement_particulier_NS_chmoy_faceperio_VEF::calcul_chmoy_faceperio(double un_temps_deb, double temps, double dt)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,mon_equation->zone_Cl_dis().valeur() );
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();

  // Calcul de la moyenne temporelle de la vitesse sur les frontieres periodiques:
  // moy(U)=1/(temps-temps_deb)*Integrale(t=temps_deb a temps)(U(t)*dt)
  chmoy_faceperio*=(temps-un_temps_deb-dt);
  int test_proc=0;
  int nb_front=zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_front; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (int num_face=num1; num_face<num2; num_face++)
            {
              for(int i=0; i<Objet_U::dimension; i++)
                chmoy_faceperio(num_face,i)+=dt*vitesse(num_face,i);
              test_proc=1;
            }
        }
    }

  chmoy_faceperio/=(temps-un_temps_deb);

  if (test_proc)
    {
      EcrFicPartage fic("chmoy_face_perio");
      fic.setf(ios::scientific);
      fic.precision(format_precision_geom);
      for (int n_bord=0; n_bord<nb_front; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          if (sub_type(Periodique,la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              int nb_faces_bord = mp_sum(le_bord.nb_faces());
              if (Process::je_suis_maitre()) fic<<nb_faces_bord<<finl;
              for (int num_face=num1; num_face<num2; num_face++)
                fic<<num_face<<" "<<chmoy_faceperio(num_face,0)<<" "<<chmoy_faceperio(num_face,1)<<" "<<chmoy_faceperio(num_face,2)<<finl;
            }
        }
      fic.syncfile();
      fic.close();
    }
}
