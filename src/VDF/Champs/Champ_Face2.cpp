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
// File:        Champ_Face2.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Face.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>

// Fonctions de calcul des composantes du tenseur GradU
// (derivees covariantes de la vitesse)
// dans le repere des coordonnees cylindriques

void Champ_Face::calculer_dercov_axi(const Zone_Cl_VDF& zone_Cl_VDF)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleTab& inco = valeurs();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const DoubleTab& xv = zone_VDF.xv();
  const DoubleTab& xp = zone_VDF.xp();
  const IntVect& type_arete_bord = zone_Cl_VDF.type_arete_bord();

  double d_teta,R;
  double deux_pi = M_PI*2.0;

  // Remplissage de tau_diag_ : termes diagonaux du tenseur GradU

  int fx0,fx1,fy0,fy1;
  int num_elem;
  for (num_elem=0; num_elem<zone_VDF.nb_elem(); num_elem++)
    {
      fx0 = elem_faces(num_elem,0);
      fx1 = elem_faces(num_elem,dimension);
      fy0 = elem_faces(num_elem,1);
      fy1 = elem_faces(num_elem,1+dimension);

      // Calcul de tau11
      tau_diag_(num_elem,0) = (inco[fx1]-inco[fx0])/(xv(fx1,0) - xv(fx0,0));

      // Calcul de tau22
      R = xp(num_elem,0);
      d_teta = xv(fy1,1) - xv(fy0,1);
      if (d_teta < 0)
        d_teta += deux_pi;
      tau_diag_(num_elem,1) =     (inco[fy1]-inco[fy0])/(R*d_teta)
                                  + 0.5*(inco[fx0]+inco[fx1])/R;

    }

  if (dimension == 3)
    {
      int fz0,fz1;
      for (num_elem=0; num_elem<zone_VDF.nb_elem(); num_elem++)
        {
          fz0 = elem_faces(num_elem,2);
          fz1 = elem_faces(num_elem,2+dimension);

          // Calcul de tau33
          tau_diag_(num_elem,2) = (inco[fz1]-inco[fz0])/(xv(fz1,2) - xv(fz0,2));

        }
    }

  // Remplissage de tau_croises_ : termes extradiagonaux du tenseur GradU
  // Les derivees croisees de la vitesse (termes extradiagonaux du tenseur
  // GradU) sont calculees sur les aretes.
  // Il y a deux derivees par arete:
  //    Pour une arete XY : tau12 et tau21
  //    Pour une arete YZ : tau23 et tau32
  //    Pour une arete XZ : tau13 et tau31

  // Boucle sur les aretes bord

  int n_arete;
  int ndeb = zone_VDF.premiere_arete_bord();
  int nfin = ndeb + zone_VDF.nb_aretes_bord();
  int ori1,ori3;
  int fac1,fac2,fac3,fac4,signe;
  double dist3;

  int n_type;

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      n_type=type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI :
          // paroi-paroi
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE:
          // paroi-fluide
          {
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            signe  = Qdm(n_arete,3);
            ori1 = orientation(fac1);
            ori3 = orientation(fac3);
            int rang1 = fac1 - zone_VDF.premiere_face_bord();
            int rang2 = fac2 - zone_VDF.premiere_face_bord();
            double vit_imp;

            if (n_type == TypeAreteBordVDF::PAROI_FLUIDE)
              // arete paroi_fluide :il faut determiner qui est la face fluide
              {
                if (est_egal(inco[fac1],0))
                  vit_imp = val_imp_face_bord(rang2,ori3);
                else
                  vit_imp = val_imp_face_bord(rang1,ori3);
              }
            else
              vit_imp = 0.5*(val_imp_face_bord(rang1,ori3)+
                             val_imp_face_bord(rang2,ori3));

            if (ori1 == 0) // bord d'equation R = cte
              {
                dist3 = xv(fac3,0) - xv(fac1,0);
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 2;

                if (ori3 == 1)
                  {
                    // calcul de tau12
                    tau_croises_(n_arete,0) = signe*(vit_imp-inco[fac3])/dist3;

                    // calcul de tau21
                    R = xv(fac1,0);
                    d_teta = xv(fac2,1) - xv(fac1,1);
                    if  (d_teta < 0)
                      d_teta += deux_pi;
                    tau_croises_(n_arete,1) = (inco[fac2]-inco[fac1])/(R*d_teta);
                  }
                else if (ori3 == 2)
                  {
                    // calcul de tau13
                    tau_croises_(n_arete,0) = signe*(vit_imp-inco[fac3])/dist3;

                    // calcul de tau31
                    tau_croises_(n_arete,1) = (inco[fac2]-inco[fac1])/(xv(fac2,2)-xv(fac1,2));

                  }

              }

            else if (ori1 == 1) // bord d'equation teta = cte
              {
                R = xv(fac3,0);
                d_teta = xv(fac3,1) - xv(fac1,1);
                if (d_teta < 0)
                  d_teta += deux_pi;
                dist3  = R*d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 2;

                if (ori3 == 0)
                  {
                    // calcul de tau21
                    tau_croises_(n_arete,0) = signe*(vit_imp-inco[fac3])/dist3
                                              - 0.5*(inco[fac1]+inco[fac2])/R;
                    // calcul de tau12
                    tau_croises_(n_arete,1) = (inco[fac2]-inco[fac1])/(xv(fac2,0) - xv(fac1,0));

                  }
                else if (ori3 == 2)
                  {
                    // calcul de tau23
                    tau_croises_(n_arete,0) = signe*(vit_imp-inco[fac3])/dist3;
                    // calcul de tau32
                    tau_croises_(n_arete,1) = (inco[fac2]-inco[fac1])/(xv(fac2,2) - xv(fac1,2));
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                dist3 = xv(fac3,2)-xv(fac1,2);
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 2;

                if (ori3 == TypeAreteBordVDF::PAROI_PAROI)
                  {
                    // calcul de tau31
                    tau_croises_(n_arete,0) = signe*(vit_imp-inco[fac3])/dist3;
                    // calcul de tau13
                    tau_croises_(n_arete,1) = (inco[fac2]-inco[fac1])/(xv(fac2,0) - xv(fac1,0));
                  }
                else if (ori3 == 1)
                  {
                    // calcul de tau32
                    tau_croises_(n_arete,0) = signe*(vit_imp-inco[fac3])/dist3;

                    // calcul de tau23
                    R = xv(fac1,0);
                    d_teta = xv(fac2,1) - xv(fac1,1);
                    if  (d_teta < 0)
                      d_teta += deux_pi;
                    tau_croises_(n_arete,1) = (inco[fac2]-inco[fac1])/(R*d_teta);
                  }

              }
            break;
          }
        case 3:
          {
            // symetrie-symetrie
            // pas de flux diffusif calcule
            break;
          }
        default :
          {
            Cerr << "On a rencontre un type d'arete non prevu\n";
            Cerr << "num arete : " << n_arete;
            Cerr << " type : " << n_type;
            exit();
            break;
          }
        }
    }

  // Boucle sur les aretes mixtes et internes

  ndeb = zone_VDF.premiere_arete_mixte();
  nfin = zone_VDF.nb_aretes();

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {

      fac1=Qdm(n_arete,0);
      fac2=Qdm(n_arete,1);
      fac3=Qdm(n_arete,2);
      fac4=Qdm(n_arete,3);
      ori1 = orientation(fac1);
      ori3 = orientation(fac3);

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          // Calcul de tau21
          R = xv(fac3,0);
          d_teta = xv(fac4,1) - xv(fac3,1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau_croises_(n_arete,1) = (inco(fac4)-inco(fac3))/(R*d_teta) - 0.5*(inco[fac1]+inco[fac2])/R;

          // Calcul de tau12
          tau_croises_(n_arete,0) = (inco(fac2)-inco(fac1))/(xv(fac2,0) - xv(fac1,0));

        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {

          // Calcul de tau32
          tau_croises_(n_arete,1) = (inco(fac4)-inco(fac3))/(xv(fac4,2) - xv(fac3,2));

          // Calcul de tau23
          R = xv(fac1,0);
          d_teta = xv(fac2,1) - xv(fac1,1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau_croises_(n_arete,0) = (inco(fac2)-inco(fac1))/(R*d_teta);

        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {

          // Calcul de tau31
          tau_croises_(n_arete,1) = (inco(fac4)-inco(fac3))/(xv(fac4,2) - xv(fac3,2));

          // Calcul de tau13
          tau_croises_(n_arete,0) = (inco(fac2)-inco(fac1))/(xv(fac2,0) - xv(fac1,0));

        }

    }
}

void calcul_interne2D(int num_elem,
                      int elx0,int elx1,
                      int ely0,int ely1,
                      const Zone_VDF& zone_VDF,const DoubleTab& val,DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1;
  double delta_x,delta_y;
  double deriv_vx,deriv_uy;

  deriv_vx = 0;
  deriv_uy = 0;

  delta_x_0 = zone_VDF.dist_elem_period(num_elem,elx0,0);
  delta_x_1 = zone_VDF.dist_elem_period(elx1,num_elem,0);
  delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
  delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);

  delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
  delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);

  deriv_vx = (delta_x_0/delta_x_1*val(elem_faces(elx1,1))
              + delta_x*val(elem_faces(num_elem,1))
              - delta_x_1/delta_x_0*val(elem_faces(elx0,1)));
  deriv_vx += (delta_x_0/delta_x_1*val(elem_faces(elx1,3))
               + delta_x*val(elem_faces(num_elem,3))
               - delta_x_1/delta_x_0*val(elem_faces(elx0,3)));
  deriv_vx *= 0.5/(delta_x_0 + delta_x_1);

  deriv_uy = (delta_y_0/delta_y_1*val(elem_faces(ely1,0))
              + delta_y*val(elem_faces(num_elem,0))
              - delta_y_1/delta_y_0*val(elem_faces(ely0,0)));
  deriv_uy += (delta_y_0/delta_y_1*val(elem_faces(ely1,2))
               + delta_y*val(elem_faces(num_elem,2))
               - delta_y_1/delta_y_0*val(elem_faces(ely0,2)));
  deriv_uy *= 0.5/(delta_y_0 + delta_y_1);

  rot[num_elem] = deriv_vx - deriv_uy;

}


