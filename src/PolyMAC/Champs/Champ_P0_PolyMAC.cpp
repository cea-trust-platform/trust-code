/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_P0_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_P0_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_dis.h>
#include <Zone_Cl_PolyMAC.h>
#include <Equation_base.h>
#include <Operateur.h>
#include <Op_Diff_PolyMAC_base.h>
#include <Domaine.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Dirichlet_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <cmath>
#include <array>
#include <Connectivite_som_elem.h>
#include <ConstDoubleTab_parts.h>

Implemente_instanciable(Champ_P0_PolyMAC,"Champ_P0_PolyMAC",Champ_Inc_P0_base);

// printOn

Sortie& Champ_P0_PolyMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


// readOn

Entree& Champ_P0_PolyMAC::readOn(Entree& s)
{
  lire_donnees(s) ;
  return s ;
}

// Description:
//
// Precondition:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Zone_dis_base& Champ_P0_PolyMAC::zone_dis_base() const
{
  return la_zone_VF.valeur();
}
// Description:
//
// Precondition:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:  z_dis
//    Signification: la zone discretise
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_P0_PolyMAC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  la_zone_VF=ref_cast(Zone_VF, z_dis);
}

// Description:
//
// Precondition:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: la_zone_PolyMAC_P0.valeur()
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Zone_PolyMAC& Champ_P0_PolyMAC::zone_PolyMAC() const
{
  return ref_cast(Zone_PolyMAC, la_zone_VF.valeur());
}

int Champ_P0_PolyMAC::imprime(Sortie& os, int ncomp) const
{
  const Zone_dis_base& zone_dis = zone_dis_base();
  const Zone& zone = zone_dis.zone();
  const DoubleTab& coord=zone.domaine().coord_sommets();
  const int nb_som = zone.domaine().nb_som();
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
  Cout << "Champ_P0_PolyMAC::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}

int Champ_P0_PolyMAC::fixer_nb_valeurs_nodales(int n)
{
  if (n == zone_dis_base().zone().nb_elem()) //champ sans flux aux faces (ex. aiguilles)
    creer_tableau_distribue(zone_dis_base().zone().md_vector_elements());
  else //champ avec flux
    creer_tableau_distribue(ref_cast(Zone_PolyMAC, zone_dis_base()).mdv_elems_faces);
  return n;
}

Champ_base& Champ_P0_PolyMAC::affecter_(const Champ_base& ch)
{
  DoubleTab noeuds;
  remplir_coord_noeuds(noeuds);

  // Modif B.M. pour ne pas faire d'interpolation sur les cases virtuelles
  DoubleTab_parts part(valeurs());
  int n = part[0].dimension(0);
  DoubleTab pos, val;
  pos.ref_tab(noeuds, 0, n);
  val.ref_tab(part[0], 0, n);
  ch.valeur_aux(pos, val);

  return *this;
}

//utilitaires pour CL
void Champ_P0_PolyMAC::init_cl() const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC,la_zone_VF.valeur());
  const Conds_lim& cls = ma_zone_cl_dis.valeur().les_conditions_limites();
  int i, f, n;

  if (icl.dimension(0)) return;
  icl.resize(zone.nb_faces_tot(), 3);
  for (n = 0; n < cls.size(); n++)
    {
      const Front_VF& fvf = ref_cast(Front_VF, cls[n].frontiere_dis());
      int idx = sub_type(Echange_externe_impose, cls[n].valeur()) + 2 * sub_type(Echange_global_impose, cls[n].valeur())
                + 3 * sub_type(Neumann_paroi, cls[n].valeur())      + 4 * (sub_type(Neumann_homogene, cls[n].valeur()) || sub_type(Neumann_sortie_libre, cls[n].valeur()) || sub_type(Symetrie, cls[n].valeur()))
                + 5 * sub_type(Dirichlet, cls[n].valeur())          + 6 * sub_type(Dirichlet_homogene, cls[n].valeur());
      if (!idx) Cerr << "Champ_P0_PolyMAC : CL non codee rencontree!" << finl, Process::exit();
      for (i = 0; i < fvf.nb_faces_tot(); i++)
        f = fvf.num_face(i), icl(f, 0) = idx, icl(f, 1) = n, icl(f, 2) = i;
    }
}

inline DoubleTab& Champ_P0_PolyMAC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double t, int distant) const
{
  /* dimensionnement du tableau de destination x si necessaire */
  const DoubleTab& src = valeurs();
  const Front_VF& fvf = ref_cast(Front_VF, fr);
  int N = src.nb_dim() > 1 ? src.dimension(1): 1;
  if (!x.dimension(0) && !x.get_md_vector().non_nul()) x.resize(fvf.nb_faces(), N);
  const Operateur_base& op_diff = equation().operateur(0).l_op_base(); //pour acceder a la diffusivite
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC, zone_dis_base());
  const IntTab& f_e = zone.face_voisins();
  zone.init_m2();

  DoubleTrav dst; //reconstruction du champ aux faces (on ne le remplit que sur le bord concerne)
  N > 1 ? dst.resize(zone.nb_faces(), N) : dst.resize(zone.nb_faces()); //aargh
  for (int i = 0; i < fvf.nb_faces(); i++) for (int n = 0, f = fvf.num_face(i); n < N; n++)
      dst.addr()[N * f + n] = src.addr()[N * f_e(f, 0) + n]; //on part de la valeur en l'element voisin

  if (src.dimension_tot(0) > zone.zone().nb_elem_tot() && sub_type(Op_Diff_PolyMAC_base, op_diff)) //si on a les flux aux faces, on corrige avec m2
    {
      const DoubleTab& nu_f = ref_cast(Op_Diff_PolyMAC_base, op_diff).get_nu_faces();
      for (int i = 0; i < fvf.nb_faces(); i++) for (int n = 0, f = fvf.num_face(i); n < N; n++) if (nu_f.addr()[N * f + n] > 1e-12)
            for (int j = zone.m2deb(f); j < zone.m2deb(f + 1); j++)
              dst.addr()[N * f + n] -= src.addr()[N * (zone.nb_elem_tot() + zone.m2ji(j, 0)) + n] * zone.m2ci(j) / zone.face_surfaces(f) / nu_f.addr()[N * f + n];
    }

  if (distant)
    fr.frontiere().trace_face_distant(dst, x);
  else
    fr.frontiere().trace_face_local(dst, x);
  // useless ? x.echange_espace_virtuel();
  return x;
}
