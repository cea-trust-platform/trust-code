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
// File:        Champ_Fonc_MED.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_MED.h>
#include <LireMED.h>
#include <Char_ptr.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDCouplingField.hxx>
#include <MEDCouplingFieldDouble.hxx>
using MEDCoupling::MEDCouplingField;
using MEDCoupling::MEDCouplingFieldDouble;
using MEDCoupling::MCAuto;
using MEDCoupling::GetTimeAttachedOnFieldIteration;
using MEDCoupling::GetAllFieldNamesOnMesh;
#endif

Implemente_instanciable_sans_constructeur(Champ_Fonc_MED,"Champ_Fonc_MED",Champ_Fonc_base);

Champ_Fonc_MED::Champ_Fonc_MED():Champ_Fonc_base::Champ_Fonc_base()
{
  last_time_only_=0;
#ifdef MEDCOUPLING_
  use_medcoupling_ = true;
  field_type = MEDCoupling::ON_CELLS;
#else
  use_medcoupling_ = false;
#endif
}

//     printOn()
/////

Sortie& Champ_Fonc_MED::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Champ_Fonc_MED::readOn(Entree& s)
{
  Nom nom_fic2,nom_dom,nom_champ;
  Motcle localisation;
  double un_temps;
  s>>nom_fic2;
  int use_existing_domain=0;
  last_time_only_=0;
  if (nom_fic2=="use_existing_domain")
    {
      use_existing_domain=1;
      s>>nom_fic2;
    }
  if (nom_fic2=="last_time")
    {
      last_time_only_=1;
      s>>nom_fic2;
    }

  lire_nom_med(nom_dom,s);
  lire_nom_med(nom_champ,s);

  s>>localisation;
  s>>un_temps;
  traite_nom_fichier_med(nom_fic2);
#ifdef MED_
  test_version(nom_fic2);
#endif
  // MEDCoupling:
  if (use_medcoupling_)
    Cerr << "Using MEDCoupling API. To switch to the MEDFile API, use Champ_Fonc_MEDfile keyword." << finl;
  // Ajout d'un test bloquant pour empecher la lecture de fichiers 000n.med
  Nom tmp(nom_fic2);
  tmp.prefix("0001.med");
  if (tmp!=nom_fic2 && Process::nproc()>1)
    {
      Cerr << "Error, Champ_Fonc_MED can't read, for the moment, partitioned MED files." << finl;
      Cerr << "Try to build a single MED file by:" << finl;
      Cerr << "A) Using LATA format for the previous calculation." << finl;
      Cerr << "B) Convert the LATA files into a single MED file with Lata_to_MED keyword." << finl;
      Cerr << "C) Use this single MED file with Champ_Fonc_MED keyword." << finl;
      exit();
    }
  nommer(nom_champ);

  bool domain_exist=((interprete().objet_existant(nom_dom))&& ( sub_type(Domaine, interprete().objet(nom_dom))));
  if (( use_existing_domain) && (!domain_exist))
    {
      Cerr<<"The domain "<< nom_dom << " does not exist !!!! In Champ_Fonc_MED "<< nom_fic2<<" "<<nom_dom<<" "<<nom_champ<<" "<<localisation<<" "<<un_temps <<finl;
      exit();
    }
  if (domain_exist && use_existing_domain)
    {
      Cerr<<nom_dom<<" was not read into the med file because it already exists." <<finl;

      Domaine& un_dom=ref_cast(Domaine, interprete().objet(nom_dom));
#ifndef NDEBUG
      // on va verifier que l'on a le meme domaine en mode debug car lent
      if (1)
        {
          LireMED liremed;
          dom.nommer(nom_dom);
          // ne pas initialiser le nom du domaine va conduire a ne pas creer les fichiers sous zones
          Nom nom_dom_trio_non_nomme;
          liremed.lire_geom(nom_fic2,dom,nom_dom,nom_dom_trio_non_nomme);
          dom.les_sommets().set_md_vector(un_dom.les_sommets().get_md_vector());
          dom.les_sommets()-=un_dom.les_sommets();
          dom.les_sommets().set_md_vector(MD_Vector());
          dom.zone(0).les_elems().set_md_vector(un_dom.zone(0).les_elems().get_md_vector());
          dom.zone(0).les_elems()-=un_dom.zone(0).les_elems();
          dom.zone(0).les_elems().set_md_vector(MD_Vector());
          if ((max_abs_array(dom.les_sommets())>1e-5)||(max_abs_array(dom.zone(0).les_elems())>0))
            {
              Cerr<<"Error the domain in the file "<<nom_fic2<<" and domain "<<nom_dom<<" are not the same (coords,conn)."<<finl;
              exit();
            }
          //
          dom=Domaine();
        }
#endif
      creer(nom_fic2,un_dom,localisation,temps_sauv_);
    }
  else
    {
      if (domain_exist)
        {
          Cerr<<"INFO: You can use Champ_fonc_med use_existing_domain "<< nom_fic2<<" "<<nom_dom<<" "<<nom_champ<<" "<<localisation<<" "<<un_temps<<" to optimize."<<finl;
        }
      LireMED liremed;
      dom.nommer(nom_dom);
      // ne pas initialiser le nom du domaine va conduire a ne pas creer les fichiers sous zones
      Nom nom_dom_trio_non_nomme;
      liremed.lire_geom(nom_fic2,dom,nom_dom,nom_dom_trio_non_nomme);
      // MODIF ELI LAUCOIN (06/03/2012) :
      // j'ajoute l'attribut temps_sauv_
      creer(nom_fic2,dom,localisation,temps_sauv_);
    }
  if (last_time_only_)
    {
      un_temps=temps_sauv_(temps_sauv_.size_array()-1);
      Cout << "The resumption time is "<<un_temps<<finl;
    }
  // FIN MODIF ELI LAUCOIN (06/03/2012)
  le_champ().nommer(nom_champ);
  le_champ().corriger_unite_nom_compo();
  mettre_a_jour(un_temps);

  return s ;
}

