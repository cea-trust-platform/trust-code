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

#include <Operateur_Statistique_tps_base.h>
#include <Ecrire_fichier_xyz_valeur.h>
#include <Schema_Temps_base.h>
#include <Postraitement.h>
#include <Equation_base.h>
#include <EcrFicPartage.h>
#include <Champ_base.h>

Implemente_instanciable(Ecrire_fichier_xyz_valeur,"Ecrire_fichier_xyz_valeur",Objet_U);

Sortie& Ecrire_fichier_xyz_valeur::printOn(Sortie& os) const
{
  return os;
}

Entree& Ecrire_fichier_xyz_valeur::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Ecrire_fichier_xyz_valeur::set_param(Param& param)
{
  param.ajouter_flag("binary_file",&binary_file_);
  param.ajouter("dt", &dt_); // XD_ADD_P floattant 1 File writing frequency
  param.ajouter_non_std("fields", (this)); // XD_ADD_P bloc_lecture Names of the fields we want to write
  param.ajouter_non_std("boundaries", (this)); // XD_ADD_P bloc_lecture Names of the boundaries on which to write fields
}

int Ecrire_fichier_xyz_valeur::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "fields")
    fill_vector_from_datafile_(is,fields_names_);
  else if(mot == "boundaries")
    fill_vector_from_datafile_(is,boundary_names_);
  else
    {
      Cerr << "Ecrire_fichier_xyz_valeur: "<< mot << " is not a recognized keyword" << finl;
      Process::exit();
    }
  return 1;
}

Entree& Ecrire_fichier_xyz_valeur::fill_vector_from_datafile_(Entree& is, std::vector<std::string>& v)
{
  Nom keyword;
  Nom opening_brace("{");
  Nom closing_brace("}");
  is >> keyword;
  if (keyword != opening_brace)
    {
      Cerr << "Error while reading the Ecrire_fichier_xyz_valeur\n";
      Cerr << "We expected a " << opening_brace << " instead of \n"
           << keyword;
      exit();
    }

  while(1)
    {
      is >> keyword;
      if (keyword == closing_brace)
        break;
      v.push_back(keyword.getString());
    }

  return is;
}

bool Ecrire_fichier_xyz_valeur::write_field_during_current_timestep_() const
{
  const double timestep = eqn_->schema_temps().temps_courant();
  const double scheme_dt = eqn_->schema_temps().pas_de_temps();
  const double tmax = eqn_->schema_temps().temps_max();
  const int nb_pas_dt_max = eqn_->schema_temps().nb_pas_dt_max();
  const int nb_pas_dt = eqn_->schema_temps().nb_pas_dt();
  const int stationnaire_atteint = eqn_->schema_temps().stationnaire_atteint();
  int ok;
  if (dt_<=scheme_dt || tmax<=timestep || nb_pas_dt_max<=nb_pas_dt || nb_pas_dt<=1 || stationnaire_atteint || eqn_->schema_temps().stop_lu())
    ok=1;
  else
    {
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      double i, j, epsilon = 1.e-8;
      modf(timestep/dt_ + epsilon, &i);
      modf((timestep-scheme_dt)/dt_ + epsilon, &j);
      ok = (i>j);
    }
  return (ok && dt_>0);
}

bool Ecrire_fichier_xyz_valeur::getStatField_(const std::string& fname, REF(Champ_base)& field, REF(Operateur_Statistique_tps_base)& op_stat) const
{
  bool champ_stat = false;

  // On recherche le champ dans le probleme contenant l'equation, et les postraitements
  // dans les postraitements ?
  for (auto &itr :  eqn_->probleme().postraitements())
    if (sub_type(Postraitement, itr.valeur()))
      {
        const Postraitement& post = ref_cast(Postraitement, itr.valeur());

        Motcle nom_test = fname;
        //La recherche est faite sur les champs statistiques a partir d un identifiant
        //Si le nom indique dans le jeux de donnes est celui d un champ statistiques
        //il doit correspondre au nom du champ de postraitement
        post.champ_fonc(nom_test, field, op_stat);
        if (field.non_nul())
          {
            champ_stat = true;
            break;
          }
      }

  return champ_stat;
}

