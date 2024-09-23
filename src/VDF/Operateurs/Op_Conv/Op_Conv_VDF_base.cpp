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

#include <Modifier_pour_fluide_dilatable.h>
#include <Discretisation_base.h>
#include <Masse_ajoutee_base.h>
#include <Op_Conv_VDF_base.h>
#include <Champ_Face_VDF.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <TRUSTTrav.h>
#include <Champ.h>

extern Stat_Counter_Id convection_counter_;

Implemente_base(Op_Conv_VDF_base,"Op_Conv_VDF_base",Operateur_Conv_base);

Sortie& Op_Conv_VDF_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Op_Conv_VDF_base::readOn(Entree& s)
{
  if (sub_type(Masse_Multiphase, equation())) //convection dans Masse_Multiphase -> champs de debit / titre
    {
      const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
      noms_cc_phases_.dimensionner(pb.nb_phases()), cc_phases_.resize(pb.nb_phases());
      noms_vd_phases_.dimensionner(pb.nb_phases()), vd_phases_.resize(pb.nb_phases());
      noms_x_phases_.dimensionner(pb.nb_phases()), x_phases_.resize(pb.nb_phases());
      for (int i = 0; i < pb.nb_phases(); i++)
        {
          noms_cc_phases_[i] = Nom("debit_") + pb.nom_phase(i);
          noms_vd_phases_[i] = Nom("vitesse_debitante_") + pb.nom_phase(i);
          noms_x_phases_[i] = Nom("titre_") + pb.nom_phase(i);
        }
    }
  return s;
}

inline void eval_fluent(const double psc, const int num1, const int num2, const int n, DoubleTab& fluent)
{
  if (psc >= 0) fluent(num2, n) += psc;
  else fluent(num1, n) -= psc;
}

void Op_Conv_VDF_base::completer()
{
  Operateur_base::completer();
  iter_->completer_();
}

int Op_Conv_VDF_base::impr(Sortie& os) const
{
  return iter_->impr(os);
}

void Op_Conv_VDF_base::preparer_calcul()
{
  Operateur_Conv_base::preparer_calcul(); /* ne fait rien */
  iter_->set_convective_op_pb_type(true /* convective op */, sub_type(Pb_Multiphase, equation().probleme()));
}

void Op_Conv_VDF_base::associer_champ_convecte_elem()
{
  if (sub_type(Pb_Multiphase, equation().probleme())) equation().init_champ_convecte();

  Op_Conv_VDF_base::preparer_calcul();
  const Champ_Inc_base& cc = equation().has_champ_convecte() ? equation().champ_convecte() : (le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue());
  iter_->associer_champ_convecte_ou_inc(cc, &vitesse());
  iter_->set_name_champ_inco(le_champ_inco.non_nul() ? nom_inconnue() : cc.le_nom().getString());
}

void Op_Conv_VDF_base::associer_champ_convecte_face()
{
  Op_Conv_VDF_base::preparer_calcul();
  const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue();
  iter_->associer_champ_convecte_ou_inc(cc, &vitesse());
  iter_->set_name_champ_inco(le_champ_inco.non_nul() ? nom_inconnue() : cc.le_nom().getString());
}

void Op_Conv_VDF_base::associer_champ_temp(const Champ_Inc_base& ch_unite, bool use_base) const
{
  const_cast<OWN_PTR(Iterateur_VDF_base)&>(iter_)->associer_champ_convecte_ou_inc(ch_unite, nullptr, use_base);
}

