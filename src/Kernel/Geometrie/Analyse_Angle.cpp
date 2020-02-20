/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Analyse_Angle.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Analyse_Angle.h>
#include <Domaine.h>


Implemente_instanciable(Analyse_Angle,"Analyse_Angle",Interprete);


// printOn et readOn

Sortie& Analyse_Angle::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Analyse_Angle::readOn(Entree& is )
{
  //
  // VERIFIER ICI QU'ON A BIEN TOUT LU;
  //
  return is;
}


#include <Linear_algebra_tools_impl.h>

double largest_angle(const DoubleTab& coords)
{
  if (((coords.dimension(0)!=4)&&(coords.dimension(0)!=3))||(coords.dimension(1)!=3))
    {
      Cerr<<" case not provided"<<finl;
      Process::exit();
    }
  int nb_face=coords.dimension(0);
  Vecteur3 normals[4];
  Vecteur3 edge[2],opp;
  edge[1].set(0,0,1);
  for (int n=0; n<nb_face; n++)
    {

      int prem=0;
      if (n==0) prem=1;
      int compteur=0;
      for (int s=0; s<nb_face; s++)
        {
          if ((s!=n) && (s!=prem))
            {
              edge[compteur].set(coords(s,0)-coords(prem,0),
                                 coords(s,1)-coords(prem,1),
                                 coords(s,2)-coords(prem,2));
              compteur++;
            }
        }
      if (compteur!=nb_face-2) abort();
      opp.set(coords(n,0)-coords(prem,0),
              coords(n,1)-coords(prem,1),
              coords(n,2)-coords(prem,2));
      Vecteur3::produit_vectoriel(edge[0],edge[1],normals[n]);
      //normals[n]=edge[0]*edge[1];
      double l = normals[n].length();
      normals[n]*=(l>0 ? 1./l : 0);
      if (Vecteur3::produit_scalaire(normals[n],opp)<0)
        normals[n]*=-1;
    }
  // on a les 4 normals orientes vers l'interieur
  double max_pscal=-100;
  for (int n1=0; n1<nb_face; n1++)
    for (int n2=n1+1; n2<nb_face; n2++)
      {
        double pscal=Vecteur3::produit_scalaire(normals[n1],normals[n2]);
        //min_pscal=pscal;
        if (pscal>max_pscal)
          max_pscal=pscal;
      }
  double tet=acos(max_pscal<=1. ? max_pscal : 1.)/acos(-1.)*180; // acos(-1) ne compile pas avec xlC

  tet=180-tet;
  return tet;
}

Entree& Analyse_Angle::interpreter(Entree& is)
{
  int nb_histo;
  Nom nom_dom;
  is >> nom_dom;
  is >> nb_histo;
  Domaine& dom=ref_cast(Domaine,objet(nom_dom));

  histogramme_angle(dom,Cout,nb_histo);
  return is;
}
void histogramme_angle(const Domaine& dom , Sortie& out,  int nb_histo )
{
  out<<finl<<"Histogram of the largest angle of each element found into the mesh "<<dom.le_nom()<<" :" << finl;
  Motcle type_elem(dom.zone(0).type_elem().valeur().que_suis_je());
  if (((type_elem!="triangle")&& (type_elem!="tetraedre")) || ((type_elem=="triangle") && (Domaine::dimension==3)))
    {
      out<<"Not available for "<<type_elem<<" in dimension "<<Domaine::dimension<<finl;
      return;
    }

  ArrOfInt histo(nb_histo+1);
  int nb_elem=dom.zone(0).nb_elem();
  const DoubleTab& som=dom.les_sommets();
  const IntTab& les_elems=dom.zone(0).les_elems();
  int dim_space=som.dimension(1);
  int nb_som_elem=les_elems.dimension(1);
  DoubleTab coords(nb_som_elem,3);
  for (int elem=0; elem<nb_elem; elem++)
    {
      for (int s=0; s<nb_som_elem; s++)
        {
          int sommet=les_elems(elem,s);
          for (int dir=0; dir<dim_space; dir++)
            coords(s,dir)=som(sommet,dir);

        }
      double teta=largest_angle(coords);

      int test=(int)(teta/180*nb_histo);
      histo(test)++;
      if (test==nb_histo)
        Cerr << "Error, the mesh cell " << elem << " is flat. Fix your mesh." << finl;
    }
  if (histo(nb_histo)>0)
    Process::exit();
  int nb_elem_tot=(int)Process::mp_sum(nb_elem);
  if (nb_elem_tot>0)
    {
      double obtuse_cells_proportion=0;
      for (int h=0; h<nb_histo; h++)
        {
          histo(h)=Process::mp_sum(histo(h));
          // Pas d'angles en dessous de 60 forcement
          if (180/nb_histo*h>=60)
            {
              obtuse_cells_proportion=histo(h)*100./nb_elem_tot;
              int angle=180/nb_histo*h;
              out <<"Between "<<angle<<" degrees and " << 180/nb_histo*(h+1)<<" degrees : "<<histo(h)<<" elements ( "<<obtuse_cells_proportion<< " %)"<<finl;
            }
        }
      // Criteria used on the last bin (170-180 degrees)
      if (obtuse_cells_proportion>20)
        {
          Cerr << finl;
          Cerr << "Warning:" << finl;
          Cerr << "Your mesh has too much obtuse cells for a TRUST calculation." << finl;
          Cerr << "Check the mesh requirements." << finl;
          //Process::exit();
        }
    }
  out << finl;
}
