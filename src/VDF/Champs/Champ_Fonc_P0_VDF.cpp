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
// File:        Champ_Fonc_P0_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_P0_VDF.h>
#include <LecFicDiffuse.h>

Implemente_instanciable(Champ_Fonc_P0_VDF,"Champ_Fonc_P0_VDF",Champ_Fonc_P0_base);

//     printOn()
/////

Sortie& Champ_Fonc_P0_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Champ_Fonc_P0_VDF::readOn(Entree& s)
{
  return s ;
}

void Champ_Fonc_P0_VDF::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}

double Champ_Fonc_P0_VDF::valeur_au_bord(int face) const
{
  double val_bord;
  const DoubleTab& val = valeurs();
  const Zone_VDF& zone_VDF=la_zone_VDF.valeur();

  int n0 = zone_VDF.face_voisins(face,0);
  if (n0 != -1)
    val_bord = val[n0];
  else
    val_bord = val[zone_VDF.face_voisins(face,1)];
  return val_bord;
}

const Zone_dis_base& Champ_Fonc_P0_VDF::zone_dis_base() const
{
  return la_zone_VDF.valeur();
}

// Description:
// renvoie la moyenne du champ au sens P0 i.e la somme, divisee par
// le volume total du domaine, des valeurs constantes par element
// multipliees par les volumes des elements

DoubleVect Champ_Fonc_P0_VDF::moyenne() const
{
  if (nproc()>1)
    {
      Cerr<<"Champ_Fonc_P0_VDF::moyenne pas //" <<finl;
      exit();
    }
  const Zone_VDF& zvdf=la_zone_VDF.valeur();
  const DoubleVect& porosite = zvdf.porosite_elem();
  const DoubleVect& volumes = zvdf.volumes();
  const DoubleTab& val = valeurs();

  int k,nb_compo = nb_comp();
  DoubleVect moy(nb_compo);
  moy =0;
  double coef,sum_vol=0;

  for (int num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
    {
      coef = porosite(num_elem)*volumes(num_elem);
      for (k=0; k<nb_compo; k++)
        moy[k] += val(num_elem,k)*coef;
      sum_vol += coef;
    }

  moy /= sum_vol;
  return moy;
}

// Description:
// Cette fonction effectue le calcul de la moyenne au sens P0
// de la kieme composante du champ
double Champ_Fonc_P0_VDF::moyenne(int ncomp) const
{
  const Zone_VDF& zvdf=la_zone_VDF.valeur();
  const DoubleVect& porosite = zvdf.porosite_elem();
  const DoubleVect& volumes = zvdf.volumes();
  const DoubleTab& val = valeurs();

  double moy=0;
  double coef,sum_vol=0;

  for (int num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
    {
      coef = porosite(num_elem)*volumes(num_elem);
      moy += val(num_elem,ncomp)*coef;
      sum_vol += coef;
    }
  moy=mp_sum(moy);
  sum_vol=mp_sum(sum_vol);
  moy /= sum_vol;
  return moy;
}

// Description:
// Ecrit le champ sous la forme IJK
int Champ_Fonc_P0_VDF::imprime(Sortie& os, int ncomp) const
{
  // valeur_au_ijk(xi,yj,zk,valeurs,ncomp);
  // principe  : creer un tableau points avec selon si champ_elem ou champ_faces
  // une grille construite a partie de xi,yj,zk lu dans un fichier
  // sub_type(Champ_P0, *this) : grille construite a partir de (xi+xi+1)/2,etc...
  // Sinon : grille construite a partir de xi,xi+1,etc...
  // Boucle sur ni,nj,nk
  // K=
  //     I=1 I=2 I=3 I=4
  // J=10 Vij
  // J=9
  // ...
  int ni,nj,nk=-1;
  int ii,jj,k;
  int np,elem;
  int cmax=7;
  DoubleVect xi,yj,zk;
  DoubleTab Grille;
  //Lecture de xi,yj,zk dans un fichier .xiyjzk
  Nom nomfic(nom_du_cas());
  nomfic+=".xiyjzk";
  LecFicDiffuse ficijk(nomfic);
  ficijk >> xi;
  ni=xi.size();
  ficijk >> yj;
  nj=yj.size();
  if (dimension==3)
    {
      ficijk >> zk;
      nk=zk.size();
    }
  if (ncomp == 1)
    {
      if (dimension==3)
        {
          // Grille ordonnee sur les centres des elements
          np=(ni-1)*(nj-1)*(nk-1);
          Grille.resize(np,dimension);
          for(k=0; k<nk-1; k++)
            for(ii=0; ii<ni-1; ii++)
              for(jj=0; jj<nj-1; jj++)
                {
                  elem=jj+(nj-1)*(ii+k*(ni-1));
                  Grille(elem,0)=0.5*(xi(ii)+xi(ii+1));
                  Grille(elem,1)=0.5*(yj(jj)+yj(jj+1));
                  Grille(elem,2)=0.5*(zk(k)+zk(k+1));
                }
          DoubleTab tab_valeurs(np, nb_compo_);
          valeur_aux(Grille, tab_valeurs);
          for(k=0; k<nk-1; k++)
            {
              os << finl;
              os << "Coupe a K= " << k << finl;
              int n1=0,n2=0;
              while (n2<ni-1)
                {
                  n1=n2;
                  n2=::min(ni-1,n2+cmax);
                  os << finl;
                  os << "I=     ";
                  for(int i=n1; i<n2; i++)
                    os << i << "              ";
                  os << finl;
                  for(int j=nj-2; j>-1; j--)
                    {
                      os << "J= " << j << " ";
                      for(int i=n1; i<n2; i++)
                        {
                          elem=j+(nj-1)*(i+k*(ni-1));
                          os << tab_valeurs(elem,0) << " ";
                        }
                      os << finl;
                    }
                }
            }
        }
      else if (dimension==2)
        {
          np=(ni-1)*(nj-1);
          Grille.resize(np,dimension);
          for(ii=0; ii<ni-1; ii++)
            for(jj=0; jj<nj-1; jj++)
              {
                elem=jj+(nj-1)*ii;
                Grille(elem,0)=0.5*(xi(ii)+xi(ii+1));
                Grille(elem,1)=0.5*(yj(jj)+yj(jj+1));
              }
          DoubleTab tab_valeurs(np, nb_compo_);
          valeur_aux(Grille, tab_valeurs);
          int n1=0,n2=0;
          while (n2<ni-1)
            {
              n1=n2;
              n2=::min(ni-1,n2+cmax);
              os << finl;
              os << "I=     ";
              for(int i=n1; i<n2; i++)
                os << i << "              ";
              os << finl;
              for(int j=nj-2; j>-1; j--)
                {
                  os << "J= " << j << " ";
                  for(int i=n1; i<n2; i++)
                    {
                      elem=j+(nj-1)*i;
                      os << tab_valeurs(elem,0) << " ";
                    }
                  os << finl;
                }
            }
        }
    }
  else
    {
      Cerr << "Champ_P0_implementation::imprime_P0_VDF avec nb_compo_>1 non code." << finl;
      exit();
    }
  return 1;
}
