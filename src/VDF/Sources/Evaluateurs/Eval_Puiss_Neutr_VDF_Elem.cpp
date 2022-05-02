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
// File:        Eval_Puiss_Neutr_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Eval_Puiss_Neutr_VDF_Elem.h>
#include <Champ_Uniforme.h>
#include <Champ_Don.h>
#include <Sous_Zone.h>
#include <Zone_VDF.h>
#include <Parser.h>

void Eval_Puiss_Neutr_VDF_Elem::associer_champs(const Champ_Don& Q)
{
  la_puissance = ref_cast(Champ_Uniforme,Q.valeur());
  puissance = Q(0,0);
}

void Eval_Puiss_Neutr_VDF_Elem::associer_repartition(const Nom& n, const Nom& nom_ssz)
{
  const int nb_elem = la_zone->nb_elem();
  const DoubleTab& xp = la_zone->xp();
  const Sous_Zone& ssz = la_zone->zone().domaine().ss_zone(nom_ssz);

  fxyz = n;

  std::string sfxyz(fxyz.getString());
  for (auto & c: sfxyz) c = toupper(c);
  Parser p(sfxyz,3);
  p.addVar("x");
  p.addVar("y");
  p.addVar("z");
  p.parseString();

  rep.resize(nb_elem);
  rep = 0.;
  for(int i=0; i<ssz.nb_elem_tot(); i++) rep(ssz(i)) = 1.;

  for (int i=0; i<nb_elem; i++)
    {
      double x,y,z;
      x = xp(i,0);
      y = xp(i,1);
      if (Objet_U::dimension == 3) z = xp(i,2);
      else z=0.;
      p.setVar("x",x);
      p.setVar("y",y);
      p.setVar("z",z);
      rep(i) *= p.eval();
    }
}

void Eval_Puiss_Neutr_VDF_Elem::mettre_a_jour( )
{
  puissance = la_puissance->valeurs()(0); // on met a jour le tableau de puissance
}

void Eval_Puiss_Neutr_VDF_Elem::completer()
{
  Evaluateur_Source_VDF_Elem::completer();
}
