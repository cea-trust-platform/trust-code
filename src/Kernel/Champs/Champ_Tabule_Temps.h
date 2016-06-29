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
// File:        Champ_Tabule_Temps.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Tabule_Temps_included
#define Champ_Tabule_Temps_included



#include <Champ_Uniforme_inst.h>
#include <Table.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Champ_Tabule_Temps
//    Cette classe derivee de Champ_Uniforme_inst qui represente
//    un champ constant dans l'espace et tabule en temps.
//    La valeur du champ a un instant quelconque est calcule par interpolation
//    lineaire a partir de la table des valeurs.
// .SECTION voir aussi
//     Champ_Uniforme_inst
//////////////////////////////////////////////////////////////////////////////
class Champ_Tabule_Temps : public Champ_Uniforme_inst
{

  Declare_instanciable(Champ_Tabule_Temps);

public:

  void me_calculer(double t);

private:

  Table la_table;
};


#endif

