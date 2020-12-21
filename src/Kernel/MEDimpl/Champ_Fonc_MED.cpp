/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <EFichier.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDCouplingField.hxx>
#include <MEDCouplingFieldDouble.hxx>
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileField.hxx>
#include <communications.h>

using MEDCoupling::MEDCouplingField;
using MEDCoupling::MEDCouplingFieldDouble;
using MEDCoupling::MCAuto;
using MEDCoupling::GetTimeAttachedOnFieldIteration;
using MEDCoupling::GetAllFieldNamesOnMesh;
using MEDCoupling::MEDFileFieldMultiTS;
using MEDCoupling::MEDFileField1TS;
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

Sortie& Champ_Fonc_MED::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


// XD champ_fonc_med field_base champ_fonc_med 0 Field to read a data field in a MED-format file .med at a specified time. It is very useful, for example, to resume a calculation with a new or refined geometry. The field post-processed on the new geometry at med format is used as initial condition for the resume.
// XD attr use_existing_domain chaine(into=["use_existing_domain"]) use_existing_domain 1 not_set
// XD attr last_time chaine(into=["last_time"]) last_time 1 to use the last time of the MED file instead of the specified time.
// XD attr option decoup option 1 Keyword for a partition file
// XD attr filename chaine filename 0 Name of the .med file.
// XD attr domain_name chaine domain_name 0 Name of the domain.
// XD attr field_name chaine field_name 0 Name of the problem unknown.
// XD attr location chaine(into=["som","elem"]) location 0 To indicate where the field has been post-processed.
// XD attr time floattant time 0 Time of the field in the .med file.

