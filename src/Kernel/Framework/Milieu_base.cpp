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

#include <Discretisation_tools.h>
#include <Discretisation_base.h>
#include <Champ_late_input_P0.h>
#include <Champ_Fonc_Tabule.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_MED.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <EChaine.h>
#include <Param.h>
#include <Champ.h>

Implemente_base_sans_constructeur(Milieu_base,"Milieu_base",Objet_U);
// XD milieu_base objet_u milieu_base -1 Basic class for medium (physics properties of medium).
// XD attr gravite field_base gravite 1 Gravity field (optional).
// XD attr porosites_champ field_base porosites_champ 1 Porosity field (optional).

Milieu_base::Milieu_base()
{
  indic_rayo_ = NONRAYO;
  deja_associe=0;
}

/*! @brief Surcharge Objet_U::printOn Imprime un mileu sur un flot de sortie.
 *
 *     Appelle Milieu_base::ecrire
 *
 * @param (Sortie& os) le flot de sortie pour l'impression
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Milieu_base::printOn(Sortie& os) const
{
  os << "{" << finl;
  ecrire(os);
  os << "}" << finl;
  return os;
}

/*! @brief Ecrit un objet milieu sur un flot de sortie.
 *
 * Ecrit les caracteristiques du milieu:
 *         - masse volumique
 *         - conductivite
 *         - capacite calorifique
 *         - beta_th
 *         - porosite
 *
 * @param (Sortie& os) le flot de sortie pour l'ecriture
 */
void  Milieu_base::ecrire(Sortie& os) const
{
  os << "rho " << rho<< finl;
  os << "lambda " << lambda << finl;
  os << "Cp " << Cp << finl;
  os << "beta_th " << beta_th << finl;
  os << "porosite " << porosites_champ << finl;
}

/*! @brief Lecture d'un milieu sur un flot d'entree.
 *
 * Format:
 *     {
 *      grandeur_physique type_champ bloc de lecture du champ
 *     }
 *  cf set_param method to know the understood keywords
 *  cf Param class to know possible options reading
 *
 * @param (Entree& is) le flot d'entree pour la lecture des parametres du milieu
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 */
Entree& Milieu_base::readOn(Entree& is)
{
  Cerr << "Reading of data for a " << que_suis_je() << " medium" << finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  creer_champs_non_lus();
  return is;
}

void Milieu_base::set_param(Param& param)
{
  param.ajouter("rho", &rho);
  param.ajouter("lambda", &lambda);
  param.ajouter("Cp", &Cp);
  param.ajouter("beta_th", &beta_th);
  param.ajouter("gravite", &g);
  set_param_porosite(param);
}

// methode utile pour F5 ! F5 n'appelle pas Milieu_base::set_param mais Milieu_base::set_param_porosite ...
void Milieu_base::set_param_porosite(Param& param)
{
  param.ajouter("porosites_champ", &porosites_champ);
  param.ajouter("porosites", &porosites_);
}

int Milieu_base::lire_motcle_non_standard(const Motcle& mot_lu, Entree& is)
{
  Cerr << mot_lu << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard" << finl;
  Process::exit();
  return -1;
}