void Op_Conv_VDF_base::dimensionner_blocs_elem(matrices_t mats, const tabs_t& semi_impl) const
{
  const Domaine_VDF& domaine = iter_->domaine();
  const IntTab& f_e = domaine.face_voisins();
  int i, j, e, eb, f, n, N = equation().inconnue().valeurs().line_size();
  const int hcc = equation().has_champ_convecte();
  const Champ_Inc_base& cc = hcc ? equation().champ_convecte() : equation().inconnue();

  for (auto &&i_m : mats)
    if (i_m.first == "vitesse" || (!hcc && i_m.first == cc.le_nom()) || (cc.derivees().count(i_m.first) && !semi_impl.count(cc.le_nom().getString())))
      {
        Matrice_Morse mat;
        IntTab stencil(0, 2);

        int m, M = equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size();
        if (i_m.first == "vitesse") /* vitesse */
          {
            const IntTab& fcl_v = ref_cast(Champ_Face_VDF, vitesse()).fcl();

            for (f = 0; f < domaine.nb_faces_tot(); f++)
              if (fcl_v(f, 0) < 2)
                for (i = 0; i < 2; i++)
                  if ((e = f_e(f, i)) >= 0 && e < domaine.nb_elem_tot())
                    for (n = 0; n < N; n++) stencil.append_line(N * e + n, M * f + n * (M > 1));
          }
        else for (f = 0; f < domaine.nb_faces_tot(); f++)
            for (i = 0; i < 2; i++)
              if ((e = f_e(f, i)) >= 0 && e < domaine.nb_elem_tot()) /* inconnues scalaires */
                for (j = 0; j < 2; j++)
                  if ((eb = f_e(f, j)) >= 0)
                    for (n = 0, m = 0; n < N; n++, m += (M > 1)) stencil.append_line(N * e + n, M * eb + m);

        tableau_trier_retirer_doublons(stencil);
        const int nl = equation().inconnue().valeurs().size_totale(),
                  nc = i_m.first == "vitesse" ? vitesse().valeurs().size_totale() : (hcc ? cc.derivees().at(i_m.first).size_totale() : nl);
        Matrix_tools::allocate_morse_matrix(nl, nc, stencil, mat);
        i_m.second->nb_colonnes() ? *i_m.second += mat : *i_m.second = mat;
      }
}

void Op_Conv_VDF_base::dimensionner_blocs_face(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_VDF& domaine = iter_->domaine();
  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue());
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& inco = ch.valeurs();

  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //pas de bloc diagonal ou semi-implicite -> rien a faire
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  //int e, eb, fb,  N = equation().inconnue().valeurs().line_size();
  // eb never used ? Warning Error on clang...
  int e, fb,  N = equation().inconnue().valeurs().line_size();
  IntTab stencil(0, 2);


  /* agit uniquement aux elements; diagonale omise */
  for (int f = 0; f < domaine.nb_faces_tot(); f++)
    if (f_e(f, 0) >= 0 && (f_e(f, 1) >= 0 || fcl(f, 0) == 3))
      for (int i = 0; i < 2; i++)
        if ((e = f_e(f, i)) >= 0)
          for (int j = 0; j < 2; j++)
            //if ((eb = f_e(f, j)) >= 0)
            if (f_e(f, j) >= 0)
              for (int k = 0; k < e_f.dimension(1); k++)
                if ((fb = e_f(e, k)) >= 0)
                  if (fb < domaine.nb_faces() && fcl(fb, 0) < 2)
                    for (int n = 0; n < N; n++)
                      for (int m = (corr ? 0 : n); m < (corr ? N : n + 1); m++) stencil.append_line(N * fb + n, N * f + m);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}


void Op_Conv_VDF_base::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(convection_counter_);
  iter_->ajouter_blocs(mats, secmem, semi_impl);
  statistiques().end_count(convection_counter_);
}

double Op_Conv_VDF_base::calculer_dt_stab() const
{
  const Domaine_VDF& domaine_VDF = iter_->domaine();
  const Domaine_Cl_VDF& domaine_Cl_VDF = iter_->domaine_Cl();
  const IntTab& face_voisins = domaine_VDF.face_voisins();
  const DoubleVect& volumes = domaine_VDF.volumes();
  const DoubleVect& face_surfaces = domaine_VDF.face_surfaces();
  const DoubleTab& vit_associe = vitesse().valeurs();
  const DoubleTab& vit= (vitesse_pour_pas_de_temps_.non_nul()?vitesse_pour_pas_de_temps_->valeurs(): vit_associe);
  const int N = std::min(vit.line_size(), equation().inconnue().valeurs().line_size());
  const DoubleTab* alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  if (!fluent_.get_md_vector().non_nul())
    {
      fluent_.resize(0, N);
      domaine_VDF.domaine().creer_tableau_elements(fluent_);
    }
  fluent_ = 0;
  // Remplissage du tableau fluent
  double psc;
  int num1, num2, face, elem1;

  // On traite les bords
  for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);
      if ( sub_type(Dirichlet_entree_fluide,la_cl.valeur()) || sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          num1 = le_bord.num_premiere_face();
          num2 = num1 + le_bord.nb_faces();
          for (face=num1; face<num2; face++)
            for (int n = 0; n < N; n++)
              {
                psc = vit(face, n) * face_surfaces(face);
                if ( (elem1 = face_voisins(face,0)) != -1)
                  {
                    if (psc < 0) fluent_(elem1, n) -= psc;
                  }
                else // (elem2 != -1)
                  if (psc > 0) fluent_(face_voisins(face,1), n) += psc;
              }
        }
    }

  // Boucle sur les faces internes pour remplir fluent
  const int domaine_VDF_nb_faces = domaine_VDF.nb_faces(), premiere_face = domaine_VDF.premiere_face_int();
  for (face = premiere_face; face < domaine_VDF_nb_faces; face++)
    for (int n = 0; n < N; n++)
      {
        psc = vit(face, n) * face_surfaces(face);
        eval_fluent(psc, face_voisins(face, 0), face_voisins(face, 1), n, fluent_);
      }

  // Calcul du pas de temps de stabilite a partir du tableau fluent
  if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(fluent_,equation().milieu());

  const double alpha_min_dt = 1e-3; // avoid stupid time steps during vanishing phase
  double dt_stab = 1.e30;
  int domaine_VDF_nb_elem=domaine_VDF.nb_elem();
  // dt_stab = min ( 1 / ( |U|/dx + |V|/dy + |W|/dz ) )
  for (int num_poly=0; num_poly<domaine_VDF_nb_elem; num_poly++)
    for (int n = 0; n < N; n++)
      if ((!alp || (*alp)(num_poly, n) > alpha_min_dt))
        {
          double dt_elem = volumes(num_poly)/(fluent_(num_poly, n)+DMINFLOAT);
          if (dt_elem<dt_stab) dt_stab = dt_elem;
        }

  dt_stab = Process::mp_min(dt_stab);

  // astuce pour contourner le type const de la methode
  Op_Conv_VDF_base& op =ref_cast_non_const(Op_Conv_VDF_base, *this);
  op.fixer_dt_stab_conv(dt_stab);
  return dt_stab;
}