// XD decoup objet_lecture nul 0 Optional keyword
// XD attr key chaine(into=["decoup"]) key 0 Name of for a partition file
// XD attr nom chaine nom 0 Name of for a partition file
Entree& Champ_Fonc_MED::readOn(Entree& s)
{
  Nom chaine_lue,nom_dom,nom_champ, nom_decoup;
  Motcle localisation;
  double un_temps;
  s>>chaine_lue;
  int use_existing_domain=0, field_size;
  last_time_only_=0;
  if (chaine_lue=="use_existing_domain")
    {
      use_existing_domain=1;
      s>>chaine_lue;
    }
  if (chaine_lue=="last_time")
    {
      last_time_only_=1;
      s>>chaine_lue;
    }
  nom_decoup = Nom("decoup/") + nom_dom + ".txt";
  if (chaine_lue=="decoup")
    {
      s>>nom_decoup;
      s>>chaine_lue;
      if (use_medcoupling_)
        {
          Cerr << "Error: Decoup option is only available for the moment with Champ_Fonc_MEDFile keyword only." << finl;
          Process::exit();
        }
      if (!use_existing_domain)
        {
          Cerr << "Error: you need to use use_existing_domain option with Decoup option." << finl;
          Process::exit();
        }
    }
  nom_fichier_med_ = chaine_lue;
  lire_nom_med(nom_dom,s);
  lire_nom_med(nom_champ,s);
  s>>localisation;
  s>>un_temps;
  traite_nom_fichier_med(nom_fichier_med_);
//#ifdef MED_
//  test_version(nom_fichier_med_);
//#endif
  // MEDCoupling:
  if (use_medcoupling_)
    Cerr << "Using MEDCoupling API. To switch to the MEDFile API, use Champ_Fonc_MEDfile keyword." << finl;
  // La lecture de fichiers multiples .med fonctionne: voir cas test Champ_fonc_MED_Parallele
  // Un test est fait plus loin pour bien verifier que les partitionnement se recouvrent pour cela
  int multiple_med = 0;
  Nom tmp(nom_fichier_med_);
  tmp.prefix("0001.med");
  if (tmp!=nom_fichier_med_ && Process::nproc()>1)
    {
      multiple_med = 1;
      /*
      // Ajout d'un test bloquant pour empecher la lecture de fichiers 000n.med
      Cerr << "Error, Champ_Fonc_MED can't read, for the moment, partitioned MED files." << finl;
      Cerr << "Try to build a single MED file by:" << finl;
      Cerr << "A) Using LATA format for the previous calculation." << finl;
      Cerr << "B) Convert the LATA files into a single MED file with Lata_to_MED keyword." << finl;
      Cerr << "C) Use this single MED file with Champ_Fonc_MED keyword." << finl;
      exit(); */
    }
  multiple_med = mp_max(multiple_med);
  nommer(nom_champ);

  bool domain_exist=( interprete().objet_existant(nom_dom) && sub_type(Domaine, interprete().objet(nom_dom)) );
  if (use_existing_domain)
    {
      if (!domain_exist)
        {
          Cerr << "The domain " << nom_dom << " does not exist !!!! In Champ_Fonc_MED " << nom_fichier_med_ << " " << nom_dom
               << " " << nom_champ << " " << localisation << " " << un_temps << finl;
          exit();
        }
      else
        {
          // use_existing_domain utilisable en parallele uniquement si le process 0 gere tout le domaine ou si decoup specifie:
          const Domaine& le_domaine=ref_cast(Domaine, interprete().objet(nom_dom));
          if (Process::nproc()>1 && mp_max(le_domaine.nb_som()>0) != 0 && use_medcoupling_==1)
            {
              Cerr << "Warning, you can't use use_existing_domain on a partitionned domain like " << nom_dom << finl;
              Cerr << "It is not parallelized yet... So we use MED mesh, which is not optimal." << finl;
              Cerr << "Try suppress use_existing_domain option." << finl;
              //Process::exit();
              use_existing_domain = 0;
            }
        }
    }
  if (domain_exist && use_existing_domain)
    {
      Cerr<<nom_dom<<" was not read into the med file because it already exists." <<finl;

      const Domaine& le_domaine=ref_cast(Domaine, interprete().objet(nom_dom));

#ifndef NDEBUG
      // on va verifier que l'on a le meme domaine en mode debug car lent
      // Ce test ne peut etre fait qu'en sequentiel car en parallele, dom=domaine_complet, un_dom=domaine_local
      if (nproc()==1)
        {
          LireMED liremed;
          dom_med_.nommer(nom_dom);
          // ne pas initialiser le nom du domaine va conduire a ne pas creer les fichiers sous zones
          Nom nom_dom_trio_non_nomme;
          liremed.lire_geom(nom_fichier_med_,dom_med_,nom_dom,nom_dom_trio_non_nomme);

          DoubleTab diff_som(dom_med_.les_sommets());
          IntTab diff_elem(dom_med_.zone(0).les_elems());
          diff_som.set_md_vector(le_domaine.les_sommets().get_md_vector());
          diff_elem.set_md_vector(le_domaine.zone(0).les_elems().get_md_vector());
          diff_som-=le_domaine.les_sommets();
          diff_elem-=le_domaine.zone(0).les_elems();
          /*
                    diff_som.set_md_vector(un_dom.les_sommets().get_md_vector());
                    diff_som-=un_dom.les_sommets();
                    diff_som.set_md_vector(MD_Vector());
                    diff_elem.set_md_vector(un_dom.zone(0).les_elems().get_md_vector());
                    diff_elem-=un_dom.zone(0).les_elems();
                    diff_elem.set_md_vector(MD_Vector());
          */
          double err_som0 = max_abs_array(diff_som);
          int err_elem0 = max_abs_array(diff_elem);


          // Le domaine un_dom a peut etre ete reordonne, on refait le test en reordonnant le domaine dom .....
          dom_med_.reordonner();
          dom_med_.les_sommets().set_md_vector(le_domaine.les_sommets().get_md_vector());
          dom_med_.les_sommets()-=le_domaine.les_sommets();
          dom_med_.les_sommets().set_md_vector(MD_Vector());
          dom_med_.zone(0).les_elems().set_md_vector(le_domaine.zone(0).les_elems().get_md_vector());
          dom_med_.zone(0).les_elems()-=le_domaine.zone(0).les_elems();
          dom_med_.zone(0).les_elems().set_md_vector(MD_Vector());
          double err_som = max_abs_array(dom_med_.les_sommets());
          int err_elem = max_abs_array(dom_med_.zone(0).les_elems());

          if ((err_som>1e-5 || err_elem>0) && (err_som0>1e-5 || err_elem0>0))
            {
              Cerr<<"Error the domain in the file "<<nom_fichier_med_<<" and domain "<<nom_dom<<" are not the same (coords,conn)."<<finl;
              exit();
            }

          // Reset:
          dom_med_=Domaine();
        }
#endif
      field_size = creer(nom_fichier_med_,le_domaine,localisation,temps_sauv_);
    }
  else
    {
      if (domain_exist && !use_existing_domain)
        {
          Cerr<<"INFO: You can use Champ_fonc_med use_existing_domain "<< nom_fichier_med_<<" "<<nom_dom<<" "<<nom_champ<<" "<<localisation<<" "<<un_temps<<" to optimize."<<finl;
        }
      LireMED liremed;
      dom_med_.nommer(nom_dom);
      // ne pas initialiser le nom du domaine va conduire a ne pas creer les fichiers sous zones
      Nom nom_dom_trio_non_nomme;
      // Remplit dom:
      liremed.lire_geom(nom_fichier_med_,dom_med_,nom_dom,nom_dom_trio_non_nomme);
      if (multiple_med)
        {
          // On verifie que l'on a bien des recouvrements identiques (verification imparfaite sur les BoundingBox)
          DoubleTab BB1 = dom_med_.getBoundingBox();
          const Domaine& dom_calcul = ref_cast(Domaine, interprete().objet(nom_dom));
          DoubleTab BB2 = dom_calcul.getBoundingBox();
          for (int i=0; i<dimension; i++)
            for (int j=0; j<2; j++)
              if (est_different(BB1(i,j), BB2(i,j)))
                {
                  Cerr << "Error, Champ_Fonc_MED can't read the partitionned MED files." << finl;
                  Cerr << "Cause if seems the MED partition is different than the " << dom_calcul.le_nom() << " domain partition." << finl;
                  Process::exit();
                }
          Cerr << "Ok MED partition matches the domain partition so reading multiple MED files..." << finl;
        }
      // MODIF ELI LAUCOIN (06/03/2012) :
      // j'ajoute l'attribut temps_sauv_
      field_size = creer(nom_fichier_med_,dom_med_,localisation,temps_sauv_);
    }
  if (last_time_only_)
    {
      un_temps=temps_sauv_(temps_sauv_.size_array()-1);
      Cout << "The resumption time is "<<un_temps<<finl;
    }
  // FIN MODIF ELI LAUCOIN (06/03/2012)
  /* si on est en parallele : creation du filtre */
  if (Process::nproc() > 1 && field_size != le_champ().valeurs().dimension(0))
    {
      ArrOfInt dec;
      filter.set_smart_resize(1);
      EFichier fdec;
      fdec.ouvrir(nom_decoup);
      // Cas ou le maillage du fichier .med suit la numerotation du maillage initial (necessite le fichier du decoupage pour retrouver la numerotation)
      if (fdec.good())
        {
          fdec >> dec;
          for (int i = 0; i < dec.size_array(); i++)
            if (dec(i) == Process::me()) filter.append_array(i + 1);//les indices commencent a 1
          if (field_size != dec.size_array())
            Cerr << "Champ_Fonc_MED on parallel domain : inconsistency between 'decoup' file and field!" << finl, Process::exit();
        }
      else // Cas ou le maillage .med suit la numerotation du maillage decoupe
        {
          int nb_item = le_champ().valeurs().dimension(0);
          int first_item = mppartial_sum(nb_item);
          for (int i=0; i<nb_item; i++)
            filter.append_array(first_item + i + 1);//les indices commencent a 1
        }
      filter.set_smart_resize(0), filter.resize(filter.size_array());
      Cerr << "Creating a filter to access efficiently values in " << nom_fichier_med_ << finl;
      if (filter.size_array() != le_champ().valeurs().dimension(0))
        Cerr << "Champ_Fonc_MED on parallel domain : inconsistency between filter and domain!" << finl, Process::exit();
    }
  else if (field_size != le_champ().valeurs().dimension(0))
    Cerr << "Champ_Fonc_MED on existing domain : inconsistency between domain file and field!" << finl, Process::exit();

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
  if (zonebidon_inst.nb_elem()==0) // Cas d'une zone vide
    {
      // Mise a jour:
      Champ_Fonc_base::mettre_a_jour(t);
      le_champ().Champ_Fonc_base::mettre_a_jour(t);
      return;
    }
#ifdef MED_
#ifdef MEDCOUPLING_
  if (use_medcoupling_)
    {
      // Read a field
      std::string  meshName = mon_dom.le_nom().getString();
      std::string fieldName = nom_champ_dans_fichier_med_.getString();
      std::string  fileName = nom_fichier_med_.getString();
      // Etude des conditions pour chercher ou nom un champ dans le fichier MED:
      bool search_field = true;
      nb_dt = temps_sauv_.size_array();
      if (nb_dt>0)
        {
          // nb_dt nombre de pas de temps dans le fichier MED
          // ndt taille du tableau temps_sauv contenant les pas de temps du champ dans le fichier MED
          // tmax dernier temps du tableau temps_sauv ?
          // dt ?
          // t temps courant pour lequel on veut remplir le champ
          // last_time_only_ specifie dans le jeu de donnees ou non
          double tmax = temps_sauv_(nb_dt - 1);
          double dt = temps_sauv_(nb_dt - 1);
          if (((nb_dt == 1) && (!est_egal(dt, t))) || ((last_time_only_ == 1) && (!est_egal(tmax, t))))
            {
              Cout << "We assume that the field " << fieldName.c_str() << " is stationary." << finl;
              search_field = false;
            }
        }
      if (search_field)
        {
          if (temps_sauv_.size_array()==0)
            temps_sauv_ = lire_temps_champ(fileName, fieldName);
          unsigned int nn = temps_sauv_.size_array();
          if (given_it == -1)
            {
              for (given_it = 0; given_it < (int) nn; given_it++)
                if (est_egal(temps_sauv_[given_it], t)) break;
              if (given_it == (int)nn)
                {
                  Cerr << "Error. Time " << t << " not found in the times list of the " << fileName.c_str() << " file" << finl;
                  for (unsigned int n=0; n<nn; n++)
                    Cerr << temps_sauv_[n] << finl;
                  Process::exit();
                }
            }
          int iteration = time_steps_[given_it].first;
          int order = time_steps_[given_it].second;
          Cerr << " at time " << t << " ... " << finl;

          // Only one MCAuto below to avoid double deletion:
          MCAuto<MEDCouplingField> ffield = lire_champ(fileName, meshName, fieldName, iteration, order);
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
        }
    }
  else
#endif
    {
      med_idt fid = MEDfileOpen(nom_fichier_med_, MED_ACC_RDONLY);
      if (fid < 0)
        {
          Cerr << "Problem while opening the file " << nom_fichier_med_ << finl;
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
      int ret, size = 0;
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
              size = MEDfieldnValue(fid, le_nom_du_champ, numdt, numo, type_ent, type_geo);
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
      // Cerr << "Provisoire temps=" << temps_sauv_ << finl;
      // Cerr << "Provisoire temps= " << nb_dt << " " << dt << " " << t << " " << tmax << " " << last_time_only_ << finl;
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
          if (filter.size_array() > 0) //lecture sur domaine parallele -> avec un filtre
            {
              Cerr << "Lecture avec un filtre..." << finl;
              med_filter flt = MED_FILTER_INIT;
              if(MEDfilterEntityCr(fid, size, 1, le_champ().valeurs().nb_dim() > 1 ? le_champ().valeurs().dimension(1) : 1, MED_ALL_CONSTITUENT,
                                   MED_FULL_INTERLACE, MED_COMPACT_STMODE, MED_ALLENTITIES_PROFILE, filter.size_array(), filter.addr(), &flt) < 0)
                Cerr << "Champ_Fonc_MED on parallel domain : error at filter creation!" << finl, Process::exit();
              ret = MEDfieldValueAdvancedRd(fid,le_nom_du_champ,numdt,numo,type_ent,type_geo,&flt,(unsigned char*)le_champ().valeurs().addr());
            }
          else if (le_champ().valeurs().dimension(0) > 0)//lecture complete
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

#ifdef MED_
med_geometry_type type_geo_trio_to_type_med(const Nom& type);
#ifdef MEDCOUPLING_
INTERP_KERNEL::NormalizedCellType type_geo_trio_to_type_medcoupling(const Nom& type, int& mesh_dimension);
#endif
#endif

int Champ_Fonc_MED::creer(const Nom& nom_fic, const Domaine& un_dom, const Motcle& localisation, ArrOfDouble& temps_sauv)
{
#ifdef MED_
  nom_fichier_med_ = nom_fic;
  mon_dom=un_dom;
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
      lire_donnees_champ(fileName,meshName,fieldName,temps_sauv,size,nbcomp,type_champ);
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
      nom_champ_dans_fichier_med_ = medinfo1champ(nom_fic, nom_champ_dans_fichier_med_, numero_ch, nbcomp, nb_dt,
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
  le_champ().fixer_nb_valeurs_nodales(type_champ == "Champ_Fonc_P0_MED" ? un_dom.zone(0).nb_elem() : un_dom.nb_som());
  //pour forcer la lecture lors du mettre a jour
  changer_temps(-1e3);

  //corriger_unite_nom_compo();
  le_champ().nommer(le_nom());
  //le_champ().corriger_unite_nom_compo();
  return size;
#else
  med_non_installe();
  return 0;
#endif
}

#ifdef MEDCOUPLING_
// Remplissage des temps du champ fieldName depuis le fichier fileName
ArrOfDouble Champ_Fonc_MED::lire_temps_champ(const std::string& fileName, const std::string& fieldName)
{
  ArrOfDouble temps_sauv;
  MCAuto<MEDFileFieldMultiTS> ft1(MEDFileFieldMultiTS::New(fileName, fieldName));
  std::vector<double> tps;
  time_steps_ = ft1->getTimeSteps(tps);
  unsigned int nn = tps.size();
  temps_sauv.resize_array(nn);
  for (unsigned it = 0; it < nn; it++)
    temps_sauv[it] = tps[it];
  return temps_sauv;
}

MCAuto<MEDCouplingField> Champ_Fonc_MED::lire_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName, const int iteration, const int order)
{
  // Flag pour lecture plus rapide du field sans lecture du mesh si le maillage MED est deja disponible:
  bool fast = meshName.c_str() == domaine().le_nom() && domaine().getUMesh() != NULL;
  Cerr << "Champ_Fonc_MED::lire() Reading" << (fast ? " (fast)" : "") << " the field " << fieldName.c_str() << " on the " << meshName.c_str() << " mesh in " << fileName.c_str() << finl;
  MCAuto<MEDCouplingField> ffield;
  if (fast)
    {
      // Lecture plus rapide du field sans lecture du mesh associe
      MCAuto<MEDFileField1TS> file = MEDCoupling::MEDFileField1TS::New(fileName, fieldName, iteration, order);
      ffield = file->getFieldOnMeshAtLevel(field_type, domaine().getUMesh(), 0);
    }
  else   // Lecture ~deux fois plus lente du field avec lecture du mesh associe
    ffield = ReadField(field_type, fileName, meshName, 0, fieldName, iteration, order);
  return ffield;
}

// Lecture du dernier champ dans le fichier juste pour decouvrir et stocker:
// les temps (temps_sauv)
// sa taille (size)
// le nombre de composantes (nbcomp)
// le type (type_champ)
void Champ_Fonc_MED::lire_donnees_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName,
                                        ArrOfDouble& temps_sauv, int& size, int& nbcomp, Nom& type_champ)
{
  temps_sauv = lire_temps_champ(fileName, fieldName);
  unsigned int nn = temps_sauv.size_array();
  int last_iter  = time_steps_[nn-1].first;
  int last_order = time_steps_[nn-1].second;

  // Only one MCAuto below to avoid double deletion:
  MCAuto<MEDCouplingField> ffield = lire_champ(fileName, meshName, fieldName, last_iter, last_order);
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
}
#endif

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
// XD Champ_Fonc_MEDfile champ_fonc_med Champ_Fonc_MEDfile 0 Obsolete keyword to read a field with MED file API
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
