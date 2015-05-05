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
// File:        Champ_Q4_implementation.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Q4_implementation.h>
#include <Champ_base.h>
#include <Domaine.h>

DoubleVect& Champ_Q4_implementation::valeur_a_elem(const DoubleVect& position, DoubleVect& val, int num_elem) const
{
  val = 0.;
  const Champ_base& cha=le_champ();
  const DoubleTab& ch = cha.valeurs();
  int nb_compo_=cha.nb_comp();

  int j,ncomp, num_som,nb_som_elem = zone().nb_som_elem();
  if (nb_compo_ == 1)
    {
      for (j=0 ; j<nb_som_elem ; j++)
        {
          num_som = zone().sommet_elem(num_elem,j);
          val(0) += ch(num_som);
        }
      val(0) /= nb_som_elem;
    }
  else
    {
      for (ncomp=0 ; ncomp<nb_compo_ ; ncomp++)
        {
          for (j=0 ; j<nb_som_elem ; j++)
            {
              num_som = zone().sommet_elem(num_elem,j);
              val(ncomp) += ch(num_som,ncomp);
            }
          val(ncomp) /= nb_som_elem;
        }
    }

  return val;
}
double Champ_Q4_implementation::valeur_a_elem_compo(const DoubleVect& position, int num_elem, int ncomp) const
{
  double val = 0.;
  const Champ_base& cha=le_champ();
  const DoubleTab& ch = cha.valeurs();
  int nb_compo_=cha.nb_comp();

  int j, num_som,nb_som_elem = zone().nb_som_elem();
  if (nb_compo_ == 1)
    {
      assert(ncomp==0);
      for (j=0 ; j<nb_som_elem ; j++)
        {
          num_som = zone().sommet_elem(num_elem,j);
          val += ch(num_som);
        }
      val /= nb_som_elem;
    }
  else
    {
      for (j=0 ; j<nb_som_elem ; j++)
        {
          num_som = zone().sommet_elem(num_elem,j);
          val += ch(num_som,ncomp);
        }
      val /= nb_som_elem;
    }


  return val;
}

DoubleTab& Champ_Q4_implementation::valeur_aux_elems(const DoubleTab&,const IntVect& les_elems,DoubleTab& val) const
{
  val = 0.;
  const Champ_base& cha=le_champ();
  const DoubleTab& ch = cha.valeurs();
  int nb_compo_=cha.nb_comp();

  int j,ncomp, num_som,nb_som_elem = zone().nb_som_elem(), ielem, num_elem,nb_elems=les_elems.size();
  assert(val.size()==nb_elems);
  if (nb_compo_ == 1)
    {
      for (ielem=0 ; ielem<nb_elems ; ielem++)
        {
          num_elem = les_elems(ielem);
          for (j=0 ; j<nb_som_elem ; j++)
            {
              num_som = zone().sommet_elem(num_elem,j);
              val(ielem,0) += ch(num_som);
            }
          val(ielem,0) /= nb_som_elem;
        }
    }
  else
    {
      for (ielem=0 ; ielem<nb_elems ; ielem++)
        {
          num_elem = les_elems(ielem);
          for (ncomp=0 ; ncomp<nb_compo_ ; ncomp++)
            {
              for (j=0 ; j<nb_som_elem ; j++)
                {
                  num_som = zone().sommet_elem(num_elem,j);
                  val(ielem,ncomp) += ch(num_som,ncomp);
                }
              val(ielem,ncomp) /= nb_som_elem;
            }
        }
    }

  return val;
}


