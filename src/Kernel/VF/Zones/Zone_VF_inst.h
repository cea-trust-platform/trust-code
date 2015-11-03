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
// File:        Zone_VF_inst.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_VF_inst_included
#define Zone_VF_inst_included


#include <math.h>
#include <Zone_VF.h>
#include <Domaine.h>
class Geometrie;

//////////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class Zone_VF_inst
//
//         Classe instanciable qui derive de Zone_VF.
//      Cette classe ne sait rien faire !!!!
//      Elle permet juste d'appeler des methodes de type Champ_Inc_base::affecter avec une zone_dis contenant une reference a une zone.
//
/////////////////////////////////////////////////////////////////////////////////

class Zone_VF_inst : public Zone_VF
{

  Declare_instanciable(Zone_VF_inst);

public :



  inline double face_surfaces(int ) const
  {
    Cerr<<"face_surfaces not coded in "<<que_suis_je()<<finl;
    exit();
    return -1;
  };
  inline double face_normales(int , int ) const
  {
    Cerr<<"face_normales not coded in "<<que_suis_je()<<finl;
    exit();
    return -1;
  };
  inline const IntVect& orientation() const
  {
    exit();
    throw;
    return orientation();
  };

private:

  inline void remplir_elem_faces()
  {
    Cerr<<"remplir_elem_faces not coded in "<<que_suis_je()<<finl;
  };

};



#endif




