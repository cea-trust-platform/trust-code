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

#include <Champ_Elem_PolyMAC.h>
#include <TRUSTTab_parts.h>


Implemente_instanciable(Champ_Elem_PolyMAC, "Champ_Elem_PolyMAC", Champ_Inc_P0_base);

Sortie& Champ_Elem_PolyMAC::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_PolyMAC::readOn(Entree& s)
{
  lire_donnees(s);
  return s;
}

int Champ_Elem_PolyMAC::imprime(Sortie& os, int ncomp) const
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
  Cout << "Champ_Elem_PolyMAC::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}

int Champ_Elem_PolyMAC::fixer_nb_valeurs_nodales(int n)
{
  if (n == domaine_dis_base().domaine().nb_elem()) //champ sans flux aux faces (ex. aiguilles)
    creer_tableau_distribue(domaine_dis_base().domaine().md_vector_elements());
  else //champ avec flux
    creer_tableau_distribue(ref_cast(Domaine_PolyMAC, domaine_dis_base()).mdv_elems_faces);
  return n;
}

Champ_base& Champ_Elem_PolyMAC::affecter_(const Champ_base& ch)
{
  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC,le_dom_VF.valeur());
  DoubleTab_parts part(valeurs());
  for (int i = 0; i < part.size(); i++) ch.valeur_aux(i ? domaine.xv() : domaine.xp(), part[i]);
  valeurs().echange_espace_virtuel();
  return *this;
}

DoubleTab& Champ_Elem_PolyMAC::valeur_aux_faces(DoubleTab& dst) const
{
  const Domaine_VF& domaine = ref_cast(Domaine_VF, le_dom_VF.valeur());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleTab& src = valeurs();

  /* vals doit etre pre-dimensionne */
  int i, e, f, n, N = (src.nb_dim() == 1 ? 1 : src.dimension(1));
  assert(dst.dimension(0) == domaine.xv().dimension(0) && N == (dst.nb_dim() == 1 ? 1 : dst.dimension(1)));

  if (src.dimension_tot(0) > domaine.nb_elem_tot()) //on a les valeurs aux faces
    for (f = 0; f < dst.dimension(0); f++)
      for (n = 0; n < N; n++) dst(f, n) = src(domaine.nb_elem_tot() + f, n);
  else for (f = 0; f < dst.dimension(0); f++) //on prend (amont + aval) / 2
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          dst(f, n) += src(e, n) * (f < domaine.premiere_face_int() ? 1 : 0.5);

  return dst;
}
