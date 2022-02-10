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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_Q4_implementation.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Q4_implementation_included
#define Champ_Q4_implementation_included

#include <Champ_implementation.h>
#include <IntTab.h>

//
class Champ_Q4_implementation : public Champ_implementation
{

public:

  DoubleVect& valeur_a_elem(const DoubleVect& position,
                            DoubleVect& val,
                            int le_poly) const override;
  double valeur_a_elem_compo(const DoubleVect& position,
                             int le_poly, int ncomp) const override;
  DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                              const IntVect& les_polys,
                              DoubleTab& valeurs) const override;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleVect& valeurs,
                                     int ncomp) const override ;
  DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const override;
  DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                       DoubleVect&, int) const override;
  DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override;
  int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                    IntVect& polys) const override;
  int imprime_Q4(Sortie& , int ) const;
};

// Description:
//    Calcule la coordonnee barycentrique d'un point (x,y) par
//    rapport au sommet specifie d'un triangle ou d'un rectange (un element)
//    Ce calcul concerne un point 2D.
// Precondition:
// Parametre: IntTab& polys
//    Signification: tableau contenant les numeros des elements
//                   par rapport auxquels on veut calculer une
//                   coordonnee barycentrique.
//                   polys(i,0) est l'indice du sommet 0 de l'element
//                   i dans le tableau des coordonnees (coord).
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& coord
//    Signification: les coordonnees des sommets par auxquels on veut
//                   calculer les coordonnees barycentriques.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: double x
//    Signification: la premiere coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: double y
//    Signification: la deuxieme coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int le_poly
//    Signification: le numero de l'element (dans le tableau polys) par
//                   rapport auquel on calculera la coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entre
// Parametre: int i
//    Signification: le numero du sommet par rapport auquel on veut la
//                   coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes: Triangle  0 <= i <= 2
//    Contraintes: Rectangle 0 <= i <= 3
//    Acces: entree
// Retour: double
//    Signification: la coordonnee barycentrique du point (x,y) par rapport
//                   au sommet specifie (i) dans l'element specifie (le_poly)
//    Contraintes: 0 <= valeur <= 1
// Excpetion: un triangle n'a pas plus de 3 sommets
// Excpetion: un rectangle n'a pas plus de 4 sommets
// Exception: erreur arithmetique, denominateur nul
// Exception: erreur de calcul, coordonnee barycentrique invalide
// Effets de bord:
// Postcondition:
inline double coord_barycentrique(const IntTab& polys,
                                  const DoubleTab& coord,
                                  double x, double y, int le_poly, int i)
{
  int som0,som1,som2;
  int nb_som_elem = polys.dimension(1);
  //Distinction du calcul de la coordonnee barycentrique en fonction du type de l element
  //Cas Triangle
  if (nb_som_elem==3)
    {
      switch(i)
        {
        case 0 :
          {
            som0=polys(le_poly , 0);
            som1=polys(le_poly , 1);
            som2=polys(le_poly , 2);
            break;
          }
        case 1 :
          {
            som0=polys(le_poly , 1);
            som1=polys(le_poly , 2);
            som2=polys(le_poly , 0);
            break;
          }
        case 2 :
          {
            som0=polys(le_poly , 2);
            som1=polys(le_poly , 0);
            som2=polys(le_poly , 1);
            break;
          }
        default :
          {
            som0=-1;
            som1=-1;
            som2=-1;
            Cerr << "Error in Champ_P1::coord_barycentrique : " << finl;
            Cerr << "A triangle does not have " << i << "nodes " << finl;
            Process::exit();
          }

        }
      double den = (coord(som2,0)-coord(som1,0))*(coord(som0,1)-coord(som1,1))
                   - (coord(som2,1)-coord(som1,1))*(coord(som0,0)-coord(som1,0));

      double num = (coord(som2,0)-coord(som1,0))*(y-coord(som1,1))
                   - (coord(som2,1)-coord(som1,1))*(x-coord(som1,0));

      assert (den != 0.);
      double coord_bary=num/den;
      if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
        {
          Cerr << "WARNING: The barycentric coordinate of point :" << finl;
          Cerr << "x= " << x << " y=" << y << finl;
          Cerr << "is not between 0 and 1 : " << coord_bary << finl;
          Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
        }
      return coord_bary;
    }
  //Cas Rectangle
  else if (nb_som_elem==4)
    {
      double alpha_x=0.;
      double alpha_y=0.;
      double delta_x, delta_y;
//      int som0,som1,som2;
      double x0,y0;

      switch(i)
        {
        case 0 :
          {
            alpha_x = -1.;
            alpha_y = -1;

            break;
          }
        case 1 :
          {
            alpha_x = 1.;
            alpha_y = -1.;

            break;
          }
        case 2 :
          {
            alpha_x = -1.;
            alpha_y = 1.;

            break;
          }
        case 3 :
          {
            alpha_x = 1.;
            alpha_y = 1.;

            break;
          }
        default :
          {
            Cerr << "Error in Champ_P1::coord_barycentrique : " << finl;
            Process::exit();
          }

        }

      som0=polys(le_poly , 0);
      som1=polys(le_poly , 1);
      som2=polys(le_poly , 2);
      delta_x = coord(som1,0)-coord(som0,0);
      delta_y = coord(som2,1)-coord(som0,1);
      x0 = coord(som0,0)+delta_x/2.;
      y0 = coord(som0,1)+delta_y/2.;
      double coord_bary = 0.25*(1.+2.*alpha_x*(x-x0)/delta_x)*(1.+2.*alpha_y*(y-y0)/delta_y);


      if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
        {
          Cerr << "WARNING: The barycentric coordinate of point :" << finl;
          Cerr << "x= " << x << " y=" << y << finl;
          Cerr << "is not between 0 and 1 : " << coord_bary << finl;
          Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
        }
      return coord_bary;
    }

  Cerr<<"The number of nodes by element " <<nb_som_elem<<" does not correspond to a treated situation."<<finl;
  Process::exit();
  return 0.;
}

