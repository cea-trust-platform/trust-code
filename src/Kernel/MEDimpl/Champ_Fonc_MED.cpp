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

#include <Champ_Fonc_MED.h>
#include <LireMED.h>
#include <Char_ptr.h>
#include <EFichier.h>
#include <EChaine.h>
#include <medcoupling++.h>
#include <TRUST_2_MED.h>
#include <Comm_Group_MPI.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDCouplingField.hxx>
#include <MEDCouplingFieldDouble.hxx>
#ifdef MPI_
#include <ParaMEDFileMesh.hxx>
#endif
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileField.hxx>
#include <communications.h>

using MEDCoupling::MEDCouplingField;
using MEDCoupling::MEDCouplingFieldDouble;
using MEDCoupling::MCAuto;
using MEDCoupling::DataArrayIdType;
using MEDCoupling::GetTimeAttachedOnFieldIteration;
using MEDCoupling::GetAllFieldNamesOnMesh;
using MEDCoupling::MEDFileFieldMultiTS;
using MEDCoupling::MEDFileField1TS;
using MEDCoupling::MEDFileMesh;
#endif

// XD champ_fonc_med field_base champ_fonc_med 1 Field to read a data field in a MED-format file .med at a specified time. It is very useful, for example, to resume a calculation with a new or refined geometry. The field post-processed on the new geometry at med format is used as initial condition for the resume.
Implemente_instanciable(Champ_Fonc_MED,"Champ_Fonc_MED",Champ_Fonc_base);

Sortie& Champ_Fonc_MED::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

void Champ_Fonc_MED::set_param(Param& param)
{
  param.ajouter_flag("use_existing_domain", &use_existing_domain_); // XD_ADD_P flag whether to optimize the field loading by indicating that the field is supported by the same mesh that was initially loaded as the domain
  param.ajouter_flag("last_time", &last_time_only_);                // XD_ADD_P flag to use the last time of the MED file instead of the specified time. Mutually exclusive with 'time' parameter.
  param.ajouter("decoup", &nom_decoup_, Param::OPTIONAL);           // XD_ADD_P chaine specify a partition file.
  param.ajouter("mesh", &nom_maillage_, Param::OPTIONAL);           // XD_ADD_P chaine Name of the mesh supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create the TRUST domain, loading can be optimized with option 'use_existing_domain'.
  param.ajouter("domain", &nom_dom_, Param::REQUIRED);              // XD_ADD_P chaine Name of the domain supporting the field. This is the name of the mesh in the MED file, and if this mesh was also used to create the TRUST domain, loading can be optimized with option 'use_existing_domain'.
  param.ajouter("file", &nom_fichier_med_, Param::REQUIRED);        // XD_ADD_P chaine Name of the .med file.
  param.ajouter("field", &nom_champ_, Param::REQUIRED);             // XD_ADD_P chaine Name of field to load.
  param.ajouter("loc", &loc_, Param::OPTIONAL);                     // XD_ADD_P chaine(into=["som","elem"]) To indicate where the field is localised. Default to 'elem'.
  param.ajouter("time", &temps_, Param::OPTIONAL);                  // XD_ADD_P double Timestep to load from the MED file. Mutually exclusive with 'last_time' flag.
}

