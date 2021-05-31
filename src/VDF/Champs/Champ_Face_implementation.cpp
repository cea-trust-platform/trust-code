/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Champ_Face_implementation.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Face_implementation.h>
#include <Champ_Inc_base.h>
#include <Zone_VDF.h>
#include <LecFicDiffuse.h>
#include <Frontiere_dis_base.h>

DoubleTab& Champ_Face_implementation::valeur_aux_elems(const DoubleTab& positions,
                                                       const IntVect& les_polys,
                                                       DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo_=cha.nb_comp();
  if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_Face_implementation::valeur_aux_elems()" << finl;
      Cerr << "Le DoubleTab val n'a pas 2 entrees" << finl;
      Process::exit();
    }

  const Zone_VDF& zone_VDF = zone_vdf();
  //  const Zone& zone_geom = zone_VDF.zone();
  const IntTab& face_sommets = zone_VDF.face_sommets();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double psi,val1,val2;
  int som0,som1;
  int le_poly;

  const DoubleTab& ch = cha.valeurs();
  const Domaine& dom=zone().domaine();

  if (nb_compo_ == 1)
    {
      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            val(rang_poly, 0) = 0;
          else
            for(int dir=0; dir<Objet_U::dimension; dir++)
              {
                int face1 = elem_faces(le_poly,dir);
                int face2 = elem_faces(le_poly,Objet_U::dimension+dir);
                val1 = ch(face1);
                val2 = ch(face2);

                som0 = face_sommets(face1,0);
                som1 = face_sommets(face2,0);

                psi = ( positions(rang_poly,dir) - dom.coord(som0,dir) )
                      / ( dom.coord(som1,dir) - dom.coord(som0,dir) ) ;
                if (est_egal(psi,0) || est_egal(psi,1))
                  val(rang_poly,0) = interpolation(val1,val2,psi);
              }
        }
    }
  else // (nb_compo_ != 1) */
    {
      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly,ncomp) = 0;
          else
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              {

                val1 = ch(elem_faces(le_poly,ncomp));
                val2 = ch(elem_faces(le_poly,Objet_U::dimension+ncomp));

                som0 = face_sommets(elem_faces(le_poly,ncomp),0);
                som1 = face_sommets(elem_faces(le_poly,Objet_U::dimension+ncomp),0);

                psi = ( positions(rang_poly,ncomp) - dom.coord(som0,ncomp) )
                      / ( dom.coord(som1,ncomp) - dom.coord(som0,ncomp) ) ;
                val(rang_poly,ncomp) = interpolation(val1,val2,psi);
              }
        }
    }
  return val;
}

double Champ_Face_implementation::interpolation(const double val1, const double val2, const double psi) const
{
  double epsilon=1.e-12;
  if (dabs(psi) < epsilon)
    return val1 ;
  else if (dabs(1.-psi) < epsilon)
    return val2 ;
  else
    return val1 + psi * (val2-val1) ;
}

DoubleVect& Champ_Face_implementation::valeur_aux_elems_compo(const DoubleTab& positions,
                                                              const IntVect& les_polys,
                                                              DoubleVect& val,
                                                              int ncomp) const
{
  const Champ_base& cha = le_champ();
  int nb_compo_ = cha.nb_comp();
  assert(val.size() == les_polys.size());
  int le_poly;
  double psi,val1,val2;
  int som0,som1;

  const Zone_VDF& zone_VDF = zone_vdf();
  //  const Zone& zone_geom = zone_VDF.zone();
  const IntTab& face_sommets = zone_VDF.face_sommets();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleTab& ch = cha.valeurs();
  const Domaine& dom=zone().domaine();

  if (nb_compo_ == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_aux_elems_compo"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  else // (nb_compo_ != 1)
    {
      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            val(rang_poly) = 0;
          else
            {
              val1 = ch(elem_faces(le_poly,ncomp));
              val2 = ch(elem_faces(le_poly,Objet_U::dimension+ncomp));
              som0 = face_sommets(elem_faces(le_poly,ncomp),0);
              som1 = face_sommets(elem_faces(le_poly,Objet_U::dimension+ncomp),0);

              psi = ( positions(rang_poly,ncomp) - dom.coord(som0,ncomp) )
                    / ( dom.coord(som1,ncomp) - dom.coord(som0,ncomp) ) ;
              val(rang_poly) = interpolation(val1,val2,psi);
            }
        }
    }
  return val;
}

