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

#include <Terme_Gravite_VDF_Face.h>
#include <Probleme_base.h>
#include <Domaine_Cl_VDF.h>
#include <Domaine_VDF.h>

Implemente_instanciable_sans_constructeur(Terme_Gravite_VDF_Face,"Gravite_VDF_Face",Terme_Source_VDF_base);

Sortie& Terme_Gravite_VDF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Gravite_VDF_Face::readOn(Entree& s)
{
  set_fichier("Gravite");
  set_description("Buoyancy term = Integral(rho*g*dv) [m/s2]");
  return s ;
}

void Terme_Gravite_VDF_Face::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis)
{
  const Domaine_VDF& zvdf = ref_cast(Domaine_VDF,domaine_dis.valeur());
  const Domaine_Cl_VDF& zclvdf = ref_cast(Domaine_Cl_VDF,domaine_cl_dis.valeur());
  iter->associer_domaines(zvdf, zclvdf);
  Eval_Gravite_VDF_Face& eval_grav = static_cast<Eval_Gravite_VDF_Face&> (iter->evaluateur());
  eval_grav.associer_domaines(zvdf,zclvdf);
}

void Terme_Gravite_VDF_Face::associer_pb(const Probleme_base& pb)
{
  const Milieu_base& le_milieu = pb.milieu();
  const Champ_Don_base& la_gravite = le_milieu.gravite();
  associer_gravite(la_gravite);
}

void Terme_Gravite_VDF_Face::associer_gravite(const Champ_Don_base& g)
{
  gravite = g;
  Eval_Gravite_VDF_Face& eval_grav = static_cast<Eval_Gravite_VDF_Face&> (iter->evaluateur());
  eval_grav.associer(g);
}
