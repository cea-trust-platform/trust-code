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
// File:        Ch_front_input_P1.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Ch_front_input_P1.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Interprete.h>
#include <Domaine.h>
#include <Frontiere_dis_base.h>
#include <communications.h>
#include <Roue.h>
#include <Convert_ICoCoTrioField.h>


Implemente_instanciable(Ch_front_input_P1,"Ch_front_input_P1",Ch_front_input);


Entree& Ch_front_input_P1::readOn(Entree& is)
{
  return Ch_front_input::readOn(is);
}

Sortie& Ch_front_input_P1::printOn(Sortie& os) const
{
  return  Ch_front_input::printOn(os);
}

int  Ch_front_input_P1::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Ch_front_input::initialiser(temps,inco))
    return 0;

  /* mis sans ch_fr_input ...
     const DoubleTab& sommets_org=(mon_pb->domaine().les_sommets());
     DoubleTab& sommets=sommets_;
     sommets=sommets_org;
     sommets.set_smart_resize(1);

     IntTab& faces=faces_;
     faces=(frontiere_dis().frontiere().faces().les_sommets());

     int ntot=0;
     ArrOfInt marqueur(sommets_org.dimension(0));
     marqueur=-1;
     for (int f=0;f<faces.dimension(0);f++)
     {
     for (int s=0;s<faces.dimension(1);s++)
     {
     int som=faces(f,s);
     if (marqueur(som)==-1)
     {

     marqueur(som)=ntot;
     for (int dir=0;dir<dimension;dir++)
     {
     sommets(ntot,dir)=sommets_org(som,dir);
     }
     ntot++;
     }
     }
     }
     sommets.resize(ntot,dimension);


     // on refait les faces
     for (int f=0;f<faces.dimension(0);f++)
     for (int s=0;s<faces.dimension(1);s++)
     faces(f,s)=faces(f,marqueur(s));

  */
  int ntot=sommets_.dimension(0);
  les_valeurs_som->fixer_nb_cases(les_valeurs->nb_cases());
  for (int i=0; i<les_valeurs_som->nb_cases(); i++)
    les_valeurs_som[i].valeurs().resize(ntot,nb_comp());

//Cerr<<" ici "<< les_valeurs->valeurs()<<finl;
  if (initial_value_.size_array()!=0)
    {
      for (int i=0; i<les_valeurs_som->nb_cases(); i++)
        for (int j=0; j<ntot; j++)
          for (int c=0; c<nb_comp(); c++)
            les_valeurs_som[i].valeurs()(j,c)=initial_value_(c);
    }
  ;
  return 1;
}

double Ch_front_input_P1::valeur_au_temps_et_au_point(double temps,int som,double x,double y, double z,int comp) const
{
  const DoubleTab& sommets=sommets_;
  for (int som2=0; som2<sommets.dimension(0); som2++)
    {
      if ((dimension<3)||(est_egal(sommets(som2,2),z)))

        if ((dimension<2)||(est_egal(sommets(som2,1),y)))
          if (est_egal(sommets(som2,0),x))

            return les_valeurs_som[1].valeurs()(som2,comp);

    }

  Cerr<<" point non trouve "<<finl;
  exit();
  return -1.;
}
void Ch_front_input_P1::getTemplate(TrioField& afield) const
{
  afield.clear();
  afield.setName(le_nom().getChar());
  afield._type=1;
  afield._time1=mon_pb->presentTime();
  afield._time2=mon_pb->futureTime();

  afield._nb_field_components=nb_comp();

  const DoubleTab& sommets=sommets_;

  const IntTab& faces=faces_;

  assert(sommets.nb_dim()==2);
  assert(faces.nb_dim()==2);

  afield._space_dim=sommets.dimension(1);
  afield._mesh_dim=afield._space_dim-1;
  afield._nbnodes=sommets.dimension(0);
  afield._nodes_per_elem=::mp_max(faces.dimension(1));
  afield._nb_elems=faces.dimension(0);

  // Includes all the nodes, even those not used in connectivity
  // order is  x y z  x y z  x y z
  affecte_double_avec_doubletab(&afield._coords,sommets);


  assert (sizeof(int)==sizeof(int));
  affecte_int_avec_inttab(&afield._connectivity,faces);

}


void Ch_front_input_P1::setValue(const TrioField& afield)
{
  //  exit();
  //Cerr<<afield._nb_elems<<" cii "<<les_valeurs_som[1].valeurs().dimension(0)<<finl;
  for (int i=1; i<les_valeurs_som->nb_cases(); i++)
    Champ_Input_Proto::setValueOnTab(afield,les_valeurs_som[i].valeurs());
  //Cout<<" IIIIIIII "<<les_valeurs_som[1].valeurs()<<endl;
  Gpoint(afield._time1,afield._time2);
}
