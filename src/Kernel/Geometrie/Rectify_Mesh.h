/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Rectify_Mesh.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Rectify_Mesh_included
#define Rectify_Mesh_included

#include <Interprete_geometrique_base.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : Rectify_Mesh
// .HEADER      : TRUST_Kernel_Addons TRUST_Kernel_Addons/src/Mesh_Refinement
// .LIBRARY     : libTRUST_Kernel_Addons
// .DESCRIPTION : class Rectify_Mesh
//
// <Description of class Rectify_Mesh>
//
/////////////////////////////////////////////////////////////////////////////

class Rectify_Mesh : public Interprete_geometrique_base
{

  Declare_instanciable(Rectify_Mesh) ;

public :
  virtual Entree& interpreter_(Entree& is);

protected :
  void apply(void);

  void check_dimension(void) const;
  void check_nb_zones(void) const;
  void check_cell_type(void) const;

  void apply_2D(void);
  void apply_3D(void);

  void check_cell_orientation_2D(void);
  void check_cell_orientation_3D(void);

  void check_cell_enumeration_3D(void);

  void swap_nodes(IntTab& cells, int cell, int node0, int node1);

};

#endif /* Rectify_Mesh_included */