void Ecrire_fichier_xyz_valeur::writeValuesOnBoundary_(const std::string& fname, const std::string& bname, const DoubleTab& pos, const DoubleTab& val) const
{
  assert(val.dimension(0) == pos.dimension(0));

  const double timestep = eqn_->schema_temps().temps_courant();
  // Construction du nom du fichier
  Nom nom_fic(eqn_->probleme().le_nom());
  nom_fic+="_";
  nom_fic+=fname;
  nom_fic+="_";
  nom_fic+=bname;
  nom_fic+="_";
  nom_fic+=Nom(timestep);
  nom_fic+=".dat";
  EcrFicPartage fic;
  fic.set_bin(binary_file_);
  fic.ouvrir(nom_fic);
  fic.setf(ios::scientific);
  fic.precision(format_precision_geom);

  // Ecriture du fichier
  const int nb_val = val.dimension(0);
  int nb_val_tot = Process::mp_sum(nb_val);
  if (Process::je_suis_maitre())
    {
      if(binary_file_)
        fic << "binary" << finl;
      fic  << nb_val_tot << finl;
    }
  barrier();
  fic.lockfile();

  const int nb_compo = val.dimension(1);
  for (int i2=0; i2<nb_val; i2++)
    {
      // Ecriture des coordonees
      for (int j2=0; j2<dimension; j2++)
        fic << pos(i2,j2) << " ";
      // Ecriture des valeurs
      for (int nb=0; nb<nb_compo; nb++)
        fic << val(i2,nb) << " " ;
      fic << finl;
    }
  fic.unlockfile();
  barrier();
  fic.syncfile();
  fic.close();
}

void Ecrire_fichier_xyz_valeur::write_fields() const
{
  if(eqn_.est_nul())
    return;

  if (!write_field_during_current_timestep_())
    return;

  int number_of_fields = (int)fields_names_.size();
  for (int fi=0; fi<number_of_fields; fi++)
    {
      const std::string& fname = fields_names_[fi];
      REF(Champ_base) field;
      REF(Operateur_Statistique_tps_base) op_stat;
      bool champ_stat = getStatField_(fname, field, op_stat);
      if(!champ_stat)
        {
          //L identifiant correspond ici a un Champ_base
          field = eqn_->probleme().get_champ(fname);
        }

      const int nb_bords_post = (int)boundary_names_.size();
      if (nb_bords_post>0) // on ne souhaite postraiter que sur certains bords
        {
          int nb_cl = eqn_->domaine_Cl_dis().nb_cond_lim();
          for (int j=0; j<nb_bords_post; j++)
            {
              int boundary_found = 0 ; // int servant a tester si le nom du bord correspond bien au nom d'une frontiere
              const Nom& bname = boundary_names_[j];
              for (int i=0; i<nb_cl && !boundary_found; i++)
                {
                  const Cond_lim_base& la_cl = eqn_->domaine_Cl_dis().les_conditions_limites(i);
                  const Frontiere& la_frontiere = la_cl.frontiere_dis().frontiere();
                  if (la_frontiere.le_nom() == bname)
                    {
                      boundary_found = 1;

                      // Construction du tableau pos contenant les centres des faces frontiere
                      DoubleTab pos;
                      la_frontiere.faces().calculer_centres_gravite(pos);

                      // Construction du tableau val contenant les valeurs aux centres des faces
                      const int nb_val = la_frontiere.nb_faces();
                      int nb_compo = field->nb_comp();
                      DoubleTab val(nb_val,nb_compo);
                      val = 0.;
                      if(champ_stat)
                        {
                          DoubleTab copie(field->valeurs());
                          field->valeurs() = op_stat->calculer_valeurs();
                          field->valeur_aux(pos, val);
                          field->valeurs() = copie;
                        }
                      else
                        field->valeur_aux(pos, val);

                      writeValuesOnBoundary_(fname, bname.getString(), pos, val);
                    }
                }
              if (!boundary_found)
                {
                  Cerr << "You try to use the method Ecrire_fichier_xyz_valeur with an unknown boundary name" << finl;
                  Cerr << "The boundary named " << bname << " is not recognized"<< finl;
                  exit();
                }
            }
        }
      else // on souhaite postraiter sur tout le domaine
        {
          Cerr << "The option of post processing the entire domain with Ecrire_fichier_xyz_valeur is now obsolete." <<finl;
          exit();
        }
    }
}
