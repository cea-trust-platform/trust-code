/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Octree_Double.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Octree_Double_included
#define Octree_Double_included
#include <Octree_Int.h>
#include <TRUSTArray.h>
#include <TRUSTTab.h>

// .DESCRIPTION : Un octree permettant de chercher dans l'espace des elements ou des points
//  decrits par des coordonnees reeles. Cet objet est base sur Octree_Int.
//  Astuces:
//   Pour chercher des points a epsilon pres on peut faire:
//    1)  build_nodes(coord, include_virt, 0.)
//      suivi de
//        search_elements_box(center, epsilon, elements);
//
//    2)  build_nodes(coord, include_virt, epsilon)
//      suivi de
//        search_elements(x,y,z,...)
//    La premiere solution prend plus de temps pour construire l'octree mais la recherche est plus rapide
//    La deuxieme, c'est inverse... et on peut choisir epsilon pour chaque point.
class Octree_Double
{
public:
  Octree_Double();
  void   reset();
  void   build_elements(const DoubleTab& coords, const IntTab& elements,
                        const double epsilon, const int include_virtual);
  void   build_nodes(const DoubleTab& coords, const int include_virtual, const double epsilon = 0.);
  int search_elements(double x, double y, double z, int& index) const;
  int search_elements_box(double xmin, double ymin, double zmin,
                          double xmax, double ymax, double zmax,
                          ArrOfInt& elements) const;
  static int search_nodes_close_to(double x, double y, double z,
                                   const DoubleTab& coords, ArrOfInt& node_list,
                                   double epsilon);
  int search_elements_box(const ArrOfDouble& center, const double radius,
                          ArrOfInt& elements) const;
  static int search_nodes_close_to(const ArrOfDouble& point,
                                   const DoubleTab& coords, ArrOfInt& node_list,
                                   double epsilon);
  int dimension() const
  {
    assert(dim_ > 0);
    return dim_;
  }
  inline const ArrOfInt& floor_elements() const
  {
    return octree_int_.floor_elements();
  };
protected:
  inline int integer_position(double x, int direction, int& ix) const;
  inline int integer_position_clip(double xmin, double xmax,
                                   int& x0, int& x1,
                                   int direction) const;
  void compute_origin_factors(const DoubleTab& coords,
                              const double epsilon,
                              const int include_virtual);

  Octree_Int octree_int_;
  // Ces deux tableaux sont toujours de taille 3 par commodite
  ArrOfDouble origin_;
  ArrOfDouble factor_;
  int dim_;
};
#endif
