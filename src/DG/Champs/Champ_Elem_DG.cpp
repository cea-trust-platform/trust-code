/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Champ_Elem_DG.h>
#include <TRUSTTab_parts.h>
#include <Domaine_Cl_dis.h>
#include <Domaine_DG.h>
#include <Option_DG.h>

Implemente_instanciable(Champ_Elem_DG, "Champ_Elem_DG", Champ_Inc_P0_base);

Sortie& Champ_Elem_DG::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_DG::readOn(Entree& s)
{
  lire_donnees(s);
  return s;
}

int Champ_Elem_DG::imprime(Sortie& os, int ncomp) const
{
  const Domaine_dis_base& domaine_dis = domaine_dis_base();
  const Domaine& domaine = domaine_dis.domaine();
  const DoubleTab& coord=domaine.coord_sommets();
  const int nb_som = domaine.nb_som();
  const DoubleTab& val = valeurs();
  int som;
  os << nb_som << finl;
  for (som=0; som<nb_som; som++)
    {
      if (dimension==3)
        os << coord(som,0) << " " << coord(som,1) << " " << coord(som,2) << " " ;
      if (dimension==2)
        os << coord(som,0) << " " << coord(som,1) << " " ;
      if (nb_compo_ == 1)
        os << val(som) << finl;
      else
        os << val(som,ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_Elem_DG::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}

int Champ_Elem_DG::fixer_nb_valeurs_nodales(int n)
{

  creer_tableau_distribue(domaine_dis_base().domaine().md_vector_elements()); // TODO 26/08/2024 nb_ddl a

  return n;
}


void Champ_Elem_DG::associer_domaine_dis_base(const Domaine_dis_base& z_dis)
{
  le_dom_VF = ref_cast(Domaine_VF, z_dis);

  int order = Option_DG::Get_order_for(nom_)+1;

  Nom nom_domaine =le_dom_VF->domaine().le_nom();
  Nom type_quadrature="Quadrature_Ord"+std::to_string(order)+"_Triangle"; //Todo DG varier avec type_elem + renommer ordre ?
  Nom nom_quadrature = type_quadrature+"_"+nom_domaine;

  Quadrature_base& quad = ref_cast(Quadrature_base, interprete().objet(nom_quadrature));
  quadrature_ = quad;
}

Champ_base& Champ_Elem_DG::affecter_(const Champ_base& ch)
{
  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());
//  DoubleTab_parts part(valeurs());
//  for (int i = 0; i < part.size(); i++) ch.valeur_aux(i ? domaine.xv() : domaine.xp(), part[i]);

  //creation d'un DoubleTab intermediaire pour recuperer les valeurs du champ ch sur les points de quadrature ?

  ch.valeur_aux(domaine.xp(), valeurs());

  // calcul de la quadrature pour remplir le tableau valeurs

  valeurs().echange_espace_virtuel();
  return *this;
}

DoubleTab& Champ_Elem_DG::valeur_aux_faces(DoubleTab& dst) const
{
  throw;
  return dst;
}
