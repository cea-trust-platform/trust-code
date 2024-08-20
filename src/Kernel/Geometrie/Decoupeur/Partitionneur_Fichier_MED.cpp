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
#include <Partitionneur_Fichier_MED.h>
#include <EFichier.h>
#include <Domaine.h>
#include <medcoupling++.h>
#include <Param.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDCouplingField.hxx>
#include <MEDCouplingFieldInt32.hxx>
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileField.hxx>
#include <communications.h>
#include <Champ_Fonc_MED.h>
#include <EChaine.h>

#endif

// XD Partitionneur_Fichier_MED partitionneur_deriv fichier_med -1 Partitioning a domain using a MED file containing an integer field providing for each element the processor number on which the element should be located.
Implemente_instanciable_sans_constructeur(Partitionneur_Fichier_MED,"Partitionneur_Fichier_MED",Partitionneur_base);

Partitionneur_Fichier_MED::Partitionneur_Fichier_MED()
{
}

/*! @brief Lecture des parametres du partitionneur sur disque.
 *
 * Fomat attendu:
 *     { file FILENAME field FIELDNAME }
 *   FILENAME est le nom d'un fichier MED, le champ FIELDNAME comporte un ArrayOfInt avec le numero du processeur.
 *
 */
Entree& Partitionneur_Fichier_MED::readOn(Entree& is)
{
  Partitionneur_base::readOn(is);
  Cerr << " filename : " << filename_ << finl;
  Cerr << " fieldname : " << fieldname_ << finl;
  return is;
}

Sortie& Partitionneur_Fichier_MED::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_MED::printOn invalid\n" << finl;
  exit();
  return os;
}

void Partitionneur_Fichier_MED::set_param(Param& param)
{
  param.ajouter("file",&filename_,Param::REQUIRED); // XD_ADD_P chaine file name of the MED file to load
  param.ajouter("field",&fieldname_,Param::OPTIONAL); // XD_ADD_P chaine field name of the integer (or double) field to load
}

void Partitionneur_Fichier_MED::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

void Partitionneur_Fichier_MED::initialiser(const char *filename)
{
  filename_ = filename;
}

/*! @brief Lit le contenu du fichier "filename_" et stocke le resultat dans elem_part
 *
 */
void Partitionneur_Fichier_MED::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
{
#if !defined(INT_is_64_) || INT_is_64_ == 1

  if (! ref_domaine_.non_nul())
    {
      Cerr << "Error in Partitionneur_Fichier_MED::construire_partition\n";
      Cerr << " The domain has not been associated" << finl;
      exit();
    }
  if (filename_ == "")
    {
      Cerr << "Error in Partitionneur_Fichier_MED::construire_partition\n";
      Cerr << " The file name has not been initialized" << finl;
      exit();
    }
  if (fieldname_ == "")
    {
      Cerr << "Error in Partitionneur_Fichier_MED::construire_partition\n";
      Cerr << " The field name has not been initialized" << finl;
      exit();
    }

  Cerr << "Reading of splitting file : " << filename_ << finl;

#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  MCAuto<MEDCouplingField> ffield = MEDCoupling::ReadField(filename_.getString(), fieldname_.getString());
  MEDCouplingFieldInt32 * field = dynamic_cast<MEDCouplingFieldInt32 *>((MEDCouplingField *)ffield);
  if(field)
    {
      // Lecture d'une partition exacte du domaine donnee par un champ d'entiers
      DataArrayInt32 *da = field->getArray();
      const True_int *field_values = da->begin();

      const mcIdType sz = field->getNumberOfTuplesExpected();
      elem_part.resize(sz);
      std::copy(field_values, field_values + sz, elem_part.addr());
      // Sanity check
      const int nelem = ref_domaine_->nb_elem();
      if (nelem != sz)
        {
          Cerr << "Error in Partitionneur_Fichier_MED::construire_partition" << finl;
          Cerr << " The file contains an array of " << sz << " values." << finl;
          Cerr << " The area contains " << nelem << " elements" << finl;
          exit();
        }
      // Checking that the full range is covered
      MCAuto<DataArrayInt32> das = da->buildUniqueNotSorted();
      MCAuto<DataArrayInt32> iot(DataArrayInt32::New());
      mcIdType dnu;
      const mcIdType max_v = das->getMaxValue(dnu);
      iot->alloc(max_v + 1, 1);
      iot->iota();
      if (!iot->isEqualWithoutConsideringStr(*das))
        {
          Cerr << "Error in Partitionneur_Fichier_MED::construire_partition" << finl;
          Cerr << " The file contains/lacks integer values which do not cover the range [0; n_elem[." << finl;
          Cerr << " Are some field values missing?" << finl;
          exit();
        }
#else
  Cerr << "Partitionneur_Fichier_MED requires TRUST compiled with MEDCoupling support!" << finl;
  exit();
#endif
    }
  else
    {
      // Lecture d'une partition d'un domaine qui recouvre partiellement le domaine par un champ de double:
      /*
        Decouper outer_domain {
              partitionneur metis { nb_parts 3 } zones_name outer_domain
              ecrire_med outer_domain.med
              }
        Decouper inner_domain {
              partitionneur fichier_med { file outer_domain_0000.med }
              zones_name inner_domain
        }
      */
      // Read the partition field with Champ_Fonc_MED:
      Nom cli("");
      cli+="{ file ";
      cli+=filename_;
      cli+=" domain ";
      cli+= ffield->getMesh()->getName();
      cli+=" field partition loc elem last_time }";
      EChaine ech(cli);
      Champ_Fonc_MED partition_field_outer_domain;
      ech >> partition_field_outer_domain;
      // Mapping between local domain and the field domain:
      int nb_elem = ref_domaine_->nb_elem();
      DoubleTab xp_elems;
      ref_domaine_->calculer_centres_gravite(xp_elems);
      IntVect elems(nb_elem);
      partition_field_outer_domain.domaine().chercher_elements(xp_elems, elems);
      DoubleTab double_elem_part(nb_elem);
      double_elem_part = -1;
      partition_field_outer_domain.valeur_aux_elems(xp_elems, elems, double_elem_part);
      if (local_min_vect(double_elem_part)<0)
        {
          Cerr << "The domain " << ref_domaine_->le_nom() << " it not fully included into the domain " <<  ffield->getMesh()->getName() << " ! " << finl;
          Cerr << "It is mandatory to use the partition field from " << filename_ << finl;
          Process::exit();
        }
      // Convert double to int to fill elem_part:
      elem_part.resize(nb_elem);
      for (int i=0; i<nb_elem; i++)
        elem_part(i)=(int)double_elem_part(i);
      nb_parts_tot = 1+int(local_max_vect(partition_field_outer_domain.valeurs()));
      return;
    }
#else
  Process::exit("Partitionneur_Fichier_MED not ported yet!! TODO.");
#endif
}
