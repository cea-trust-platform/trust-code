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
// File:        LecFicDiffuse.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <LecFicDiffuse.h>
#include <communications.h>

Implemente_instanciable_sans_constructeur(LecFicDiffuse,"LecFicDiffuse",Lec_Diffuse_base);

Entree& LecFicDiffuse::readOn(Entree& s)
{
  throw;
  return s;
}

Sortie& LecFicDiffuse::printOn(Sortie& s) const
{
  throw;
  return s;
}

// LecFicDiffuse::LecFicDiffuse():EFichier()
LecFicDiffuse::LecFicDiffuse()
{
  file_.set_error_action(ERROR_CONTINUE);
}

// Description: ouverture du fichier name. Cette methode
//  doit etre appelee sur tous les processeurs. En cas
//  d'echec : exit()
LecFicDiffuse::LecFicDiffuse(const char* name,
                             IOS_OPEN_MODE mode)
{
  file_.set_error_action(ERROR_CONTINUE);
  int ok = ouvrir(name, mode);
  if (!ok && Process::je_suis_maitre())
    {
      Cerr << "File " << name << " does not exist (LecFicDiffuse)" << finl;
      Process::exit();
    }
}

// Description: Ouverture du fichier. Cette methode doit etre appelee
//  par tous les processeurs du groupe.
// Valeur de retour: 1 si ok, 0 sinon
int LecFicDiffuse::ouvrir(const char* name,
                          IOS_OPEN_MODE mode)
{
  int ok = 0;
  if(Process::je_suis_maitre())
    ok = file_.ouvrir(name, mode);
  envoyer_broadcast(ok, 0);
  return ok;
}

// Description:
istream& LecFicDiffuse::get_istream()
{
  if(!Process::je_suis_maitre())
    {
      Cerr << "Error get_istream (LecFicDiffuse)" << finl;
      Process::exit();
    }
  return file_.get_istream();
}

// Description:
const istream& LecFicDiffuse::get_istream() const
{
  if(!Process::je_suis_maitre())
    {
      Cerr << "Error get_istream (LecFicDiffuse)" << finl;
      Process::exit();
    }
  return file_.get_istream();
}