DoubleVect& Champ_Q4_implementation::valeur_aux_elems_compo(const DoubleTab&,const IntVect& les_elems,DoubleVect& val,int ncomp) const
{
  val = 0.;
  const Champ_base& cha=le_champ();
  const DoubleTab& ch = cha.valeurs();
  int nb_compo_=cha.nb_comp();

  int j, num_som,nb_som_elem = zone().nb_som_elem(), ielem, num_elem,nb_elems=les_elems.size();
  assert(val.size()==nb_elems);
  if (nb_compo_ == 1)
    {
      assert(ncomp==0);
      for (ielem=0 ; ielem<nb_elems ; ielem++)
        {
          num_elem = les_elems(ielem);
          for (j=0 ; j<nb_som_elem ; j++)
            {
              num_som = zone().sommet_elem(num_elem,j);
              val(ielem) += ch(num_som);
            }
          val(ielem) /= nb_som_elem;
        }
    }
  else
    {
      for (ielem=0 ; ielem<nb_elems ; ielem++)
        {
          num_elem = les_elems(ielem);
          for (j=0 ; j<nb_som_elem ; j++)
            {
              num_som = zone().sommet_elem(num_elem,j);
              val(ielem) += ch(num_som,ncomp);
            }
          val(ielem) /= nb_som_elem;
        }
    }

  return val;
}


DoubleTab& Champ_Q4_implementation::valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  const DoubleTab& ch = cha.valeurs();
  //val = cha.valeurs();
  int nb_compo_=cha.nb_comp();

  if (nb_compo_==1)
    {
      int isom,nbsom = ch.size();
      for (isom=0 ; isom<nbsom ; isom++)
        {
          val(isom,0) = ch(isom);
        }
    }
  else
    {
      int isom,nbsom = ch.size();
      for (int ncomp=0 ; ncomp<nb_compo_ ; ncomp++)
        {
          for (isom=0 ; isom<nbsom ; isom++)
            {
              val(isom,ncomp) = ch(isom,ncomp);
            }
        }
    }
  return val;
}
DoubleVect& Champ_Q4_implementation::valeur_aux_sommets_compo(const Domaine& dom, DoubleVect& val, int ncomp) const
{
  const Champ_base& cha=le_champ();
  const DoubleTab& ch = cha.valeurs();
  int nb_compo_=cha.nb_comp();

  if (nb_compo_==1)
    {
      assert(ncomp==0);
      int isom,nbsom = ch.size();
      for (isom=0 ; isom<nbsom ; isom++)
        {
          val(isom) = ch(isom);
        }
    }
  else
    {
      int isom,nbsom = ch.size();
      for (isom=0 ; isom<nbsom ; isom++)
        {
          val(isom) = ch(isom,ncomp);
        }
    }
  return val;
}

DoubleTab& Champ_Q4_implementation::remplir_coord_noeuds(DoubleTab& positions) const
{
  const DoubleTab& coordsom = zone().domaine().coord_sommets();
  positions = coordsom;
  return positions;
}

int Champ_Q4_implementation::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                           IntVect& polys) const
{
  Cerr<<"Champ_Q4_implementation::remplir_coord_noeuds_et_polys not implemented"<<finl;
  assert(0);

  int nb_elem=zone().nb_elem();
  polys.resize(nb_elem);
  for(int i=0; i<nb_elem; i++)
    polys(i)=i;
  return 1;
}

int Champ_Q4_implementation::imprime_Q4(Sortie& os, int ncomp) const
{
  const Champ_base& cha=le_champ();
  int nb_compo_=cha.nb_comp();
  const Zone& la_zone=zone();
  int nb_som_tot = la_zone.nb_som_tot();
  const DoubleTab& val = cha.valeurs();
  int som;
  // On recalcule les centres de gravite :
  const DoubleTab pos_som = la_zone.domaine().coord_sommets();
  os << nb_som_tot << finl;
  for (som=0; som<nb_som_tot; som++)
    {
      if (Objet_U::dimension==3)
        os << pos_som(som,0) << " " << pos_som(som,1) << " " << pos_som(som,2) << " " ;
      if (Objet_U::dimension==2)
        os << pos_som(som,0) << " " << pos_som(som,1) << " " ;
      if (nb_compo_ == 1)
        os << val(som) << finl;
      else
        os << val(som,ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_Q4_implementation::imprime_Q4 END >>>>>>>>>> " << finl;
  return 1;
}
