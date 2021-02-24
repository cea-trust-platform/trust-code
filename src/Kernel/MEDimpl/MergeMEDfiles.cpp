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
// File:        MergeMEDfiles.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#include <MergeMEDfiles.h>
#include <Nom.h>
#include <glob.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MCAuto.hxx>
#include <MEDLoader.hxx>
#include <MEDFileMesh.hxx>
#include <MEDCouplingFieldDouble.hxx>
// #pragma GCC diagnostic ignored "-Wreorder"
using namespace MEDCoupling;
#endif

Implemente_instanciable(MergeMEDfiles,"Merge_MED",Interprete);
// XD Merge_MED interprete Merge_MED 0 This keyword allows to merge multiple MED files produced during a parallel computation into a single MED file.
// XD attr med_files_base_name chaine(into=["NOM_DU_CAS","med_files_base_name"]) med_files_base_name 0 Base name of multiple med files that should appear as base_name_xxxxx.med, where xxxxx denotes the MPI rank number.
// XD attr time_iterations chaine(into=["all_times","last_time"]) time_iterations 0 Identifies whether to merge all time iterations present in the MED files or only the last one.


// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& MergeMEDfiles::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a: Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& MergeMEDfiles::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Method that glob all med files found in the directory and returns
// a list of strings
std::vector<std::string> glob_med_files(const std::string& fileName)
{
  glob_t glob_result;

  glob(fileName.c_str(),GLOB_TILDE,NULL,&glob_result);

  std::vector<std::string> ls_med;
  for(unsigned int i=0; i<glob_result.gl_pathc; ++i)
    ls_med.push_back(std::string(glob_result.gl_pathv[i]));

  globfree(&glob_result);

  return ls_med;
}

#ifdef MEDCOUPLING_
// This is an internal function that performs the field merging
// and write a single med file
// Two cases are examined depending whether the field is a P0 or a P1 field
void MergeMEDfiles::mergeFields(const std::vector< std::string >& field_names,
                                const std::vector< std::string >& meshes_names,
                                const std::vector<std::string>& listmed,
                                const std::vector< std::pair< std::pair< True_int,True_int>,double >>& lst_dt,
                                const int mesh_numb,
                                const int iter_numb,
                                Nom out_file,
                                bool& first_time, const bool isCell) const
{
  // Wee loop on all field names
  for (const auto & fld_name: field_names)
    {
      int fld_idx=0;
      std::vector<const MEDCouplingFieldDouble *> fields(listmed.size());
      // We loop over all med files
      for (size_t i=0; i<listmed.size(); i++)
        {
          MEDCouplingFieldDouble *f = nullptr;
          if (isCell)
            {
              f = dynamic_cast<MEDCouplingFieldDouble *>
                  (ReadFieldCell(listmed[i],meshes_names[mesh_numb] ,0,fld_name,
                                 lst_dt[iter_numb].first.first,lst_dt[iter_numb].first.second));
            }
          else
            {
              f = dynamic_cast<MEDCouplingFieldDouble *>
                  (ReadFieldNode(listmed[i],meshes_names[mesh_numb] ,0,fld_name,
                                 lst_dt[iter_numb].first.first,lst_dt[iter_numb].first.second));
            }
          if (f == nullptr)
            {
              Cerr << "Unexpected internal error!!" << finl;
              Process::exit(-1);
            }
          fields[fld_idx] = f;
          fld_idx++;
        } // end loop over all med files
      MCAuto<MEDCouplingFieldDouble> merged_field = MEDCouplingFieldDouble::MergeFields(fields);
      merged_field->mergeNodes(Objet_U::precision_geom, 1.0e-12);  // first epsilon for node coincidence, second for different field values if on nodes.

      if (je_suis_maitre())
        {
          if (first_time)
            WriteField(out_file.getString(),merged_field,true);
          else
            WriteField(out_file.getString(),merged_field,false); // append on the existing file
        }
      // the single med file is created. We will append it !
      first_time=0;

      // Memory release:
      for (auto & f: fields)
        f->decrRef();
    } // end loop over all field names
}

/* Used for lexicographical ordering of time steps */
struct less_than_key
{
  inline bool operator() (const std::pair< std::pair< int,int>,double >& a, const std::pair< std::pair< int,int>,double >& b) const
  {
    if (a.first.first == b.first.first)
      return (a.first.second < b.first.second);
    return (a.first.first < b.first.first);
  }
};

