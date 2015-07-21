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
// File:        Turbulence_hyd_sous_maille_Smago_filtre_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_Smago_filtre_VEF.h>
#include <Champ_P1NC.h>
#include <Zone_VEF.h>

Implemente_instanciable(Turbulence_hyd_sous_maille_Smago_filtre_VEF,"Modele_turbulence_hyd_sous_maille_Smago_filtre_VEF",Turbulence_hyd_sous_maille_Smago_VEF);

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_Smago_filtre_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Turbulence_hyd_sous_maille_Smago_filtre_VEF::readOn(Entree& s )
{
  return Turbulence_hyd_sous_maille_Smago_VEF::readOn(s) ;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_Smago_filtre_VEF
//
//////////////////////////////////////////////////////////////////////////////


// PQ:07/09/05
//Champ_Fonc& Turbulence_hyd_sous_maille_Smago_filtre_VEF::calculer_energie_cinetique_turb(const DoubleTab& SMA,const DoubleVect& l)
//{
//  double C2 = 0.43;
//  double temps = mon_equation->inconnue().temps();
//  DoubleVect& k = energie_cinetique_turb_.valeurs();
//  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
//  const int nb_elem = zone_VEF.nb_elem();
//  double temp;
//
//  if (k.size() != nb_elem)
//    {
//      Cerr << "erreur dans la taille du DoubleVect valeurs de l'energie cinetique turbulente" << finl;
//      exit();
//    }
//
//  for (int elem=0 ; elem<nb_elem; elem++)
//    {
//      temp = 0.;
//      for (int i=0 ; i<dimension ; i++)
//        for (int j=0 ; j<dimension ; j++)
//          temp+=SMA(elem,i,j)*SMA(elem,i,j);
//
//      k(elem)=C2*C2*l(elem)*l(elem)*2*temp;
//    }
//
//  energie_cinetique_turb_.changer_temps(temps);
//  return energie_cinetique_turb_;
//}
//

void Turbulence_hyd_sous_maille_Smago_filtre_VEF::calculer_S_barre()
{
  const DoubleTab& la_vitesse = mon_equation->inconnue().valeurs();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem();

  const DoubleVect& vol = zone_VEF.volumes();
  //  const DoubleTab& xgravite = zone_VEF.xp();
  const Zone& zone=zone_VEF.zone();
  //  const DoubleTab& xsom = zone.domaine().coord_sommets();
  //  int nb_som_elem=zone.nb_som_elem();
  int nb_faces_elem=zone.nb_faces_elem();
  //  const IntTab& elem_som=zone.les_elems();

  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  //  const int nb_face_bord = zone_VEF.nb_faces_bord();
  const int nb_face = zone_VEF.nb_faces();

  int i,elem;
  int fac=0;


  //////////////////////////////
  //Filtrage du champ de vitesse
  //////////////////////////////
  DoubleTab vitesse(la_vitesse);

  for (; fac<nb_face ; fac++)
    {
      int num1;
      num1 = face_voisins(fac,0);
      int num2;
      num2 = face_voisins(fac,1);

      //DoubleVect vit(dimension);

      int fac1, fac2, facel;

      vitesse = 0. ;

      for(facel=0; facel<nb_faces_elem; facel++)
        {
          fac1 = elem_faces(num1,facel) ;
          for (i=0 ; i<dimension ; i++)
            ////vit(i) += la_vitesse(fac1,i)/double(2*nb_faces_elem) ;
            //Correction pour avoir le champ de vitesse par face
            vitesse(fac,i) += la_vitesse(fac1,i)/double(2*nb_faces_elem) ;
          fac2 = elem_faces(num2,facel) ;
          for (i=0 ; i<dimension ; i++)
            ////vit(i) += la_vitesse(fac2,i)/double(2*nb_faces_elem) ;
            vitesse(fac,i) += la_vitesse(fac2,i)/double(2*nb_faces_elem) ;
        }

    }

  Champ_P1NC::calcul_S_barre(vitesse,SMA_barre,zone_Cl_VEF);

  // On recalcule la longueur caracteristique de l'element

  for (elem=0; elem<nb_elem; elem ++)
    {
      double voltot = vol(elem) ;
      double eldif = 1. ;
      int num1, facel ;

      for(facel=0; facel<nb_faces_elem; facel++)
        {
          fac = elem_faces(elem,facel) ;
          num1 = face_voisins(fac,0);
          if (num1 == elem) num1 = face_voisins(fac,1);

          if (num1 != -1)
            {
              voltot+= vol(num1) ;
              eldif += 1. ;
            }
        }

      voltot /= eldif ;
      l_(elem) = 2.0*pow(6.*voltot,1./double(dimension)) ;
    }

}