void Milieu_base::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Cerr << "Medium discretization." << finl;
  Champ_Don& ch_lambda = conductivite();
  Champ_Don& ch_alpha = diffusivite();
  Champ_Don& ch_beta_th = beta_t();
  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  // PL: pas le temps de faire plus propre, je fais comme dans Fluide_Incompressible::discretiser
  // pour gerer une conductivite lue dans un fichier MED. Test: Reprise_grossier_fin_VEF
  // ToDo: reecrire ces deux methodes discretiser

  // E. Saikali
  // The thermal conductivity and diffusivity fields are considered as
  // multi_scalaire fields, sure if the number of components read
  // in the data file for lambda > 1, i.e: case of anisotropic diffusion !

  int lambda_nb_comp = 0;

  if(ch_lambda.non_nul())
    {
      // Returns number of components of lambda field
      lambda_nb_comp = ch_lambda.nb_comp( );
      if (sub_type(Champ_Fonc_MED,ch_lambda.valeur()))
        {
          double temps=ch_lambda.valeur().temps();
          Cerr<<"Convert Champ_fonc_MED lambda in Champ_Don..."<<finl;
          Champ_Don ch_lambda_prov;
          dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_lambda_prov);
          ch_lambda_prov.affecter_(ch_lambda.valeur());
          ch_lambda.detach();
          ch_alpha.detach();
          dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_lambda);
          ch_lambda.valeur().valeurs()=ch_lambda_prov.valeur().valeurs();
        }

      if(lambda_nb_comp >1) // Pour anisotrope
        ch_lambda.valeur().fixer_nature_du_champ(multi_scalaire);

      dis.nommer_completer_champ_physique(zone_dis,"conductivite","W/m/K",ch_lambda.valeur(),pb);

      // le vrai nom sera donne plus tard
      if (sub_type(Champ_Fonc_Tabule,ch_lambda.valeur()))
        {
          double temps=ch_lambda.valeur().temps();
          dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_alpha);
        }
      champs_compris_.ajoute_champ(ch_lambda.valeur());
    }
  if (!ch_alpha.non_nul()&&(ch_lambda.non_nul()))
    {
      double temps=ch_lambda.valeur().temps();
      // ch_alpha (i.e. diffusivite_thermique) will have same component number as ch_lambda
      dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_alpha);
    }
  if (ch_alpha.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"diffusivite_thermique","m2/s",ch_alpha.valeur(),pb);
      champs_compris_.ajoute_champ(ch_alpha.valeur());
    }
  if (ch_beta_th.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"dilatabilite","K-1",ch_beta_th.valeur(),pb);
      champs_compris_.ajoute_champ(ch_beta_th.valeur());
    }
  if  (Cp.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"capacite_calorifique","J/kg/K",Cp.valeur(),pb);
      champs_compris_.ajoute_champ(Cp.valeur());
    }
  if  (rho.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"masse_volumique","kg/m^3",rho.valeur(),pb);
      champs_compris_.ajoute_champ(rho.valeur());
    }
  if (rho.non_nul() && Cp.non_nul())
    {
      assert (rho.nb_comp() == Cp.nb_comp());
      if(!rho_cp_comme_T_.non_nul())
        {
          const double temps = pb.schema_temps().temps_courant();
          // E. Saikali
          // XXX : j'ai remis nb_comp = 1, sinon ca bloque dans Solveur_Masse_base => tab_divide_any_shape
          // parce qu'on a pas line_size % line_size_vx == 0 (cas nb_comp >1 pour rho et cp
          // TODO : FIXME : faut coder un cas generique dans DoubleVect::tab_divide_any_shape... bon courage
          dis.discretiser_champ("temperature", zone_dis, "rho_cp_comme_T", "J/m^3/K", 1 /* rho.nb_comp() */, temps, rho_cp_comme_T_);
          dis.discretiser_champ( "champ_elem", zone_dis,    "rho_cp_elem", "J/m^3/K", 1 /* rho.nb_comp() */, temps,    rho_cp_elem_);
        }
      champs_compris_.ajoute_champ(rho_cp_comme_T_.valeur());
      champs_compris_.ajoute_champ(rho_cp_elem_.valeur());
    }

  discretiser_porosite(pb,dis);
}

