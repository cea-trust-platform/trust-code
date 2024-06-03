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

#include <Convection_Diffusion_Temperature.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Fluide_Ostwald.h>
#include <Discret_Thyd.h>
#include <Frontiere_dis_base.h>
#include <Param.h>
#include <Transport_Interfaces_base.h>
#include <TRUSTTrav.h>
#include <SFichier.h>
#include <Domaine_VF.h>
#include <Matrice_Morse.h>
#include <Champ_Uniforme.h>
#include <Statistiques.h>

extern Stat_Counter_Id assemblage_sys_counter_;
extern Stat_Counter_Id source_counter_;

Implemente_instanciable_sans_constructeur(Convection_Diffusion_Temperature,"Convection_Diffusion_Temperature",Convection_Diffusion_std);

Convection_Diffusion_Temperature::Convection_Diffusion_Temperature()
{
//  champs_compris_.ajoute_nom_compris("temperature_paroi");
  champs_compris_.ajoute_nom_compris("gradient_temperature");
  champs_compris_.ajoute_nom_compris("h_echange_");
  champs_compris_.ajoute_nom_compris("temperature_residu");
  eta = 1.0;
  is_penalized = 0;
  tag_indic_pena_global = -1;
  indic_pena_global = 0;
  indic_face_pena_global = 0;
  choix_pena = 0;
}
/*! @brief Simple appel a: Convection_Diffusion_std::printOn(Sortie&)
 *
 * @param (Sortie& is) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Convection_Diffusion_Temperature::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

/*! @brief Verifie si l'equation a une temperature et un fluide associe cf Convection_Diffusion_std::readOn(Entree&).
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree& is) le flot d'entree modifie
 */
Entree& Convection_Diffusion_Temperature::readOn(Entree& is)
{
  assert(la_temperature.non_nul());
  assert(le_fluide.non_nul());
  Convection_Diffusion_std::readOn(is);
  //Nom unite;
  //if (dimension+bidim_axi==2) unite="[W/m]";
  //else unite="[W]";
  Nom num=inconnue().le_nom(); // On prevoir le cas d'equation de scalaires passifs
  num.suffix("temperature");
  Nom nom="Convection_chaleur";
  nom+=num;
  terme_convectif.set_fichier(nom);
  //terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-rho*cp*T*u*ndS) "+unite);
  terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-rho*cp*T*u*ndS) [W] if SI units used");
  nom="Diffusion_chaleur";
  nom+=num;
  terme_diffusif.set_fichier(nom);
  //terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) "+unite);
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");
  solveur_masse->set_name_of_coefficient_temporel("rho_cp_comme_T");
  return is;
}

void Convection_Diffusion_Temperature::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
  param.ajouter_non_std("penalisation_L2_FTD",(this));
}

int Convection_Diffusion_Temperature::lire_motcle_non_standard(const Motcle& un_mot, Entree& is)
{
  if (un_mot=="penalisation_L2_FTD")
    {
      is_penalized = 1;
      eta = 1.e-12;
      Cerr << " Equation_base :: penalisation_L2_FTD" << finl;
      Cerr << "Equation_base: modele penalisation ibc = "<< choix_pena << finl;
      Nom mot, nom_equation;
      int dim;
      is >> mot;
      Motcle accolade_fermee="}", accolade_ouverte="{";
      if (mot == accolade_ouverte)
        {
          is >> mot;
          while (mot != accolade_fermee)
            {
              nom_equation = mot;

              Cerr << "penalisation_L2_FTD equation: "<< nom_equation << finl;
              const Transport_Interfaces_base& eq = ref_cast(Transport_Interfaces_base,probleme().get_equation_by_name(nom_equation));
              ref_penalisation_L2_FTD.add(eq);

              is >> dim;
              DoubleTab tab = DoubleTab();
              tab.resize(dim);
              for ( int i = 0 ; i<dim ; i++)
                {
                  is >> tab(i);
                  Cerr << "Lecture de la valeur de la penalisation suivant la direction " << i+1 <<" :" << tab(i) << finl;
                }
              tab_penalisation_L2_FTD.add(tab);
              is >> mot ;
            }
        }
      else
        {
          Cerr << "Erreur a la lecture des parametres de la penalisation L2 " << finl;
          Cerr << "On attendait : " << accolade_ouverte << finl;
          exit();
        }
      const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis().valeur());
      domaine_vf.domaine().creer_tableau_elements(indic_pena_global);
      domaine_vf.creer_tableau_faces(indic_face_pena_global);
    }
  else
    return Convection_Diffusion_std::lire_motcle_non_standard(un_mot,is);
  return 1;
}

/*! @brief Associe un milieu physique a l'equation, le milieu est en fait caste en Fluide_base ou en Fluide_Ostwald.
 *
 * @param (Milieu_base& un_milieu)
 * @throws les proprietes physiques du fluide ne sont pas toutes specifiees
 */
void Convection_Diffusion_Temperature::associer_milieu_base(const Milieu_base& un_milieu)
{
  if (sub_type(Fluide_base,un_milieu)) associer_fluide(ref_cast(Fluide_base, un_milieu));
  else Process::exit(que_suis_je() + " : le fluide " + un_milieu.que_suis_je() + " n'est pas de type Fluide_base!");
}


/*! @brief Discretise l'equation.
 *
 */
void Convection_Diffusion_Temperature::discretiser()
{


  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  if (Process::je_suis_maitre())
    Cerr << "Energy equation discretization" << finl;
  dis.temperature(schema_temps(), domaine_dis(), la_temperature);
  champs_compris_.ajoute_champ(la_temperature);

  Equation_base::discretiser();

  if (Process::je_suis_maitre())
    Cerr << "Convection_Diffusion_Temperature::discretiser() ok" << finl;
}

int Convection_Diffusion_Temperature::preparer_calcul()
{
  /* derniere chance pour faire ceci : */
  if  (le_fluide->conductivite().est_nul() || le_fluide->capacite_calorifique().est_nul() || le_fluide->beta_t().est_nul())
    {
      Cerr << "Vous n'avez pas defini toutes les proprietes physiques du fluide " << finl;
      Cerr << "necessaires pour resoudre l'equation d'energie " << finl;
      Cerr << "Verifier que vous avez defini: la conductivite (lambda)"<< finl;
      Cerr << "                                  la capacite calorifique (Cp)"<< finl;
      Cerr << "                                  le coefficient de dilatation thermique (beta_th)"<< finl;
      exit();
    }
  return Equation_base::preparer_calcul();
}

