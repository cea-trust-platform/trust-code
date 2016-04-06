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
// File:        Traitement_particulier_NS_THI_thermo_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_THI_thermo_VDF.h>
#include <Zone_VDF.h>
#include <Navier_Stokes_std.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Chaleur_QC.h>
#include <Probleme_base.h>
#include <SFichier.h>

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
Sortie& Traitement_particulier_NS_THI_thermo_VDF::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_THI_thermo_VDF::readOn(Entree& is)
{
  return is;
}




Implemente_instanciable(Traitement_particulier_NS_THI_thermo_VDF,"Traitement_particulier_NS_THI_thermo_VDF",Traitement_particulier_NS_THI_VDF);


void Traitement_particulier_NS_THI_thermo_VDF::init_calc_spectre(void)
{


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
      Cerr << "Erreur dans Traitement_particulier_NS_THI_thermo_VDF::init_calc_spectre " << finl;
      Cerr << "L'equation d'energie n'a pas ete trouvee...!" << finl;
      Cerr << "On ne peut pas demander de spectres sur la temperature" << finl;
      Cerr << "si on ne resout pas une equation de la chaleur." << finl;
      Cerr << "Solution : enlever le suffixe '_thermo'." << finl;
      exit();
    }

  Traitement_particulier_NS_THI_VDF::init_calc_spectre();

}





void Traitement_particulier_NS_THI_thermo_VDF::calcul_spectre(void)
{

  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_elem = zone.nb_elem();
  //  double nb=pow(nb_elem*1.,1./3.);
  //  int nb_elem_dir = (int)(nb)+1;
  calcul_nb_elem_dir(zone);

  double temps_crt = mon_equation->inconnue().temps();
  const DoubleTab& Temp = mon_equation_NRJ.valeur().inconnue().valeurs();

  int i,k;

  ////////////////
  // Ecriture du champ de temperature dans un fichier
  // pour les calculs de spectres.
  ////////////////
  if (je_suis_maitre())
    {
      Nom fichier_temp = "chp_temperature_";
      Nom tps = Nom(temps_crt);
      fichier_temp += tps;
      SFichier fic12 (fichier_temp);
      fic12 << nb_elem_dir <<finl;
      fic12 << nb_elem_dir <<finl;
      fic12 << nb_elem_dir <<finl;

      for (k=0; k<3; k++)
        for (i=0; i<nb_elem; i++)
          fic12 << Temp(i) <<finl;

      sorties_fichiers();
    }
  Traitement_particulier_NS_THI_VDF::calcul_spectre();

}




void Traitement_particulier_NS_THI_thermo_VDF::sorties_fichiers(void)
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_elem = zone.nb_elem();
  int i;

  double temps_crt = mon_equation->inconnue().temps();
  const DoubleTab& Temp = mon_equation_NRJ.valeur().inconnue().valeurs();

  SFichier fic45("Sorties_THI_Thermo.dat",ios::app);

  //  Tmean est la temperature moyenne dans le bocal.
  //  Tmax_mean est le rapport entre la temperature maximale et la temperature moyenne.
  //  Tmin_mean ___________________________________ minimale _________________________.
  //  Tp2 est le carre de l'ecart de temperature local a la moyenne.
  //   ie : Tp = T-Tmean   et   Tp2 = Tp^2.
  // DT est l'enstrophie sur la temperature.

  double Tmean=0.,Tmax_mean=-10000.,Tmin_mean=10000.,Tp2=0.;
  double DT=0;
  for (i=0; i<nb_elem; i++)
    {
      Tmean+=Temp(i);
      if(Temp(i)>Tmax_mean) Tmax_mean=Temp(i);
      if(Temp(i)<Tmin_mean) Tmin_mean=Temp(i);
    }

  Tmean/=nb_elem;
  Tmax_mean/=Tmean;
  Tmin_mean/=Tmean;

  for (i=0; i<nb_elem; i++)
    {
      Tp2+=(Temp(i)-Tmean)*(Temp(i)-Tmean);
    }

  Tp2/=nb_elem;

  DT=calcul_enstrophie();

  fic45 << temps_crt << " " << Tmean << " " << Tmax_mean << " " << Tmin_mean << " " << Tp2 << " " << DT << finl;


  // Calcul de la PDF :

  DoubleVect PDF(50);
  PDF=1.e-10;
  int ni=50;
  double delta=(Tmax_mean-Tmin_mean)/ni;
  int k;

  Nom fichier_pdf = "PDF_";
  Nom tps = Nom(temps_crt);
  fichier_pdf += tps;
  fichier_pdf += ".dat";
  SFichier fic46 (fichier_pdf);

  for (i=0; i<nb_elem; i++)
    {
      // On ne peut pas utiliser modf (voir Schema_Temps_base::limpr) car k sert d'indice de tableau
      k=(int)(((Temp(i)/Tmean)-Tmin_mean)/delta);
      if(k>=ni) k=ni-1;
      if(k<0)  k=0;
      PDF[k]+=1.;
    }

  // On divise par le nombre d'elements
  // et par la plage d'integration
  // pour que l'integrale de la PDF soit egale a 1.

  PDF/=nb_elem;
  PDF/=delta;

  for(i=0; i<ni; i++)
    {
      fic46 << (Tmin_mean+i*delta) << " " << PDF[i] <<finl;
    }


}


double Traitement_particulier_NS_THI_thermo_VDF::calcul_enstrophie(void)
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,zdis.valeur());
  int nb_elem = zone.nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  const DoubleTab& Temp = mon_equation_NRJ.valeur().inconnue().valeurs();

  double DT=0;
  double gradT=0.;

  int face0,face1;
  int element0,element1;

  for(int elem=0; elem<nb_elem; elem++)
    {
      for(int ori=0; ori<dimension; ori++)
        {
          gradT = 0.;
          face0 = elem_faces(elem,ori);
          face1 = elem_faces(elem,ori+dimension);
          element0 = face_voisins(face0,0);
          element1 = face_voisins(face1,1);
          gradT += pow((Temp(element0)-Temp(elem))/zone_VDF.dist_elem(element0,elem,ori),2);
          gradT += pow((Temp(element1)-Temp(elem))/zone_VDF.dist_elem(element1,elem,ori),2);

          DT+=0.5*gradT;
        }
    }

  DT=0.5*DT/nb_elem;

  return DT;
}


