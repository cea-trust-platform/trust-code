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
// File:        Reordonner_faces_periodiques.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Reordonner_faces_periodiques_included
#define Reordonner_faces_periodiques_included

#include <Interprete_geometrique_base.h>
#include <TRUSTTab.h>
class Frontiere;
class Domaine;

// .DESCRIPTION
//  Cet interprete permet de reordonner les faces d'un bord periodique selon la convention
//  utilisee dans le decoupeur (d'abord les faces d'une extremite du domaine, puis dans le
//  meme ordre, les faces jumelles sur le bord oppose).
class Reordonner_faces_periodiques : public Interprete_geometrique_base
{
public:
  static int reordonner_faces_periodiques(const Domaine& domaine,
                                          IntTab& faces,
                                          const ArrOfDouble& direction_perio,
                                          const double epsilon);

  static void renum_som_perio(const Domaine&,
                              const Noms& liste_bords_periodiques,
                              ArrOfInt& renum_som_perio,
                              const int calculer_espace_virtuel);

  static int check_faces_periodiques(const Frontiere& frontiere,
                                     ArrOfDouble& vecteur_delta,
                                     ArrOfDouble& erreur,
                                     const int verbose = 0);

  static void chercher_direction_perio(ArrOfDouble& direction_perio,
                                       const Domaine& dom,
                                       const Nom& bord);

};
#endif