// Calculation of local time: Vect of size number of faces of the domain This is the equivalent of "Op_Conv_VDF_base :: calculer_dt_stab ()"
void Op_Conv_VDF_base::calculer_dt_local(DoubleTab& dt_face) const
{
  const Domaine_VDF& domaine_VDF = iter_->domaine();
  const Domaine_Cl_VDF& domaine_Cl_VDF = iter_->domaine_Cl();
  const DoubleVect& volumes_entrelaces= domaine_VDF.volumes_entrelaces();
  const DoubleVect& face_surfaces = domaine_VDF.face_surfaces();
  //const DoubleVect& vit= vitesse_pour_pas_de_temps_->valeurs();
  const DoubleVect& vit=equation().inconnue().valeurs();
  DoubleTrav fluent(volumes_entrelaces);

  // Remplissage du tableau fluent
  // On traite les bords
  for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);

      if ( sub_type(Dirichlet_entree_fluide,la_cl.valeur()) || sub_type(Neumann_sortie_libre,la_cl.valeur())  )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          const int num1 = le_bord.num_premiere_face(), num2 = num1 + le_bord.nb_faces();
          for (int face = num1; face < num2; face++)
            {
              double value = vit[face]*face_surfaces(face);
              if (value >0) fluent[face] = value;
              else fluent[face] -= value;
            }
        }
    }

  // Boucle sur les faces internes pour remplir fluent
  const int domaine_VDF_nb_faces = domaine_VDF.nb_faces(), premiere_face = domaine_VDF.premiere_face_int();
  for (int face = premiere_face; face < domaine_VDF_nb_faces; face++)
    {
      const double value = vit[face]*face_surfaces(face);
      if (value >0) fluent[face] = value;
      else fluent[face] -= value;
    }


  // Calcul du pas de temps de stabilite a partir du tableau fluent
  if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(fluent,equation().milieu());

  dt_face=(volumes_entrelaces);
  // Boucle sur les faces de bords
  for (int n_bord=0; n_bord<domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
      for (int num_face = ndeb; num_face < nfin; num_face++)
        {
          if( sup_strict(fluent[num_face], 1.e-16) ) dt_face(num_face)= volumes_entrelaces(num_face)/fluent[num_face];
          else dt_face(num_face) = -1.;
        }
    }

  // Boucle sur les faces internes
  for (int num_face = premiere_face; num_face<domaine_VDF_nb_faces; num_face++)
    {
      if( sup_strict(fluent[num_face], 1.e-16) ) dt_face(num_face)= volumes_entrelaces(num_face)/fluent[num_face];
      else dt_face(num_face) = -1.;
    }

  double max_dt_local= dt_face.mp_max_abs_vect();
  const int taille = dt_face.size();
  for(int i = 0; i < taille; i++)
    if(! sup_strict(dt_face(i), 1.e-16)) dt_face(i) = max_dt_local;

  dt_face.echange_espace_virtuel();

  for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          const int nb_faces_bord = le_bord.nb_faces();
          for (int ind_face = 0; ind_face < nb_faces_bord; ind_face++)
            {
              int ind_face_associee = la_cl_perio.face_associee(ind_face);
              int face = le_bord.num_face(ind_face), face_associee = le_bord.num_face(ind_face_associee);
              if (!est_egal(dt_face(face),dt_face(face_associee),1.e-8))
                {
                  dt_face(face) = std::min(dt_face(face),dt_face(face_associee));
                }
            }
        }
    }
  dt_face.echange_espace_virtuel();
