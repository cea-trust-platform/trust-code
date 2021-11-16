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
// File:        ConstDoubleTab_parts.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////
#include <ConstDoubleTab_parts.h>
#include <MD_Vector_composite.h>
#include <IntTab.h>

// GF je retire le template car insure se plaint

static void init_parts(DoubleVect& vect, VECT(DoubleTab) & parts, DoubleTab *dummy_type_ptr)
{
  const MD_Vector& md = vect.get_md_vector();
  if (! md.non_nul() || !sub_type(MD_Vector_composite, md.valeur()))
    {
      // Ce n'est pas un tableau a plusieurs sous-parties, on cree juste une
      //  partie qui pointe sur vect.
      parts.dimensionner(1);
      DoubleTab& part = parts[0];
      // On preserve le 'shape' du tableau de depart, si c'est un DoubleTab...
      if (sub_type(DoubleTab, vect))
        part.ref(ref_cast(DoubleTab, vect));
      else
        part.ref(vect);
    }
  else
    {

      ArrOfInt shape;
      const int line_size = vect.line_size();
      if (sub_type(DoubleTab, vect))
        {
          const DoubleTab& tab = ref_cast(DoubleTab, vect);
          const int n = tab.nb_dim();
          shape.resize_array(n);
          for (int i = 0; i < n; i++)
            shape[i] = tab.dimension_tot(i);
        }
      else
        {
          if (line_size == 1)
            {
              shape.resize_array(1);
              shape[0] = vect.size_totale();
            }
          else
            {
              shape.resize_array(2);
              shape[0] = vect.size_totale() / line_size;
              shape[1] = line_size;
            }
        }

      const MD_Vector_composite& mdata = ref_cast(MD_Vector_composite, md.valeur());
      const int n = mdata.data_.size();
      parts.dimensionner(n);
      for (int i = 0, j; i < n; i++)
        {
          ArrOfInt shape_i;
          if (mdata.shapes_[i] == 0) shape_i = shape; //si mdata.shapes_[i] > 0, alors la sous-partie a une dimension mineure en plus
          else for (shape_i.resize(shape.size_array() + 1), shape_i(1) = mdata.shapes_[i], j = 1; j < shape.size_array(); j++) shape_i(j + 1) = shape(j);
          const int offset = mdata.parts_offsets_[i];
          const MD_Vector& md_part = mdata.data_[i];
          shape_i[0] = md_part.valeur().get_nb_items_tot();
          DoubleTab& part = parts[i];
          // Fait pointer la zone de memoire sur le sous-tableau (pour l'instant tableau monodimensionnel)
          part.ref_array(vect, offset * line_size, shape_i[0] * line_size * std::max(mdata.shapes_[i], 1));
          // Change le "shape" du tableau pour mettre le nombre de lignes et de colonnes
          // (nombre total d'items inchange, donc resize autorise)
          part.resize(shape_i);
          // Associe le md_vector
          part.set_md_vector(md_part);
        }
    }
}
static void init_parts(IntVect& vect, VECT(IntTab) & parts, IntTab *dummy_type_ptr)
{
  const MD_Vector& md = vect.get_md_vector();
  if (! md.non_nul() || !sub_type(MD_Vector_composite, md.valeur()))
    {
      // Ce n'est pas un tableau a plusieurs sous-parties, on cree juste une
      //  partie qui pointe sur vect.
      parts.dimensionner(1);
      IntTab& part = parts[0];
      // On preserve le 'shape' du tableau de depart, si c'est un DoubleTab...
      if (sub_type(IntTab, vect))
        part.ref(ref_cast(IntTab, vect));
      else
        part.ref(vect);
    }
  else
    {

      ArrOfInt shape;
      const int line_size = vect.line_size();
      if (sub_type(IntTab, vect))
        {
          const IntTab& tab = ref_cast(IntTab, vect);
          const int n = tab.nb_dim();
          shape.resize_array(n);
          for (int i = 0; i < n; i++)
            shape[i] = tab.dimension_tot(i);
        }
      else
        {
          if (line_size == 1)
            {
              shape.resize_array(1);
              shape[0] = vect.size_totale();
            }
          else
            {
              shape.resize_array(2);
              shape[0] = vect.size_totale() / line_size;
              shape[1] = line_size;
            }
        }

      const MD_Vector_composite& mdata = ref_cast(MD_Vector_composite, md.valeur());
      const int n = mdata.data_.size();
      parts.dimensionner(n);
      for (int i = 0, j; i < n; i++)
        {
          ArrOfInt shape_i;
          if (mdata.shapes_[i] == 0) shape_i = shape; //si mdata.shapes_[i] > 0, alors la sous-partie a une dimension mineure en plus
          else for (shape_i.resize(shape.size_array() + 1), shape_i(1) = mdata.shapes_[i], j = 1; j < shape.size_array(); j++) shape_i(j + 1) = shape(j);
          const int offset = mdata.parts_offsets_[i];
          const MD_Vector& md_part = mdata.data_[i];
          shape_i[0] = md_part.valeur().get_nb_items_tot();
          IntTab& part = parts[i];
          // Fait pointer la zone de memoire sur le sous-tableau (pour l'instant tableau monodimensionnel)
          part.ref_array(vect, offset * line_size, shape_i[0] * line_size * std::max(mdata.shapes_[i], 1));
          // Change le "shape" du tableau pour mettre le nombre de lignes et de colonnes
          // (nombre total d'items inchange, donc resize autorise)
          part.resize(shape_i);
          // Associe le md_vector
          part.set_md_vector(md_part);
        }
    }
}