Entree& Champ_Fonc_MED::readOn(Entree& is)
{
#ifdef MEDCOUPLING_
  Nom chaine_lue;
  bool nom_decoup_lu = false;
  nom_decoup_ = "";

  is>>chaine_lue;
  if (chaine_lue == "{") // New syntax !!
    {
      Nom s = "{ ";
      int cnt = 1;
      while (cnt)
        {
          Nom motlu;
          is >> motlu;
          if (motlu == "{") cnt++;
          if (motlu == "}") cnt --;
          s += Nom(" ") + motlu;
        }
      Param param(que_suis_je());
      set_param(param);

      EChaine is2(s);
      loc_ = "";
      constexpr double MIN_INF = -1.0e+300;
      temps_ = MIN_INF;
      param.lire_avec_accolades(is2);
      // Localisation is elem by default
      if (loc_ == "") loc_ = "elem";
      if (nom_decoup_ != "") nom_decoup_lu = true;
      // At least one time needed!
      if (!last_time_only_ && temps_ == MIN_INF) // no time was provided
        {
          Cerr << "ERROR: reading 'Champ_fonc_MED': no time was provided! Either 'last_time' or 'time' parameter must be specified!" << finl;
          Process::exit(-1);
        }
      if (last_time_only_ && temps_ != MIN_INF) // too many time options
        {
          Cerr << "ERROR: reading 'Champ_fonc_MED': both 'time' and 'last_time' were provided! Choose only one!" << finl;
          Process::exit(-1);
        }

    }
  else  // Old syntax?
    {
      Cerr << "ERROR: reading 'Champ_fonc_MED': Expected opening brace '{' - are you using the new syntax?" << finl;
      Cerr << "If you are still using the old syntax (before TRUST v1.9.3), \nyou can use -convert_data option of your application script:" << finl;
      Cerr << "   trust -convert_data " << Objet_U::nom_du_cas() << ".data" << finl;
      Process::exit(-1);
    }

  //
  // Finished interpreting ... processing now:
  //
  if (!nom_decoup_lu)
    nom_decoup_ = Nom("decoup/") + nom_dom_ + ".txt"; //valeur par defaut de nom_decoup

  traite_nom_fichier_med(nom_fichier_med_);
  // La lecture de fichiers multiples .med fonctionne: voir cas test Champ_fonc_MED_Parallele
  // Un test est fait plus loin pour bien verifier que les partitionnement se recouvrent pour cela
  int multiple_med = 0;
  Nom tmp(nom_fichier_med_);
  tmp.prefix("0001.med");
  if (tmp!=nom_fichier_med_ && Process::is_parallel())
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
  nommer(nom_champ_);

  int field_size;
  bool domain_exist=( interprete().objet_existant(nom_dom_) && sub_type(Domaine, interprete().objet(nom_dom_)) );
  if (use_existing_domain_)
    {
      if (!domain_exist)
        {
          Cerr << "The domain " << nom_dom_ << " does not exist !!!! In Champ_Fonc_MED " << nom_fichier_med_ << " " << nom_dom_
               << " " << nom_champ_ << " " << loc_ << " " << temps_ << finl;
          exit();
        }
      else
        {
          // use_existing_domain utilisable en parallele uniquement si le process 0 gere tout le domaine ou si decoup specifie:
          const Domaine& le_domaine=ref_cast(Domaine, interprete().objet(nom_dom_));
          if (Process::is_parallel() && mp_max((int)(le_domaine.nb_som()>0)) != 0 && !nom_decoup_lu)
            {
              Cerr << "Warning, you can't use use_existing_domain on a partitionned domain like " << nom_dom_ << finl;
              Cerr << "It is not parallelized yet... So we use MED mesh, which is not optimal." << finl;
              Cerr << "Try suppress use_existing_domain option." << finl;
              //Process::exit();
              use_existing_domain_ = 0;
            }
        }
    }
  if (domain_exist && use_existing_domain_)
    {
      Cerr<<nom_dom_<<" was not read into the med file because it already exists." <<finl;

      const Domaine& le_domaine=ref_cast(Domaine, interprete().objet(nom_dom_));

#ifndef NDEBUG
      // In debug, check that we really have the same domain.
      // Only doable in sequential, since in //, dom=full_domain, un_dom=local_domain
      if (nproc()==1)
        {
          Cerr << "Checking whether domain in the file "<<nom_fichier_med_<<" and domain "<<nom_dom_<<" are the same (coords,connectivity) ..."<<finl;
          LireMED liremed(nom_fichier_med_, nom_maillage_ == "??" ? nom_dom_ : nom_maillage_);
          dom_med_.nommer(nom_dom_);
          liremed.associer_domaine(dom_med_);
          liremed.retrieve_MC_objects();
          const MEDCouplingUMesh* new_um = liremed.get_mc_mesh();
          const MEDCouplingUMesh* root_um = le_domaine.get_mc_mesh();
          try
            {
              DataArrayIdType *dnup1=nullptr, *dnup2=nullptr;
              // Less strict checkGeoEquivalWith (levOfCheck = 12 instead of 2). we use now "nodal" comparison.
              // Two cells are considered equal if they are based on same nodes and have the same type.
              // This is the weakest policy, it can be used by users not sensitive to cell orientation.
              // if levOfCheck set to 2, some F5 and G3 tests fail

              root_um->checkGeoEquivalWith(new_um, /* levOfCheck=  */ 12, Objet_U::precision_geom, dnup1, dnup2);
              //MCAuto<DataArrayIdType> dnu1(dnup1), dnu2(dnup2);
            }
          catch(INTERP_KERNEL::Exception& e)
            {
              Cerr << "Comparison of the two domains failed, with the following message from MEDCoupling::checkGeoEquivalWith()" << finl;
              Cerr << e.what() << finl;
              Process::exit();
            }
          dom_med_=Domaine();  // Reset
        }
#endif
      field_size = creer(nom_fichier_med_,le_domaine,loc_,temps_sauv_);
    }
  else
    {
      if (domain_exist && !use_existing_domain_)
        Cerr<<"INFO: You can toggle the flag 'use_existing_domain' in 'Champ_Fonc_MED' to optimize reading since it seems that the domain already exists."<<finl;
      LireMED liremed(nom_fichier_med_, nom_maillage_ == "??" ? nom_dom_ : nom_maillage_);
      dom_med_.nommer(nom_dom_);
      //Nom nom_dom_trio_non_nomme;
      // Remplit dom:
      liremed.associer_domaine(dom_med_);
      liremed.lire_geom(false); // false: do not create sub-dom files
      if (multiple_med)
        {
          // On verifie que l'on a bien des recouvrements identiques (verification imparfaite sur les BoundingBox)
          DoubleTab BB1 = dom_med_.getBoundingBox();
          const Domaine& dom_calcul = ref_cast(Domaine, interprete().objet(nom_dom_));
          DoubleTab BB2 = dom_calcul.getBoundingBox();
          for (int i=0; i<dimension; i++)
            for (int j=0; j<2; j++)
              if (est_different(BB1(i,j), BB2(i,j)))
                {
                  Cerr << "Error, Champ_Fonc_MED can't read the partitionned MED files." << finl;
                  Cerr << "=> it seems that the MED partition is different than the '" << dom_calcul.le_nom() << "' domain partition." << finl;
                  Process::exit();
                }
          Cerr << "Ok MED partition matches the domain partition so reading multiple MED files..." << finl;
        }
      // MODIF ELI LAUCOIN (06/03/2012) :
      // j'ajoute l'attribut temps_sauv_
      field_size = creer(nom_fichier_med_,dom_med_,loc_,temps_sauv_);
    }
  if (last_time_only_)
    {
      temps_=temps_sauv_[temps_sauv_.size_array()-1];
      Cout << "The resumption time is "<<temps_<<finl;
    }
  // FIN MODIF ELI LAUCOIN (06/03/2012)
  /* si on est en parallele : creation du filtre */
  if (Process::is_parallel() && field_size < 0)
    {
      EFichier fdec;
      fdec.ouvrir(nom_decoup_);
      // Cas ou le maillage du fichier .med suit la numerotation du maillage initial (necessite le fichier du decoupage pour retrouver la numerotation)
      if (fdec.good())
        {
          int dec_size=-1;
          if(loc_ == "elem")
            {
              ArrOfInt dec;
              fdec >> dec;
              dec_size = dec.size_array();
              for (int i = 0; i < dec_size; i++)
                if (dec[i] == Process::me()) filter.push_back(i);
            }
          else if(loc_ == "som")
            {
              int nbNodes;
              fdec >> nbNodes;
              std::vector<std::set<int>> dec(nbNodes);
              for(int n=0; n<nbNodes; n++)
                {
                  int node,size;
                  fdec >> node >> size;
                  for(int p=0; p<size; p++)
                    {
                      int proc;
                      fdec >> proc;
                      dec[node].insert(proc);
                    }
                }
              dec_size = (int)dec.size();
              for (int n=0; n < dec_size; n++)
                for(std::set<int>::iterator it = dec[n].begin(); it!=dec[n].end(); ++it)
                  if (*it == Process::me()) filter.push_back(n);
            }
          else
            {
              Cerr << "Champ_Fonc_MED on parallel domain : decoup file only handled for field located on nodes or on cells!" << finl;
              Process::exit();
            }
        }
      else // Cas ou le maillage .med suit la numerotation du maillage decoupe
        {
          int nb_item = le_champ().valeurs().dimension(0);
          int first_item = mppartial_sum(nb_item);
          for (int i=0; i<nb_item; i++)
            filter.push_back(first_item);
        }
      Cerr << "Creating a filter to access efficiently values in " << nom_fichier_med_ << finl;
      if ((int) filter.size() != le_champ().valeurs().dimension(0))
        {
          Cerr << "Champ_Fonc_MED on parallel domain : inconsistency between filter and domain (not the same number of entities)!" << finl;
          Process::exit();
        }
    }
  else if (field_size != le_champ().valeurs().dimension(0))
    {
      Cerr << "Champ_Fonc_MED on existing domain : inconsistency between domain file and field (not the same number of entities)!" << finl;
      Process::exit();
    }
  le_champ().nommer(nom_champ_);
  le_champ().corriger_unite_nom_compo();
  mettre_a_jour(temps_);
#endif

  return is ;
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

#ifdef MED_
med_geometry_type type_geo_trio_to_type_med(const Nom& type);
#ifdef MEDCOUPLING_
INTERP_KERNEL::NormalizedCellType type_geo_trio_to_type_medcoupling(const Nom& type, int& mesh_dimension);
#endif
#endif

void Champ_Fonc_MED::lire(double t, int given_it)
{
  if (domainebidon_inst.nb_elem()==0) // Cas d'un domaine vide
    {
      // Mise a jour:
      Champ_Fonc_base::mettre_a_jour(t);
      le_champ().Champ_Fonc_base::mettre_a_jour(t);
      return;
    }
#ifdef MED_
#ifdef MEDCOUPLING_
  // Read a field
  std::string  meshName = mon_dom->le_nom().getString();
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
      double tmax = temps_sauv_[nb_dt - 1];
      double dt = temps_sauv_[nb_dt - 1];
      if (((nb_dt == 1) && (!est_egal(dt, t))) || ((last_time_only_ == 1) && (!est_egal(tmax, t))))
        {
          Cout << "We assume that the field " << fieldName << " is stationary." << finl;
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
              Cerr << "Error. Time " << t << " not found in the times list of the " << fileName << " file" << finl;
              for (unsigned int n=0; n<nn; n++)
                Cerr << temps_sauv_[n] << finl;
              Process::exit();
            }
        }
      int iteration = time_steps_[given_it].first;
      int order = time_steps_[given_it].second;

      if(filter.size()) // Partial reading of field (only field values are loaded, not its structure!)
        {
#ifdef MPI_
          MEDFileUtilities::AutoFid fid(MEDCoupling::OpenMEDFileForRead(fileName));
          int mesh_dimension = -1;
          std::vector<std::pair<MEDCoupling::TypeOfField,INTERP_KERNEL::NormalizedCellType>> tmp= { { field_type, type_geo_trio_to_type_medcoupling(mon_dom->type_elem()->que_suis_je(), mesh_dimension) } };
          INTERP_KERNEL::AutoCppPtr<MEDCoupling::MEDFileEntities> entities(MEDCoupling::MEDFileEntities::BuildFrom(&tmp));
          MCAuto<MEDCoupling::MEDFileAnyTypeField1TS> fieldFile = MEDCoupling::MEDFileAnyTypeField1TS::NewAdv(fid, fieldName, iteration, order, entities, filter);
          MCAuto<MEDCoupling::MEDFileField1TS> ret(MEDCoupling::DynamicCast<MEDCoupling::MEDFileAnyTypeField1TS, MEDCoupling::MEDFileField1TS>(fieldFile));
          if (!ret)
            Process::exit("MED field: wrong type!");
          MEDCoupling::DataArrayDouble *field_values = ret->getUndergroundDataArray();
          std::copy(field_values->begin(),field_values->end(),
                    le_champ().valeurs().addr());
#endif
        }
      else
        {
          // Only one MCAuto below to avoid double deletion:
          MCAuto<MEDCouplingField> ffield = lire_champ(fileName, meshName, fieldName, iteration, order);
          Cerr << " at time " << t << " ... " << finl;
          MEDCouplingFieldDouble * field = dynamic_cast<MEDCouplingFieldDouble *>((MEDCouplingField *)ffield);
          if (field == 0)
            {
              Cerr << "ERROR reading MED field! Not a MEDCouplingFieldDouble!!" << finl;
              Process::exit(-1);
            }
          assert(field->getNumberOfTuplesExpected() == le_champ().valeurs().dimension(0));
          assert((int) field->getNumberOfComponents() ==
                 (le_champ().valeurs().nb_dim() == 1 ? 1 : le_champ().valeurs().dimension(1)));
          MEDCoupling::DataArrayDouble *field_values = field->getArray();
          std::copy(field_values->begin(),field_values->end(),
                    le_champ().valeurs().addr());
        }
    }
