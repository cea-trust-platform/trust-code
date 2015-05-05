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
// File:        Remove_Invalid_Internal_Boundaries.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Remove_Invalid_Internal_Boundaries.h>
#include <Domaine.h>
#include <Param.h>
#include <Static_Int_Lists.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Scatter.h>

Implemente_instanciable(Remove_Invalid_Internal_Boundaries,"Remove_Invalid_Internal_Boundaries",Interprete_geometrique_base) ;

Sortie& Remove_Invalid_Internal_Boundaries::printOn(Sortie& os) const
{
  Interprete::printOn(os);
  return os;
}

Entree& Remove_Invalid_Internal_Boundaries::readOn(Entree& is)
{
  Interprete::readOn(is);
  return is;
}

Entree& Remove_Invalid_Internal_Boundaries::interpreter_(Entree& is)
{
  associer_domaine(is);

  Domaine& domain = domaine(0);
  Zone&    zone   = domain.zone(0);

  Scatter::uninit_sequential_domain(domain);

  Static_Int_Lists incidence;
  construire_connectivite_som_elem(domain.nb_som_tot(), zone.les_elems(), incidence, 1);

  LIST(Nom) name_of_useless_boundaries;
  LIST(Nom) name_of_useless_connectors;
  ArrOfInt nodes_of_frontier_face;
  ArrOfInt cells_on_frontier_face;
  cells_on_frontier_face.set_smart_resize(1);

  {
    LIST_CURSEUR(Bord) cursor(zone.faces_bord());
    while (cursor)
      {
        const Faces& frontier_faces    = cursor.valeur().faces();
        const int nb_frontier_faces = frontier_faces.nb_faces_tot();
        const int nb_nodes_per_face = frontier_faces.nb_som_faces();

        nodes_of_frontier_face.resize_array(nb_nodes_per_face);
        cells_on_frontier_face.resize_array(0);

        for (int i=0; i<nb_nodes_per_face; ++i)
          {
            nodes_of_frontier_face[i] = frontier_faces.sommet(0,i);
          }
        find_adjacent_elements(incidence,nodes_of_frontier_face,cells_on_frontier_face);

        bool remove_frontier = false;

        if (cells_on_frontier_face.size_array() == 1)
          {
            remove_frontier = false;
          }
        else if (cells_on_frontier_face.size_array() == 2)
          {
            remove_frontier = true;
            Nom dummy;
            Nom& new_useless_frontier_name = name_of_useless_boundaries.add(dummy);
            new_useless_frontier_name = cursor.valeur().le_nom();
          }
        else
          {
            Cerr << "Error in 'Remove_Invalid_Internal_Boundaries::interpreter_()' :" << finl;
            Cerr << "  Invalid incidence" << finl;
            Process::exit();
          }

        for (int f=0; f<nb_frontier_faces; ++f)
          {
            for (int i=0; i<nb_nodes_per_face; ++i)
              {
                nodes_of_frontier_face[i] = frontier_faces.sommet(0,i);
              }
            find_adjacent_elements(incidence,nodes_of_frontier_face,cells_on_frontier_face);

            if (remove_frontier)
              {
                if (cells_on_frontier_face.size_array() != 2)
                  {
                    Cerr << "Error in 'Remove_Invalid_Internal_Boundaries::interpreter_()' :" << finl;
                    Cerr << "  Invalid incidence" << finl;
                    Process::exit();
                  }
              }
            else
              {
                if (cells_on_frontier_face.size_array() != 1)
                  {
                    Cerr << "Error in 'Remove_Invalid_Internal_Boundaries::interpreter_()' :" << finl;
                    Cerr << "  Invalid incidence" << finl;
                    Process::exit();
                  }
              }
          }
        ++cursor;
      }
  }

  {
    LIST_CURSEUR(Raccord) cursor(zone.faces_raccord());
    while (cursor)
      {
        const Faces& frontier_faces    = cursor.valeur().valeur().faces();
        const int nb_frontier_faces = frontier_faces.nb_faces_tot();
        const int nb_nodes_per_face = frontier_faces.nb_som_faces();

        nodes_of_frontier_face.resize_array(nb_nodes_per_face);
        cells_on_frontier_face.resize_array(0);

        for (int i=0; i<nb_nodes_per_face; ++i)
          {
            nodes_of_frontier_face[i] = frontier_faces.sommet(0,i);
          }
        find_adjacent_elements(incidence,nodes_of_frontier_face,cells_on_frontier_face);

        bool remove_frontier = false;

        if (cells_on_frontier_face.size_array() == 1)
          {
            remove_frontier = false;
          }
        else if (cells_on_frontier_face.size_array() == 2)
          {
            remove_frontier = true;
            Nom dummy;
            Nom& new_useless_frontier_name = name_of_useless_connectors.add(dummy);
            new_useless_frontier_name = cursor.valeur().valeur().le_nom();
          }
        else
          {
            Cerr << "Error in 'Remove_Invalid_Internal_Boundaries::interpreter_()' :" << finl;
            Cerr << "  Invalid incidence" << finl;
            Process::exit();
          }

        for (int f=0; f<nb_frontier_faces; ++f)
          {
            for (int i=0; i<nb_nodes_per_face; ++i)
              {
                nodes_of_frontier_face[i] = frontier_faces.sommet(0,i);
              }
            find_adjacent_elements(incidence,nodes_of_frontier_face,cells_on_frontier_face);

            if (remove_frontier)
              {
                if (cells_on_frontier_face.size_array() != 2)
                  {
                    Cerr << "Error in 'Remove_Invalid_Internal_Boundaries::interpreter_()' :" << finl;
                    Cerr << "  Invalid incidence" << finl;
                    Process::exit();
                  }
              }
            else
              {
                if (cells_on_frontier_face.size_array() != 1)
                  {
                    Cerr << "Error in 'Remove_Invalid_Internal_Boundaries::interpreter_()' :" << finl;
                    Cerr << "  Invalid incidence" << finl;
                    Process::exit();
                  }
              }
          }
        ++cursor;
      }
  }

  {
    LIST_CURSEUR(Nom) cursor(name_of_useless_boundaries);
    while (cursor)
      {
        Bords& boundaries = zone.faces_bord();
        boundaries.suppr(zone.bord(cursor.valeur()));
        ++cursor;
      }
  }

  {
    LIST_CURSEUR(Nom) cursor(name_of_useless_connectors);
    while (cursor)
      {
        Raccords& connectors = zone.faces_raccord();
        connectors.suppr(zone.raccord(cursor.valeur()));
        ++cursor;
      }
  }

  Scatter::init_sequential_domain(domain);

  return is;
}