ConstDoubleTab_parts::ConstDoubleTab_parts(const DoubleVect& vect)
{
  // Cast en non const: pour remplir les ref dans parts_.
  // Pas grave, ensuite les sous parties ne seront accessibles qu'en const de l'exterieur.
  DoubleVect& vect2=ref_cast_non_const(DoubleVect, vect);
  init_parts( vect2, parts_, (DoubleTab*) 0);
}

void ConstDoubleTab_parts::initialize(const DoubleVect& vect)
{
  reset();
  // Cast en non const: pour remplir les ref dans parts_.
  // Pas grave, ensuite les sous parties ne seront accessibles qu'en const de l'exterieur.
  DoubleVect& vect2=ref_cast_non_const(DoubleVect, vect);
  init_parts( vect2, parts_, (DoubleTab*) 0);
}

void ConstDoubleTab_parts::reset()
{
  parts_.dimensionner_force(0);
}

DoubleTab_parts::DoubleTab_parts(DoubleVect& vect)
{
  init_parts(vect, parts_, (DoubleTab*) 0);
}

void DoubleTab_parts::initialize(DoubleVect& vect)
{
  reset();
  init_parts(vect, parts_, (DoubleTab*) 0);
}

void DoubleTab_parts::reset()
{
  parts_.dimensionner_force(0);
}


ConstIntTab_parts::ConstIntTab_parts(const IntVect& vect)
{
  // Cast en non const: pour remplir les ref dans parts_.
  // Pas grave, ensuite les sous parties ne seront accessibles qu'en const de l'exterieur.
  IntVect& vect2=ref_cast_non_const(IntVect, vect);
  init_parts(vect2, parts_, (IntTab*) 0);
}

void ConstIntTab_parts::initialize(const IntVect& vect)
{
  reset();
  // Cast en non const: pour remplir les ref dans parts_.
  // Pas grave, ensuite les sous parties ne seront accessibles qu'en const de l'exterieur.
  IntVect& vect2=ref_cast_non_const(IntVect, vect);
  init_parts(vect2, parts_, (IntTab*) 0);
}

void ConstIntTab_parts::reset()
{
  parts_.dimensionner_force(0);
}

IntTab_parts::IntTab_parts(IntVect& vect)
{
  init_parts(vect, parts_, (IntTab*) 0);
}

void IntTab_parts::initialize(IntVect& vect)
{
  reset();
  init_parts(vect, parts_, (IntTab*) 0);
}

void IntTab_parts::reset()
{
  parts_.dimensionner_force(0);
}

// Description:
// ATTENTION: les operateurs suivants sont interdits !
// Si une classe contient ceci:
//  class Toto {
//    DoubleVect x;
//    ConstDoubleTab_parts x_parts; // pointe sur x
//  };
// Alors l'operateur= et le constructeur par copie devraient faire pointer
// la classe x_parts sur la copie de x. Ce n'est pas possible avec un
// constructeur par defaut qui appellerait les methodes ci-dessous.
// Donc pour empecher une erreur, j'interdit ces methodes !
// (note: probleme identique avec les REF, mais en general on n'a aucun
// interet a mettre une REF qui pointe a l'interieur de la classe...)
ConstDoubleTab_parts& ConstDoubleTab_parts::operator=(const ConstDoubleTab_parts&)
{
  Cerr << "Internal error: call to ConstDoubleTab_parts::operator=(const ConstDoubleTab_parts &) is forbidden" << finl;
  Process::exit();
  return *this;
}

// Description: Interdit !
ConstDoubleTab_parts::ConstDoubleTab_parts(const ConstDoubleTab_parts&)
{
  Cerr << "Internal error: call to copy constructor ConstDoubleTab_parts::ConstDoubleTab_parts(const ConstDoubleTab_parts &) is forbidden" << finl;
  Process::exit();
}

// Description: Interdit !
DoubleTab_parts& DoubleTab_parts::operator=(const DoubleTab_parts&)
{
  Cerr << "Internal error: call to DoubleTab_parts::operator=(const DoubleTab_parts &) is forbidden" << finl;
  Process::exit();
  return *this;
}

// Description: Interdit !
DoubleTab_parts::DoubleTab_parts(const DoubleTab_parts&)
{
  Cerr << "Internal error: call to copy constructor DoubleTab_parts::DoubleTab_parts(const DoubleTab_parts &) is forbidden" << finl;
  Process::exit();
}

// Description: Interdit !
ConstIntTab_parts& ConstIntTab_parts::operator=(const ConstIntTab_parts&)
{
  Cerr << "Internal error: call to ConstIntTab_parts::operator=(const ConstIntTab_parts &) is forbidden" << finl;
  Process::exit();
  return *this;
}

// Description: Interdit !
ConstIntTab_parts::ConstIntTab_parts(const ConstIntTab_parts&)
{
  Cerr << "Internal error: call to copy constructor ConstIntTab_parts::ConstIntTab_parts(const ConstIntTab_parts &) is forbidden" << finl;
  Process::exit();
}

// Description: Interdit !
IntTab_parts& IntTab_parts::operator=(const IntTab_parts&)
{
  Cerr << "Internal error: call to IntTab_parts::operator=(const IntTab_parts &) is forbidden" << finl;
  Process::exit();
  return *this;
}

// Description: Interdit !
IntTab_parts::IntTab_parts(const IntTab_parts&)
{
  Cerr << "Internal error: call to copy constructor IntTab_parts::IntTab_parts(const IntTab_parts &) is forbidden" << finl;
  Process::exit();
}

