/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Polyedre_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Polyedre_CoviMAC.h>
#include <Polyedre.h>
#include <Domaine.h>
#include <Zone_CoviMAC.h>
#include <Linear_algebra_tools_impl.h>


Implemente_instanciable(Polyedre_CoviMAC,"Polyedre_CoviMAC",Elem_CoviMAC_base);

// printOn et readOn


Sortie& Polyedre_CoviMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Polyedre_CoviMAC::readOn(Entree& s )
{
  return s ;
}

void Polyedre_CoviMAC::normale(int num_Face,DoubleTab& Face_normales,
                               const IntTab& Face_sommets,
                               const IntTab& Face_voisins,
                               const IntTab& elem_faces,
                               const Zone& zone_geom) const
{

  //Cerr << " num_Face " << num_Face << finl;
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();

  // Cerr << "les face sommet " << Face_sommets << finl;


  int nb_som_face_max= Face_sommets.dimension(1);
  int n0 = Face_sommets(num_Face,0);

  const IntTab& elem=zone_geom.les_elems();
  Vecteur3 n(0,0,0);
  Vecteur3 moinsS0(les_coords(n0,0),les_coords(n0,1),les_coords(n0,2));
  moinsS0*=-1;
  for (int s=1; s<nb_som_face_max-1 ; s++)
    {
      int n1 = Face_sommets(num_Face,s);
      int n2 = Face_sommets(num_Face,s+1);
      if (n2<0)
        {
          break;
        }
      Vecteur3 S1(les_coords(n1,0),les_coords(n1,1),les_coords(n1,2));
      Vecteur3 S2(les_coords(n2,0),les_coords(n2,1),les_coords(n2,2));
      S1+=moinsS0;
      S2+=moinsS0;
      Vecteur3 nTr;
      Vecteur3::produit_vectoriel(S1,S2,nTr);
      n+=nTr;
    }
  n*=0.5;

  /*
    int elem1=Face_voisins(num_Face,0);
  DoubleVect xp;
  ref_cast(Poly_geom_base,zone_geom.type_elem().valeur()).calculer_un_centre_gravite(elem1,xp);
  Vecteur3 M(xp(0),xp(1),xp(2));
  */
  // on prend un point a l'interieur (pas le cg trop long)

  int num_poly=Face_voisins(num_Face,0);

  Vecteur3 xg(0,0,0);
  int nb_som_max=elem.dimension(1);
  int nb_som_reel;
  for (nb_som_reel=0; nb_som_reel<nb_som_max; nb_som_reel++)
    {
      int ns=elem(num_poly,nb_som_reel);
      if (ns<0)
        break;
      Vecteur3 S(les_coords(ns,0),les_coords(ns,1),les_coords(ns,2));
      xg+=S;
    }
  xg*=1./nb_som_reel;


  xg+=moinsS0;
  double prod_scal=Vecteur3::produit_scalaire(n,xg);
  if ( (prod_scal) > 0 )
    {
      n*=-1;
    }


  Face_normales(num_Face,0) = n[0];
  Face_normales(num_Face,1) = n[1];
  Face_normales(num_Face,2) = n[2];

  // double s=sqrt(Vecteur3::produit_scalaire(n,n));
  // Cerr << "Face_normales " << num_Face<< " s "<< s << finl;
  // assert(s>0);
  //  Cerr << "Face_normales " << num_Face<< " s "<< s << finl;

}
