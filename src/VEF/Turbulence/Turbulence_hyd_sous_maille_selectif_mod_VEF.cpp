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
// File:        Turbulence_hyd_sous_maille_selectif_mod_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_selectif_mod_VEF.h>
#include <VEF_discretisation.h>
#include <Champ_P1NC.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Zone_VEF.h>

Implemente_instanciable(Turbulence_hyd_sous_maille_selectif_mod_VEF,"Modele_turbulence_hyd_sous_maille_selectif_VEF",Turbulence_hyd_sous_maille_VEF);

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_selectif_mod_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Turbulence_hyd_sous_maille_selectif_mod_VEF::readOn(Entree& s )
{

  // PQ:12/09/05: programmation incompatible avec tableau F2 defini aux faces dans la classe mere Turbulence_hyd_sous_maille_VEF
  // A revoir

  Cerr << " Turbulence_hyd_sous_maille_selectif_mod_VEF " << finl;
  Cerr << " selective SGS model in VEF  not reliable : please, select another one " << finl;
  exit();

  if (dimension != 3)
    {
      Cerr << " Ce modele n a de sens qu'en dimension 3!!!!!!!!!" << finl;
      exit();

    }
  return Turbulence_hyd_sous_maille_VEF::readOn(s) ;
}

// Entree& Turbulence_hyd_sous_maille_selectif_mod_VEF::lire(const Motcle& motlu, Entree& is)
// {
//   if (motlu == "regression_lineaire")
//     {
//       regression = 0;
//       Cerr << "Le mot lu est :" << motlu << finl;
//       Cerr << "Vous allez utiliser la REGRESSION LINEAIRE" <<finl;
//       Cerr << "pour determiner l'angle" << finl;
//     }
//   else
//     if (motlu == "regression_log")
//       {
//         regression = 1;
//         Cerr << "Le mot lu est :" << motlu << finl;
//         Cerr << "Vous allez utiliser la REGRESSION LOG" <<finl;
//         Cerr << "pour determiner l'angle" << finl;
//       }
//     else
//       return  Turbulence_hyd_sous_maille_VEF::lire(motlu,is);
//   return is;
// }

/////////////////////////////////////////////////////////////////////////////////////
//
//           Implementation de fonctions de la classe
//
//           Turbulence_hyd_sous_maille_selectif_mod_VEF
//
/////////////////////////////////////////////////////////////////////////////////////

void Turbulence_hyd_sous_maille_selectif_mod_VEF::discretiser()
{
  // Cerr << "Turbulence_hyd_sous_maille_selectif_mod_VEF::discretiser()" << finl;
  Mod_turb_hyd_ss_maille::discretiser();
  const VEF_discretisation& dis=ref_cast(VEF_discretisation, mon_equation->discretisation());
  dis.vorticite(mon_equation->zone_dis(),mon_equation->inconnue(), la_vorticite);
}

int Turbulence_hyd_sous_maille_selectif_mod_VEF::a_pour_Champ_Fonc(const Motcle& mot,
                                                                   REF(Champ_base)& ch_ref ) const
{
  Motcles les_motcles(7);
  {
    les_motcles[0] ="viscosite_turbulente";
    les_motcles[1] ="k";
    les_motcles[2] ="vorticite";

  }
  int rang = les_motcles.search(mot);
  switch(rang)
    {
    case 0:
      {
        ch_ref = la_viscosite_turbulente.valeur();
        return 1;
      }
    case 1:
      {
        ch_ref = energie_cinetique_turb_.valeur();
        return 1;
      }
    case 2:
      {
        ch_ref = la_vorticite.valeur();
        return 1;
      }
    default:
      return 0;
    }
}