/*! @brief Renvoie le fluide incompressible associe a l'equation.
 *
 * (version const)
 *
 * @return (Fluide_base&) le fluide incompressible associe a l'equation
 * @throws pas de fluide associe a l'eqaution
 */
const Fluide_base& Convection_Diffusion_Temperature::fluide() const
{
  if(le_fluide.est_nul())
    {
      Cerr << "A fluid has not been associated to "
           << "the Convection_Diffusion_Temperature equation" << finl;
      exit();
    }
  return le_fluide.valeur();
}


/*! @brief Renvoie le fluide incompressible associe a l'equation.
 *
 * @return (Fluide_base&) le fluide incompressible associe a l'equation
 * @throws pas de fluide associe a l'eqaution
 */
Fluide_base& Convection_Diffusion_Temperature::fluide()
{
  if(le_fluide.est_nul())
    {
      Cerr << "A fluid has not been associated to"
           << "the Convection_Diffusion_Temperature equation" << finl;
      exit();
    }
  return le_fluide.valeur();
}

inline int string2int(const char* digit, int& result)
{
  result = 0;

  //--- Convert each digit char and add into result.
  while (*digit >= '0' && *digit <='9')
    {
      result = (result * 10) + (*digit - '0');
      digit++;
    }

  //--- Check that there were no non-digits at end.
  if (*digit != 0)
    {
      return 0;
    }

  return 1;
}

void Convection_Diffusion_Temperature::creer_champ(const Motcle& motlu)
{
  Convection_Diffusion_std::creer_champ(motlu);

  Motcle nom_mot(motlu),temp_mot(nom_mot);
  /*  if (motlu == "temperature_paroi")
      {
        if (!temperature_paroi.non_nul())
          {
            const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
            dis.t_paroi(domaine_dis(),domaine_Cl_dis(),*this,temperature_paroi);
            champs_compris_.ajoute_champ(temperature_paroi);
          }
      } */
  if (motlu == "gradient_temperature")
    {
      if (gradient_temperature.est_nul())
        {
          const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
          dis.grad_T(domaine_dis(),domaine_Cl_dis(),la_temperature,gradient_temperature);
          champs_compris_.ajoute_champ(gradient_temperature);
        }
    }
  if (nom_mot.debute_par("H_ECHANGE"))
    {
      if (h_echange.est_nul())
        {
          const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
          temp_mot.suffix("H_ECHANGE_");
          int temperature;
          string2int(temp_mot,temperature);
          dis.h_conv(domaine_dis(),domaine_Cl_dis(),la_temperature,h_echange,nom_mot,temperature);
          champs_compris_.ajoute_champ(h_echange);
        }
    }
}

const Champ_base& Convection_Diffusion_Temperature::get_champ(const Motcle& nom) const
{


  /*  if (nom=="temperature_paroi")
      {
        double temps_init = schema_temps().temps_init();
        Champ_Fonc_base& ch_tp=ref_cast_non_const(Champ_Fonc_base,temperature_paroi.valeur());
        if (((ch_tp.temps()!=la_temperature->temps()) || (ch_tp.temps()==temps_init)) && ((la_temperature->mon_equation_non_nul())))
          ch_tp.mettre_a_jour(la_temperature->temps());
        return champs_compris_.get_champ(nom);
      } */
  if (nom=="gradient_temperature")
    {
      double temps_init = schema_temps().temps_init();
      Champ_Fonc_base& ch_gt=ref_cast_non_const(Champ_Fonc_base,gradient_temperature.valeur());
      if (((ch_gt.temps()!=la_temperature->temps()) || (ch_gt.temps()==temps_init)) && (la_temperature->mon_equation_non_nul()))
        ch_gt.mettre_a_jour(la_temperature->temps());
      return champs_compris_.get_champ(nom);
    }
  if (h_echange.non_nul())
    if (nom==h_echange.valeur().le_nom())
      {
        double temps_init = schema_temps().temps_init();
        Champ_Fonc_base& ch_hconv=ref_cast_non_const(Champ_Fonc_base,h_echange.valeur());
        if (((ch_hconv.temps()!=la_temperature->temps()) || (ch_hconv.temps()==temps_init)) && (la_temperature->mon_equation_non_nul()))
          {
            ch_hconv.mettre_a_jour(la_temperature->temps());

          }
        return champs_compris_.get_champ(nom);
      }

  try
    {
      return Convection_Diffusion_std::get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }

  throw Champs_compris_erreur();
  REF(Champ_base) ref_champ;

  return ref_champ;
}

/*! @brief Renvoie le nom du domaine d'application de l'equation.
 *
 * Ici "Thermique".
 *
 * @return (Motcle&) le nom du domaine d'application de l'equation
 */
const Motcle& Convection_Diffusion_Temperature::domaine_application() const
{
  static Motcle domaine = "Thermique";
  return domaine;
}

DoubleTab& Convection_Diffusion_Temperature::derivee_en_temps_inco(DoubleTab& derivee)
{
  has_time_factor_ = 1;
  if (!is_penalized)
    {
      return Equation_base::derivee_en_temps_inco(derivee);
    }
  else
    {
      const double rhoCp = get_time_factor();
      // Specific code if temperature equation is penalized
      derivee=inconnue().valeurs();
      //   Mise en place d'une methode de mise en place d'un domaine fantome
      mise_en_place_domaine_fantome(derivee);
      DoubleTab& inc=inconnue().valeurs();
      inc = derivee;

      DoubleTrav secmem(derivee);

      if (nombre_d_operateurs()>1)
        {
          derivee_en_temps_conv(secmem, derivee);
          secmem *= rhoCp;
        }

      // Transport des ibcs et de la couche limite
      DoubleTrav secmem_conv_vr(derivee);
      //on advecte la variable avec la vitesse ibc
      transport_ibc(secmem_conv_vr, derivee);
      //on retranche cette advection
      secmem -= secmem_conv_vr;

      DoubleTab derivee_conv;
      derivee_conv.copy(secmem, Array_base::COPY_INIT);

      les_sources.ajouter(secmem);

      filtrage_si_appart_ibc(derivee_conv,secmem);
      secmem.echange_espace_virtuel();

      if (calculate_time_derivative())
        {
          // Store dI/dt(n) = M-1 secmem :
          derivee_en_temps().valeurs()=secmem;
          solveur_masse.appliquer(derivee_en_temps().valeurs());
          schema_temps().modifier_second_membre((*this),secmem); // Change secmem for some schemes (eg: Adams_Bashforth)
        }

      solveur_masse.appliquer(secmem);
      Equation_base::Gradient_conjugue_diff_impl(secmem, derivee);

      //  penalisation de la temperature
      penalisation_L2(derivee);
      return derivee;
    }
}

