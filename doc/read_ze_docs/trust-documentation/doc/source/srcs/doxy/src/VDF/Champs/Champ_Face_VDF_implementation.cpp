/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Champ_Face_VDF_implementation.h>
#include <Frontiere_dis_base.h>
#include <Champ_Inc_base.h>
#include <LecFicDiffuse.h>
#include <Domaine_VDF.h>
#include <TRUSTTab.h>

DoubleTab& Champ_Face_VDF_implementation::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  return valeur_aux_elems_(le_champ().valeurs(), positions, les_polys, val_elem);
}

DoubleTab& Champ_Face_VDF_implementation::valeur_aux_elems_passe(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  return valeur_aux_elems_(le_champ().passe(), positions, les_polys, val_elem);
}

DoubleVect& Champ_Face_VDF_implementation::valeur_a_elem(const DoubleVect& position, DoubleVect& val, int e) const
{
  return valeur_a_elem_(le_champ().valeurs(), position, val, e);
}

DoubleTab& Champ_Face_VDF_implementation::valeur_aux_elems_(const DoubleTab& val_face, const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val_elem) const
{
  if (val_elem.nb_dim() > 2)
    {
      Cerr << "Erreur TRUST dans Champ_Face_implementation::valeur_aux_elems()" << finl;
      Cerr << "Le DoubleTab val a plus de 2 entrees" << finl;
      Process::exit();
    }

  const int N = val_face.line_size(), D = Objet_U::dimension, M = le_champ().nb_comp();
  DoubleVect val_e(N * D), x(D);
  val_elem = 0.0;

  for (int p = 0; p < les_polys.size(); p++)
    {
      for (int d = 0; d < D; d++) x(d) = positions(p, d);
      valeur_a_elem_(val_face, x, val_e, les_polys(p));
      for (int i = 0; i < N * std::min(D, M); i++) val_elem(p, i) = val_e(i);
    }

  return val_elem;
}

DoubleVect& Champ_Face_VDF_implementation::valeur_a_elem_(const DoubleTab& val_face, const DoubleVect& position, DoubleVect& val, int e) const
{
  val = 0.0;
  if (e == -1) return val;

  const int N = le_champ().valeurs().line_size(), D = Objet_U::dimension;
  const Domaine_VDF& domaine_VDF = domaine_vdf();
  const Domaine& domaine_geom = get_domaine_geom();
  const IntTab& f_s = domaine_VDF.face_sommets(), &e_f = domaine_VDF.elem_faces();

  for (int d = 0; d < D; d++)
    {
      const int som0 = f_s(e_f(e, d), 0), som1 = f_s(e_f(e, d + D), 0);
      const double psi = (position(d) - domaine_geom.coord(som0, d)) / (domaine_geom.coord(som1, d) - domaine_geom.coord(som0, d));
      for (int n = 0; n < N; n++)
        {
          // TODO : FIXME : cas avec line_size 1 mais nb_dim != 2 ... vu dans cathare3D
          const double val1 = (N == 1) ? val_face(e_f(e, d)) : val_face(e_f(e, d), n);
          const double val2 = (N == 1) ? val_face(e_f(e, d + D)) : val_face(e_f(e, d + D), n);
          if (le_champ().nb_comp() == 1)
            {
              if (est_egal(psi, 0) || est_egal(psi, 1))
                val(0) = interpolation(val1, val2, psi);
            }
          else
            val(N * d + n) = interpolation(val1, val2, psi);
        }
    }
  return val;
}

double Champ_Face_VDF_implementation::interpolation(const double val1, const double val2, const double psi) const
{
  double epsilon=1.e-12;
  if (std::fabs(psi) < epsilon)
    return val1 ;
  else if (std::fabs(1.-psi) < epsilon)
    return val2 ;
  else
    return val1 + psi * (val2-val1) ;
}

DoubleVect& Champ_Face_VDF_implementation::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& val, int ncomp) const
{
  assert(val.size() == les_polys.size());

  const int D = Objet_U::dimension;
  DoubleVect x(D);

  for(int p = 0; p < les_polys.size(); p++)
    {
      for (int d = 0; d < D; d++) x(d) = positions(p, d);
      val(p) = valeur_a_elem_compo(x, les_polys(p), ncomp);
    }

  return val;
}