// Description:
//   MergeMEDfiles::interpreter(Entree& is)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& MergeMEDfiles::interpreter(Entree& is)
{
  Cerr<<"Syntax MergeMEDfiles::interpreter [NOM_DU_CAS|med_files_base_name] [all_times|last_time]"<<finl;
  Nom med_name,time_opt;
  is >> med_name>> time_opt;

  if (med_name == "NOM_DU_CAS")
    med_name= nom_du_cas();

  Nom files=med_name+"_*.med";

  // call the magic glob function
  std::vector<std::string> listmed=glob_med_files(files.getString());

  if (listmed.size() == 0)
    {
      Cerr << "No multiple med files with a basename " << med_name << " are present in this directory !" << finl;
      Cerr << "Verify your data file or check if the files " << med_name << "_xxxx.med are present in this directory !" << finl;
      Process::exit();
    }
  else
    {
      Cerr << "In process of merging " << (int)listmed.size() << " med files in a single med file ..." << finl;
    }

  const std::vector< std::string > meshes_names = GetMeshNames(listmed[0]);
  // GetAllFieldNames() works very good if all fields are written on a single mesh
  // see later GetAllFieldNamesOnMesh() while dealing with several meshes ...
  const std::vector< std::string > all_field_names = GetAllFieldNames(listmed[0]);

  // we use all_field_names[0] to check the iterations hoping that the med files are written correctly
  // with same number of iterations for all fields ...
  std::vector< std::pair< std::pair< True_int,True_int>,double >> lst_dt=GetAllFieldIterations(listmed[0],all_field_names[0]);
  // Sort the iterations incrementally ...
  std::sort(lst_dt.begin(), lst_dt.end(), less_than_key());

  int nb_steps = lst_dt.size();
  Cerr << "Number of iterations detected : " << nb_steps << finl;
  Cerr << "Number of meshes detected : " <<  (int)meshes_names.size() << finl;

  // TODO : FIXME : use  std::vector<std::vector<std::string>> to avoid multiple calls for MEDCoupling functions ??
  //std::vector< std::string > field_names;
  std::vector< std::string > cell_fields;
  std::vector< std::string > node_fields;

  int count = 0;
  for (size_t i=0; i<meshes_names.size(); i++)
    {
      // field_names = GetAllFieldNamesOnMesh(listmed[0],meshes_names[i]);
      cell_fields = GetCellFieldNamesOnMesh(listmed[0],meshes_names[i]);
      node_fields = GetNodeFieldNamesOnMesh(listmed[0],meshes_names[i]);
      int tot_fld_num = (int)cell_fields.size() + (int)node_fields.size();
      Cerr << "Number of fields detected on mesh " << (int)i +1 << " : " <<  tot_fld_num << finl;
      Cerr << " - Number of P0 fields : " << (int)cell_fields.size() << finl;
      Cerr << " - Number of P1 fields : " << (int)node_fields.size() << finl;
      count = count + tot_fld_num;
    }
  // check if it is ok
  assert(count == (int)all_field_names.size());

  // Output file we are waiting for
  Nom out_file=med_name+".med";
  // test if the single med file is created or no
  bool first_time = 1;

  size_t first_iter = time_opt == "last_time" ? (lst_dt.size()-1) : 0;
  for (size_t iter=first_iter; iter<lst_dt.size(); iter++) // loop over all iterations
    {
      for (size_t msh=0; msh<meshes_names.size(); msh++) // loop over all meshes
        {
          cell_fields = GetCellFieldNamesOnMesh(listmed[0],meshes_names[msh]);
          node_fields = GetNodeFieldNamesOnMesh(listmed[0],meshes_names[msh]);

          mergeFields(cell_fields,meshes_names,listmed,lst_dt,msh,iter,out_file,first_time, true);
          mergeFields(node_fields,meshes_names,listmed,lst_dt,msh,iter,out_file,first_time, false);
        }
    }

  return is;
}

#else /* No MEDCOUPLING_ */

Entree& MergeMEDfiles::interpreter(Entree& is)
{
  Cerr << "Version compiled without MEDCoupling" << finl;
  Process::exit();

  return is;
}

void MergeMEDfiles::mergeFields(const std::vector< std::string >& field_names,
                                const std::vector< std::string >& meshes_names,
                                const std::vector<std::string>& listmed,
                                const std::vector< std::pair< std::pair< True_int,True_int>,double >>& lst_dt,
                                const int mesh_numb,
                                const int iter_numb,
                                Nom out_file,
                                bool& first_time, const bool isCell) const
{
  Cerr << "Version compiled without MEDCoupling" << finl;
  Process::exit();
}

#endif /* ifdef MEDCOUPLING_ */