double Convection_Diffusion_Temperature::get_time_factor() const
{
  return milieu().capacite_calorifique().valeurs()(0, 0) * milieu().masse_volumique().valeurs()(0, 0);
}

// ajoute les contributions des operateurs et des sources
void Convection_Diffusion_Temperature::assembler(Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  const double rhoCp = get_time_factor();

  // Test de verification de la methode contribuer_a_avec
  for (int op=0; op<nombre_d_operateurs(); op++)
    operateur(op).l_op_base().tester_contribuer_a_avec(inco, matrice);

  // Contribution des operateurs et des sources:
  // [Vol/dt+A]Inco(n+1)=somme(residu)+Vol/dt*Inco(n)
  // Typiquement: si Op=flux(Inco) alors la matrice implicite A contient une contribution -dflux/dInco
  // Exemple: Op flux convectif en VDF:
  // Op=T*u*S et A=-d(T*u*S)/dT=-u*S
  const Discretisation_base::type_calcul_du_residu& type_codage=probleme().discretisation().codage_du_calcul_du_residu();
  if (type_codage==Discretisation_base::VIA_CONTRIBUER_AU_SECOND_MEMBRE)
    {
      if ( probleme().discretisation().que_suis_je() == "EF")
        {
          // On calcule somme(residu) par contribuer_au_second_membre (typiquement CL non implicitees)
          // Cette approche necessite de coder 3 methodes (contribuer_a_avec, contribuer_au_second_membre et ajouter pour l'explicite)
          sources().contribuer_a_avec(inco,matrice);
          statistiques().end_count(assemblage_sys_counter_,0,0);
          sources().ajouter(resu);
          statistiques().begin_count(assemblage_sys_counter_);
          matrice.ajouter_multvect(inco, resu); // Add source residual first
          for (int op = 0; op < nombre_d_operateurs(); op++)
            {
              operateur(op).l_op_base().contribuer_a_avec(inco, matrice);
              operateur(op).l_op_base().contribuer_au_second_membre(resu);
            }
        }
      else
        {
          Cerr << "VIA_CONTRIBUER_AU_SECOND_MEMBRE pas code pour " << que_suis_je() << ":assembler" << finl;
          Cerr << "avec discretisation " <<  probleme().discretisation().que_suis_je() << "" << finl;
          Process::exit();
        }
      // // On calcule somme(residu) par contribuer_au_second_membre (typiquement CL non implicitees)
      // // Cette approche necessite de coder 3 methodes (contribuer_a_avec, contribuer_au_second_membre et ajouter pour l'explicite)
      // sources().contribuer_a_avec(inco,matrice);
      // statistiques().end_count(assemblage_sys_counter_,0,0);
      // sources().ajouter(resu);
      // statistiques().begin_count(assemblage_sys_counter_);
      // matrice.ajouter_multvect(inco, resu); // Add source residual first
      // for (int op = 0; op < nombre_d_operateurs(); op++)
      //   {
      //     operateur(op).l_op_base().contribuer_a_avec(inco, matrice);
      //     operateur(op).l_op_base().contribuer_au_second_membre(resu);
      //   }
    }
  else if (type_codage==Discretisation_base::VIA_AJOUTER)
    {
      // On calcule somme(residu) par somme(operateur)+sources+A*Inco(n)
      // Cette approche necessite de coder seulement deux methodes (contribuer_a_avec et ajouter)
      // Donc un peu plus couteux en temps de calcul mais moins de code a ecrire/maintenir
      for (int op=0; op<nombre_d_operateurs(); op++)
        {
          Matrice_Morse mat(matrice);
          mat.get_set_coeff() = 0.0;
          operateur(op).l_op_base().contribuer_a_avec(inco, mat);
          if (op == 1) mat *= rhoCp; // la derivee est multipliee par rhoCp pour la convection
          matrice += mat;
          statistiques().end_count(assemblage_sys_counter_, 0, 0);
          {
            DoubleTab resu_tmp(resu);
            resu_tmp = 0.;
            operateur(op).ajouter(inco, resu_tmp);
            if (op == 1) resu_tmp *= rhoCp;
            resu += resu_tmp;
          }
          statistiques().begin_count(assemblage_sys_counter_);
        }
      sources().contribuer_a_avec(inco,matrice);
      statistiques().end_count(assemblage_sys_counter_,0,0);
      sources().ajouter(resu);
      statistiques().begin_count(assemblage_sys_counter_);
      matrice.ajouter_multvect(inco, resu); // Ajout de A*Inco(n)
      // PL (11/04/2018): On aimerait bien calculer la contribution des sources en premier
      // comme dans le cas VIA_CONTRIBUER_AU_SECOND_MEMBRE mais le cas Canal_perio_3D (keps
      // periodique plante: il y'a une erreur de periodicite dans les termes sources du modele KEps...
    }
  else
    {
      Cerr << "Unknown value in Equation_base::assembler for " << (int)type_codage << finl;
      Process::exit();
    }
}

