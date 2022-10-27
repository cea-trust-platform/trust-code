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

#include <Op_Conv_VDF_Elem_leaves.h>
#include <Pb_Multiphase.h>
#include <cfloat>

Implemente_instanciable_sans_constructeur(Op_Conv_Amont_VDF_Elem,"Op_Conv_Amont_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Amont_VDF_Elem)
Sortie& Op_Conv_Amont_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Amont_VDF_Elem::readOn(Entree& s ) { return s ; }
// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_Amont_VDF_Elem::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (!sub_type(Pb_Multiphase, equation().probleme()))  Op_Conv_VDF_base::ajouter_blocs(mats, secmem, semi_impl);
  else
    {
      const Zone_VF& zone = ref_cast(Zone_VF, equation().zone_dis().valeur());
      const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Inc_P0_base, equation().inconnue().valeur()).fcl(), &fcl_v = ref_cast(Champ_Face_base, vitesse()).fcl();
      const DoubleVect& fs = zone.face_surfaces(), &pf = equation().milieu().porosite_face();
      const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco->valeur() : equation().champ_convecte();
      const std::string& nom_cc = cc.le_nom().getString();
      const DoubleTab& vit = vitesse().valeurs(), &vcc = semi_impl.count(nom_cc) ? semi_impl.at(nom_cc) : cc.valeurs(), bcc = cc.valeur_aux_bords();
      int i, j, e, eb, f, n, m, N = vcc.line_size(), Mv = vit.line_size(), M;

      const double alpha = 1.0; // amont

      Matrice_Morse *m_vit = mats.count("vitesse") ? mats.at("vitesse") : NULL;
      std::vector<std::tuple<const DoubleTab *, Matrice_Morse *, int>> d_cc; //liste des derivees de cc a renseigner : couples (derivee de cc, matrice, nb de compos de la variable)
      if (!semi_impl.count(nom_cc))
        for (auto &i_m : mats)
          if (cc.derivees().count(i_m.first))
            d_cc.push_back(std::make_tuple(&cc.derivees().at(i_m.first), i_m.second, equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size()));

      DoubleTrav dv_flux(N), dc_flux(2, N); //derivees du flux convectif a la face par rapport a la vitesse / au champ convecte amont / aval

      /* convection aux faces internes (fcl(f, 0) == 0), de Neumann_val_ext ou de Dirichlet */
      for (f = 0; f < zone.nb_faces(); f++)
        if (!fcl(f, 0) || (fcl(f, 0) > 4 && fcl(f, 0) < 7))
          {
            for (dv_flux = 0, dc_flux = 0, i = 0; i < 2; i++)
              for (e = f_e(f, i), n = 0, m = 0; n < N; n++, m += (Mv > 1))
                {
                  double v = vit(f, m) ? vit(f, m) : DBL_MIN, fac = pf(f) * fs(f) * (1. + (v * (i ? -1 : 1) > 0 ? 1. : -1) * alpha) / 2;
                  dv_flux(n) += fac * (e >= 0 ? vcc(e, n) : bcc(f, n)); //f est reelle -> indice trivial dans bcc
                  dc_flux(i, n) = e >= 0 ? fac * vit(f, m) : 0;
                }

            //second membre
            for (i = 0; i < 2; i++)
              if ((e = f_e(f, i)) >= 0 && e < zone.nb_elem())
                for (n = 0, m = 0; n < N; n++, m += (Mv > 1))
                  secmem(e, n) -= (i ? -1 : 1) * dv_flux(n) * vit(f, m);
            //derivees : vitesse
            if (m_vit && fcl_v(f, 0) < 2)
              for (i = 0; i < 2; i++)
                if ((e = f_e(f, i)) >= 0 && e < zone.nb_elem())
                  for (n = 0, m = 0; n < N; n++, m += (Mv > 1))
                    (*m_vit)(N * e + n, Mv * f + m) += (i ? -1 : 1) * dv_flux(n);
            //derivees : champ convecte
            for (auto &&d_m_i : d_cc)
              for (i = 0; i < 2; i++)
                if ((e = f_e(f, i)) >= 0 && e < zone.nb_elem())
                  for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                    for (n = 0, m = 0, M = std::get<2>(d_m_i); n < N; n++, m += (M > 1))
                      (*std::get<1>(d_m_i))(N * e + n, M * eb + m) += (i ? -1 : 1) * dc_flux(j, n) * (*std::get<0>(d_m_i))(eb, m);
          }
    }
}

void Op_Conv_Amont_VDF_Elem::preparer_calcul()
{
  if (!sub_type(Pb_Multiphase, equation().probleme()))
    Op_Conv_VDF_base::preparer_calcul();
  else
    {
      Op_Conv_VDF_base::preparer_calcul();
      equation().init_champ_convecte();
      const int ncomp = equation().inconnue().valeurs().line_size();
      flux_bords().resize(ref_cast(Zone_VF,equation().zone_dis().valeur()).nb_faces_bord(),ncomp);
      flux_bords()=0;
    }
}


Op_Conv_Amont_VDF_Elem::Op_Conv_Amont_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Amont_VDF_Elem)()) { }

Implemente_instanciable_sans_constructeur(Op_Conv_Centre_VDF_Elem,"Op_Conv_Centre_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Centre_VDF_Elem)
Sortie& Op_Conv_Centre_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Centre_VDF_Elem::readOn(Entree& s ) { return s ; }
Op_Conv_Centre_VDF_Elem::Op_Conv_Centre_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Centre_VDF_Elem)()) { }

Implemente_instanciable_sans_constructeur(Op_Conv_Centre4_VDF_Elem,"Op_Conv_Centre4_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Centre4_VDF_Elem)
Sortie& Op_Conv_Centre4_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Centre4_VDF_Elem::readOn(Entree& s ) { return s ; }
Op_Conv_Centre4_VDF_Elem::Op_Conv_Centre4_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Centre4_VDF_Elem)()) { }

Implemente_instanciable_sans_constructeur(Op_Conv_Quick_VDF_Elem,"Op_Conv_Quick_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Quick_VDF_Elem)
Sortie& Op_Conv_Quick_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Quick_VDF_Elem::readOn(Entree& s ) { return s ; }
Op_Conv_Quick_VDF_Elem::Op_Conv_Quick_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Quick_VDF_Elem)()) { }