Champ_Fonc& Turbulence_hyd_sous_maille_selectif_mod_VEF::calculer_viscosite_turbulente()
{
  static const double Csm1 = 0.086;
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  DoubleVect volume = zone_VEF.volumes();
  int num_elem;

  F2.resize(nb_elem_tot);

  calculer_fonction_structure();

  if (visco_turb.size() != nb_elem)
    {
      Cerr << "erreur dans la taille du DoubleTab valeurs de la viscosite" << finl;
      exit();
    }
  visco_turb=0.;

  Debog::verifier("Turbulence_hyd_sous_maille_selectif_mod_VEF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  for (num_elem = 0; num_elem<nb_elem; num_elem++)
    visco_turb(num_elem) = Csm1*l_[num_elem]*sqrt(F2(num_elem));

  Debog::verifier("Turbulence_hyd_sous_maille_selectif_mod_VEF::calculer_viscosite_turbulente visco_turb 1",visco_turb);

  la_viscosite_turbulente.changer_temps(temps);

  return la_viscosite_turbulente;
}



void Turbulence_hyd_sous_maille_selectif_mod_VEF::calculer_fonction_structure()
{
  Turbulence_hyd_sous_maille_VEF::calculer_fonction_structure();

  // Comprendre!!!!! DEB------------------------------
  //   Turbulence_hyd_sous_maille_1elt_VEF::calculer_viscosite_turbulente(FS,delta_c);
  //   DoubleVect F2 = FS;
  // Comprendre!!!!! FIN------------------------------

  cutoff();

  // Comprendre!!!!! DEB------------------------------
  //   calculer_viscosite_turbulente(FS,delta_c);
  //   F2-=FS;
  //   for (int i=0; i< F2.size();i++)
  //     if (F2[i] < 0.)
  //       Cerr << "i=" << i << "  F2[i]" << F2[i]  << finl;
  // Comprendre!!!!! FIN------------------------------
}

// Fonction qui permet d'appliquer un filtre sur la fonction de structure
// La fonction de structure d'un element est mise a zero si il existe une
// deviation inferieure a N degres entre son vecteur vorticite et le
// vecteur moyen des vorticites des 6 elements les plus proches
// l angle de coupure varie en fonction du delta c (regression log)



void Turbulence_hyd_sous_maille_selectif_mod_VEF::cutoff()
{
  //  static const double Sin2Angl = SIN2ANGL_new2;
  double Sin2Angl;
  const Champ_P1NC& vitesse = ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  //  const Zone& zone = zone_VEF.zone();
  //  int nfac = zone.nb_faces_elem();
  //  int nfac = 4; // en 3D 4 faces!!!
  DoubleTab& vorticite = la_vorticite.valeurs();
  // const DoubleTab& xp = zone_VEF.xp();

  la_vorticite.mettre_a_jour(vitesse.temps());
  vorticite.echange_espace_virtuel();

  int el0,el1,el2,el3;
  double norme,norme_moyen,prod,angle,delta;
  DoubleVect vorti_moyen(3);


  for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      delta = l_[num_elem];
      angle = 0.494*pow(delta,0.4075);
      Sin2Angl = sin(angle);
      Sin2Angl *= Sin2Angl;

      el0 = face_voisins(elem_faces(num_elem,0),0);
      if (el0 == num_elem)
        el0 = face_voisins(elem_faces(num_elem,0),1);
      el1 = face_voisins(elem_faces(num_elem,1),0);
      if (el1 == num_elem)
        el1 = face_voisins(elem_faces(num_elem,1),1);
      el2 = face_voisins(elem_faces(num_elem,2),0);
      if (el2 == num_elem)
        el2 = face_voisins(elem_faces(num_elem,2),1);
      el3 = face_voisins(elem_faces(num_elem,3),0);
      if (el3 == num_elem)
        el3 = face_voisins(elem_faces(num_elem,3),1);

      //      double d0,d1,d2,d3,d;
      // double x,y,z;
      // x = xp(num_elem,0);
      // y = xp(num_elem,1);
      // z = xp(num_elem,2);

      if ( (el0 != -1) && (el1 != -1) && (el2 != -1)
           && (el3 != -1)  )
        // Cas d'un element interne
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) =  ( vorticite(el0,k)+vorticite(el1,k)
                                + vorticite(el2,k)+vorticite(el3,k) );
        }
      else if ( (el0 != -1) && (el1 != -1) && (el2 != -1)  )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) =  ( vorticite(el0,k)+vorticite(el1,k)
                                + vorticite(el2,k) );
        }
      else if ( (el0 != -1) && (el1 != -1)  )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) =  ( vorticite(el0,k)+vorticite(el1,k));
        }
      else  // Cas d'un element coin ; on met FS a zero
        // On rend nul le vecteur vorti_moyen(k) ce qui provoquera la mise a zero de FS
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) = 0;
        }

      //      if ( (el0 != -1) && (el1 != -1) && (el2 != -1)
      //            && (el3 != -1)  )
      //         // Cas d'un element interne
      //         {
      //           d = 0.;
      //           d0 = (x-xp(el0,0))*(x-xp(el0,0))+(y-xp(el0,1))*(y-xp(el0,1))+(z-xp(el0,2))*(z-xp(el0,2));
      //           d0 = sqrt(d0);
      //           d += d0;
      //           d1 = (x-xp(el1,0))*(x-xp(el1,0))+(y-xp(el1,1))*(y-xp(el1,1))+(z-xp(el1,2))*(z-xp(el1,2));
      //           d1 = sqrt(d1);
      //           d += d1;
      //           d2 = (x-xp(el2,0))*(x-xp(el2,0))+(y-xp(el2,1))*(y-xp(el2,1))+(z-xp(el2,2))*(z-xp(el2,2));
      //           d2 = sqrt(d2);
      //           d += d2;
      //           d3 = (x-xp(el3,0))*(x-xp(el3,0))+(y-xp(el3,1))*(y-xp(el3,1))+(z-xp(el3,2))*(z-xp(el3,2));
      //           d3 = sqrt(d3);
      //           d += d3;

      //           for (int k=0; k<3; k++)
      //             vorti_moyen(k) =  ( (d-d0)*vorticite(el0,k)+(d-d1)*vorticite(el1,k)
      //                                 + (d-d2)*vorticite(el2,k)+(d-d3)*vorticite(el3,k) )/d/3.;
      //         }
      //       else if ( (el0 != -1) && (el1 != -1) && (el2 != -1)  )
      //         {
      //           d = 0.;
      //           d0 = (x-xp(el0,0))*(x-xp(el0,0))+(y-xp(el0,1))*(y-xp(el0,1))+(z-xp(el0,2))*(z-xp(el0,2));
      //           d0 = sqrt(d0);
      //           d += d0;
      //           d1 = (x-xp(el1,0))*(x-xp(el1,0))+(y-xp(el1,1))*(y-xp(el1,1))+(z-xp(el1,2))*(z-xp(el1,2));
      //           d1 = sqrt(d1);
      //           d += d1;
      //           d2 = (x-xp(el2,0))*(x-xp(el2,0))+(y-xp(el2,1))*(y-xp(el2,1))+(z-xp(el2,2))*(z-xp(el2,2));
      //           d2 = sqrt(d2);
      //           d += d2;
      //           for (int k=0; k<3; k++)
      //             vorti_moyen(k) =  ( (d-d0)*vorticite(el0,k)+(d-d1)*vorticite(el1,k)
      //                                 + (d-d2)*vorticite(el2,k) )/d/2.;
      //         }
      //       else if ( (el0 != -1) && (el1 != -1)  )
      //         {
      //           d = 0.;
      //           d0 = (x-xp(el0,0))*(x-xp(el0,0))+(y-xp(el0,1))*(y-xp(el0,1))+(z-xp(el0,2))*(z-xp(el0,2));
      //           d0 = sqrt(d0);
      //           d += d0;
      //           d1 = (x-xp(el1,0))*(x-xp(el1,0))+(y-xp(el1,1))*(y-xp(el1,1))+(z-xp(el1,2))*(z-xp(el1,2));
      //           d1 = sqrt(d1);
      //           d += d1;
      //           for (int k=0; k<3; k++)
      //             vorti_moyen(k) =  ( (d-d0)*vorticite(el0,k)+(d-d1)*vorticite(el1,k))/d;
      //         }
      //       else  // Cas d'un element coin ; on met FS a zero
      //         // On rend nul le vecteur vorti_moyen(k) ce qui provoquera la mise a zero de FS
      //         {
      //           for (int k=0; k<3; k++)
      //             vorti_moyen(k) = 0;
      //         }


      // Calcul du produit vectoriel entre la vorticite dans l'element
      // et le vecteur des vorticites des elements voisins

      norme = 0;
      int k;
      for (k=0; k<3; k++)
        norme += carre(vorticite(num_elem,k));

      norme_moyen = 0;
      for (k=0; k<3; k++)
        norme_moyen += carre(vorti_moyen(k));

      if ( (norme > 1.e-10) && (norme_moyen > 1.e-10 ) )
        {
          prod = carre(vorti_moyen(1)*vorticite(num_elem,2)-vorti_moyen(2)*vorticite(num_elem,1))
                 +  carre(vorti_moyen(2)*vorticite(num_elem,0)-vorti_moyen(0)*vorticite(num_elem,2))
                 +  carre(vorti_moyen(0)*vorticite(num_elem,1)-vorti_moyen(1)*vorticite(num_elem,0));
          prod /= (norme*norme_moyen);

          if (prod <= Sin2Angl)
            F2(num_elem) = 0;
        }
      else // bruit numerique ou element de coin
        F2(num_elem) = 0;
      F2.echange_espace_virtuel();

    }
}