void Convection_Diffusion_Temperature::assembler_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (discretisation().is_polymac_family() || probleme().que_suis_je() == "Pb_Multiphase")
    {
      Equation_base::assembler_blocs(matrices, secmem, semi_impl);
      return;
    }
  const double rhoCp = get_time_factor();
  /* mise a zero */
  secmem = 0;
  for (auto &&i_m : matrices) i_m.second->get_set_coeff() = 0;
  matrices_t mats2;
  for (auto &&i_m : matrices)
    {
      Matrice_Morse* mat2 = new Matrice_Morse;
      *mat2=*i_m.second;
      mats2[i_m.first] = mat2;
    }

  /* operateurs, sources, masse */
  for (int i = 0; i < nombre_d_operateurs(); i++)
    {
      DoubleTab secmem_tmp(secmem);
      secmem_tmp = 0.;
      for (auto &&i_m : mats2)
        mats2[i_m.first]->get_set_coeff() = 0;

      operateur(i).l_op_base().ajouter_blocs(mats2, secmem_tmp, semi_impl);

      if (i == 1)
        {
          secmem_tmp *= rhoCp;
          for (auto &&i_m : mats2) *i_m.second *= rhoCp;
        }

      for (auto &&i_m : matrices) *i_m.second += *mats2[i_m.first];
      secmem += secmem_tmp;

    }
  statistiques().end_count(assemblage_sys_counter_, 0, 0);

  statistiques().begin_count(source_counter_);
  for (int i = 0; i < les_sources.size(); i++)
    les_sources(i).valeur().ajouter_blocs(matrices, secmem, semi_impl);
  statistiques().end_count(source_counter_);

  statistiques().begin_count(assemblage_sys_counter_);

  const std::string& nom_inco = inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco)?matrices.at(nom_inco):NULL;
  if(mat) mat->ajouter_multvect(inconnue().valeurs(), secmem);

  for (auto &&i_m : mats2)
    delete i_m.second;

}
int Convection_Diffusion_Temperature::verifier_tag_indicatrice_pena_glob()
{
  //Les ibcs ont elles ete modifiees ?
  int maj = 0;
  int tag_all = 0;
  for ( int w = 0; w<ref_penalisation_L2_FTD.size() ; ++w)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
      tag_all += nom_eq.get_mesh_tag();
    }
  if (tag_all == tag_indic_pena_global) maj = 1;
  return maj;
}

int Convection_Diffusion_Temperature::mise_a_jour_tag_indicatrice_pena_glob()
{
  //Les ibcs ont elles ete modifiees ?
  int maj = 0;
  int tag_all = 0;
  for ( int w = 0; w<ref_penalisation_L2_FTD.size() ; ++w)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
      tag_all += nom_eq.get_mesh_tag();
    }
  if (tag_all != tag_indic_pena_global)
    {
      tag_indic_pena_global = tag_all;
      maj = 1;
    }
  return maj;
}

void Convection_Diffusion_Temperature::set_indic_pena_globale()
{
  //  Set de la fonction characteristique ibc globlale (element et faces)
  int maj = verifier_tag_indicatrice_pena_glob();
  if (maj == 0)
    {
      calcul_indic_pena_global(indic_pena_global, indic_face_pena_global);
      maj = mise_a_jour_tag_indicatrice_pena_glob();
      assert(maj == 1);
    }
}

void Convection_Diffusion_Temperature::transport_ibc(DoubleTrav& secmem_conv_vr, DoubleTab& inco_conv_vr)
{
  secmem_conv_vr = 0.;
  const double rhoCp = get_time_factor();

  //fonction characteristique globale ibc au temps courant
  set_indic_pena_globale();

  // on calcule l indicatrice epaisse
  const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis().valeur());
  const IntTab& faces_elem = domaine_vf.face_voisins();
  IntTrav indic_pena_global_fat(indic_pena_global);
  for (int i_face = 0; i_face < indic_face_pena_global.size(); i_face++)
    {
      if (indic_face_pena_global(i_face) > 0.)
        {
          const int voisin_0 = faces_elem(i_face, 0);
          if (voisin_0 >= 0) indic_pena_global_fat(voisin_0) = 1;
          const int voisin_1 = faces_elem(i_face, 1);
          if (voisin_1 >= 0) indic_pena_global_fat(voisin_1) = 1;
        }
    }
  // post-traitement particulier pour les coins
  const IntTab& elem_faces = domaine_vf.elem_faces();
  const int nb_faces_elem = elem_faces.dimension(1);
  const int nb_elem  = indic_pena_global.dimension(0);
  for (int i_elem = 0; i_elem < nb_elem;  i_elem++)
    {
      if (indic_pena_global_fat(i_elem) == 0)
        {
          int coeff = 0;
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0)
                {
                  if (indic_pena_global_fat(voisin) != 0)
                    {
                      ++coeff;
                    } ;
                }
            }
          if (coeff > 1) indic_pena_global_fat(i_elem) = 1;
        }
    }

  // Si on veut un traitement local a chaque indicatrice (necessaire si on utilise vitesse imposee)
  // IntTrav indic_pena_fat(indic_pena_global);
  // for (int w = 0; w < ref_penalisation_L2_FTD.size(); w++)
  //   {
  //     Transport_Interfaces_base & nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
  //     // on calcul le filtre local a l ibc
  //     const DoubleTab & indicatrice_face = nom_eq.get_compute_indicatrice_faces().valeurs();
  //     indic_pena_fat = 0;
  //     for (int i_face = 0; i_face < indicatrice_face.size(); i_face++)
  //         {
  //           if (indicatrice_face(i_face) > 0.)
  //             {
  //               const int voisin_0 = faces_elem(i_face, 0);
  //               if (voisin_0 >= 0) indic_pena_fat(voisin_0) = 1;
  //               const int voisin_1 = faces_elem(i_face, 1);
  //               if (voisin_1 >= 0) indic_pena_fat(voisin_1) = 1;
  //             }
  //         }
  //     indic_pena_global_fat += indic_pena_fat;
  //   }

  // on convecte le champ de variable avec la vitesse fluide pour les faces ibc
  if ( nombre_d_operateurs() > 1 )
    {
      derivee_en_temps_conv(secmem_conv_vr, inco_conv_vr);
      secmem_conv_vr *= rhoCp;
    }

  //on ne garde la contribution que pour les cellules dont indicatrice fat est <> 0
  for (int i_elem = 0; i_elem < nb_elem;  i_elem++)
    {
      if (indic_pena_global_fat(i_elem) == 0) secmem_conv_vr(i_elem) = 0.;
    }
}

