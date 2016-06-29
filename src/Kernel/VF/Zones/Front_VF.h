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
// File:        Front_VF.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Front_VF_included
#define Front_VF_included



//
// .DESCRIPTION class Front_VF
//
// .SECTION voir aussi
// Frontiere_dis_base

#include <Frontiere_dis_base.h>
#include <Frontiere.h>

//////////////////////////////////////////////////////////////////////////////////
//
// CLASS: Front_VF
//
/////////////////////////////////////////////////////////////////////////////////

class Front_VF : public Frontiere_dis_base
{
  Declare_instanciable(Front_VF);

public :

  inline int nb_faces() const;                 // Nombre de faces reelles du bord
  inline int nb_faces_tot() const;                // Nombre de faces reelles et virtuelles du bord
  inline int num_premiere_face() const;        // Numero de la premiere face du bord dans la liste des faces
  inline int num_face(const int&) const;        // Renvoie le numero de face de la ieme face du bord
  inline int num_local_face(const int&) const; //Renvoie le numero local de la face (inverse de num_face())

protected:

};

// Fonctions inline

inline int Front_VF::nb_faces() const
{
  return frontiere().nb_faces();
}

inline int Front_VF::nb_faces_tot() const
{
  return frontiere().nb_faces()+frontiere().get_faces_virt().size_array();
}

inline int Front_VF::num_premiere_face() const
{
  return frontiere().num_premiere_face();
}

inline int Front_VF::num_face(const int& ind_face) const
{
  if (ind_face<nb_faces())                 // Face de bord reelle
    return num_premiere_face()+ind_face;
  else                                        // Face de bord virtuelle
    return frontiere().get_faces_virt()(ind_face-nb_faces());
}

inline int Front_VF::num_local_face(const int& ind_global_face) const
{
  if (ind_global_face<num_premiere_face()+nb_faces())
    {
      return ind_global_face-num_premiere_face();
    }
  else
    {
      for (int i=0; i<frontiere().get_faces_virt().size_array(); i++)
        {
          if (frontiere().get_faces_virt()(i)==ind_global_face)
            return i+nb_faces();
        }
    }
  return -1;
}

#endif