// methode utile pour F5 ! F5 n'appelle pas Milieu_base::discretiser mais Milieu_base::discretiser_porosite ...
void Milieu_base::discretiser_porosite(const Probleme_base& pb, const Discretisation_base& dis)
{
  pb_ = pb;
  const Zone_dis_base& zone_dis = pb_->equation(0).zone_dis();
  const double temps = pb_->schema_temps().temps_courant();
  Nom fld_name = "porosite_volumique", fld_unit = "rien";

  // On construit porosite_face_ avec un descripteur parallele
  const MD_Vector& md = ref_cast(Zone_VF, zone_dis).md_vector_faces();
  MD_Vector_tools::creer_tableau_distribue(md, porosite_face_, Array_base::NOCOPY_NOINIT);
  assert (ref_cast(Zone_VF, zone_dis).nb_faces_tot() == porosite_face_.size_totale());
  porosite_face_ = 1.;

  if (porosites_champ.non_nul()) // Lu par porosites_champ
    {
      assert (!is_user_porosites());
      if (porosites_.is_read())
        Cerr << "WHAT ?? You can not define in your medium both porosites_champ & porosites ! Remove one of them !" << finl, Process::exit();

      is_field_porosites_ = true;
      if (sub_type(Champ_late_input_P0, porosites_champ.valeur()))
        {
          Cerr << "We continue reading the Champ_late_input_P0 field ..." << finl;
          ref_cast(Champ_late_input_P0,porosites_champ.valeur()).complete_readOn();
          porosites_champ->fixer_unite(fld_unit);
          porosites_champ->valeurs() = 1.; // On initialise a 1 ...
        }
      else if (sub_type(Champ_Fonc_MED,porosites_champ.valeur()))
        {
          Cerr<<"Convert Champ_fonc_MED " << fld_name << " to a Champ_Don ..."<<finl;
          Champ_Don tmp_fld;
          dis.discretiser_champ("champ_elem",zone_dis,"neant",fld_unit,1,temps,tmp_fld);
          tmp_fld.affecter_(porosites_champ.valeur()); // interpolate ...
          porosites_champ.detach();
          dis.discretiser_champ("champ_elem",zone_dis,fld_name,fld_unit,1,temps,porosites_champ);
          porosites_champ->valeurs() = tmp_fld->valeurs();
        }
      else if (sub_type(Champ_Uniforme,porosites_champ.valeur())) // blabla ...
        {
          const double val = porosites_champ->valeurs()(0,0);
          porosites_champ.detach();
          dis.discretiser_champ("champ_elem",zone_dis,fld_name,fld_unit,1,temps,porosites_champ);
          porosites_champ->valeurs() = val;
        }
      else
        dis.nommer_completer_champ_physique(zone_dis, fld_name, fld_unit, porosites_champ.valeur(), pb);

      champs_compris_.ajoute_champ(porosites_champ.valeur());
    }
  else if (porosites_.is_read()) // via porosites
    {
      assert (!is_field_porosites());
      if (porosites_champ.non_nul())
        Cerr << "WHAT ?? You can not define in your medium both porosites_champ & porosites ! Remove one of them !" << finl, Process::exit();

      is_user_porosites_ = true;
      // On va utiliser porosites_champ maintenant !
      dis.discretiser_champ("champ_elem", zone_dis, fld_name, fld_unit, 1, temps, porosites_champ);
      champs_compris_.ajoute_champ(porosites_champ.valeur());
      Zone_VF& zvf = ref_cast_non_const(Zone_VF, zone_dis);
      porosites_champ->valeurs() = 1.; // On initialise a 1 ...
      porosites_.remplir_champ(zvf, porosites_champ->valeurs(), porosite_face_);
    }
  else // Pas defini par l'utilisateur
    {
      // On va utiliser porosites_champ maintenant !
      dis.discretiser_champ("champ_elem", zone_dis, fld_name, fld_unit, 1, temps, porosites_champ);
      champs_compris_.ajoute_champ(porosites_champ.valeur());
      porosites_champ->valeurs() = 1.; // On initialise a 1 ...
    }
  assert(mp_min_vect(porosites_champ->valeurs()) >= 0. && mp_max_vect(porosites_champ->valeurs()) <= 1.);
}

void Milieu_base::verifier_coherence_champs(int& err,Nom& msg)
{
  if (err==1)
    {
      Cerr<<"Error while reading the physical properties of a "<<que_suis_je()<<" medium."<<finl;
      Cerr<<msg<<finl;
      Process::exit();
    }
  else if (err==2)
    {
      Cerr<<"Warning while reading the physical properties of a "<<que_suis_je()<<" medium."<<finl;
      Cerr<<msg<<finl;
    }
  else
    Cerr<<"The physical properties of a "<<que_suis_je()<<" medium have been successfully checked."<<finl;
}

