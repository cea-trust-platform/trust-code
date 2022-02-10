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
// File:        Reorienter_tetraedres.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Reorienter_tetraedres_included
#define Reorienter_tetraedres_included



//
// .DESCRIPTION class Reorienter_tetra
// Balaye les tetraedres du maillage pour qu'ils soient directs.
//
// .SECTION voir aussi
// Interprete

#include <Interprete_geometrique_base.h>
#include <Domaine.h>

class Zone;

class Reorienter_tetraedres : public Interprete_geometrique_base
{
  Declare_instanciable(Reorienter_tetraedres);

public :

  Entree& interpreter_(Entree&) override;
  void Reorienter(Domaine&) const;

protected :
  enum Sens {DIRECT , INDIRECT};

  Reorienter_tetraedres::Sens test_orientation_tetra(IntTab& les_elems, int ielem, const DoubleTab& coord_sommets) const;
  Reorienter_tetraedres::Sens reorienter_tetra(IntTab& les_elems, int ielem) const;
};

#endif

