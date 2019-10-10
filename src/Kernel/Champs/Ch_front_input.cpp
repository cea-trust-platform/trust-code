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
// File:        Ch_front_input.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Ch_front_input.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Interprete.h>
#include <Domaine.h>
#include <Frontiere_dis_base.h>
#include <communications.h>
#include <Convert_ICoCoTrioField.h>

Implemente_instanciable(Ch_front_input,"Ch_front_input",Ch_front_var_instationnaire_dep);


Entree& Ch_front_input::readOn(Entree& is)
{
  sous_zone_ok=false;
  read(is);
  mon_pb->addInputField(*this);
  return is;
}

Sortie& Ch_front_input::printOn(Sortie& os) const
{
  return os;
}

void Ch_front_input::set_nb_comp(int i)
{
  fixer_nb_comp(i);
}

void Ch_front_input::set_name(const Nom& name)
{
  nommer(name);
}

const Nom& Ch_front_input::get_name() const
{
  return le_nom();
}

// Description
// Provides afield with a name, a time interval, components, and no field ownership.
// The geometry includes all nodes of the domain and all faces of the boundary.
// WEC : optimization possible here
void Ch_front_input::getTemplate(TrioField& afield) const
{

  afield.clear();
  afield.setName(le_nom().getChar());

  afield._time1=mon_pb->presentTime();
  afield._time2=mon_pb->futureTime();

  afield._nb_field_components=nb_comp();

  int org=1;
  if (faces_.dimension(1)==4) org=0;
  org=0;
  // en mettant org a o1 on revient a l'ancien codage


  const DoubleTab& sommets=org==1?mon_pb->domaine().les_sommets():sommets_;
  const IntTab& faces=org==1?frontiere_dis().frontiere().faces().les_sommets():faces_;

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

// In the case of a mobile mesh (ALE frame) we need to call this function after each displacement of the mesh
void Ch_front_input::buildSommetsFaces() const
{
  const DoubleTab& sommets_org=(mon_pb->domaine().les_sommets());
  DoubleTab& sommets=sommets_;
  sommets.resize(sommets_org.dimension_tot(0),sommets_org.dimension(1));
  sommets.set_smart_resize(1);

  IntTab& faces=faces_;
  const IntTab& faces_org=(frontiere_dis().frontiere().faces().les_sommets());
  faces=faces_org;
  int ntot=0;
  ArrOfInt marqueur(sommets_org.dimension(0));
  marqueur=-1;
  for (int f=0; f<faces.dimension(0); f++)
    {
      for (int s=0; s<faces.dimension(1); s++)
        {
          int som=faces(f,s);
          if (som >= 0 && marqueur(som)==-1)
            {

              marqueur(som)=ntot;
              for (int dir=0; dir<dimension; dir++)
                {
                  sommets(ntot,dir)=sommets_org(som,dir);
                }
              ntot++;
            }
        }
    }
  sommets.resize(ntot,dimension);


  // on refait les faces
  for (int f=0; f<faces.dimension(0); f++)
    for (int s=0; s<faces.dimension(1); s++)
      faces(f,s) = faces_org(f, s) >= 0 ? marqueur(faces_org(f,s)) : -1;

}

// Description
// Uses the first value in afield as uniform value, regardless of geometry.
void Ch_front_input::setValue(const TrioField& afield)
{
  for (int i=1; i<les_valeurs->nb_cases(); i++)
    Champ_Input_Proto::setValueOnTab(afield,les_valeurs[i].valeurs());
  Gpoint(afield._time1,afield._time2);
}
int  Ch_front_input::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Ch_front_var_instationnaire_dep::initialiser(temps,inco))
    return 0;

  /*
    if (nb_comp()==1)
    for (int i=0;i<les_valeurs->nb_cases();i++)
    les_valeurs[i].valeurs()=9.9e5;
  */

  buildSommetsFaces();

  return 1;
}