void Convection_Diffusion_Temperature::mise_en_place_domaine_fantome(DoubleTab& solution)
{
  int maj = verifier_tag_indicatrice_pena_glob();
  if (maj == 1) return;

  // garde-fou
  if (schema_temps().facteur_securite_pas() > 1.0)
    {
      Cerr << "Convection_Diffusion_Temperature::mise_en_place_domaine_fantome Facteur securite doit etre <= 1"<<finl;
      exit();
    }

  // fonction characteristique globale ibc penalisees actuelle
  IntTrav indicatrice_totale;
  IntTrav indicatrice_face_totale ;
  calcul_indic_pena_global(indicatrice_totale,indicatrice_face_totale);
  // etat des lieux actuel
  const int nb_elem  = indicatrice_totale.dimension(0);
  int x=0;
  int j=0;
  for (int k_elem =0 ; k_elem <nb_elem; ++k_elem)  ((indicatrice_totale(k_elem)!=0) ? ++x : ++j);

  //traitement cellules fantomes (if any)
  const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis().valeur());
  const IntTab& elem_faces = domaine_vf.elem_faces();
  const IntTab& faces_elem = domaine_vf.face_voisins();
  const int nb_faces_elem = elem_faces.dimension(1);
  int k=0; //nb cellules fantomes ibc -> fluide
  int k_cor=0; //nb cellules fantomes (ibc -> fluide) corrigees
  int k_cor2=0; //nb cellules fantomes (fluide -> ibc) corrigees
  int u=0; //nb cellules fluide -> ibc
  for (int i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      //cellules fantomes (actuellement fluide et precedement ibc) => on modifie solution
      double somme_inc = 0.;
      double coeff = 0.;
      if (indic_pena_global(i_elem) != 0 && indicatrice_totale(i_elem) == 0)
        {
          ++k;
          // boucle sur les faces pour determiner la valeur de la variable pour les cellules voisines strictement fluide
          IntTrav stok_vois(nb_faces_elem);
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0) // le voisin existe
                {
                  if ((indic_pena_global(voisin) == indicatrice_totale(voisin)) &&  (indic_pena_global(voisin) == indicatrice_totale(i_elem))) // strictement fluide aux deux pas de temps
                    {
                      somme_inc += solution(voisin);
                      ++coeff;
                    }
                  else     // On sauve le numero du voisin
                    {
                      stok_vois(i_face) = voisin;
                    }
                }
            }
          // Si on n a pas trouve de cellules fluide, on regarde les voisins des voisins
          if (coeff == 0.)
            {
              for (int i_vois = 0; i_vois < nb_faces_elem; i_vois++)
                {
                  for (int ii_face = 0; ii_face < nb_faces_elem; ii_face++)
                    {
                      const int face_v = elem_faces(stok_vois(i_vois), ii_face);
                      const int voisin_v = faces_elem(face_v, 0) + faces_elem(face_v, 1) - stok_vois(i_vois);
                      if ((voisin_v >= 0) && (voisin_v != i_elem) )// le voisin existe et pas i_elem
                        {
                          if ((indic_pena_global(voisin_v) == indicatrice_totale(voisin_v)) &&  (indic_pena_global(voisin_v) == indicatrice_totale(i_elem))) // strictement fluide aux deux pas de temps
                            {
                              somme_inc += solution(voisin_v);
                              ++coeff;
                            }
                        }
                    }
                }
            }
          // definition d une nouvelle valleur de la variable pour la cellule fantome
          if (coeff > 0.)
            {
              solution(i_elem) = somme_inc / coeff;
              k_cor += 1;
            }
        }
      //cellules actuellement ibc et precedement fluide
      if (indic_pena_global(i_elem) == 0 && indicatrice_totale(i_elem) != 0)
        {
          ++u;
          // boucle sur les faces pour determiner la valeur de la variable pour les cellules voisines strictement ibc
          IntTrav stok_vois(nb_faces_elem);
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0) // le voisin existe
                {
                  // strictement ibc aux deux pas de temps (la meme) car solution a ete modifiee ci-dessus
                  if ((indic_pena_global(voisin) == indicatrice_totale(voisin)) && (indic_pena_global(voisin) == indicatrice_totale(i_elem)))
                    {
                      somme_inc += solution(voisin);
                      ++coeff;
                    }
                  else     // On sauve le numero du voisin
                    {
                      stok_vois(i_face) = voisin;
                    }
                }
            }
          // Si on n a pas trouve de cellules ibc, on regarde les voisins des voisins
          if (coeff == 0.)
            {
              for (int i_vois = 0; i_vois < nb_faces_elem; i_vois++)
                {
                  for (int ii_face = 0; ii_face < nb_faces_elem; ii_face++)
                    {
                      const int face_v = elem_faces(stok_vois(i_vois), ii_face);
                      const int voisin_v = faces_elem(face_v, 0) + faces_elem(face_v, 1) - stok_vois(i_vois);
                      if ((voisin_v >= 0) && (voisin_v != i_elem) )// le voisin existe et pas i_elem
                        {
                          if ((indic_pena_global(voisin_v) == indicatrice_totale(voisin_v)) && (indic_pena_global(voisin_v) == indicatrice_totale(i_elem))) // strictement ibc aux deux pas de temps (la meme) car solution a ete modifiee ci-dessus
                            {
                              somme_inc += solution(voisin_v);
                              ++coeff;
                            }
                        }
                    }
                }
            }
          // definition d une nouvelle valleur de la variable pour la cellule fantome
          if (coeff > 0.)
            {
              solution(i_elem) = somme_inc / coeff;
              k_cor2 += 1;
            }
          else
            {
              solution(i_elem) = tab_penalisation_L2_FTD[indicatrice_totale(i_elem)-1](0);
              k_cor2 += 1;
            }
        }
    }
  solution.echange_espace_virtuel();
  //     Debog::verifier("Convection_Diffusion_Temperature::mise_en_place_domaine_fantome solution ",solution);
  Cout <<"Mise_en_place_domaine_fantome : fluide -> ibc : "<<u<< " elem. (dont "<<k_cor2<<" corrigees) et ibc -> fluide : ";
  Cout<<k<<" elem.(dont "<<k_cor<<" corrigees)"<<finl;
  Cout <<"             nb elem. ibc : "<<x<<" et nb elem fluide : "<<j<<" sur un total de : "<<nb_elem<<finl;
  if ((u != k_cor2) || (k !=k_cor))
    {
      Cerr <<"Mise_en_place_domaine_fantome : Les cellules fantomes ne sont pas toutes corrigees"<<finl;
      exit();
    }

  indic_pena_global = indicatrice_totale;
  indic_face_pena_global = indicatrice_face_totale;
  maj = mise_a_jour_tag_indicatrice_pena_glob();
  assert(maj == 1);
}