void Milieu_base::preparer_calcul()
{
  int err=0;
  Nom msg;
  verifier_coherence_champs(err,msg);
}

void Milieu_base::creer_champs_non_lus()
{
  /* je teste la gravite ici ... */
  if (g.non_nul())
    if(Objet_U::dimension != g->nb_comp())
      {
        Cerr << "The dimension is " << Objet_U::dimension << " and you create a gravity vector with " << g->nb_comp() << " components." << finl;
        Process::exit();
      }

  if (rho.non_nul() && lambda.non_nul() && Cp.non_nul())
    creer_alpha();
}

void Milieu_base::warn_old_syntax()
{
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << finl;
  Cerr << "*** ATTENTION *** YOU ARE USING THE OLD SYNTAX OF A DATA FILE :-( :-( :-( " << finl;
  Cerr << "*** ATTENTION *** STARTING FROM TRUST-V1.9.3 : THE GRAVITY SHOULD BE READ INSIDE THE MEDIUM AND NOT VIA ASSOSCIER ... " << finl;
  Cerr << "*** ATTENTION *** STARTING FROM TRUST-V1.9.3 : THIS OLD SYNTAX WILL NOT BE SUPPORTED ANYMORE ... " << finl;
  Cerr << "*** ATTENTION *** HAVE A LOOK TO ANY TRUST TEST CASE TO SEE HOW IT SHOULD BE DONE ($TRUST_ROOT/tests/)  ... " << finl;
  Cerr << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << finl;
//  throw;
}

/*! @brief Associe la gravite en controlant dynamiquement le type de l'objet a associer.
 *
 *     Si l'objet est du type Champ_Don ou Champ_Don_base
 *     l'association reussit, sinon elle echoue.
 *
 * @param (Objet_U& ob) un objet TRUST devant representer un champ de gravite
 * @return (int) 1 si l'association a reussie, 0 sinon.
 */
int Milieu_base::associer_(Objet_U& ob)
{
  if (sub_type(Champ_Don_base,ob))
    {
      warn_old_syntax();
      via_associer_ = true;
      warn_old_syntax();
      associer_gravite(ref_cast(Champ_Don_base, ob));
      return 1;
    }
  if (sub_type(Champ_Don,ob))
    {
      warn_old_syntax();
      via_associer_ = true;
      warn_old_syntax();
      associer_gravite(ref_cast(Champ_Don, ob).valeur());
      return 1;
    }
  return 0;
}

/*! @brief Associe (affecte) un champ de gravite au milieu.
 *
 * @param (Champ_Don_base& gravite) champ donne representant la gravite
 */
void Milieu_base::associer_gravite(const Champ_Don_base& la_gravite)
{
  // On verifie que la gravite est de la bonne dimension
  if (Objet_U::dimension!=la_gravite.nb_comp())
    {
      Cerr << "The dimension is " << Objet_U::dimension << " and you create a gravity vector with " << la_gravite.nb_comp() << " components." << finl;
      exit();
    }
  g_via_associer_ = la_gravite;

  if (g_via_associer() && g.non_nul())
    {
      assert (g_via_associer_.non_nul());
      Cerr << "WHAT ?? Remove the associer gravity line from your jdd because it is already in the medium !!!" << finl;
      Process::exit();
    }
}

/*! @brief Calcul de alpha=lambda/(rho*Cp).
 *
 * Suivant lambda, rho et Cp alpha peut-etre type comme
 *     un champ uniforme ou un champ uniforme par morceau.
 *
 * @throws violation d'une precondition
 * @throws impossible de calculer alpha car le type des champs
 * lambda,rho,Cp n'est pas compatible ou pas gere.
 */