#endif  // MEDCOUPLING_
  // Mise a jour:
  Champ_Fonc_base::mettre_a_jour(t);
  le_champ().Champ_Fonc_base::mettre_a_jour(t);
#else   // MED_
  med_non_installe();
#endif
}

int Champ_Fonc_MED::creer(const Nom& nom_fic, const Domaine& un_dom, const Motcle& localisation, ArrOfDouble& temps_sauv)
{
#ifdef MED_
  nom_fichier_med_ = nom_fic;
  mon_dom=un_dom;
  int nbcomp,size;
  const Nom& type_elem=un_dom.type_elem()->que_suis_je();
  Nom type_champ;
  nom_champ_dans_fichier_med_ = le_nom();
#ifdef MEDCOUPLING_
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
  std::string meshName = nom_maillage_ == "??" ? mon_dom->le_nom().getString() : nom_maillage_.getString();
  std::string fileName = nom_fic.getString();
  // Try to guess the field name in the MED file:
  Noms fieldNamesGuess;
  fieldNamesGuess.add((Motcle)nom_champ_dans_fichier_med_+"_"+mon_dom->le_nom());
  if (localisation != Nom())
    fieldNamesGuess.add((Motcle)nom_champ_dans_fichier_med_+"_"+localisation+"_"+mon_dom->le_nom());
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
      Cerr << "Unable to find into file '" << fileName << "' a field named like :" << finl;
      Cerr << "  " << fieldNamesGuess << finl;
      Cerr << "supported by mesh '" << meshName << "'" << finl;
      Cerr << finl;
      Cerr << "This file contains the field(s) named:" << finl;
      for (unsigned i=0; i<fieldNames.size(); i++)
        Cerr << fieldNames[i] << finl;
      Process::exit();
    }
  else
    Cerr << "Ok, we find into file " << fileName << " a field named " << nom_champ_dans_fichier_med_ << finl;

  std::string fieldName = nom_champ_dans_fichier_med_.getString();
  lire_donnees_champ(fileName,meshName,fieldName,temps_sauv,size,nbcomp,type_champ);
