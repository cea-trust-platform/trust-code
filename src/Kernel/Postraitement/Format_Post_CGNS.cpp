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

#include <Format_Post_CGNS.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Format_Post_CGNS, "Format_Post_CGNS", Format_Post_base);

Format_Post_CGNS::Format_Post_CGNS()
{
  reset();
}

void Format_Post_CGNS::reset()
{
  cgns_basename_ = "??";
}

Sortie& Format_Post_CGNS::printOn(Sortie& os) const
{
  Process::exit("Format_Post_CGNS::printOn - Should not be called ! ");
  return os;
}

Entree& Format_Post_CGNS::readOn(Entree& is)
{
  verify_if_cgns(__func__);
  return Format_Post_base::readOn(is);
}

void Format_Post_CGNS::set_param(Param& param)
{
  verify_if_cgns(__func__);
  param.ajouter("nom_fichier", &cgns_basename_, Param::REQUIRED);
}

int Format_Post_CGNS::initialize_by_default(const Nom& file_basename)
{
  verify_if_cgns(__func__);
  cgns_basename_ = file_basename;
  return 1;
}

int Format_Post_CGNS::initialize(const Nom& file_basename, const int format, const Nom& option_para)
{
  verify_if_cgns(__func__);
  cgns_basename_ = file_basename;
  return 1;
}

// E Saikali : on ajoute cette liste qui est utile pour un probleme couple dans le cas ou on ecrit dans meme fichier
// Soucis plus tard car ecrire_temps est pas appelee si pb > 1. donc pour le moment on n'autorise paq
static Noms liste_cgns_ecrit;

int Format_Post_CGNS::ecrire_entete(const double temps_courant,const int reprise,const int est_le_premier_post)
{
  verify_if_cgns(__func__);
  std::string fn = cgns_basename_.getString() + ".cgns"; // file name
  const bool not_in_list =  !liste_cgns_ecrit.contient_(cgns_basename_);

  if (not_in_list) liste_cgns_ecrit.add(cgns_basename_);
  else
    {
      Cerr << "==============================================================================================================================================================" << finl;
      Cerr << "==============================================================================================================================================================" << finl;
      Cerr << "Error Format_Post_CGNS::ecrire_entete - We can not write coupled problems in same file. Please use the ***fichier*** keyword to dump field in seperate files !" << finl;
      Cerr << "==============================================================================================================================================================" << finl;
      Cerr << "==============================================================================================================================================================" << finl;
      Process::exit();
    }

  if (est_le_premier_post)
    {
#ifdef HAS_CGNS
      cgns_writer_.open_cgns_file(fn);
#endif
    }
  return 1;
}

int Format_Post_CGNS::ecrire_temps(const double t)
{
#ifdef HAS_CGNS
  cgns_writer_.add_time(t);
#endif
  return 1;
}

int Format_Post_CGNS::finir(const int est_le_dernier_post)
{
  if (est_le_dernier_post)
    {
#ifdef HAS_CGNS
      std::string fn = cgns_basename_.getString() + ".cgns"; // file name
      cgns_writer_.close_cgns_file(fn);
#endif
    }
  return 1;
}

int Format_Post_CGNS::ecrire_domaine(const Domaine& domaine, const int est_le_premier_post)
{
#ifdef HAS_CGNS
  cgns_writer_.cgns_write_domaine(domaine, domaine.le_nom());

  // Si on a des frontieres domaine, on les ecrit egalement
  const LIST(REF(Domaine)) &bords = domaine.domaines_frontieres();
  for (auto &itr : bords) ecrire_domaine(itr.valeur(), est_le_premier_post);
#endif
  return 1;
}

int Format_Post_CGNS::ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps,
                                   const Nom& id_du_champ, const Nom& id_du_domaine, const Nom& localisation,
                                   const Nom& nature, const DoubleTab& valeurs)
{
#ifdef HAS_CGNS
  cgns_writer_.cgns_write_field(domaine, noms_compo, temps, id_du_champ, id_du_domaine, localisation, valeurs );
#endif
  return 1;
}
