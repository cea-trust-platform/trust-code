/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Connectivite_som_elem.h>
#include <Champ_Elem_PolyMAC.h>
#include <Champ_Elem_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <TRUSTTab_parts.h>
#include <MD_Vector_base.h>
#include <Equation_base.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_dis.h>
#include <Zone.h>
#include <array>

Implemente_instanciable(Champ_Elem_PolyMAC,"Champ_Elem_PolyMAC",Champ_Inc_P0_base);

Sortie& Champ_Elem_PolyMAC::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_PolyMAC::readOn(Entree& s)
{
  lire_donnees(s) ;
  return s ;
}

const Zone_PolyMAC& Champ_Elem_PolyMAC::zone_PolyMAC() const
{
  return ref_cast(Zone_PolyMAC, le_dom_VF.valeur());
}

int Champ_Elem_PolyMAC::imprime(Sortie& os, int ncomp) const
{
  const Zone_dis_base& zone_dis = zone_dis_base();
  const Zone& zone = zone_dis.zone();
  const DoubleTab& coord=zone.coord_sommets();
  const int nb_som = zone.nb_som();
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
  Cout << "Champ_Elem_PolyMAC::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}

int Champ_Elem_PolyMAC::fixer_nb_valeurs_nodales(int n)
{
  assert (n == zone_dis_base().zone().nb_elem() || n < 0); //on accepte a la fois les conventions VEF et VDF
  creer_tableau_distribue(zone_dis_base().zone().md_vector_elements());
  return n;
}

int Champ_Elem_PolyMAC::nb_valeurs_nodales() const
{
  return le_dom_VF->nb_elem(); //on ignore les variables auxiliaires
}

void Champ_Elem_PolyMAC::init_auxiliary_variables()
{
  const Zone_PolyMAC& zone = zone_PolyMAC();
  const IntTab& f_e = zone.face_voisins();
  for (int n = 0; n < nb_valeurs_temporelles(); n++)
    if (futur(n).size_reelle_ok())
      {
        DoubleTab& vals = futur(n);
        vals.set_md_vector(MD_Vector()); //on enleve le MD_Vector...
        vals.resize_dim0(zone.mdv_elems_faces.valeur().get_nb_items_tot()); //...on dimensionne a la bonne taille...
        vals.set_md_vector(zone.mdv_elems_faces); //...et on remet le bon MD_Vector
        /* initialisation des variables aux faces : par celle de l'elem amont */
        for (int f = 0, ne_tot = zone.nb_elem_tot(); f < zone.nb_faces(); f++)
          for (int m = 0, e = f_e(f, 0); m < vals.dimension(1); m++)
            vals(ne_tot + f, m) = vals(e, m);
        vals.echange_espace_virtuel();
      }
}

int Champ_Elem_PolyMAC::reprendre(Entree& fich)
{
  const Zone_PolyMAC* zone = le_dom_VF.non_nul() ? &ref_cast( Zone_PolyMAC,le_dom_VF.valeur()) : NULL;
  valeurs().set_md_vector(MD_Vector()); //on enleve le MD_Vector...
  valeurs().resize(0);
  int ret = Champ_Inc_base::reprendre(fich);
  //et on remet le bon si on peut
  if (zone) valeurs().set_md_vector(valeurs().dimension_tot(0) > zone->nb_elem_tot() ? zone->mdv_elems_faces : zone->zone().md_vector_elements());
  return ret;
}

Champ_base& Champ_Elem_PolyMAC::affecter_(const Champ_base& ch)
{
  const Zone_PolyMAC& zone = zone_PolyMAC();
  if (ch.valeurs().dimension_tot(0) > zone.nb_elem_tot())
    init_auxiliary_variables(), valeurs() = ch.valeurs();
  else
    {
      DoubleTab_parts part(valeurs());
      for (int i = 0; i < part.size(); i++) ch.valeur_aux(i ? zone.xv() : zone.xp(), part[i]);
      valeurs().echange_espace_virtuel();
    }
  return *this;
}

DoubleTab& Champ_Elem_PolyMAC::valeur_aux_faces(DoubleTab& dst) const
{
  const Zone_PolyMAC& zone = zone_PolyMAC();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& src = valeurs();

  /* vals doit etre pre-dimensionne */
  int i, e, f, n, N = (src.nb_dim() == 1 ? 1 : src.dimension(1));
  assert(dst.dimension(0) == zone.xv().dimension(0) && N == (dst.nb_dim() == 1 ? 1 : dst.dimension(1)));

  if (src.dimension_tot(0) > zone.nb_elem_tot()) //on a les valeurs aux faces
    for (f = 0; f < dst.dimension(0); f++)
      for (n = 0; n < N; n++) dst(f, n) = src(zone.nb_elem_tot() + f, n);
  else for (f = 0; f < dst.dimension(0); f++) //on prend (amont + aval) / 2
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          dst(f, n) += src(e, n) * (f < zone.premiere_face_int() ? 1 : 0.5);

  return dst;
}