void Milieu_base::calculer_alpha()
{
  if(lambda.non_nul())
    {
      DoubleTab& tabalpha = alpha.valeurs();
      tabalpha = lambda.valeurs();

      // [ABN]: allows variable rho, Cp at this level (will be used by Solide_Milieu_Variable for instance).
      if (sub_type(Champ_Uniforme,rho.valeur()))
        tabalpha /= rho.valeurs()(0,0);
      else
        tab_divide_any_shape(tabalpha,rho.valeurs());

      if (sub_type(Champ_Uniforme,Cp.valeur()))
        tabalpha /= Cp.valeurs()(0,0);
      else
        tab_divide_any_shape(tabalpha,Cp.valeurs());
    }
  else
    Cerr << "alpha calculation is not possible since lambda is not known." << finl;
}

void Milieu_base::update_porosity_values()
{
  assert(is_field_porosites());
  const Zone_dis_base& zdb = pb_->equation(0).zone_dis().valeur();
  Zone_VF& zvf = ref_cast_non_const(Zone_VF, zdb);

  // update porosite_faces
  const int nb_face_tot = zvf.nb_faces_tot();
  assert (nb_face_tot == porosite_face_.size_totale());
  const IntTab& face_voisins = zvf.face_voisins();

  for (int face = 0; face < nb_face_tot; face++)
    {
      const int elem1 = face_voisins(face, 1), elem2 = face_voisins(face, 0);
      if ((elem1 != -1) && (elem2 != -1))
        porosite_face_(face) = 2. / (1. / porosites_champ->valeurs()(elem1) + 1. / porosites_champ->valeurs()(elem2));
      else if (elem1 != -1)
        porosite_face_(face) = porosites_champ->valeurs()(elem1);
      else
        porosite_face_(face) = porosites_champ->valeurs()(elem2);
    }
  porosite_face_.echange_espace_virtuel();
}

void Milieu_base::mettre_a_jour(double temps)
{
  //Cerr << que_suis_je() << "Milieu_base::mettre_a_jour" << finl;
  if (rho.non_nul()) rho.mettre_a_jour(temps);

  if (g.non_nul()) g->mettre_a_jour(temps);

  if (g_via_associer_.non_nul()) g_via_associer_->mettre_a_jour(temps);

  if (lambda.non_nul()) lambda.mettre_a_jour(temps);

  if (Cp.non_nul()) Cp.mettre_a_jour(temps);

  if (beta_th.non_nul()) beta_th.mettre_a_jour(temps);

  if ( (lambda.non_nul()) && (Cp.non_nul()) && (rho.non_nul()) )
    {
      calculer_alpha();
      alpha.valeur().changer_temps(temps);
    }

  if (rho_cp_comme_T_.non_nul()) update_rho_cp(temps);

  mettre_a_jour_porosite(temps); // pour F5 !
}

// methode utile pour F5 ! F5 n'appelle pas Milieu_base::mettre_a_jour mais Milieu_base::mettre_a_jour_porosite ...
void Milieu_base::mettre_a_jour_porosite(double temps)
{
  assert(porosites_champ.non_nul());
  if (is_field_porosites())
    if (sub_type(Champ_late_input_P0, porosites_champ.valeur())) update_porosity_values();

  porosites_champ.mettre_a_jour(temps);
}

void Milieu_base::update_rho_cp(double temps)
{
  rho_cp_elem_.changer_temps(temps);
  rho_cp_elem_.valeur().changer_temps(temps);
  DoubleTab& rho_cp=rho_cp_elem_.valeurs();
  if (sub_type(Champ_Uniforme,rho.valeur()))
    rho_cp=rho.valeurs()(0,0);
  else
    {
      // AB: rho_cp = rho.valeurs() turns rho_cp into a 2 dimensional array with 1 compo. We want to stay mono-dim:
      rho_cp = 1.;
      tab_multiply_any_shape(rho_cp, rho.valeurs());
    }
  if (sub_type(Champ_Uniforme,Cp.valeur()))
    rho_cp*=Cp.valeurs()(0,0);
  else
    tab_multiply_any_shape(rho_cp,Cp.valeurs());
  rho_cp_comme_T_.changer_temps(temps);
  rho_cp_comme_T_.valeur().changer_temps(temps);
  const MD_Vector& md_som = rho_cp_elem_.zone_dis_base().domaine_dis().domaine().md_vector_sommets(),
                   &md_faces = ref_cast(Zone_VF,rho_cp_elem_.zone_dis_base()).md_vector_faces();
  if (rho_cp_comme_T_.valeurs().get_md_vector() == rho_cp_elem_.valeurs().get_md_vector())
    rho_cp_comme_T_.valeurs() = rho_cp;
  else if (rho_cp_comme_T_.valeurs().get_md_vector() == md_som)
    Discretisation_tools::cells_to_nodes(rho_cp_elem_,rho_cp_comme_T_);
  else if (rho_cp_comme_T_.valeurs().get_md_vector() == md_faces)
    Discretisation_tools::cells_to_faces(rho_cp_elem_,rho_cp_comme_T_);
  else
    {
      Cerr<< que_suis_je()<<(int)__LINE__<<finl;
      Process::exit();
    }
}

