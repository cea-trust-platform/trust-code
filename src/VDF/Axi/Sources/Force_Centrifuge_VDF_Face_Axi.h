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
// File:        Force_Centrifuge_VDF_Face_Axi.h
// Directory:   $TRUST_ROOT/src/VDF/Axi/Sources
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Force_Centrifuge_VDF_Face_Axi_included
#define Force_Centrifuge_VDF_Face_Axi_included

#include <TRUSTTabs_forward.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Ref_Champ_Face.h>
#include <Ref_Zone_VDF.h>
#include <Source_base.h>


//
// .DESCRIPTION class Force_Centrifuge_VDF_Face_Axi
//
// Cette classe represente la force centrifuge
// a prendre en compte dans les equations de Navier-Stokes
// en coordonnees cylindriques pour une dicretisation VDF
//

// .SECTION voir aussi
// Source_base
//

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Force_Centrifuge_VDF_Face_Axi
//
//
//////////////////////////////////////////////////////////////////////////////

class Force_Centrifuge_VDF_Face_Axi : public Source_base
{

  Declare_instanciable(Force_Centrifuge_VDF_Face_Axi);

public:

  void associer_pb(const Probleme_base& ) override;
  DoubleTab& calculer(DoubleTab& ) const override ;
  void completer() override;
  void mettre_a_jour(double temps) override
  {
    ;
  }

  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;
  inline int has_interface_blocs() const override
  {
    return 1;
  };

protected:

  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  REF(Champ_Face) la_vitesse;

  IntTab elem_faces;
  IntVect orientation;
  DoubleTab xp;
  DoubleTab xv;
  DoubleVect volume_entrelaces;
  DoubleVect porosite_surf;

  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) override;

};

#endif
