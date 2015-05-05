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
// File:        Raccord_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Raccord_base_included
#define Raccord_base_included


#include <Frontiere.h>

class Domaine;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Raccord_base
//    Cette classe est simplement une frontiere, c'est la classe de base de la
//    hierarchie des raccords. Un raccord intervient dans Trio-U lorsque
//    l'on resoud des problemes differents sur des domaines differents
//    mais que l'on desire les coupler. On couple alors ces problemes avec
//    des raccords.
//
//    Si les problemes sont resolus dans Trio-U les raccords sont dit
//    locaux, si un probleme est resolu dans Trio-U et l'autre dans un
//    autre code le raccord est dit distant.
//    Cette distinction donne lieu a 2 classes derivees de Raccord_base:
//           Raccord_local et Raccord_distant
// .SECTION voir aussi
//    Frontiere Raccord Raccord_distant Raccord_local
//////////////////////////////////////////////////////////////////////////////
class Raccord_base : public Frontiere
{
  Declare_base(Raccord_base);
public:
private :
};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Raccord_local
//    Cette classe represente un raccord entre 2 probleme resolus
//    par Trio-U.
// .SECTION voir aussi
//    Raccord_base Raccord Raccord_distant Raccord
//////////////////////////////////////////////////////////////////////////////
class Raccord_local : public Raccord_base
{
  Declare_base(Raccord_local);
public:
private :
};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Raccord_distant
//    Cette classe represente un raccord entre 2 probleme dont un est
//    resolu par Trio-U et l'autre par un autre code
// .SECTION voir aussi
//    Raccord_base Raccord Raccord_local
//////////////////////////////////////////////////////////////////////////////
class Raccord_distant : public Raccord_base
{
  Declare_base(Raccord_distant);
public:
private :
};

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Raccord_local_homogene
//    Cette classe represente un Raccord_local ou les maillages de part et
//    d'autres coincident.
// .SECTION voir aussi
//    Raccord_base Raccord Raccord_distant Raccord_local
//////////////////////////////////////////////////////////////////////////////
class Raccord_local_homogene : public Raccord_local
{
  Declare_instanciable(Raccord_local_homogene);
public:
private :
};

#endif
