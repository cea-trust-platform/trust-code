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
// File:        Turbulence_hyd_sous_maille_selectif_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_selectif_VDF.h>
#include <math.h>
#include <VDF_discretisation.h>
#include <Champ_Face.h>
#include <Equation_base.h>
#include <Zone_VDF.h>

Implemente_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_selectif_VDF,"Modele_turbulence_hyd_sous_maille_selectif_VDF",Turbulence_hyd_sous_maille_VDF);

Turbulence_hyd_sous_maille_selectif_VDF::Turbulence_hyd_sous_maille_selectif_VDF()
{
  Csm1_ = CSMS1;
  Csm2_ = 1.146;  // si CSM1 = 0.112  (autre 0.162)
  //  CSM2 = 27/8*M_PI^2*CSM1^2*C_k^3
}

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_selectif_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Turbulence_hyd_sous_maille_selectif_VDF::readOn(Entree& s )
{
  return Turbulence_hyd_sous_maille_VDF::readOn(s);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//           Implementation de fonctions de la classe
//
//           Turbulence_hyd_sous_maille_selectif_VDF
//
/////////////////////////////////////////////////////////////////////////////////////

void Turbulence_hyd_sous_maille_selectif_VDF::discretiser()
{
  // Cerr << "Turbulence_hyd_sous_maille_selectif_VDF::discretiser()" << finl;
  Mod_turb_hyd_ss_maille::discretiser();
  const VDF_discretisation& dis=ref_cast(VDF_discretisation, mon_equation->discretisation());
  dis.vorticite(mon_equation->zone_dis(),mon_equation->inconnue(), la_vorticite);
}

int Turbulence_hyd_sous_maille_selectif_VDF::a_pour_Champ_Fonc(const Motcle& mot,
                                                               REF(Champ_base)& ch_ref ) const
{
  Motcles les_motcles(3);
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


void Turbulence_hyd_sous_maille_selectif_VDF::calculer_fonction_structure()
{

  Turbulence_hyd_sous_maille_VDF::calculer_fonction_structure();
  cutoff();
}

// Fonction qui permet d'appliquer un filtre sur la fonction de structure
// La fonction de structure d'un element est mise a zero si il existe une
// deviation inferieure a 20 degres entre son vecteur vorticite et le
// vecteur moyen des vorticites des 6 elements les plus proches
// La constante Sin2Angl contient le carre du sinus d'un angle de 20 degres

void Turbulence_hyd_sous_maille_selectif_VDF::cutoff()
{
  static const double Sin2Angl = SIN2ANGL;
  const Champ_Face& vitesse = ref_cast(Champ_Face,mon_equation->inconnue().valeur());
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int nb_poly = zone_VDF.zone().nb_elem();
  DoubleTab& vorticite = la_vorticite.valeurs();

  la_vorticite.mettre_a_jour(vitesse.temps());
  vorticite.echange_espace_virtuel();

  int elx0,elx1,ely0,ely1,elz0,elz1,i;
  double norme,norme_moyen,prod;
  DoubleVect vorti_moyen(3);

  for (int num_elem=0; num_elem<nb_poly; num_elem++)
    {
      elx0 = face_voisins(elem_faces(num_elem,0),0);
      elx1 = face_voisins(elem_faces(num_elem,3),1);
      ely0 = face_voisins(elem_faces(num_elem,1),0);
      ely1 = face_voisins(elem_faces(num_elem,4),1);
      elz0 = face_voisins(elem_faces(num_elem,2),0);
      elz1 = face_voisins(elem_faces(num_elem,5),1);


      double dx0=0.,dx1=0.,dy0=0.,dy1=0.,dz0=0.,dz1=0.;
      for (i=0; i<3; i++)
        {
          if (elx0 != -1)
            dx0 += zone_VDF.dist_elem_period(num_elem,elx0,i)*zone_VDF.dist_elem_period(num_elem,elx0,i);
          if (elx1 != -1)
            dx1 += zone_VDF.dist_elem_period(num_elem,elx1,i)*zone_VDF.dist_elem_period(num_elem,elx1,i);
          if (ely0 != -1)
            dy0 += zone_VDF.dist_elem_period(num_elem,ely0,i)*zone_VDF.dist_elem_period(num_elem,ely0,i);
          if (ely1 != -1)
            dy1 += zone_VDF.dist_elem_period(num_elem,ely1,i)*zone_VDF.dist_elem_period(num_elem,ely1,i);
          if (elz0 != -1)
            dz0 += zone_VDF.dist_elem_period(num_elem,elz0,i)*zone_VDF.dist_elem_period(num_elem,elz0,i);
          if (elz1 != -1)
            dz1 += zone_VDF.dist_elem_period(num_elem,elz1,i)*zone_VDF.dist_elem_period(num_elem,elz1,i);
        }

      if (dabs(dx0)>DMINFLOAT)
        dx0 = 1./sqrt(dx0);
      if (dabs(dx1)>DMINFLOAT)
        dx1 = 1./sqrt(dx1);
      if (dabs(dy0)>DMINFLOAT)
        dy0 = 1./sqrt(dy0);
      if (dabs(dy1)>DMINFLOAT)
        dy1 = 1./sqrt(dy1);
      if (dabs(dz0)>DMINFLOAT)
        dz0 = 1./sqrt(dz0);
      if (dabs(dz1)>DMINFLOAT)
        dz1 = 1./sqrt(dz1);

      if ( (elx0 != -1) && (elx1 != -1) && (ely0 != -1)
           && (ely1 != -1) && (elz0 != -1) && (elz1 != -1) )
        // Cas d'un element interne
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) =  ( dx0*vorticite(elx0,k)+dx1*vorticite(elx1,k)
                                + dy0*vorticite(ely0,k)+dy1*vorticite(ely1,k)
                                + dz0*vorticite(elz0,k)+dz1*vorticite(elz1,k) )
                              /(dx0+dx1+dy0+dy1+dz0+dz1);
        }
      else if ( (elx0 != -1) && (elx1 != -1) && (ely0 != -1)
                && (ely1 != -1) )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) =  ( dx0*vorticite(elx0,k)+dx1*vorticite(elx1,k)
                                + dy0*vorticite(ely0,k)+dy1*vorticite(ely1,k) )
                              /(dx0+dx1+dy0+dy1);
        }
      else if ( (elx0 != -1) && (elx1 != -1) && (elz0 != -1)
                && (elz1 != -1) )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) =  ( dx0*vorticite(elx0,k)+dx1*vorticite(elx1,k)
                                + dz0*vorticite(elz0,k)+dz1*vorticite(elz1,k) )
                              /(dx0+dx1+dz0+dz1);
        }
      else if ( (ely0 != -1) && (ely1 != -1) && (elz0 != -1)
                && (elz1 != -1) )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) =  ( dy0*vorticite(ely0,k)+dy1*vorticite(ely1,k)
                                + dz0*vorticite(elz0,k)+dz1*vorticite(elz1,k) )
                              /(dy0+dy1+dz0+dz1);
        }
      else if ( (elx0 != -1) && (elx1 != -1) )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) = (dx0*vorticite(elx0,k)+dx1*vorticite(elx1,k))/(dx0+dx1);
        }
      else if ( (ely0 != -1) && (ely1 != -1) )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) = (dy0*vorticite(ely0,k)+dy1*vorticite(ely1,k))/(dy0+dy1);
        }
      else if ( (elz0 != -1) && (elz1 != -1) )
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) = (dz0*vorticite(elz0,k)+dz1*vorticite(elz1,k)) /(dz0+dz1);
        }
      else  // Cas d'un element coin ; on met FS a zero
        // On rend nul le vecteur vorti_moyen(k) ce qui provoquera la mise a zero de FS
        {
          for (int k=0; k<3; k++)
            vorti_moyen(k) = 0;
        }


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

    }
  // Vu ou etait l'echange avant ca plantait des que le nbre de maille
  // etait different sur un des procs !!!!
  F2.echange_espace_virtuel();
}