void Milieu_base::abortTimeStep()
{
  if (rho.non_nul()) rho->abortTimeStep();
}

bool Milieu_base::initTimeStep(double dt)
{
  return true;
}

void Milieu_base::creer_alpha()
{
  if (Process::je_suis_maitre())
    Cerr << "Milieu_base::creer_alpha (champ non lu)" << finl;
  assert(lambda.non_nul());
  assert(rho.non_nul());
  assert(Cp.non_nul());
  alpha=lambda;
  alpha->nommer("alpha");
}

/*! @brief Renvoie la gravite du milieu si elle a ete associe provoque une erreur sinon.
 *
 *     (version const)
 *
 * @return (Champ_Don_base&) le champ representant la gravite du milieu
 * @throws pas de gravite associee au milieu
 */
const Champ_Don_base& Milieu_base::gravite() const
{
  if (!g.non_nul() && !g_via_associer_.non_nul())
    {
      Cerr << "The gravity has not been associated with the medium" << finl;
      Process::exit();
    }

  return g.non_nul() ? g.valeur() : g_via_associer_.valeur();
}

/*! @brief Renvoie la gravite du milieu si elle a ete associe provoque une erreur sinon.
 *
 * @return (Champ_Don_base&) le champ representant la gravite du milieu
 */
Champ_Don_base& Milieu_base::gravite()
{
  if (!g.non_nul() && !g_via_associer_.non_nul())
    {
      Cerr << "The gravity has not been associated with the medium" << finl;
      Process::exit();
    }

  return g.non_nul() ? g.valeur() : g_via_associer_.valeur();
}

int Milieu_base::initialiser(const double temps)
{
  Cerr << que_suis_je() << "Milieu_base:::initialiser" << finl;
  if (sub_type(Champ_Don_base, rho.valeur())) ref_cast(Champ_Don_base, rho.valeur()).initialiser(temps);

  if (g.non_nul()) g->initialiser(temps);

  if (g_via_associer_.non_nul()) g_via_associer_->initialiser(temps);

  if (lambda.non_nul()) lambda.initialiser(temps);

  if (Cp.non_nul()) Cp.initialiser(temps);

  if (beta_th.non_nul()) beta_th.initialiser(temps);

  if ( (lambda.non_nul()) && (Cp.non_nul()) && (rho.non_nul()) )
    {
      calculer_alpha();
      alpha.valeur().changer_temps(temps);
    }

  if (rho_cp_comme_T_.non_nul()) update_rho_cp(temps);

  return initialiser_porosite(temps);
}

// methode utile pour F5 ! F5 n'appelle pas Milieu_base::initialiser mais Milieu_base::initialiser_porosite ...
int Milieu_base::initialiser_porosite(const double temps)
{
  // TODO : XXX : a voir si ICoCo ? faut l'initialiser dans le main ?
  assert(porosites_champ.non_nul());
  if (is_field_porosites()) update_porosity_values(); /* sinon c'est deja rempli ... */
  porosites_champ.initialiser(temps);
  return 1;
}

/*! @brief Renvoie la masse volumique du milieu.
 *
 * (version const)
 *
 * @return (Champ_base&) le champ donne representant la masse volumique
 */