//Champ_Fonc& Turbulence_hyd_sous_maille_selectif_mod_VEF::calculer_energie_cinetique_turb()
//{
//  //  static const double Csms2 = 1.8764;  // si CSMS1 = 0.143
//  //  static const double Csms2 = 0.676;  // si CSMS1 = 0.087 pour 64^3
//    static const double Csms2 = 0.6703;  // si CSMS1 = 0.086 pour 32^3
//  //  CSMS2 = 27/8*M_PI^2*CSMS1^2*C_k^3
//  double temps = mon_equation->inconnue().temps();
//  DoubleVect& k = energie_cinetique_turb_.valeurs();
//  int nb_poly = la_zone_VEF->zone().nb_elem();
//
//  if (k.size() != nb_poly) {
//    Cerr << "erreur dans la taille du DoubleVect valeurs de l'energie cinetique turbulente" << finl;
//    exit();
//  }
//
//  for (int elem=0; elem<nb_poly; elem++)
//    k[elem] = Csms2*F2[elem];
//
//  Cerr << "On est passe dans Turbulence_hyd_sous_maille_selectif_mod_VEF::calculer_energie_cinetique_turb" <<  finl;
//
//  energie_cinetique_turb_.changer_temps(temps);
//  return energie_cinetique_turb_;
//}