void calcul_bord2D(int num_elem,
                   int elx0,int elx1,
                   int ely0,int ely1,
                   const Zone_VDF& zone_VDF,const DoubleTab& val,DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1;
  double delta_x,delta_y;
  double deriv_vx,deriv_uy;

  deriv_vx = 0;
  deriv_uy = 0;

  // Traitement des elements bord
  if ( (elx0 == -1) || (elx1 == -1) )
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem_period(elx1,num_elem,0);
              deriv_vx = (val(elem_faces(elx1,1))-val(elem_faces(num_elem,1))
                          + val(elem_faces(elx1,3))-val(elem_faces(num_elem,3)));
              deriv_vx *= 0.5/delta_x_1;

            }
          else
            deriv_vx = 0;
        }
      else // elx1 = -1 et elx0 != -1
        {
          delta_x_0 = zone_VDF.dist_elem_period(num_elem,elx0,0);
          deriv_vx = (val(elem_faces(num_elem,1))-val(elem_faces(elx0,1))
                      + val(elem_faces(num_elem,3))-val(elem_faces(elx0,3)));
          deriv_vx *= 0.5/delta_x_0;

        }
    }
  else // elx0 != -1 et elx1 != -1
    {
      delta_x_0 = zone_VDF.dist_elem_period(num_elem,elx0,0);
      delta_x_1 = zone_VDF.dist_elem_period(elx1,num_elem,0);
      delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
      deriv_vx = (delta_x_0/delta_x_1*val(elem_faces(elx1,1))
                  + delta_x*val(elem_faces(num_elem,1))
                  - delta_x_1/delta_x_0*val(elem_faces(elx0,1)));
      deriv_vx += (delta_x_0/delta_x_1*val(elem_faces(elx1,3))
                   + delta_x*val(elem_faces(num_elem,3))
                   - delta_x_1/delta_x_0*val(elem_faces(elx0,3)));
      deriv_vx *= 0.5/(delta_x_0 + delta_x_1);

    }

  if ( (ely0 == -1) || (ely1 == -1) )
    {
      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);
              deriv_uy = (val(elem_faces(ely1,0))-val(elem_faces(num_elem,0))
                          + val(elem_faces(ely1,2))-val(elem_faces(num_elem,2)));
              deriv_uy *= 0.5/delta_y_1;

            }
          else
            deriv_uy = 0;
        }
      else // ely1 = -1 et ely0 != -1
        {
          delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
          deriv_uy = (val(elem_faces(num_elem,0))-val(elem_faces(ely0,0))
                      + val(elem_faces(num_elem,2))-val(elem_faces(ely0,2)));
          deriv_uy *= 0.5/delta_y_0;

        }
    }
  else  // ely0 != -1 et ely1 != -1
    {
      delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
      delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);
      delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);

      deriv_uy = (delta_y_0/delta_y_1*val(elem_faces(ely1,0))
                  + delta_y*val(elem_faces(num_elem,0))
                  - delta_y_1/delta_y_0*val(elem_faces(ely0,0)));
      deriv_uy += (delta_y_0/delta_y_1*val(elem_faces(ely1,2))
                   + delta_y*val(elem_faces(num_elem,2))
                   - delta_y_1/delta_y_0*val(elem_faces(ely0,2)));
      deriv_uy *= 0.5/(delta_y_0 + delta_y_1);

    }

  rot[num_elem] = deriv_vx - deriv_uy;
}

void calrotord2centelemdim2(DoubleTab& rot,
                            const DoubleTab& val, const Zone_VDF& zone_VDF,
                            int nb_elem, const IntTab& face_voisins,
                            const IntTab& elem_faces)
{
  if(rot.dimension(0)!=nb_elem)
    rot.resize(nb_elem);
  int elx0,elx1,ely0,ely1;

  for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      elx0 = face_voisins(elem_faces(num_elem,0),0);
      elx1 = face_voisins(elem_faces(num_elem,2),1);
      ely0 = face_voisins(elem_faces(num_elem,1),0);
      ely1 = face_voisins(elem_faces(num_elem,3),1);

      if ( (elx0 != -1) && (elx1 != -1) && (ely0 != -1)
           && (ely1 != -1))
        // Cas d'un element interne

        calcul_interne2D(num_elem,elx0,elx1,ely0,ely1,zone_VDF,val,rot);
      else
        calcul_bord2D(num_elem,elx0,elx1,ely0,ely1,zone_VDF,val,rot);
    }
}

