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
// File:        Zone_Cl_CoviMAC.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_Cl_CoviMAC_included
#define Zone_Cl_CoviMAC_included



//
// .DESCRIPTION class Zone_Cl_CoviMAC
//  Cette classe porte les tableaux qui servent a mettre en oeuvre
//  les condition aux limites dans la formulation CoviMAC

//
// .SECTION voir aussi
// Zone_Cl_dis_base


#include <Zone_Cl_dis_base.h>
#include <Champ_Don.h>


class Champ_Inc;
class Zone_CoviMAC;
class Matrice_Morse;

class Zone_Cl_CoviMAC : public Zone_Cl_dis_base
{

  Declare_instanciable(Zone_Cl_CoviMAC);

public :

  void associer(const Zone_CoviMAC& );
  void completer(const Zone_dis& );
//  void mettre_a_jour(double );
  virtual int initialiser(double temps);
  virtual void imposer_cond_lim(Champ_Inc&, double);



  int nb_faces_sortie_libre() const;
  Zone_CoviMAC& zone_CoviMAC();
  const Zone_CoviMAC& zone_CoviMAC() const;

  int nb_bord_periodicite() const;
protected:

  // Attributs:

  int modif_perio_fait_;

  // Fonctions de creation des membres prives de la zone:

  void remplir_type_elem_Cl(const Zone_CoviMAC& );
};

//
// Fonctions inline de la classe Zone_Cl_CoviMAC
//





#endif