double Champ_Face_VDF_implementation::valeur_a_elem_compo(const DoubleVect& position, int e, int d) const
{
  assert (le_champ().nb_comp() > 1); // a scalar field should not be a champ_face
  assert(le_champ().valeurs().line_size() == 1); // not compatible with multiphase
  if (e == -1) return 0;

  const IntTab& f_s = domaine_vdf().face_sommets(), &e_f = domaine_vdf().elem_faces();
  const DoubleTab& vals = le_champ().valeurs();
  const Domaine& dom = domaine_vdf().domaine();
  const int D = Objet_U::dimension;

  const double val1 = vals(e_f(e, d)), val2 = vals(e_f(e, D + d));
  const int som0 = f_s(e_f(e, d), 0), som1 = f_s(e_f(e, D + d), 0);
  const double psi = (position(d) - dom.coord(som0, d)) / (dom.coord(som1, d) - dom.coord(som0, d));

  return interpolation(val1, val2, psi);
}

DoubleTab& Champ_Face_VDF_implementation::valeur_aux_sommets(const Domaine& dom, DoubleTab& ch_som) const
{
  if (le_champ().nb_comp() == 1)
    {
      Cerr<<"Champ_Face_VDF_implementation::valeur_aux_sommets"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }

  const int nb_elem_tot = dom.nb_elem_tot(), nb_som = dom.nb_som(), nb_som_elem = dom.nb_som_elem();
  const int N = le_champ().valeurs().line_size(), D = Objet_U::dimension;
  IntVect compteur(nb_som);
  ch_som = 0, compteur = 0;

  DoubleVect position(D), val_e(N * D);
  for (int e = 0; e < nb_elem_tot; e++)
    for (int j = 0, s; j < nb_som_elem; j++)
      if ((s = dom.sommet_elem(e, j)) < nb_som)
        {
          for(int d = 0; d < D; d++)
            position(d) = dom.coord(s, d);

          compteur[s]++;
          valeur_a_elem(position, val_e, e);
          for (int n = 0; n < N; n++)
            for (int d = 0; d < D; d++)
              ch_som(s, N * d + n) += val_e(N * d + n);
        }

  for (int s = 0; s < nb_som; s++)
    for (int n = 0; n < N; n++)
      for (int d = 0; d < D; d++)
        ch_som(s, N * d + n) /= compteur[s];

  return ch_som;
}
DoubleVect& Champ_Face_VDF_implementation::valeur_aux_sommets_compo(const Domaine& dom,
                                                                    DoubleVect& ch_som,
                                                                    int ncomp) const
{
  if (le_champ().nb_comp() == 1)
    {
      Cerr<<"Champ_Face_VDF_implementation::valeur_aux_sommets_compo"<<finl;
      Cerr <<"A scalar field cannot be of Champ_Face type." << finl;
      Process::exit();
    }
  assert(le_champ().valeurs().line_size() == 1); // not compatible with multiphase

  int nb_elem_tot = dom.nb_elem_tot();
  int nb_som = dom.nb_som();
  int nb_som_elem = dom.nb_som_elem();
  IntVect compteur(nb_som);
  int num_elem,num_som,j;
  ch_som = 0;
  compteur = 0;

  DoubleVect position(Objet_U::dimension);
  for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
    for (j=0; j<nb_som_elem; j++)
      {
        num_som = dom.sommet_elem(num_elem,j);
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

  return ch_som;
}

DoubleTab& Champ_Face_VDF_implementation::remplir_coord_noeuds(DoubleTab& positions) const
{
  const Domaine_VDF& le_dom_vdf = ref_cast(Domaine_VDF,get_domaine_dis());
  const DoubleTab& xv = le_dom_vdf.xv();
  int nb_fac = le_dom_vdf.nb_faces_tot();
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

int Champ_Face_VDF_implementation::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                                 IntVect& polys) const
{
  const IntTab& face_voisins = domaine_vdf().face_voisins();
  int nb_faces=domaine_vdf().nb_faces();
  remplir_coord_noeuds(positions);
  polys.resize(nb_faces);

  for(int face=0; face<nb_faces; face++)
    if( (polys(face)=face_voisins(face,0)) == -1)
      polys(face)=face_voisins(face,1);
  return 1;
}

int Champ_Face_VDF_implementation::imprime_Face(Sortie& os, int ncomp) const
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
              n2=std::min(ni,n2+cmax);
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
          n2=std::min(ni,n2+cmax);
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

DoubleTab& Champ_Face_VDF_implementation::trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const
{
  assert(distant==0);
  const Front_VF& fr_vf=ref_cast(Front_VF, fr);
  const Domaine_VDF& zvdf=domaine_vdf();
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