DoubleTab& Convection_Diffusion_Temperature::filtrage_si_appart_ibc(DoubleTab& u_conv, DoubleTab& u)
{
  // caclul de la fonction caracteristique globale des ibc pour le calcul de T_voisinage
  set_indic_pena_globale();

  for (int j = 0 ; j< indic_pena_global.size() ; ++j)
    {
      if ( indic_pena_global(j) != 0) u(j) = u_conv(j);
    }
  u.echange_espace_virtuel();
  //   Debog::verifier("Convection_Diffusion_Temperature::filtrage_si_appart_ibc u ",u);
  return u;
}

void Convection_Diffusion_Temperature::calcul_indic_pena_global(IntTab& indicatrice_totale, IntTab& indicatrice_face_totale)
{
  // fonction characteristique globale ibc penalisees
  indicatrice_totale.copy(indic_pena_global);
  indicatrice_face_totale.copy(indic_face_pena_global);
  indicatrice_totale = 0;
  indicatrice_face_totale = 0;
  int x=0;
  int xf=0;
  const int nb_elem  = indicatrice_totale.dimension_tot(0);
  const int nfaces = indicatrice_face_totale.dimension_tot(0);
  // boucle sur les ibc
  for ( int w = 0 ; w<ref_penalisation_L2_FTD.size() ; ++w)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[w].valeur());
      const DoubleTab& indicatrice = nom_eq.get_update_indicatrice().valeurs();
      // fonction characteristique (0 ou 1) pour l'ensemble des ibc
      for (int k_elem =0 ; k_elem <nb_elem; ++k_elem)
        {
          if (indicatrice(k_elem)>0.)
            {
              if(indicatrice_totale(k_elem) != 0)
                {
                  Cerr<<"calcul_indic_pena_global : Attention: les elements des indicatrices se chevauchent "<<finl;
                  exit();
                }
              indicatrice_totale(k_elem) = w+1;
              ++x;
            }
        }
      // fonction characteristique (numero ibc) pour l'ensemble des ibc
      const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis().valeur());
      const IntTab& face_voisins = domaine_vf.face_voisins();
      for (int i = 0; i < nfaces ; i++)
        {
          const int elem0 = face_voisins(i, 0);
          const int elem1 = face_voisins(i, 1);
          double indi = 0.;
          if (elem0 >= 0) indi = indicatrice(elem0);
          if (elem1 >= 0) indi += indicatrice(elem1);
          if(indi > 0.)
            {
              if(indicatrice_face_totale(i) !=  0)
                {
                  Cerr<<"calcul_indic_pena_global : Attention: les faces des indicatrices se chevauchent "<<finl;
                  exit();
                }
              indicatrice_face_totale(i) = w+1;
              ++xf;
            }
        }
    }
  Cout<<"Calcul_indic_pena_global : Nb d elements de l indicatrice globale des IBCs: "<<x;
  Cout<<"  et nb de faces : "<<xf<<finl;
  indicatrice_totale.echange_espace_virtuel();
  indicatrice_face_totale.echange_espace_virtuel();
  //      Debog::verifier("Convection_Diffusion_Temperature::calcul_indic_pena_global indicatrice_totale ",indicatrice_totale);
  //      Debog::verifier("Convection_Diffusion_Temperature::calcul_indic_pena_global indicatrice_face_totale ",indicatrice_face_totale);
}

