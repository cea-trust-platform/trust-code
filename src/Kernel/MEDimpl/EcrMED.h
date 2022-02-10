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
// File:        EcrMED.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EcrMED_included
#define EcrMED_included




///////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// Classe EcrMED
//    Ecr un fichier MED
//    Structure du jeu de donnee (en dimension 2) :
//    EcrMED dom medfile
// .SECTION voir aussi
//
//
///////////////////////////////////////////////////////////////////////////
#include <Interprete.h>
#include <Ref_Zone_dis_base.h>
#include <med++.h>

class Domaine;
class Nom;
class Noms;
class DoubleTab;
class Champ_Inc_base;
class Zone_dis_base;
class Domaine;

class EcrMED : public Interprete
{
  Declare_instanciable_sans_constructeur(EcrMED);
public :
  EcrMED();
  ///! Set major mode for MED file writing. See major_mode member below.
  void setMajorMode(bool majorMod)
  {
    major_mode = majorMod;
  };
  bool getMajorMode()
  {
    return major_mode;
  }
  ///! Set use_medcoupling flag
  void setMEDCoupling(bool mc)
  {
    use_medcoupling_ = mc;
  }
  Nom version()
  {
    return major_mode ? _MED_VERSION(MED_NUM_MAJEUR,0,0) : MED_VERSION_STR;
  }
  Entree& interpreter(Entree&) override;
  void ecrire_domaine(const Nom& nom_fic,const Domaine& dom,const Nom& nom_dom,int mode=0);
  void ecrire_domaine_dis(const Nom& nom_fic,const Domaine& dom,const REF(Zone_dis_base)& zone_dis_base,const Nom& nom_dom,int mode=0);
  void ecrire_champ(const Nom& type,const Nom& nom_fic,const Domaine& dom,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Noms& noms_compo,const Nom& type_elem,double time,int compteur);
  void ecrire_champ(const Nom& type,const Nom& nom_fic,const Domaine& dom,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite,const Nom& type_elem,double time,int compteur,const Champ_Inc_base& le_champ);

protected:
  bool use_medcoupling_;

private:
  ///! False by default. If true, the MED file will be written in the major mode of the release version (3.0 for example if current MED version is 3.2)
  bool major_mode;
};
#endif
