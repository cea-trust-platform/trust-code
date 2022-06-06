/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Traitement_particulier_NS_canal_VEF.h>
#include <Navier_Stokes_std.h>
#include <Mod_turb_hyd_base.h>
#include <Ref_Equation_base.h>
#include <Zone_VEF.h>
#include <Milieu_base.h>
#include <Fluide_base.h>

Implemente_instanciable(Traitement_particulier_NS_canal_VEF,"Traitement_particulier_NS_CANAL_VEF",Traitement_particulier_NS_canal);


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
Sortie& Traitement_particulier_NS_canal_VEF::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_canal_VEF::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_canal_VEF::lire(Entree& is)
{
  return Traitement_particulier_NS_canal::lire(is);
}

void Traitement_particulier_NS_canal_VEF::remplir_Y(DoubleVect& tabY,  DoubleVect& tabcompt, int& aNy) const
{
  // On va initialiser les differents parametres membres de la classe
  // utiles au calcul des differentes moyennes
  // Initialisation de : Y, compt

  const Zone_dis_base& zdisbase = mon_equation->inconnue().zone_dis_base();
  const Zone_VF& zone_VF=ref_cast(Zone_VF, zdisbase);
  const DoubleTab& xv = zone_VF.xv();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const ArrOfInt& faces_doubles = zone_VEF.faces_doubles();
  int nb_faces = zone_VF.nb_faces();
  int num_face,j,indic,trouve;
  double y;

  j=0;
  indic = 0;

  tabY.resize(200);
  tabcompt.resize(200);

  tabY = -100.;
  tabcompt = 0;

  //Remplissage du tableau Y
  ////////////////////////////////////////////////////////

  // GF PQ. on ajoute 2 a compt dans le cas des faces standards, 1 pour les faces doubles
  // et on divise compt a la fin par 2.
  // pour etre coherent sur le nombre de faces vis a vis de calculer_moyenne_spatiale_vitesse_rho_mu

  for (num_face=0; num_face<nb_faces; num_face++)
    {
      int c = (faces_doubles[num_face]==1) ? 1 : 2 ;
      y = xv(num_face,1);
      trouve = 0;

      for (j=0; j<indic+1; j++)
        {
          if(est_egal(y,tabY[j]))
            {
              tabcompt[j] +=c;
              j=indic+1;
              trouve = 1;
              break;
            }
        }
      if (trouve==0)
        {
          tabY[indic]=y;
          tabcompt[indic] +=c;
          indic++;

          tabY.resize(indic+1);
          tabY(indic)=-100.;
          tabcompt.resize(indic+1);
        }
    }

  aNy = indic;

  tabY.resize(aNy);
  tabcompt.resize(aNy);
  tabcompt/=2;
}