//  dt_conv_locaux=dt_face;
}

// cf Op_Conv_VDF_base::calculer_dt_stab() pour choix de calcul de dt_stab
void Op_Conv_VDF_base::calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const
{
  if (Motcle(option)=="stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage->valeurs();
      es_valeurs = 1.e30;

      const Domaine_VDF& domaine_VDF = iter_->domaine();
      const Domaine_Cl_VDF& domaine_Cl_VDF = iter_->domaine_Cl();
      const IntTab& face_voisins = domaine_VDF.face_voisins();
      const DoubleVect& volumes = domaine_VDF.volumes();
      const DoubleVect& face_surfaces = domaine_VDF.face_surfaces();
      const DoubleVect& vit = vitesse().valeurs();
      const int N = std::min(vit.line_size(), equation().inconnue().valeurs().line_size());
      DoubleTrav fluent(domaine_VDF.domaine().nb_elem_tot(), N);
      assert(N == 1); // en attendant de coder les boucles...

      // Remplissage du tableau fluent
      fluent = 0;
      double psc;
      int num1, num2, face, elem1;

      // On traite les bords
      for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);
          if ( sub_type(Dirichlet_entree_fluide,la_cl.valeur()) || sub_type(Neumann_sortie_libre,la_cl.valeur())  )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
              num1 = le_bord.num_premiere_face();
              num2 = num1 + le_bord.nb_faces();
              for (face = num1; face < num2; face++)
                {
                  psc = vit[face]*face_surfaces(face);
                  if ( (elem1 = face_voisins(face,0)) != -1)
                    {
                      if (psc < 0) fluent[elem1] -= psc;
                    }
                  else // (elem2 != -1)
                    if (psc > 0) fluent[face_voisins(face,1)] += psc;
                }
            }
        }

      // Boucle sur les faces internes pour remplir fluent
      const int domaine_VDF_nb_faces = domaine_VDF.nb_faces();
      for (face = domaine_VDF.premiere_face_int(); face < domaine_VDF_nb_faces; face++)
        {
          psc = vit[face]*face_surfaces(face);
          eval_fluent(psc,face_voisins(face,0),face_voisins(face,1),0,fluent);
        }
      //fluent.echange_espace_virtuel();
      if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
        diviser_par_rho_si_dilatable(fluent,equation().milieu());

      const int domaine_VDF_nb_elem=domaine_VDF.nb_elem();
      for (int num_poly=0; num_poly<domaine_VDF_nb_elem; num_poly++)
        es_valeurs(num_poly) = volumes(num_poly)/(fluent[num_poly]+1.e-30);

      //double dt_min = mp_min_vect(es_valeurs);
      //assert(dt_min==calculer_dt_stab());
    }
  else
    Operateur_Conv_base::calculer_pour_post(espace_stockage,option,comp);
}

Motcle Op_Conv_VDF_base::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="stabilite") loc = "elem";
  else return Operateur_Conv_base::get_localisation_pour_post(option);
  return loc;
}

void Op_Conv_VDF_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Operateur_Conv_base::get_noms_champs_postraitables(nom,opt);
  Noms noms_compris;

  if (sub_type(Masse_Multiphase, equation()))
    {
      const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());

      for (int i = 0; i < pb.nb_phases(); i++)
        {
          noms_compris.add(noms_cc_phases_[i]);
          noms_compris.add(noms_vd_phases_[i]);
          noms_compris.add(noms_x_phases_[i]);
        }
    }
  if (opt==DESCRIPTION)
    Cerr<<" Op_Conv_VDF_base : "<< noms_compris <<finl;
  else
    nom.add(noms_compris);
}
void Op_Conv_VDF_base::creer_champ(const Motcle& motlu)
{
  Operateur_Conv_base::creer_champ(motlu); // Do nothing mais bon :-) Maybe some day it will
  if (sub_type(Masse_Multiphase, equation())) //convection dans Masse_Multiphase -> champs de debit / titre
    {
      int i = noms_cc_phases_.rang(motlu), j = noms_vd_phases_.rang(motlu), k = noms_x_phases_.rang(motlu);
      if (i >= 0 && !cc_phases_[i].non_nul())
        {
          equation().discretisation().discretiser_champ("vitesse", equation().domaine_dis(), noms_cc_phases_[i], "kg/m2/s",dimension, 1, 0, cc_phases_[i]);
          champs_compris_.ajoute_champ(cc_phases_[i]);
        }
      if (j >= 0 && !vd_phases_[j].non_nul())
        {
          equation().discretisation().discretiser_champ("vitesse", equation().domaine_dis(), noms_vd_phases_[j], "m/s",dimension, 1, 0, vd_phases_[j]);
          champs_compris_.ajoute_champ(vd_phases_[j]);
        }
      if (k >= 0 && !x_phases_[k].non_nul())
        {
          equation().discretisation().discretiser_champ("temperature", equation().domaine_dis(), noms_x_phases_[k], "m/s",1, 1, 0, x_phases_[k]);
          champs_compris_.ajoute_champ(x_phases_[k]);
        }
    }
}

