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
// File:        Champ_front_contact_fictif_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_contact_fictif_VEF_included
#define Champ_front_contact_fictif_VEF_included

#include <Champ_front_contact_VEF.h>

//class Equation_base;
//class Milieu_base;
//class Zone_dis_base;
//class Zone_Cl_dis_base;
//class Front_dis_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_contact_fictif_VEF
//     Classe derivee de Champ_front_contact_VEF elle-meme derivant de
//        Champ_front_var qui represente les
//     champs a la frontiere obtenus en prenant la trace
//     d'un objet de type Champ_Inc (champ inconnue d'une equation)
// .SECTION voir aussi
//     Champ_front_var_instationnaire Champ_Inc
//////////////////////////////////////////////////////////////////////////////
class Champ_front_contact_fictif_VEF : public Champ_front_contact_VEF
{

  Declare_instanciable(Champ_front_contact_fictif_VEF);

public:

  void mettre_a_jour(double temps);


protected :

  double conduct_fictif; // conductivite du solide fictif
  double ep_fictif; // epaisseur du solide fictif

};

#endif
