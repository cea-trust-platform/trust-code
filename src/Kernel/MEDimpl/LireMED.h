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
// File:        LireMED.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LireMED_included
#define LireMED_included




///////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// Classe LireMED
//    Lire un fichier MED
//    Structure du jeu de donnee (en dimension 2) :
//    LireMED dom medfile
// .SECTION voir aussi
//
//
///////////////////////////////////////////////////////////////////////////
#include <Interprete_geometrique_base.h>
#include <med++.h>

class Nom;
class Domaine;
class ArrOfDouble;

class LireMED : public Interprete_geometrique_base
{
  Declare_instanciable(LireMED);
public :
  Entree& interpreter_(Entree&);
  void lire_geom(Nom& nom_fic,Domaine& dom,const Nom& nom_dom,const Nom& nom_dom1,int isvef=0, int isfamilyshort=0);
};

// fonctions utiles
inline void med_non_installe()
{
  Cerr << "This version has not been built with MED library." << finl;
  Process::exit();
}
class Char_ptr;
void lire_nom_med(Nom& nom_champ,Entree& is);
void test_version(Nom& nom) ;
void dimensionne_char_ptr_taille(Char_ptr& nom ,int taille_d_un_mot,int nb=1);
void traite_nom_fichier_med(Nom& nom_fic);
void medinfochamp_existe(const Nom& nom_fic,Noms& nomschamp,const Domaine& dom,ArrOfDouble& temps_sauv);
#ifdef MED_
Nom medinfo1champ(const Nom& nom_fic, const char* nomchamp,int& numero,int& nbcomp,int& ndt,med_entity_type& type_ent, med_geometry_type& type_geo,int& size, const Nom& nom_dom,int verifie_type,ArrOfDouble& temps_sauv);
#endif
#endif