void Champ_Fonc_MED::mettre_a_jour(double t)
{
  if (t<0)
    {
      // Si t negatif convention (non indique dans la doc Trust) pour specifier un numero de pas de temps:
      int given_it = -(int) t - 1;
      lire(-1, given_it);
    }
  else
    lire(t);
}

void Champ_Fonc_MED::lire(double t, int given_it)
{
#ifdef MED_
#ifdef MEDCOUPLING_
  if (use_medcoupling_)
    {
      // Read a field
      std::string  meshName = mon_dom.le_nom().getString();
      std::string fieldName = nom_champ_dans_fichier_med_.getString();
      std::string  fileName = nom_fic.getString();
      // Etude des conditions pour chercher ou nom un champ dans le fichier MED:
      bool search_field = true;
      nb_dt = temps_sauv_.size_array();
      if (nb_dt>0)
        {
          // nb_dt nombre de pas de temps dans le fichier MED
          // ndt taille du tableau temps_sauv contenant les pas de temps du champ dans le fichier MED
          // tmax dernier temps du tableau temps_sauv ?
          // dt ? ToDo
          // t temps courant pour lequel on veut remplir le champ
          // last_time_only_ specifie dans le jeu de donnees ou non
          double tmax = temps_sauv_(nb_dt - 1);
          double dt = temps_sauv_(nb_dt - 1);
          Cerr << "Provisoire temps=" << temps_sauv_ << finl;
          Cerr << "Provisoire temps= " << nb_dt << " " << dt << " " << t << " " << tmax << " " << last_time_only_
               << finl;
          if (((nb_dt == 1) && (!est_egal(dt, t))) || ((last_time_only_ == 1) && (!est_egal(tmax, t))))
            {
              Cout << "We assume that the field " << fieldName.c_str() << " is stationary." << finl;
              search_field = false;
            }
        }
      if (search_field)
        {
          bool read_field = false;
          Cerr << "Searching for the field " << fieldName.c_str() << " at ";
          if (given_it != -1)
            Cerr << "iteration " << given_it + 1;
          else
            Cerr << "time " << t;
          Cerr << " on the mesh " << meshName.c_str() << " in " << fileName.c_str() << " ..." << finl;
          // Loop on field iterations to find the good time:
          std::vector<std::pair<int, int> > Iterations = GetFieldIterations(field_type, fileName, meshName,
                                                                            fieldName);
          for (unsigned it = 0; it < Iterations.size(); it++)
            {
              int iteration = Iterations[it].first;
              int order = Iterations[it].second;
              // Either, given iteration matches or fieldTime matches given time t:
              double fieldTime = GetTimeAttachedOnFieldIteration(fileName, fieldName, iteration, order);
              if (given_it != -1 ? given_it == (int) it : est_egal(fieldTime, t))
                {
                  // ToDo can we avoid reloading the mesh ...
                  // Only one MCAuto below to avoid double deletion:
                  MCAuto<MEDCouplingField> ffield = ReadField(field_type, fileName, meshName, 0, fieldName,
                                                              iteration, order);
                  MEDCouplingFieldDouble * field = dynamic_cast<MEDCouplingFieldDouble *>((MEDCouplingField *)ffield);
                  if (field == 0)
                    {
                      Cerr << "ERROR reading MED field! Not a MEDCouplingFieldDouble!!" << finl;
                      Process::exit(-1);
                    }
                  const double *field_values = field->getArray()->begin();
                  assert(field->getNumberOfTuplesExpected() == le_champ().valeurs().dimension(0));
                  assert((int) field->getNumberOfComponents() ==
                      (le_champ().valeurs().nb_dim() == 1 ? 1 : le_champ().valeurs().dimension(1)));
                  memcpy(le_champ().valeurs().addr(), field_values,
                         le_champ().valeurs().size_array() * sizeof(double));
                  read_field = true;
                  Cerr << "OK, reading the field." << finl;
                  break;
                }
            }
          if (!read_field)
            {
              Cerr << "Field not found !" << finl;
              Process::exit();
            }
        }
    }
  else
#endif
    {
      int fid = MEDfileOpen(nom_fic, MED_ACC_RDONLY);
      if (fid < 0)
        {
          Cerr << "Problem while opening the file " << nom_fic << finl;
          exit();
        }

      Char_ptr pflname;
      pflname.allocate(MED_NAME_SIZE);
      if (given_it != -1)
        {
          Cerr<<"Searching for the "<<given_it+1<< " iteration of the field "<<le_nom()<<finl;
        }

      Char_ptr dt_unit;
      dt_unit.allocate(MED_SNAME_SIZE);
      // on cherche le numero du pas de temps
      med_int numdt, numo;
      double dt = -2;
      int ret;
      Char_ptr maa_ass;
      maa_ass.allocate(MED_NAME_SIZE);
      //Cerr<<"nb_dt"<<nb_dt<<finl;
      Nom nom_dom(mon_dom.le_nom());
      int i2 = 0;
      Nom& le_nom_du_champ = nom_champ_dans_fichier_med_;
      med_field_type typcha;
      //  Nom meshname; dimensionne_nom_taille(meshname,MED_NAME_SIZE);
      Char_ptr dtunit;
      dtunit.allocate(MED_SNAME_SIZE);
      med_bool localmesh;
      med_int nbofcstp;
      int ncomp = MEDfieldnComponentByName(fid, le_nom_du_champ);
      Char_ptr comp;
      comp.allocate(MED_SNAME_SIZE * ncomp);
      Char_ptr unit;
      unit.allocate(MED_SNAME_SIZE * ncomp);
      ret = MEDfieldInfoByName(fid, le_nom_du_champ, maa_ass, &localmesh, &typcha, comp, unit, dtunit, &nbofcstp);
      int nb_dt_tot = nbofcstp;
      double tmax = -1;
      for (int j = 0; j < nb_dt_tot; j++)
        {
          med_int meshnumdt, meshnumit;
          MEDfieldComputingStepMeshInfo(fid, le_nom_du_champ, j + 1, &numdt, &numo, &dt, &meshnumdt, &meshnumit);
          tmax = dt;
        }
      for (int j = 0; j < nb_dt_tot; j++)
        {
          if (i2 >= nb_dt) exit();

          med_int meshnumdt, meshnumit;
          MEDfieldComputingStepMeshInfo(fid, le_nom_du_champ, j + 1, &numdt, &numo, &dt, &meshnumdt, &meshnumit);
          Nom Nmaa_ass(maa_ass);
          if (est_egal(dt, t))
            {
              int size = MEDfieldnValue(fid, le_nom_du_champ, numdt, numo, type_ent, type_geo);
              if ((size != 0) && (nom_dom == Nmaa_ass))
                break;
            }
          if (Nmaa_ass == nom_dom)
            {
              if (given_it == i2)
                {
                  t = dt;
                  given_it = j;
                  break;
                }
              i2++;
            }
        }
      //dt=t;
      //numdt=1;
      //numo=MED_NONOR;
      Cerr << "Provisoire temps=" << temps_sauv_ << finl;
      Cerr << "Provisoire temps= " << nb_dt << " " << dt << " " << t << " " << tmax << " " << last_time_only_ << finl;
      if (((nb_dt == 1) && (!est_egal(dt, t))) || ((last_time_only_ == 1) && (!est_egal(tmax, t))))
        {
          Cout << "We assume that the field " << le_nom_du_champ << " is stationary." << finl;
        }
      else
        {
          if (!est_egal(dt, t))
            {
              Cerr.precision(12);
              Cerr << "Error: we did not find the time " << t << finl;
              Cerr << "Available times for " << le_nom_du_champ << " domain " << nom_dom << " are:" << finl;
              for (int j = 0; j < nb_dt; j++)
                {
                  med_int meshnumdt, meshnumit;
                  MEDfieldComputingStepMeshInfo(fid, le_nom_du_champ, j + 1, &numdt, &numo, &dt, &meshnumdt, &meshnumit);
                  Cerr << dt << finl;
                }
              exit();
            }
          Cerr << "Reading the time step " << (int) numdt << " of field " << le_nom_du_champ << " time " << t
               << " type ent " << (int) type_ent << finl;
          ret = MEDfieldValueRd(fid, le_nom_du_champ, numdt, numo, type_ent, type_geo, MED_FULL_INTERLACE,
                                MED_ALL_CONSTITUENT, (unsigned char *) le_champ().valeurs().addr());
          if (ret < 0)
            {
              Cerr << "Problem while reading field " << le_nom_du_champ << " name of domain " << mon_dom.le_nom() << finl;
              exit();
            }
          MEDfileClose(fid);
        }
    }
  // Mise a jour:
  Champ_Fonc_base::mettre_a_jour(t);
  le_champ().Champ_Fonc_base::mettre_a_jour(t);
#else
  med_non_installe();
#endif
}
const Domaine& Champ_Fonc_MED::domaine() const
{
  return mon_dom;
}