void Traitement_particulier_NS_canal_VEF::calculer_moyenne_spatiale_vitesse_rho_mu(DoubleTab& val_moy) const
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleTab& xv = zone_VEF.xv();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  double y,u,v,wl;
  int nb_faces = zone_VEF.nb_faces();
  int num_face,i,elem0,elem1;
  const ArrOfInt& faces_doubles = zone_VEF.faces_doubles();
  double c;

  const Fluide_base& le_fluide = ref_cast(Fluide_base,mon_equation->milieu());
  const DoubleTab& visco_dyn = le_fluide.viscosite_dynamique()->valeurs();
  const DoubleTab& tab_rho_face = le_fluide.masse_volumique().valeurs();
  int taille_mu=visco_dyn.dimension(0);
  int taille_rho=tab_rho_face.dimension(0);

  if (visco_dyn.nb_dim()>2)
    {
      Cerr<<"Error in Traitement_particulier_NS_canal_VEF::calculer_moyenne_spatiale_vitesse_rho_mu()"<<finl;
      Process::exit();
    }

  for (num_face=0; num_face<nb_faces; num_face++)
    {
      c = (faces_doubles[num_face]==1) ? 0.5 : 1. ;

      y=xv(num_face,1);

      u = vitesse(num_face,0);
      v = vitesse(num_face,1);

      for (i=0; i<Ny; i++)
        if(est_egal(y,Y[i])) break;

      val_moy(i,0) += c*u;
      val_moy(i,1) += c*v;
      val_moy(i,3) += c*u*u;
      val_moy(i,4) += c*v*v;
      val_moy(i,6) += c*u*v;

      if(dimension==2)   val_moy(i,9) += c*sqrt(u*u);      //vitesse tangentielle pour calcul du frottement

      if(dimension==3)
        {

          wl = vitesse(num_face,2);

          val_moy(i,2) += c*wl;
          val_moy(i,5) += c*wl*wl;
          val_moy(i,7) += c*u*wl;
          val_moy(i,8) += c*v*wl;
          val_moy(i,9) += c*sqrt(u*u+wl*wl);      //vitesse tangentielle pour calcul du frottement
        }


      if (taille_rho==1)  val_moy(i,10) += c*tab_rho_face(0,0);
      else                val_moy(i,10) += c*tab_rho_face[num_face];



      elem0 = face_voisins(num_face,0);
      elem1 = face_voisins(num_face,1);

      if (taille_mu==1)
        val_moy(i,11) += c*visco_dyn(0,0);
      else
        {
          if (elem1!=-1)
            {
              val_moy(i,11) += c*0.5*(visco_dyn(elem0,0)+visco_dyn(elem1,0));
            }
          else
            {
              val_moy(i,11) += c*visco_dyn(elem0,0);
            }
        }
    }
}

void Traitement_particulier_NS_canal_VEF::calculer_moyenne_spatiale_nut(DoubleTab& val_moy) const
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleTab& xv = zone_VEF.xv();
  const Equation_base& N_S_Turb  = mon_equation.valeur();
  const RefObjU& modele_turbulence_hydr = N_S_Turb.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const DoubleTab& nu_t = le_modele.viscosite_turbulente().valeurs();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  double y;
  int nb_faces = zone_VEF.nb_faces();
  int num_face,i,elem0,elem1;
  const ArrOfInt& faces_doubles = zone_VEF.faces_doubles();
  double c;

  for (num_face=0; num_face<nb_faces; num_face++)
    {
      c = (faces_doubles[num_face]==1) ? 0.5 : 1. ;

      y=xv(num_face,1);

      for (i=0; i<Ny; i++)
        if(est_egal(y,Y[i])) break;

      elem0 = face_voisins(num_face,0);
      elem1 = face_voisins(num_face,1);

      if (elem1!=-1)
        {
          val_moy(i,12) += c*0.5*(nu_t[elem0]+nu_t[elem1]);
        }
      else
        {
          val_moy(i,12) += c*nu_t[elem0];
        }
    }
}

void Traitement_particulier_NS_canal_VEF::calculer_moyenne_spatiale_Temp(DoubleTab& val_moy) const
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleTab& xv = zone_VEF.xv();
  const DoubleTab& temperature = Temp.valeur().valeurs();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  double y,u,v,wl,T;
  int nb_faces = zone_VEF.nb_faces();
  int num_face,i;
  const ArrOfInt& faces_doubles = zone_VEF.faces_doubles();
  double c;


  for (num_face=0; num_face<nb_faces; num_face++)
    {
      c = (faces_doubles[num_face]==1) ? 0.5 : 1. ;

      y=xv(num_face,1);

      T = temperature[num_face];
      u = vitesse(num_face,0);
      v = vitesse(num_face,1);

      for (i=0; i<Ny; i++)
        if(est_egal(y,Y[i])) break;

      val_moy(i,13) += c*T;
      val_moy(i,14) += c*T*T;
      val_moy(i,15) += c*u*T;
      val_moy(i,16) += c*v*T;

      if(Objet_U::dimension==3)
        {
          wl = vitesse(num_face,2);

          val_moy(i,17) += c*wl*T;
        }
    }
}