void Op_Conv_VDF_base::mettre_a_jour(double temps)
{
  Operateur_Conv_base::mettre_a_jour(temps); // Do nothing mais bon :-) Maybe some day it will

  if (sub_type(Masse_Multiphase, equation())) //convection dans Masse_Multiphase -> champs de debit / titre
    {
      const Domaine_VDF& domaine = iter_->domaine();
      const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
      const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().champ_convecte();
      const DoubleVect& pf = equation().milieu().porosite_face(), &pe = equation().milieu().porosite_elem(), &fs = domaine.face_surfaces(), &ve = domaine.volumes();
      const DoubleTab& vit = vitesse().valeurs(), &vcc = cc.valeurs(), bcc = cc.valeur_aux_bords(), &xv = domaine.xv(), &xp = domaine.xp();
      DoubleTab balp;
      if (vd_phases_.size()) balp = equation().inconnue().valeur_aux_bords();

      int i, e, f, d, D = dimension, n, m, N = vcc.line_size(), M = vit.line_size();
      //DoubleTrav cc_f(N); //valeur du champ convecte aux faces

      if (cc_phases_.size())
        for (n = 0, m = 0; n < N; n++, m += (M > 1))
          if (cc_phases_[n].non_nul()) /* mise a jour des champs de debit */
            {
              Champ_Face_VDF& c_ph = ref_cast(Champ_Face_VDF, cc_phases_[n].valeur());
              DoubleTab& v_ph = c_ph.valeurs();
              for (f = 0; f < domaine.nb_faces(); v_ph(f) *= vit(f, m) * pf(f), f++)
                for (v_ph(f) = 0, i = 0; i < 2; i++) v_ph(f) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * 1.0 /* FIXME : amont */) / 2 * ((e = f_e(f, i)) >= 0 ? vcc(e, n) : bcc(f, n));
              c_ph.changer_temps(temps);
            }

      if (vd_phases_.size())
        for (n = 0, m = 0; n < N; n++, m += (M > 1))
          if (vd_phases_[n].non_nul()) /* mise a jour des champs de vitesse debitante */
            {
              const DoubleTab& alp = equation().inconnue().valeurs();
              Champ_Face_VDF& c_ph = ref_cast(Champ_Face_VDF, vd_phases_[n].valeur());
              DoubleTab& v_ph = c_ph.valeurs();
              /* on remplit la partie aux faces, puis on demande au champ d'interpoler aux elements */
              for (f = 0; f < domaine.nb_faces(); v_ph(f) *= vit(f, m) * pf(f), f++)
                for (v_ph(f) = 0, i = 0; i < 2; i++) v_ph(f) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * 1.0 /* FIXME : amont */) / 2 * ((e = f_e(f, i)) >= 0 ? alp(e, n) : balp(f, n));
              c_ph.changer_temps(temps);
            }

      DoubleTrav G(N), v(N, D);
      double Gt;
      if (x_phases_.size())
        for (e = 0; e < domaine.nb_elem(); e++) //titre : aux elements
          {
            for (v = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
              for (n = 0; n < N; n++)
                for (d = 0; d < D; d++)
                  v(n, d) += fs(f) * pf(f) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * vit(f, n) / (pe(e) * ve(e));
            for (Gt = 0, n = 0; n < N; Gt += G(n), n++) G(n) = vcc(e, n) * sqrt(domaine.dot(&v(n, 0), &v(n, 0)));
            for (n = 0; n < N; n++)
              if (x_phases_[n].non_nul()) x_phases_[n]->valeurs()(e) = Gt ? G(n) / Gt : 0;
          }
      if (x_phases_.size())
        for (n = 0; n < N; n++)
          if (x_phases_[n].non_nul()) x_phases_[n]->changer_temps(temps);
    }
}