DoubleTab& Convection_Diffusion_Temperature::penalisation_L2(DoubleTab& u)
{
  // garde-fou
  assert(ref_penalisation_L2_FTD.size() != 0);
  assert(ref_penalisation_L2_FTD.size() == tab_penalisation_L2_FTD.size());

  const double dt = schema_temps().pas_de_temps();
  DoubleTab u_old(u);

  // caclul de la fonction caracteristique globale des ibc pour le calcul de T_voisinage
  set_indic_pena_globale();

  //calcul de T_voisinage pour tous les elemnts
  const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis().valeur());
  const IntTab& elem_faces = domaine_vf.elem_faces();
  const IntTab& faces_elem = domaine_vf.face_voisins();
  const int nb_faces_elem = elem_faces.dimension(1);
  const DoubleTab& inc=inconnue().valeurs();
  // inconnue doit etre scalaire
  assert(inc.line_size() == 1);
  DoubleTrav t_voisinage(inc);
  DoubleTrav u_voisinage(u);
  const int nb_elem  = u.dimension_tot(0);
  for (int i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      double somme = 0.;
      double somme_deriv = 0.;
      double coeff = 0.;

      if( indic_pena_global(i_elem) != 0)
        {
          // boucle sur les faces de l element
          for (int i_face = 0; i_face < nb_faces_elem; i_face++)
            {
              const int face = elem_faces(i_elem, i_face);
              const int voisin = faces_elem(face, 0) + faces_elem(face, 1) - i_elem;
              if (voisin >= 0)
                {
                  if (indic_pena_global(voisin) == 0)
                    {
                      somme += inc(voisin);
                      somme_deriv += u(voisin);
                      coeff += 1.;
                    }
                }
            }
        }

      if (coeff > 0.)
        {
          t_voisinage(i_elem) = somme / coeff;
          u_voisinage(i_elem) = somme_deriv / coeff;
        }
      else
        {
          // cas pas de voisin dans le fluide
          t_voisinage(i_elem) = inc(i_elem);
          u_voisinage(i_elem) = u(i_elem);
        }
    }
  t_voisinage.echange_espace_virtuel();
  u_voisinage.echange_espace_virtuel();

  //determination du champ global de penalisation L2
  DoubleTrav pena_glob(inc); //initialise a zero
  DoubleTrav tab_(inc); //initialise a zero
  DoubleTrav denom(inc); //initialise a zero

  //boucle ibc
  for ( int i = 0 ; i < ref_penalisation_L2_FTD.size() ; ++i)
    {
      Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[i].valeur());
      const DoubleTab& tab = tab_penalisation_L2_FTD[i];
      //verification dimensions tab t obstacle = nb dim de l inconnu
      if ( tab.dimension(0) != inc.line_size() )
        {
          Cerr << " penalisation_L2: Les champs de temperature impose et calcule n'ont pas les memes dimensions" <<finl;
          Cerr << " Dimension du champ de temperature impose "<< tab.dimension(0) << finl;
          Cerr << " Dimension du champ de temperature calcule par TRUST "<<inc.line_size() <<finl;
          Cerr << " Nombre d'elements stockes au dans l'iconnue " <<inc.size() <<finl;
          Process::exit();
        }
      const DoubleTab& indicatrice = nom_eq.get_update_indicatrice().valeurs();

      DoubleTrav pena_loc(indicatrice);
      // Determination du terme de penalisation pour l ibc courante
      // boucle sur les elements
      for (int j = 0; j<nb_elem ; ++j)
        {
          //calcul de la fonction characteristique Ksi de l ibc courante -> pena_loc
          if (indicatrice(j) > 0.)
            {
              double coeff_p=1.;
              pena_loc(j) = coeff_p;
              //calcul du denominateur du terme de penalisation ( Sigma_ibc Ksi_ibc(j) )
              denom(j) += coeff_p;

              //determination valeur a imposer T_ref
              double tref_j = 0.;
              switch (choix_pena)
                {
                case 0 :
                  // approximation de T_ref par T_ref = T_imp
                  tref_j = tab(0);
                  break;
                case 1 :
                  // approximation de T_ref par T_ref = T+dt*derivee
                  tref_j = inc(j) + dt*u_old(j);
                  break;
                case 2 :
                  // approximation de T_imp par T_ref= tau*T_imp + (1-tau)*T_voisin
                  // approximation de T_voisin par T_voisin= 1/n * somme(1,n)(T_environnant+dt*derivee_environnant)
                  tref_j = (indicatrice(j)*tab(0)+(1.-indicatrice(j))*(t_voisinage(j)+dt*u_voisinage(j)));
                  break;
                case 3 :
                  // approximation de T_imp par T_ref= [(1-tau)*T_voisin + (1/2)*T_imp] / (3/2 -tau)
                  // approximation de T_voisin par T_voisin= 1/n * somme(1,n)(T_environnant+dt*derivee_environnant)
                  tref_j = ((1.-indicatrice(j))*(t_voisinage(j)+dt*u_voisinage(j))+0.5*tab(0))/(1.5-indicatrice(j));
                  break;
                case 4 :
                  // approximation de T_imp par T_ref= T_imp si tau=1 sinon T_ref=T_voisin
                  if (indicatrice(j) == 1.)
                    {
                      tref_j = tab(0);
                    }
                  else
                    {
                      tref_j = (t_voisinage(j)+dt*u_voisinage(j));
                    }
                  break;
                default:
                  Cerr << "Penalisation_L2: choix_pena invalide "<< choix_pena <<finl;
                  exit();
                  break;
                }
              // sauvegarde de Sigma_ibc Ksi_ibc(j) Tref_ibc pour post-traitement
              tab_(j) += tref_j;
              // penalisation_global = Sigma_ibc Ksi_ibc(j)*(Tref_ibc - inc) /  eta;
              pena_loc(j) *= (tref_j-inc(j));
              pena_loc(j) /= eta;
              pena_glob(j)  += pena_loc(j);
            }
        }
    }

  //  Realisation de la penalisation L2 de la derivee en temps: (dT_t_* + pena_glob) / ( 1 + (dt*Sigma_ibc Ksi_ibc(j)) / eta )
  // Tref est la moyenne arithmetique de Tref_j lorsque 0<eta<<1
  for (int j = 0 ; j<nb_elem ; ++j)
    {
      u(j) = u_old(j) + pena_glob(j);
      u(j) /= (1. + denom(j)*dt/eta);
    }
  u.echange_espace_virtuel();

  //  Sauvegarde des flux
  ecrire_fichier_pena_th(u_old,u,tab_,denom);

  //   Debog::verifier("Convection_Diffusion_Temperature::penalisation_L2 u ",u);
  return u;
}

void ouvrir_fichier_pena_th(SFichier& os, const Nom& type, const int flag, const Transport_Interfaces_base& equation)
{
  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;
  Nom fichier=Objet_U::nom_du_cas();
  fichier+="_";
  fichier+=type;
  fichier+="_";
  fichier+=equation.le_nom();
  fichier+=".out";
  const Schema_Temps_base& sch=equation.probleme().schema_temps();
  const int precision=sch.precision_impr();
  // On cree le fichier a la premiere impression avec l'en tete
  if (sch.nb_impr()==1 && !equation.probleme().reprise_effectuee())
    {
      os.ouvrir(fichier);
      SFichier& fic=os;
      Nom espace="\t\t";
      fic << (Nom)"# Impression puissance thermique";
      fic << " interface " << equation.le_nom();
      fic << " en W" << "." << finl;
      fic << finl << "# Temps";

      Nom ch=espace;
      ch+="P";
      fic << ch << finl;
    }
  // Sinon on l'ouvre
  else
    {
      os.ouvrir(fichier,ios::app);
    }
  os.precision(precision);
  os.setf(ios::scientific);
}

