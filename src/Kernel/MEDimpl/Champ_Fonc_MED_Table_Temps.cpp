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

#include <Champ_Fonc_MED_Table_Temps.h>
#include <Lecture_Table.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDCouplingFieldDouble.hxx>
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileField.hxx>
#include <MEDCouplingField.hxx>
using MEDCoupling::MEDCouplingField;
using MEDCoupling::MEDCouplingFieldDouble;
using MEDCoupling::MCAuto;
using MEDCoupling::MEDFileFieldMultiTS;
#endif

Implemente_instanciable( Champ_Fonc_MED_Table_Temps, "Champ_Fonc_MED_Table_Temps", Champ_Fonc_MED ) ;

Sortie& Champ_Fonc_MED_Table_Temps::printOn( Sortie& os ) const
{
  Champ_Fonc_MED::printOn( os );
  return os;
}

Entree& Champ_Fonc_MED_Table_Temps::readOn( Entree& is )
{
  Motcle mot;

  is >> mot;
  if (mot=="table_temps")
    {
      Lecture_Table lec_table;
      lec_table.lire_table(is,la_table);
    }
  else
    {
      Cerr << "Error while reading " << que_suis_je() << ". We expected a time table."<< finl;
      Process::exit();
    }

  return Champ_Fonc_MED::readOn( is );
}

void Champ_Fonc_MED_Table_Temps::lire_donnees_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName,
                                                    ArrOfDouble& temps_sauv, int& size, int& nbcomp, Nom& type_champ)
{
#ifdef MEDCOUPLING_
  MCAuto<MEDFileFieldMultiTS> ft1(MEDFileFieldMultiTS::New(fileName,fieldName));
  std::vector<double> tps;
  std::vector< std::pair<int,int> > tst = ft1->getTimeSteps(tps);

  temps_sauv.resize_array(1);

  int first_iter  = tst[0].first;
  int first_order = tst[0].second;
  temps_sauv[0] = tps[0];

  // Only one MCAuto below to avoid double deletion:
  MCAuto<MEDCouplingField> ffield = ReadField(field_type, fileName, meshName, 0, fieldName,
                                              first_iter, first_order);
  MEDCouplingFieldDouble * field = dynamic_cast<MEDCouplingFieldDouble *>((MEDCouplingField *)ffield);
  if (field == 0)
    {
      Cerr << "ERROR reading MED field! Not a MEDCouplingFieldDouble!!" << finl;
      Process::exit(-1);
    }
  size = field->getNumberOfTuplesExpected();
  nbcomp = (int) field->getNumberOfComponents();

  if (field_type == MEDCoupling::ON_NODES)
    {
      if ((cell_type == INTERP_KERNEL::NORM_QUAD4) || (cell_type == INTERP_KERNEL::NORM_HEXA8))
        type_champ = "Champ_Fonc_Q1_MED";
      else
        type_champ = "Champ_Fonc_P1_MED";
    }
  else if (field_type == MEDCoupling::ON_CELLS)
    type_champ = "Champ_Fonc_P0_MED";

  vrai_champ0_.typer(type_champ);
  fixer_nb_comp(nbcomp);
  le_champ0().fixer_nb_comp(nbcomp);
  zonebidon_inst.associer_zone(mon_dom.valeur().zone(0));
  le_champ0().associer_zone_dis_base(zonebidon_inst);
  if (zonebidon_inst.nb_elem()==0) size = 0;
  le_champ0().fixer_nb_valeurs_nodales(size);
  if (size != le_champ0().valeurs().dimension(0))
    {
      Cerr << "Error in " << que_suis_je() << " : nb_ddl incorrect" << finl;
      Process::exit();
    }
  const double *field_values = field->getArray()->begin();
  assert(field->getNumberOfTuplesExpected() == le_champ0().valeurs().dimension(0));
  assert((int) field->getNumberOfComponents() ==
         (le_champ0().valeurs().nb_dim() == 1 ? 1 : le_champ0().valeurs().dimension(1)));
  memcpy(le_champ0().valeurs().addr(), field_values,
         le_champ0().valeurs().size_array() * sizeof(double));
#endif
}

void Champ_Fonc_MED_Table_Temps::lire(double t, int given_it)
{
#ifdef MED_
#ifndef MEDCOUPLING_
  Cerr << "Error while reading " << que_suis_je() << ". MEDCoupling library is mandatory." << finl;
  Process::exit();
#endif
#endif

  if (zonebidon_inst.nb_elem()>0)
    {
      double frac = la_table.val(t);
      const DoubleTab& vals0 = le_champ0().valeurs();
      DoubleTab& vals = le_champ().valeurs();

      vals = vals0;
      vals *=frac;
    }
  Champ_Fonc_base::mettre_a_jour(t);
  le_champ().Champ_Fonc_base::mettre_a_jour(t);
}
