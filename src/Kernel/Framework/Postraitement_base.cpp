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
#include <Postraitement_base.h>
#include <Motcle.h>
#include <Param.h>

Implemente_base_sans_constructeur(Postraitement_base,"Postraitement_base",Objet_U);

const char * const Postraitement_base::demande_description = "DESCRIPTION";

Postraitement_base::Postraitement_base() :
  temps_(0.)
{
}

Sortie& Postraitement_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Postraitement_base::readOn(Entree& is)
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" post-processing object "<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Postraitement_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
  exit();
  return -1;
}

void Postraitement_base::associer_nom_et_pb_base(const Nom& nom,
                                                 const Probleme_base& mon_pb)
{
  le_nom_ = nom;
  mon_probleme = mon_pb;
}

const Nom& Postraitement_base::le_nom() const
{
  return le_nom_;
}

void Postraitement_base::mettre_a_jour(double temps)
{
  temps_ = temps;
}

int Postraitement_base::sauvegarder(Sortie& os) const
{
  return 0;
}

int Postraitement_base::reprendre(Entree& is)
{
  return 0;
}