// Description:
//    Calcule la coordonnee barycentrique d'un point (x,y,z) par
//    rapport au sommet specifie d'un tetraedre ou d'un hexaedre (un element)
//    Ce calcul concerne un point 3D.
// Precondition:
// Parametre: IntTab& polys
//    Signification: tableau contenant les numeros des elements
//                   par rapport auxquels on veut calculer une
//                   coordonnee barycentrique.
//                   polys(i,0) est l'indice du sommet 0 de l'element
//                   i dans le tableau des coordonnees (coord).
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& coord
//    Signification: les coordonnees des sommets par auxquels on veut
//                   calculer les coordonnees barycentriques.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: double x
//    Signification: la premiere coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: double y
//    Signification: la deuxieme coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: double z
//    Signification: la troisieme coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int le_poly
//    Signification: le numero de l'element (dans le tableau polys) par
//                   rapport auquel on calculera la coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entre
// Parametre: int i
//    Signification: le numero du sommet par rapport auquel on veut la
//                   coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes: Tetraedre 0 <= i <= 3
//    Contraintes: Hexaedre  0 <= i <= 7
//    Acces: entree
// Retour: double
//    Signification: la coordonnee barycentrique du point (x,y,z) par rapport
//                   au sommet specifie (i) dans l'element specifie (le_poly)
//    Contraintes: 0 <= valeur <= 1
// Exception: un tetraedre n'a pas plus de 4 sommets
// Exception: un hexaedre n'a pas plus de 8 sommets
// Exception: erreur arithmetique, denominateur nul
// Exception: erreur de calcul, coordonnee barycentrique invalide
// Effets de bord:
// Postcondition:
inline double coord_barycentrique(const IntTab& polys,
                                  const DoubleTab& coord,
                                  double x, double y, double z,
                                  int le_poly, int i)
{
  int som0,som1,som2,som3;
  int nb_som_elem = polys.dimension(1);
  //Distinction du calcul de la coordonnee barycentrique en fonction du type de l element
  //Cas Tetraedre
  if (nb_som_elem==4)
    {
      switch(i)
        {
        case 0 :
          {
            som0=polys(le_poly , 0);
            som1=polys(le_poly , 1);
            som2=polys(le_poly , 2);
            som3=polys(le_poly , 3);
            break;
          }
        case 1 :
          {
            som0=polys(le_poly , 1);
            som1=polys(le_poly , 2);
            som2=polys(le_poly , 3);
            som3=polys(le_poly , 0);
            break;
          }
        case 2 :
          {
            som0=polys(le_poly , 2);
            som1=polys(le_poly , 3);
            som2=polys(le_poly , 0);
            som3=polys(le_poly , 1);
            break;
          }
        case 3 :
          {
            som0=polys(le_poly , 3);
            som1=polys(le_poly , 0);
            som2=polys(le_poly , 1);
            som3=polys(le_poly , 2);
            break;
          }
        default :
          {
            som0=-1;
            som1=-1;
            som2=-1;
            som3=-1;
            Cerr << "Error in Champ_P1::coord_barycentrique : " << finl;
            Cerr << "A tetrahedron does not have " << i << "nodes " << finl;
            Process::exit();
          }
        }

      double xp = (coord(som2,1)-coord(som1,1))*(coord(som0,2)-coord(som1,2))
                  - (coord(som2,2)-coord(som1,2))*(coord(som0,1)-coord(som1,1));
      double yp = (coord(som2,2)-coord(som1,2))*(coord(som0,0)-coord(som1,0))
                  - (coord(som2,0)-coord(som1,0))*(coord(som0,2)-coord(som1,2));
      double zp = (coord(som2,0)-coord(som1,0))*(coord(som0,1)-coord(som1,1))
                  - (coord(som2,1)-coord(som1,1))*(coord(som0,0)-coord(som1,0));
      double den = xp * (coord(som3,0)-coord(som1,0))
                   + yp * (coord(som3,1)-coord(som1,1))
                   + zp * (coord(som3,2)-coord(som1,2));

      xp = (coord(som2,1)-coord(som1,1))*(z-coord(som1,2))
           - (coord(som2,2)-coord(som1,2))*(y-coord(som1,1));
      yp = (coord(som2,2)-coord(som1,2))*(x-coord(som1,0))
           - (coord(som2,0)-coord(som1,0))*(z-coord(som1,2));
      zp = (coord(som2,0)-coord(som1,0))*(y-coord(som1,1))
           - (coord(som2,1)-coord(som1,1))*(x-coord(som1,0));
      double num = xp * (coord(som3,0)-coord(som1,0))
                   + yp * (coord(som3,1)-coord(som1,1))
                   + zp * (coord(som3,2)-coord(som1,2));

      assert (den != 0.);
      double coord_bary = num/den;
      if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
        {
          Cerr << "WARNING: The barycentric coordinate of point :" << finl;
          Cerr << "x= " << x << " y=" << y << " z=" << z << finl;
          Cerr << "is not between 0 and 1 : " << coord_bary << finl;
          Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
        }
      return coord_bary;
    }
  //Cas Hexaedre
  else if (nb_som_elem==8)
    {
      double alpha_x=0.;
      double alpha_y=0.;
      double alpha_z=0.;
      double delta_x, delta_y, delta_z;
      //int som0,som1,som2;
      double x0,y0,z0;

      switch(i)
        {
        case 0 :
          {
            alpha_x = -1.;
            alpha_y = -1.;
            alpha_z = -1.;
            break;
          }
        case 1 :
          {
            alpha_x = 1.;
            alpha_y = -1.;
            alpha_z = -1.;
            break;
          }
        case 2 :
          {
            alpha_x = -1.;
            alpha_y = 1.;
            alpha_z = -1.;
            break;
          }
        case 3 :
          {
            alpha_x = 1.;
            alpha_y = 1.;
            alpha_z = -1.;
            break;
          }
        case 4 :
          {
            alpha_x = -1.;
            alpha_y = -1;
            alpha_z = 1.;
            break;
          }
        case 5 :
          {
            alpha_x = 1.;
            alpha_y = -1.;
            alpha_z = 1.;
            break;
          }
        case 6 :
          {
            alpha_x = -1.;
            alpha_y = 1.;
            alpha_z = 1.;
            break;
          }
        case 7 :
          {
            alpha_x = 1.;
            alpha_y = 1.;
            alpha_z = 1.;
            break;
          }
        default :
          {
            Cerr << "Error in Champ_P1::coord_barycentrique : " << finl;
            Process::exit();
          }
        }

      som0=polys(le_poly , 0);
      som1=polys(le_poly , 1);
      som2=polys(le_poly , 2);
      som3=polys(le_poly , 4);

      delta_x = coord(som1,0)-coord(som0,0);
      delta_y = coord(som2,1)-coord(som0,1);
      delta_z = coord(som3,2)-coord(som0,2);

      x0 = coord(som0,0)+delta_x/2.;
      y0 = coord(som0,1)+delta_y/2.;
      z0 = coord(som0,2)+delta_z/2.;

      double coord_bary = (1./8.)*(1.+2.*alpha_x*(x-x0)/delta_x)*(1.+2.*alpha_y*(y-y0)/delta_y)*(1.+2.*alpha_z*(z-z0)/delta_z);


      if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
        {
          Cerr << "WARNING: The barycentric coordinate of point :" << finl;
          Cerr << "x= " << x << " y=" << y <<" z="<< z <<finl;
          Cerr << "is not between 0 and 1 : " << coord_bary << finl;
          Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
        }
      return coord_bary;
    }

  Cerr<<"The number of nodes by element " <<nb_som_elem<<" does not correspond to a treated situation."<<finl;
  Process::exit();
  return 0.;
}

#endif
