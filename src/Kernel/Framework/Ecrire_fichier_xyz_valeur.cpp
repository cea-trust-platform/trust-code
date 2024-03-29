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
// XD ecrire_fichier_xyz_valeur objet_u ecrire_fichier_xyz_valeur -1 This keyword is used to write the values of a field only for some boundaries in a text file with the following format: n_valeur NL2 x_1 y_1 [z_1] val_1 NL2 ... NL2 x_n y_n [z_n] val_n NL2 The created files are named : pbname_fieldname_[boundaryname]_time.dat

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
  param.ajouter_flag("binary_file",&binary_file_); // XD_ADD_P flag To write file in binary format
  param.ajouter("dt", &dt_); // XD_ADD_P floattant File writing frequency
  param.ajouter("fields", &fields_names_); // XD_ADD_P listchaine Names of the fields we want to write
  param.ajouter("boundaries", &boundary_names_); // XD_ADD_P listchaine Names of the boundaries on which to write fields
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
      // See Schema_Temps_base::limpr for info on epsilon and modf
      double i, j, epsilon = 1.e-8;
      modf(timestep/dt_ + epsilon, &i);
      modf((timestep-scheme_dt)/dt_ + epsilon, &j);
      ok = (i>j);
    }
  return (ok && dt_>0);
}

bool Ecrire_fichier_xyz_valeur::getStatField_(const Nom& fname, REF(Champ_base)& field, REF(Operateur_Statistique_tps_base)& op_stat) const
{
  bool champ_stat = false;

  // Searching field among post-processing
  for (auto &itr :  eqn_->probleme().postraitements())
    if (sub_type(Postraitement, itr.valeur()))
      {
        const Postraitement& post = ref_cast(Postraitement, itr.valeur());

        Motcle nom_test = fname;
        // Statistical fields are searched using an identifier.
        // If the name indicated in the dataset is that of a statistical field,
        // it must correspond to the name of the post-processing field.
        post.champ_fonc(nom_test, field, op_stat);
        if (field.non_nul())
          {
            champ_stat = true;
            break;
          }
      }

  return champ_stat;
}

void Ecrire_fichier_xyz_valeur::writeValuesOnBoundary_(const Nom& fname, const std::string& bname, const DoubleTab& pos, const DoubleTab& val) const
{
  assert(val.dimension(0) == pos.dimension(0));

  const double timestep = eqn_->schema_temps().temps_courant();
  // Building filename
  Nom nom_fic(eqn_->probleme().le_nom());
  nom_fic+="_";
  nom_fic+=fname;
  nom_fic+="_";
  nom_fic+=bname;
  nom_fic+="_";
  nom_fic+=Nom(timestep);
  nom_fic+=".dat";

  // Opening file
  EcrFicPartage fic;
  fic.set_bin(binary_file_);
  fic.ouvrir(nom_fic);
  fic.setf(ios::scientific);
  fic.precision(format_precision_geom);

  // Writing file
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
      // Writing coords
      for (int j2=0; j2<dimension; j2++)
        fic << pos(i2,j2) << " ";
      // Writing values
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

  for (auto fname : fields_names_ )
    {
      REF(Champ_base) field;
      REF(Operateur_Statistique_tps_base) op_stat;
      bool champ_stat = getStatField_(fname, field, op_stat);
      if(!champ_stat)
        {
          // If the field is not a statistical field,
          // we retrieve a Champ_base
          field = eqn_->probleme().get_champ(fname);
        }

      const int nb_bords_post = (int)boundary_names_.size();
      if (nb_bords_post>0) // we'd like to post-process on some boundaries only
        {
          int nb_cl = eqn_->domaine_Cl_dis().nb_cond_lim();
          for (auto bname : boundary_names_ )
            {
              int boundary_found = 0 ; // to assess that the name of the boundary does correspond to a border
              for (int i=0; i<nb_cl && !boundary_found; i++)
                {
                  const Cond_lim_base& la_cl = eqn_->domaine_Cl_dis().les_conditions_limites(i);
                  const Frontiere& la_frontiere = la_cl.frontiere_dis().frontiere();
                  if (la_frontiere.le_nom() == bname)
                    {
                      boundary_found = 1;

                      // Array containing the centers of the boundary faces
                      DoubleTab pos;
                      la_frontiere.faces().calculer_centres_gravite(pos);

                      // Array containing the field values at the face centers
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

                      // writing everything into the file
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
      else // we'd like to post-process the entire domain
        {
          Cerr << "The option of post processing the entire domain with Ecrire_fichier_xyz_valeur is now obsolete." <<finl;
          exit();
        }
    }
}