#endif  // MEDCOUPLING_
  // Definition:
  vrai_champ_.typer(type_champ);
  fixer_nb_comp(nbcomp);
  le_champ().fixer_nb_comp(nbcomp);
  domainebidon_inst.associer_domaine(un_dom);
  le_champ().associer_domaine_dis_base(domainebidon_inst);
  le_champ().fixer_nb_valeurs_nodales(type_champ == "Champ_Fonc_P0_MED" ? un_dom.nb_elem() : un_dom.nb_som());
  //pour forcer la lecture lors du mettre a jour
  changer_temps(-1e3);

  //corriger_unite_nom_compo();
  le_champ().nommer(le_nom());
  //le_champ().corriger_unite_nom_compo();
  return size;
#else    // MED_
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
  unsigned int nn = (unsigned)tps.size();
  temps_sauv.resize_array(nn);
  for (unsigned it = 0; it < nn; it++)
    temps_sauv[it] = tps[it];
  return temps_sauv;
}

MCAuto<MEDCouplingField> Champ_Fonc_MED::lire_champ(const std::string& fileName, const std::string& meshName,
                                                    const std::string& fieldName, const int iteration, const int order)
{
  // Flag pour lecture plus rapide du field sans lecture du mesh si le maillage MED est deja disponible:
  bool fast = meshName == domaine().le_nom() && domaine().is_mc_mesh_ready();
  Cerr << "Reading" << (fast ? " (fast)" : "") << " the field " << fieldName << " on the " << meshName << " mesh into " << fileName << " file";
  MCAuto<MEDCouplingField> ffield;
  Cerr << "meshName " << meshName << " " << domaine().le_nom()  << " " << (int)(domaine().get_mc_mesh() != nullptr) << finl;

  if (fast) // Lecture plus rapide du field sans lecture du mesh associe
    {
      MCAuto<MEDFileField1TS> file = MEDFileField1TS::New(fileName, fieldName, iteration, order);
      ffield = file->getFieldOnMeshAtLevel(field_type, domaine().get_mc_mesh(), 0);
    }
  else   // Lecture ~deux fois plus lente du field avec lecture du mesh associe
    {
      ffield = ReadField(field_type, fileName, meshName, 0, fieldName, iteration, order);
    }

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
  MCAuto<MEDFileField1TS> field = MEDFileField1TS::New(fileName, fieldName, last_iter, last_order);
  nbcomp = (int) field->getNumberOfComponents();
  if (is_parallel() && use_existing_domain_)
    size = -1;
  else
    {
      MCAuto<MEDFileMesh> mesh = MEDFileMesh::New(fileName, meshName, last_iter, last_order);
      size = field_type == MEDCoupling::ON_NODES ? mesh->getNumberOfNodes() : mesh->getNumberOfCellsAtLevel(0);
    }

  if (field_type == MEDCoupling::ON_CELLS)
    type_champ = "Champ_Fonc_P0_MED";
  else if (field_type == MEDCoupling::ON_NODES)
    type_champ = cell_type == INTERP_KERNEL::NORM_QUAD4 || cell_type == INTERP_KERNEL::NORM_HEXA8 ? "Champ_Fonc_Q1_MED" : "Champ_Fonc_P1_MED";
}
#endif // MEDCOUPLING_

const Domaine_dis_base& Champ_Fonc_MED::domaine_dis_base() const
{
  return domainebidon_inst;
}

const ArrOfDouble& Champ_Fonc_MED::get_saved_times(void) const
{
  return temps_sauv_;
}
