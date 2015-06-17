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
// File:        Traitement_particulier_NS_canal_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_canal_VDF.h>
#include <Zone_VDF.h>
#include <Pave.h>

#include <Fluide_Incompressible.h>
#include <Navier_Stokes_std.h>
#include <Mod_turb_hyd_base.h>

Implemente_instanciable(Traitement_particulier_NS_canal_VDF,"Traitement_particulier_NS_canal_VDF",Traitement_particulier_NS_canal);


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
Sortie& Traitement_particulier_NS_canal_VDF::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_canal_VDF::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_canal_VDF::lire(Entree& is)
{
  return Traitement_particulier_NS_canal::lire(is);
}

void Traitement_particulier_NS_canal_VDF::remplir_Y(DoubleVect& tab_Y,  DoubleVect& tab_compt, int& nNy) const
{
  // On va initialiser les differents parametres membres de la classe
  // utiles au calcul des differentes moyennes
  // Initialisation de : Y, compt

  const Zone_dis_base& zdisbase = mon_equation->inconnue().zone_dis_base();
  const Zone_VF& zone_VF = ref_cast(Zone_VF, zdisbase);
  const DoubleTab& xp = zone_VF.xp();
  int nb_elems = zone_VF.zone().nb_elem();
  int num_elem,j,indic,trouve;
  double y;

  j=0;
  indic = 0;

  tab_Y.resize(1);
  tab_compt.resize(1);
  tab_Y = -100.;
  tab_compt = 0;

  //Remplissage du tableau Y
  ////////////////////////////////////////////////////////

  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      y = xp(num_elem,1);
      trouve = 0;

      for (j=0; j<indic+1; j++)
        {
          if(est_egal(y,tab_Y[j]))
            {
              tab_compt[j] ++;
              j=indic+1;
              trouve = 1;
            }
        }
      if (trouve==0)
        {
          tab_Y[indic]=y;
          tab_compt[indic] ++;
          indic++;

          tab_Y.resize(indic+1);
          tab_Y(indic)=-100.;
          tab_compt.resize(indic+1);
        }
    }

  nNy = indic;

  tab_Y.resize(nNy);
  tab_compt.resize(nNy);
}

void Traitement_particulier_NS_canal_VDF::calculer_moyenne_spatiale_vitesse_rho_mu(DoubleTab& val_moy) const
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp = zone_VDF.xp();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  double y,u,v,wl;
  int nb_elems = zone_VDF.zone().nb_elem();
  int num_elem,i;
  int face_x_0,face_y_0,face_y_1,face_z_0;


  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,mon_equation->milieu());
  const DoubleTab& visco_dyn = le_fluide.viscosite_dynamique();
  const DoubleTab& tab_rho_elem = le_fluide.masse_volumique();
  int taille_mu=visco_dyn.dimension(0);
  int taille_rho=tab_rho_elem.dimension(0);

  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      y=xp(num_elem,1);

      face_x_0 = elem_faces(num_elem,0);
      //      face_x_1 = elem_faces(num_elem,dimension);
      face_y_0 = elem_faces(num_elem,1);
      face_y_1 = elem_faces(num_elem,1+dimension);

      // PQ : 12/10 : pour eviter de moyenner localement la vitesse u et w
      //              on fait le choix de "deplacer" celles-ci
      //              du centre de la face au centre de l'element
      //              en se basant sur le principe que l'ecoulement est homogene
      //                    suivant les plans xz
      //                    Pour v, la moyenne s'impose si l'on veut revenir au centre des elements

      //  u = .5*(vitesse[face_x_0]+vitesse[face_x_1]);
      u = vitesse[face_x_0];
      v = .5*(vitesse[face_y_0]+vitesse[face_y_1]);

      for (i=0; i<Ny; i++)
        if(est_egal(y,Y[i])) break;

      val_moy(i,0) += u;
      val_moy(i,1) += v;
      val_moy(i,3) += u*u;
      val_moy(i,4) += v*v;
      val_moy(i,6) += u*v;

      if(dimension==2)   val_moy(i,9) += sqrt(u*u);      //vitesse tangentielle pour calcul du frottement

      if(dimension==3)
        {
          face_z_0 = elem_faces(num_elem,2);
          //  face_z_1 = elem_faces(num_elem,2+dimension);

          //     w = .5*(vitesse[face_z_0]+vitesse[face_z_1]);
          wl = vitesse[face_z_0];

          val_moy(i,2) += wl;
          val_moy(i,5) += wl*wl;
          val_moy(i,7) += u*wl;
          val_moy(i,8) += v*wl;
          val_moy(i,9) += sqrt(u*u+wl*wl);      //vitesse tangentielle pour calcul du frottement
        }

      if (taille_rho==1)  val_moy(i,10) += tab_rho_elem(0,0);
      else                val_moy(i,10) += tab_rho_elem[num_elem];


      if (taille_mu==1)   val_moy(i,11) += visco_dyn(0,0);
      else                val_moy(i,11) += visco_dyn[num_elem];
    }
}

void Traitement_particulier_NS_canal_VDF::calculer_moyenne_spatiale_nut(DoubleTab& val_moy) const
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp = zone_VDF.xp();
  const RefObjU& modele_turbulence = mon_equation.valeur().get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const DoubleTab& nu_t = mod_turb.viscosite_turbulente()->valeurs();

  int nb_elems = zone_VDF.zone().nb_elem();
  int num_elem,i;
  double y;

  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      y=xp(num_elem,1);

      for (i=0; i<Ny; i++)
        if(est_egal(y,Y[i])) break;

      val_moy(i,12) += nu_t[num_elem];
    }
}

void Traitement_particulier_NS_canal_VDF::calculer_moyenne_spatiale_Temp(DoubleTab& val_moy) const
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp = zone_VDF.xp();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleTab& temperature = Temp.valeur().valeurs();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  double y,u,v,wl,T;
  int nb_elems = zone_VDF.zone().nb_elem();
  int num_elem,i;
  int face_x_0,face_x_1,face_y_0,face_y_1,face_z_0,face_z_1;

  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      y=xp(num_elem,1);

      T = temperature[num_elem];

      face_x_0 = elem_faces(num_elem,0);
      face_x_1 = elem_faces(num_elem,dimension);
      face_y_0 = elem_faces(num_elem,1);
      face_y_1 = elem_faces(num_elem,1+dimension);

      u = .5*(vitesse[face_x_0]+vitesse[face_x_1]);
      v = .5*(vitesse[face_y_0]+vitesse[face_y_1]);

      for (i=0; i<Ny; i++)
        if(est_egal(y,Y[i])) break;

      val_moy(i,13) += T;
      val_moy(i,14) += T*T;
      val_moy(i,15) += u*T;
      val_moy(i,16) += v*T;

      if(Objet_U::dimension==3)
        {
          face_z_0 = elem_faces(num_elem,2);
          face_z_1 = elem_faces(num_elem,2+dimension);

          wl = .5*(vitesse[face_z_0]+vitesse[face_z_1]);

          val_moy(i,17) += wl*T;
        }
    }
}