#ifdef MED_
med_geometry_type type_geo_trio_to_type_med(const Nom& type);
#ifdef MEDCOUPLING_
INTERP_KERNEL::NormalizedCellType type_geo_trio_to_type_medcoupling(const Nom& type, int& mesh_dimension);
#endif
#endif

void Champ_Fonc_MED::creer(const Nom& nomfic, const Domaine& un_dom, const Motcle& localisation, ArrOfDouble& temps_sauv)
{
#ifdef MED_

  mon_dom=un_dom;
  nom_fic=nomfic;
  int nbcomp,size;
  const Nom& type_elem=un_dom.zone(0).type_elem()->que_suis_je();
  Nom type_champ;
  nom_champ_dans_fichier_med_ = le_nom();
#ifdef MEDCOUPLING_
  if (use_medcoupling_)
    {
      // MEDCoupling
      int mesh_dimension = -1;
      cell_type = type_geo_trio_to_type_medcoupling(type_elem, mesh_dimension);
      if (localisation != Nom())
        {
          if (localisation == "som")
            {
              field_type = MEDCoupling::ON_NODES;
              if ((cell_type == INTERP_KERNEL::NORM_QUAD4) || (cell_type == INTERP_KERNEL::NORM_HEXA8))
                type_champ = "Champ_Fonc_Q1_MED";
              else
                type_champ = "Champ_Fonc_P1_MED";
            }
          else if (localisation == "elem")
            {
              field_type = MEDCoupling::ON_CELLS;
              type_champ = "Champ_Fonc_P0_MED";
            }
          else
            {
              Cerr << localisation << " localization unknown." << finl;
              exit();
            }
        }
      std::string meshName = mon_dom.le_nom().getString();
      std::string fileName = nom_fic.getString();
      // Try to guess the field name in the MED file:
      Noms fieldNamesGuess;
      fieldNamesGuess.add((Motcle)nom_champ_dans_fichier_med_+"_"+mon_dom.le_nom());
      if (localisation != Nom())
        fieldNamesGuess.add((Motcle)nom_champ_dans_fichier_med_+"_"+localisation+"_"+mon_dom.le_nom());
      fieldNamesGuess.add((Motcle)nom_champ_dans_fichier_med_);
      if (!fieldNamesGuess.contient_(nom_champ_dans_fichier_med_))
        fieldNamesGuess.add(nom_champ_dans_fichier_med_);
      bool ok = false;
      std::vector<std::string> fieldNames = GetAllFieldNamesOnMesh(fileName, meshName);
      for (int i=0; i<fieldNamesGuess.size(); i++)
        {
          if (std::find(fieldNames.begin(), fieldNames.end(), fieldNamesGuess[i].getString()) != fieldNames.end())
            {
              nom_champ_dans_fichier_med_ = fieldNamesGuess[i];
              ok = true;
              break;
            }
        }
      if (!ok)
        {
          Cerr << "Unable to find into file " << fileName.c_str() << " a field named like :" << finl;
          Cerr << fieldNamesGuess << finl;
          Cerr << "This file contains the field(s) named:" << finl;
          for (unsigned i=0; i<fieldNames.size(); i++)
            Cerr << fieldNames[i].c_str() << finl;
          Process::exit();
        }
      else
        Cerr << "Ok, we find into file " << fileName.c_str() << " a field named " << nom_champ_dans_fichier_med_ << finl;
      std::string fieldName = nom_champ_dans_fichier_med_.getString();
      std::vector< std::pair<int,int> > Iterations = GetFieldIterations(field_type, fileName, meshName, fieldName);
      temps_sauv.resize_array(Iterations.size());
      for (unsigned it=0; it<Iterations.size(); it++)
        {
          int iteration = Iterations[it].first;
          int order = Iterations[it].second;
          // Either, given iteration matches or fieldTime matches given time t:
          double fieldTime = GetTimeAttachedOnFieldIteration(fileName, fieldName, iteration, order);
          temps_sauv[it] = fieldTime;
          // Last time:
          if (it==Iterations.size()-1)
            {
              // ToDo can we avoid reloading the mesh ...
              // Only one MCAuto below to avoid double deletion:
              MCAuto<MEDCouplingField> ffield = ReadField(field_type, fileName, meshName, 0, fieldName, iteration,
                                                          order);
              MEDCouplingFieldDouble * field = dynamic_cast<MEDCouplingFieldDouble *>((MEDCouplingField *)ffield);
              if (field == 0)
                {
                  Cerr << "ERROR reading MED field! Not a MEDCouplingFieldDouble!!" << finl;
                  Process::exit(-1);
                }
              size = field->getNumberOfTuplesExpected();
              nbcomp = (int) field->getNumberOfComponents();
            }
        }
      if (field_type == MEDCoupling::ON_NODES)
        {
          if ((cell_type == INTERP_KERNEL::NORM_QUAD4) || (cell_type == INTERP_KERNEL::NORM_HEXA8))
            type_champ = "Champ_Fonc_Q1_MED";
          else
            type_champ = "Champ_Fonc_P1_MED";
        }
      else if (field_type == MEDCoupling::ON_CELLS)
        type_champ = "Champ_Fonc_P0_MED";
    }
  else
#endif
    {
      // MEDFile
      type_geo = type_geo_trio_to_type_med(type_elem);
      int verifie_type = 0;
      if (localisation != Nom())
        {
          verifie_type = 1;
          if (localisation == "som")
            type_ent = MED_NODE;
          else if (localisation == "elem")
            type_ent = MED_CELL;
          else
            {
              Cerr << localisation << " localisation not understood " << finl;
              exit();
            }
        }
      nom_champ_dans_fichier_med_ = medinfo1champ(nomfic, nom_champ_dans_fichier_med_, numero_ch, nbcomp, nb_dt,
                                                  type_ent, type_geo, size, un_dom.le_nom(), verifie_type, temps_sauv);
      if (type_ent == MED_NODE)
        {
          if ((type_geo == MED_QUAD4) || (type_geo == MED_HEXA8))
            type_champ = "Champ_Fonc_Q1_MED";
          else
            type_champ = "Champ_Fonc_P1_MED";
        }
      else if (type_ent == MED_CELL)
        {
          type_champ = "Champ_Fonc_P0_MED";
        }
    }
  // Definition:
  vrai_champ_.typer(type_champ);
  fixer_nb_comp(nbcomp);
  le_champ().fixer_nb_comp(nbcomp);
  zonebidon_inst.associer_zone(un_dom.zone(0));
  le_champ().associer_zone_dis_base(zonebidon_inst);
  le_champ().fixer_nb_valeurs_nodales(size);
  if (size != le_champ().valeurs().dimension(0))
    {
      Cerr << "Error in Champ_Fonc_MED: nb_ddl incorrect" << finl;
      exit();
    }
  //pour forcer la lecture lors du mettre a jour
  changer_temps(-1e3);

  //corriger_unite_nom_compo();
  le_champ().nommer(le_nom());
  //le_champ().corriger_unite_nom_compo();
#else
  med_non_installe();
#endif
}

const Zone_dis_base& Champ_Fonc_MED::zone_dis_base() const
{
  return zonebidon_inst;
}

const ArrOfDouble& Champ_Fonc_MED::get_saved_times(void) const
{
  return temps_sauv_;
}


/**
 * Read field with MEDFile API (soon deprecated)
 */
Implemente_instanciable_sans_constructeur(Champ_Fonc_MEDfile, "Champ_Fonc_MEDfile", Champ_Fonc_MED);
// XD Champ_Fonc_MEDfile field_base Champ_Fonc_MEDfile -1 Obsolete keyword to read a field with MED file API
Champ_Fonc_MEDfile::Champ_Fonc_MEDfile()
{
  use_medcoupling_ = false;
}

Sortie& Champ_Fonc_MEDfile::printOn(Sortie& os) const
{
  return Champ_Fonc_MED::printOn(os);
}

Entree& Champ_Fonc_MEDfile::readOn(Entree& is)
{
  return Champ_Fonc_MED::readOn(is);
}



