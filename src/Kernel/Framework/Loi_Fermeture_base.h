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
// File:        Loi_Fermeture_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Loi_Fermeture_base_included
#define Loi_Fermeture_base_included
#include <Ref_Probleme_base.h>
#include <Champs_compris_interface.h>
#include <Champs_compris.h>

class Param;
class Champ_base;
class Motcle;
class Discretisation_base;

// .DESCRIPTION: Classe de base des lois de fermetures.
//  Cette classe calcule des champs qui peuvent dependre
//  de plusieurs inconnues, de grandeurs physiques du milieu etc.
//  Les champs de la classe sont rendus accessibles a tout le probleme
//  a travers la methode get_champ() et leur mise a jour est declanchee
//  par le probleme, apres la mise a jour du milieu et des equations.
class Loi_Fermeture_base : public Objet_U, public Champs_compris_interface
{
  Declare_base(Loi_Fermeture_base);
public:
  // Reimplementation de Objet_U
  void nommer(const Nom& nom)
  {
    nom_ = nom;
  }
  const Nom& le_nom () const
  {
    return nom_;
  }
  // Implementation des methodes de Champs_compris_interface:
  virtual void  creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom, Option opt=NONE) const;
  // Nouvelles methodes:
  virtual void associer_pb_base(const Probleme_base&);
  virtual void discretiser(const Discretisation_base& );
  virtual void set_param(Param& param);
  virtual void completer();
  virtual void preparer_calcul();
  virtual void mettre_a_jour(double temps);

protected:
  const Probleme_base& mon_probleme() const;
  Champs_compris champs_compris_;

  enum Status { INITIAL, PB_ASSOCIE, DISCRETISE, READON_FAIT, COMPLET };
  Status status_;


private:
  Nom nom_;
  // Prive car ce membre donne acces au probleme non const. On le cache.
  REF(Probleme_base) mon_probleme_;



};


#endif