DoubleVect& Champ_Face_implementation::valeur_a_elem(const DoubleVect& position,
                                                     DoubleVect& val,
                                                     int le_poly) const
{
  const Zone_VDF& zone_VDF = zone_vdf();
  const Zone& zone_geom = zone();
  const IntTab& face_sommets = zone_VDF.face_sommets();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const Champ_base& cha=le_champ();

  double psi,val1,val2;
  int som0,som1;
  const DoubleTab& ch = cha.valeurs();
  int nb_compo_=cha.nb_comp();
  const Domaine& dom=zone_geom.domaine();

  if (nb_compo_ == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_a_elem"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  else // (nb_compo_ != 1)
    {
      if (le_poly == -1)
        for(int ncomp=0; ncomp<nb_compo_; ncomp++)
          val(ncomp) = 0;
      else
        for(int ncomp=0; ncomp<nb_compo_; ncomp++)
          {
            val1 = ch(elem_faces(le_poly,ncomp));
            val2 = ch(elem_faces(le_poly,Objet_U::dimension+ncomp));

            som0 = face_sommets(elem_faces(le_poly,ncomp),0);
            som1 = face_sommets(elem_faces(le_poly,Objet_U::dimension+ncomp),0);

            psi = ( position(ncomp) - dom.coord(som0,ncomp) )
                  / ( dom.coord(som1,ncomp) - dom.coord(som0,ncomp) ) ;

            val(ncomp) = interpolation(val1,val2,psi);
          }
    }
  return val;
}


double Champ_Face_implementation::valeur_a_elem_compo(const DoubleVect& position,
                                                      int le_poly,int ncomp) const
{
  const Zone_VDF& zone_VDF = zone_vdf();
  const Zone& zone_geom = zone();
  const IntTab& face_sommets = zone_VDF.face_sommets();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const Champ_base& cha = le_champ();
  int nb_compo_ = cha.nb_comp();

  double psi,val1,val2;
  double val = 0.;
  int som0,som1;
  const DoubleTab& ch = cha.valeurs();
  const Domaine& dom=zone_geom.domaine();

  if (nb_compo_ == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_a_elem_compo"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  else // (nb_compo_ != 1)
    {
      if (le_poly == -1)
        return 0;
      else
        {
          val1 = ch(elem_faces(le_poly,ncomp));
          val2 = ch(elem_faces(le_poly,Objet_U::dimension+ncomp));

          som0 = face_sommets(elem_faces(le_poly,ncomp),0);
          som1 = face_sommets(elem_faces(le_poly,Objet_U::dimension+ncomp),0);

          psi = ( position(ncomp) - dom.coord(som0,ncomp) )
                / ( dom.coord(som1,ncomp) - dom.coord(som0,ncomp) ) ;
          val = interpolation(val1,val2,psi);
        }
    }
  return val;
}

DoubleTab& Champ_Face_implementation::valeur_aux_sommets(const Domaine& dom,
                                                         DoubleTab& ch_som) const
{
  //  const Champ_base& cha_tmp=le_champ();
  //  const DoubleTab& ch_tmp = cha_tmp.valeurs();

  const Champ_base& cha=le_champ();
  int nb_compo_=cha.nb_comp();
  const Zone& mazone = dom.zone(0);

  //  int nb_elem = mazone.nb_elem();
  int nb_elem_tot = mazone.nb_elem_tot();
  int nb_som = mazone.nb_som();
  int nb_som_elem = mazone.nb_som_elem();
  //  const DoubleTab& ch = cha.valeurs();
  IntVect compteur(nb_som);
  int ncomp;
  int num_elem,num_som,j;
  ch_som = 0;
  compteur = 0;

  if (nb_compo_ == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_aux_sommets"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  else // (nb_compo_ != 1)
    {
      DoubleVect position(Objet_U::dimension);
      for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
        for (j=0; j<nb_som_elem; j++)
          {
            num_som = mazone.sommet_elem(num_elem,j);

            for(int k=0; k<Objet_U::dimension; k++)
              position(k)=dom.coord(num_som,k);
            if(num_som < nb_som)
              {
                compteur[num_som]++;
                for (ncomp=0; ncomp<nb_compo_; ncomp++)
                  {
                    ch_som(num_som,ncomp) += valeur_a_elem_compo(position,num_elem,ncomp);
                  }
              }
          }

      for (num_som=0; num_som<nb_som; num_som++)
        for (ncomp=0; ncomp<nb_compo_; ncomp++)
          ch_som(num_som,ncomp) /= compteur[num_som];
    }

  return ch_som;
}
DoubleVect& Champ_Face_implementation::valeur_aux_sommets_compo(const Domaine& dom,
                                                                DoubleVect& ch_som,
                                                                int ncomp) const
{
  const Champ_base& cha=le_champ();
  int nb_compo_=cha.nb_comp();
  const Zone& mazone = dom.zone(0);

  //  int nb_elem = mazone.nb_elem();
  int nb_elem_tot = mazone.nb_elem_tot();
  int nb_som = mazone.nb_som();
  int nb_som_elem = mazone.nb_som_elem();
  //  const DoubleTab& ch = cha.valeurs();
  IntVect compteur(nb_som);
  int num_elem,num_som,j;
  ch_som = 0;
  compteur = 0;

  if (nb_compo_ == 1)
    {
      Cerr<<"Champ_Face_implementation::valeur_aux_sommets_compo"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  else // (nb_compo_ != 1)
    {
      DoubleVect position(Objet_U::dimension);
      for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
        for (j=0; j<nb_som_elem; j++)
          {
            num_som = mazone.sommet_elem(num_elem,j);
            for(int k=0; k<Objet_U::dimension; k++)
              position(k)=dom.coord(num_som,k);
            if(num_som < nb_som)
              {
                compteur[num_som]++;
                ch_som(num_som) += valeur_a_elem_compo(position,num_elem,ncomp);
              }
          }

      for (num_som=0; num_som<nb_som; num_som++)
        ch_som(num_som) /= compteur[num_som];
    }
  return ch_som;
}

DoubleTab& Champ_Face_implementation::remplir_coord_noeuds(DoubleTab& positions) const
{
  const Zone_VDF& la_zone_vdf = ref_cast(Zone_VDF,zone_dis_base());
  const DoubleTab& xv = la_zone_vdf.xv();
  int nb_fac = la_zone_vdf.nb_faces_tot();
  if ( (xv.dimension(0) == nb_fac ) && (xv.dimension(1) == Objet_U::dimension) )
    positions.ref(xv);
  else
    {
      Cerr << "Erreur dans Champ_Face::remplir_coord_noeuds()" << finl;
      Cerr << "Les centres de gravite des faces n'ont pas ete calcules" << finl;
      Process::exit();
    }
  return positions;
}

int Champ_Face_implementation::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                             IntVect& polys) const
{
  const IntTab& face_voisins = zone_vdf().face_voisins();
  int nb_faces=zone_vdf().nb_faces();
  remplir_coord_noeuds(positions);
  polys.resize(nb_faces);

  for(int face=0; face<nb_faces; face++)
    if( (polys(face)=face_voisins(face,0)) == -1)
      polys(face)=face_voisins(face,1);
  return 1;
}

int Champ_Face_implementation::imprime_Face(Sortie& os, int ncomp) const
{
  const Champ_base& champ=le_champ();
  int ni,nj,nk=-1;
  IntVect m(3);
  int i,j,k;
  int np,elem;
  int cmax=7;
  DoubleVect xi,yj,zk;
  DoubleTab Grille;
  //Lecture de xi,yj,zk dans un fichier .xiyjzk
  Nom nomfic(Objet_U::nom_du_cas());
  nomfic+=".xiyjzk";
  LecFicDiffuse ficijk(nomfic);
  ficijk >> xi;
  ni=xi.size();
  ficijk >> yj;
  nj=yj.size();
  if (Objet_U::dimension==3)
    {
      ficijk >> zk;
      nk=zk.size();
    }
  m=1;
  m(ncomp)=0;
  if (Objet_U::dimension==3)
    {
      // Grille ordonnee sur les faces des elements
      ni-=m(0);
      nj-=m(1);
      nk-=m(2);
      np=ni*nj*nk;
      Grille.resize(np,Objet_U::dimension);
      for(k=0; k<nk; k++)
        for(i=0; i<ni; i++)
          for(j=0; j<nj; j++)
            {
              elem=j+nj*(i+k*ni);
              if (ncomp==0)
                Grille(elem,0)=xi(i);
              else
                Grille(elem,0)=0.5*(xi(i)+xi(i+1));
              if (ncomp==1)
                Grille(elem,1)=yj(j);
              else
                Grille(elem,1)=0.5*(yj(j)+yj(j+1));
              if (ncomp==2)
                Grille(elem,2)=zk(k);
              else
                Grille(elem,2)=0.5*(zk(k)+zk(k+1));
            }
      DoubleVect valeurs(np);
      champ.valeur_aux_compo(Grille, valeurs, ncomp);
      for(k=0; k<nk; k++)
        {
          os << finl;
          os << "Coupe a K= " << k << finl;
          int n1=0,n2=0;
          while (n2<ni)
            {
              n1=n2;
              n2=::min(ni,n2+cmax);
              os << finl;
              os << "I=     ";
              for(i=n1; i<n2; i++)
                os << i << "              ";
              os << finl;
              for(j=nj-1; j>-1; j--)
                {
                  os << "J= " << j << " ";
                  for(i=n1; i<n2; i++)
                    {
                      elem=j+nj*(i+k*ni);
                      os << valeurs(elem) << " ";
                    }
                  os << finl;
                }
            }
        }
    }
  else if (Objet_U::dimension==2)
    {
      ni-=m(0);
      nj-=m(1);
      np=ni*nj;
      Grille.resize(np,Objet_U::dimension);
      for(i=0; i<ni; i++)
        for(j=0; j<nj; j++)
          {
            elem=j+nj*i;
            if (ncomp==0)
              Grille(elem,0)=xi(i);
            else
              Grille(elem,0)=0.5*(xi(i)+xi(i+1));
            if (ncomp==1)
              Grille(elem,1)=yj(j);
            else
              Grille(elem,1)=0.5*(yj(j)+yj(j+1));
          }
      DoubleVect valeurs(np);
      champ.valeur_aux_compo(Grille, valeurs, ncomp);
      int n1=0,n2=0;
      while (n2<ni)
        {
          n1=n2;
          n2=::min(ni,n2+cmax);
          os << finl;
          os << "I=     ";
          for(i=n1; i<n2; i++)
            os << i << "              ";
          os << finl;
          for(j=nj-1; j>-1; j--)
            {
              os << "J= " << j << " ";
              for(i=n1; i<n2; i++)
                {
                  elem=j+nj*i;
                  os << valeurs(elem) << " ";
                }
              os << finl;
            }
        }
    }
  return 1;
}

DoubleTab& Champ_Face_implementation::trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const
{
  assert(distant==0);
  const Front_VF& fr_vf=ref_cast(Front_VF, fr);
  const Zone_VDF& zvdf=zone_vdf();
  const IntVect& ori = zvdf.orientation();
  const IntTab& face_voisins = zvdf.face_voisins();
  const IntTab& elem_faces = zvdf.elem_faces();
  int elem1,elem2;
  int face,i,f1,f2,f3,f4;
  int nb_faces = fr_vf.nb_faces();
  if (x.dimension(0)!=nb_faces)
    {
      Cerr << "The number of faces " << nb_faces << " on the remote boundary " << fr.le_nom() << finl;
      Cerr << "does not match the number of faces " << x.dimension(0) << " on the local boundary." << finl;
      Cerr << "Please, check if the boundary condition is not applied on wrong boundaries." << finl;
      Process::exit();
    }
//  assert(x.dimension(1)==Objet_U::dimension);

  if (x.dimension(1) == 1)
    {
      for (i=0; i<fr_vf.nb_faces(); i++)
        {
          face=fr_vf.num_premiere_face()+i;
          x(i,0)=y(face);
        }
      return x;
    }

  for (i=0; i<fr_vf.nb_faces(); i++)
    {
      face=fr_vf.num_premiere_face()+i;
      x(i,ori[face])=y(face);
    }

  for (i=0; i<fr_vf.nb_faces(); i++)
    {
      face=fr_vf.num_premiere_face()+i;
      elem1 = face_voisins(face,0);
      if (elem1 != -1)
        {
          if (Objet_U::dimension == 2)
            {
              if (ori[face] == 0)
                {
                  f1 = elem_faces(elem1,1);
                  f2 = elem_faces(elem1,3);
                  x(i,1)= 0.5*(y[f1] + y[f2]);
                }
              else
                {
                  f1 = elem_faces(elem1,0);
                  f2 = elem_faces(elem1,2);
                  x(i,0)= 0.5*(y[f1] + y[f2]);
                }
            }
          else if (Objet_U::dimension == 3)
            {
              if (ori[face] == 0)
                {
                  f1 = elem_faces(elem1,1);
                  f2 = elem_faces(elem1,4);
                  f3 = elem_faces(elem1,2);
                  f4 = elem_faces(elem1,5);
                  x(i,1)= 0.5*(y[f1] + y[f2]);
                  x(i,2)= 0.5*(y[f3] + y[f4]);
                }
              else if (ori[face] == 1)
                {
                  f1 = elem_faces(elem1,0);
                  f2 = elem_faces(elem1,3);
                  f3 = elem_faces(elem1,2);
                  f4 = elem_faces(elem1,5);
                  x(i,0)= 0.5*(y[f1] + y[f2]);
                  x(i,2)= 0.5*(y[f3] + y[f4]);
                }
              else // ori[face] = 2
                {
                  f1 = elem_faces(elem1,0);
                  f2 = elem_faces(elem1,3);
                  f3 = elem_faces(elem1,1);
                  f4 = elem_faces(elem1,4);
                  x(i,0)= 0.5*(y[f1] + y[f2]);
                  x(i,1)= 0.5*(y[f3] + y[f4]);
                }
            }
        }
      else  // elem1 = -1
        {
          elem2 = face_voisins(face,1);
          if (Objet_U::dimension == 2)
            {
              if (ori[face] == 0)
                {
                  f1 = elem_faces(elem2,1);
                  f2 = elem_faces(elem2,3);
                  x(i,1)= 0.5*(y[f1] + y[f2]);
                }
              else
                {
                  f1 = elem_faces(elem2,0);
                  f2 = elem_faces(elem2,2);
                  x(i,0)= 0.5*(y[f1] + y[f2]);
                }
            }
          else if (Objet_U::dimension == 3)
            {
              if (ori[face] == 0)
                {
                  f1 = elem_faces(elem2,1);
                  f2 = elem_faces(elem2,4);
                  f3 = elem_faces(elem2,2);
                  f4 = elem_faces(elem2,5);
                  x(i,1)= 0.5*(y[f1] + y[f2]);
                  x(i,2)= 0.5*(y[f3] + y[f4]);
                }
              else if (ori[face] == 1)
                {
                  f1 = elem_faces(elem2,0);
                  f2 = elem_faces(elem2,3);
                  f3 = elem_faces(elem2,2);
                  f4 = elem_faces(elem2,5);
                  x(i,0)= 0.5*(y[f1] + y[f2]);
                  x(i,2)= 0.5*(y[f3] + y[f4]);
                }
              else // ori[face] = 2
                {
                  f1 = elem_faces(elem2,0);
                  f2 = elem_faces(elem2,3);
                  f3 = elem_faces(elem2,1);
                  f4 = elem_faces(elem2,4);
                  x(i,0)= 0.5*(y[f1] + y[f2]);
                  x(i,1)= 0.5*(y[f3] + y[f4]);
                }
            }
        }
    }
  // Useless ?x.echange_espace_virtuel();
  return x;
}