const Champ& Milieu_base::masse_volumique() const
{
  return rho;
}

/*! @brief Renvoie la masse volumique du milieu.
 *
 * @return (Champ_base&) le champ donne representant la masse volumique
 */
Champ& Milieu_base::masse_volumique()
{
  return rho;
}

/*! @brief Renvoie la diffusivite du milieu.
 *
 * (version const)
 *
 * @return (Champ_Don&) le champ donne representant la diffusivite
 */
const Champ_Don& Milieu_base::diffusivite() const
{
  return alpha;
}

/*! @brief Renvoie la diffusivite du milieu.
 *
 * @return (Champ_Don&) le champ donne representant la diffusivite
 */
Champ_Don& Milieu_base::diffusivite()
{
  return alpha;
}

/*! @brief Renvoie la conductivite du milieu.
 *
 * (version const)
 *
 * @return (Champ_Don&) le champ donne representant la conductivite
 */
const Champ_Don& Milieu_base::conductivite() const
{
  return lambda;
}

/*! @brief Renvoie la conductivite du milieu.
 *
 * @return (Champ_Don&) le champ donne representant la conductivite
 */
Champ_Don& Milieu_base::conductivite()
{
  return lambda;
}

/*! @brief Renvoie la capacite calorifique du milieu.
 *
 * (version const)
 *
 * @return (Champ_Don&) le champ donne representant la capacite calorifique
 */
const Champ_Don& Milieu_base::capacite_calorifique() const
{
  return Cp;
}

/*! @brief Renvoie la capacite calorifique du milieu.
 *
 * @return (Champ_Don&) le champ donne representant la capacite calorifique
 */
Champ_Don& Milieu_base::capacite_calorifique()
{
  return Cp;
}

/*! @brief Renvoie beta_t du milieu.
 *
 * (version const)
 *
 * @return (Champ_Don&) le champ donne representant beta_t
 */
const Champ_Don& Milieu_base::beta_t() const
{
  return beta_th;
}

/*! @brief Renvoie beta_t du milieu.
 *
 * @return (Champ_Don&) le champ donne representant beta_t
 */
Champ_Don& Milieu_base::beta_t()
{
  return beta_th;
}

/*! @brief Renvoie 1 si la gravite a ete initialisee
 *
 * @return (int) 1 si g.non_nul
 */
int Milieu_base::a_gravite() const
{
  return (g.non_nul() || g_via_associer_.non_nul()) ? 1 : 0;
}

int Milieu_base::is_rayo_semi_transp() const
{
  return 0;
}

int Milieu_base::is_rayo_transp() const
{
  return 0;
}

void Milieu_base::creer_champ(const Motcle& motlu)
{

}

const Champ_base& Milieu_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}
void Milieu_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<que_suis_je()<<" : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}


/*! @brief Renvoie 0 si le milieu est deja associe a un probleme, 1 sinon
 *
 * @return (int)
 */
int Milieu_base::est_deja_associe()
{
  if (deja_associe==1)
    return 0;
  deja_associe=1;
  return 1;
}

void Milieu_base::associer_equation(const Equation_base *eqn) const
{
  std::string nom_inco(eqn->inconnue().le_nom().getString());
  // E. Saikali
  // At the initialization step, FT problem can have several equations with same unknown name "concentration"
  if (equation_.count(nom_inco) && eqn->probleme().que_suis_je() != "Probleme_FT_Disc_gen")
    Cerr << que_suis_je() << " multiple equations solve the unknown " << eqn->inconnue().le_nom() << " !" << finl, Process::exit();
  equation_[nom_inco] = eqn;
}

const Equation_base& Milieu_base::equation(const std::string& inco) const
{
  return *equation_.at(inco);
}

void Milieu_base::set_id_composite(const int i)
{
  id_composite = i;
}

void Milieu_base::nommer(const Nom& nom)
{
  nom_ = nom;
}
const Nom& Milieu_base::le_nom() const
{
  return nom_;
}
