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

#include <Champ_Inc_P0_base.h>
#include <Zone.h>
#include <Equation_base.h>
#include <Zone_dis_base.h>
#include <Frontiere_dis_base.h>
#include <Zone_Cl_dis.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Dirichlet_homogene.h>
#include <Neumann_homogene.h>
#include <Echange_externe_impose.h>
#include <Echange_global_impose.h>
#include <Neumann_paroi.h>
#include <Neumann_val_ext.h>

Implemente_base(Champ_Inc_P0_base,"Champ_Inc_P0_base",Champ_Inc_base);

Sortie& Champ_Inc_P0_base::printOn(Sortie& os) const
{
  os << que_suis_je() << " " << le_nom();
  return os;
}

Entree& Champ_Inc_P0_base::readOn(Entree& is)
{
  lire_donnees(is) ;
  return is;
}

int Champ_Inc_P0_base::fixer_nb_valeurs_nodales(int n)
{
  // Encore une syntaxe a la con, sinon on ne sait pas s'il faut appeler
  // zone_dis_base() de champ_inc_base ou de champ_impl...
  const Zone_dis_base& zonedis = ref_cast(Champ_Inc_base, *this).zone_dis_base();
  assert(n == zonedis.zone().nb_elem());
  const MD_Vector& md = zonedis.zone().md_vector_elements();
  creer_tableau_distribue(md);
  return 1;
}

Champ_base& Champ_Inc_P0_base::affecter_(const Champ_base& ch)
{
  if (Champ_implementation_P0::affecter_(ch)) return *this;
  else return Champ_Inc_base::affecter_(ch);
}

double Champ_Inc_P0_base::valeur_au_bord(int face) const
{
  assert(le_dom_VF.non_nul());
  const DoubleTab& val = valeurs();
  double la_val_bord;

  int n0 = le_dom_VF->face_voisins(face, 0);
  if (n0 != -1) la_val_bord = val[n0];
  else la_val_bord = val[le_dom_VF->face_voisins(face, 1)];

  return la_val_bord;
}

/*! @brief Trace du champ P0 sur la frontiere
 *
 */
DoubleTab& Champ_Inc_P0_base::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps,int distant) const
{
  return Champ_implementation_P0::trace(fr, valeurs(tps), x,  distant);
}

//utilitaires pour CL
void Champ_Inc_P0_base::init_fcl() const
{
  const Conds_lim& cls = mon_dom_cl_dis.valeur().les_conditions_limites();
  int i, f, n;

  const Zone_VF& zone = ref_cast(Zone_VF, mon_equation->zone_dis().valeur());
  fcl_.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Echange_externe_impose, cls[n].valeur()) + 2 * sub_type(Echange_global_impose, cls[n].valeur())
                + 4 * sub_type(Neumann_paroi, cls[n].valeur())      + 5 * (sub_type(Neumann_homogene, cls[n].valeur()) || sub_type(Neumann_val_ext, cls[n].valeur()) || sub_type(Symetrie, cls[n].valeur()))
                + 6 * sub_type(Dirichlet, cls[n].valeur())          + 7 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (cls[n].valeur().que_suis_je().debute_par("Paroi_Echange_contact"))
        idx = 3;
      if (!idx)
        {
          Cerr << "Champ_Inc_P0_base : CL non codee rencontree! " << cls[n].valeur().que_suis_je() << finl;
          Process::exit();
        }
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), fcl_(f, 0) = idx, fcl_(f, 1) = n, fcl_(f, 2) = i;
    }
  fcl_init_ = 1;
}