void Convection_Diffusion_Temperature::ecrire_fichier_pena_th(DoubleTab& u_old, DoubleTab& u, DoubleTab& tref, DoubleTab& denom)
{

  if (le_schema_en_temps->limpr())
    {

      const Domaine_VF& domaine_vf = ref_cast(Domaine_VF, domaine_dis().valeur());
      const DoubleVect& vol_maille = domaine_vf.volumes();
      const Fluide_base& fluide_inc = ref_cast(Fluide_base, milieu());
      const DoubleTab& tab_rho = fluide_inc.masse_volumique().valeurs();
      const double rho = tab_rho(0,0);
      const DoubleTab& tab_cp = fluide_inc.capacite_calorifique().valeur().valeurs();
      const double cp = tab_cp(0,0);

      //  Methode pour calculer le flux total sur les ibc

      //Calcul de T_n+1 apres penalisation
      const DoubleTab& inc=inconnue().valeurs();
      const int nb_elem  = u.dimension(0);
      const double dt = schema_temps().pas_de_temps();
      DoubleTab tkp1(inc);
      for (int k = 0 ; k<nb_elem ; ++k) tkp1(k) += u(k)*dt;

      //Terme de penalisation:       Sigma_ibc Ksi_ibc * ( T_n+1 - Tref_ibc) / eta
      DoubleTrav F(inc);
      for (int j = 0; j<nb_elem; ++j)  F(j) = (tref(j)-denom(j)*tkp1(j))/eta;

      // boucle sur les IBC

      DoubleTrav filtre(nb_elem);
      DoubleTrav filtre_glob(nb_elem);
      const double temps_flux = le_schema_en_temps->temps_courant();
      Nom espace=" \t";

      for ( int i = 0 ; i < ref_penalisation_L2_FTD.size() ; ++i) //boucle sur le nombre d'ibc
        {
          Transport_Interfaces_base& nom_eq = ref_cast(Transport_Interfaces_base,ref_penalisation_L2_FTD[i].valeur());
          const DoubleTab& indicatrice = nom_eq.get_update_indicatrice().valeurs();
          double Flux_pena= 0.;
          double Flux_pena_old= 0.;
          double Flux_temp_interne= 0.;
          //Filtre ibc (1) / non ibc (0)
          filtre = 0.;
          for ( int j = 0 ; j<nb_elem ; ++j)
            {
              if (indicatrice(j) > 0.)
                {
                  filtre(j) = 1.;
                  filtre_glob(j)=1.;
                }
              //bilans
              //Attention rho et cp constants
              Flux_pena_old += u_old(j) * filtre(j) *rho *cp *vol_maille(j);
              Flux_pena += F(j) * filtre(j) * vol_maille(j) *rho *cp;
              Flux_temp_interne += filtre(j)*rho*cp*vol_maille(j)*u(j);
            }

          // Ajout des differents processeurs en //
          Flux_pena_old = mp_sum(Flux_pena_old);
          Flux_pena = mp_sum(Flux_pena);
          Flux_temp_interne = mp_sum(Flux_temp_interne);

          //ecriture
          if (Process::je_suis_maitre())
            {
              SFichier FTE;
              ouvrir_fichier_pena_th(FTE,"Puissance_penalisation_thermique_ibc",1,nom_eq);
              FTE << temps_flux;
              FTE << espace << Flux_pena;
              FTE << finl;

              SFichier FTI;
              ouvrir_fichier_pena_th(FTI,"Puissance_thermique_ibc",1,nom_eq);
              FTI << temps_flux;
              FTI << espace << Flux_pena_old;
              FTI << finl;

              SFichier FTTI;
              ouvrir_fichier_pena_th(FTTI,"Derivee_temps_temperature_ibc",1,nom_eq);
              FTTI << temps_flux;
              FTTI << espace << Flux_temp_interne;
              FTTI << finl;
            }
        }

      //bilan Derivee_temps_temperature_fluide
      //Attention rho et cp constants
      double Flux_temp_externe= 0.;
      for (int j = 0; j<nb_elem; ++j)
        {
          Flux_temp_externe += (1.-filtre_glob(j))  *rho *cp *vol_maille(j) * u(j); //fluide
        }

      // Ajout des differents processeurs en //
      Flux_temp_externe = mp_sum(Flux_temp_externe); //fluide

      //ecriture
      if (Process::je_suis_maitre())
        {
          SFichier fichier;
          Nom fichier_nom=Objet_U::nom_du_cas();
          fichier_nom+="_";
          fichier_nom+="Derivee_temps_temperature_fluide.out";
          if (le_schema_en_temps->nb_impr()==1 && !probleme().reprise_effectuee())
            {
              fichier.ouvrir(fichier_nom);
            }
          else
            {
              fichier.ouvrir(fichier_nom,ios::app);
            }
          //if (fichier)
          {
            (fichier) <<temps_flux<<" \t"<<Flux_temp_externe<<finl;

          }
        }
    }
}

void Convection_Diffusion_Temperature::calculer_rho_cp_T(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Equation_base& eqn = ref_cast(Equation_base, obj);
  const Fluide_base& fl = ref_cast(Fluide_base, eqn.milieu());
  const Champ_Inc_base& ch_T = eqn.inconnue().valeur();
  const Champ_base& ch_rho = fl.masse_volumique().valeur();
  assert(sub_type(Champ_Uniforme, ch_rho));
  const Champ_Don& ch_cp = fl.capacite_calorifique();
  const DoubleTab& cp = fl.capacite_calorifique().valeurs(), &rho = ch_rho.valeurs(), &T = ch_T.valeurs();

  /* valeurs du champ */
  const int N = val.line_size(), Nl = val.dimension_tot(0), cCp = sub_type(Champ_Uniforme, ch_cp.valeur());
  for (int i = 0; i < Nl; i++)
    for (int n = 0; n < N; n++) val(i, n) = rho(0, n) * cp(!cCp * i, n) * T(i, n);

  /* on ne peut utiliser valeur_aux_bords que si ch_rho a un domaine_dis_base */
  DoubleTab b_cp = cCp ? cp : ch_cp->valeur_aux_bords(), b_T = ch_T.valeur_aux_bords();
  int Nb = b_T.dimension_tot(0);
  // on suppose que rho est un champ_uniforme : on utilise directement le tableau du champ
  for (int i = 0; i < Nb; i++)
    for (int n = 0; n < N; n++) bval(i, n) = b_cp(!cCp * i, n) * rho(0, n) * b_T(i, n);

  DoubleTab& d_T = deriv["temperature"];
  d_T.resize(Nl, N);
  for (int i = 0; i < Nl; i++)
    for (int n = 0; n < N; n++) d_T(i, n) = rho(0, n) * cp(!cCp * i, n);

}
