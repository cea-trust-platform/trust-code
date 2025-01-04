/****************************************************************************
* Copyright (c) 2025, CEA
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
#include <Decouper.h>
#include <Domaine.h>
#include <DomaineCutter.h>
#include <Format_Post_Lata.h>
#include <EChaine.h>
#include <SFichier.h>
#include <Param.h>
#include <communications.h>
#include <Synonyme_info.h>
#include <vector>
#include <set>

Implemente_instanciable_32_64(Decouper_32_64,"Decouper",Interprete);
Add_synonym(Decouper, "Partition");
Add_synonym(Decouper_64, "Partition_64");

template <typename _SIZE_>
Sortie& Decouper_32_64<_SIZE_>::printOn(Sortie& os) const
{
  Process::exit();
  return os;
}

template <typename _SIZE_>
Entree& Decouper_32_64<_SIZE_>::readOn(Entree& is)
{
  Process::exit();
  return is;
}

/*! @brief lecture du partitionneur dans le jeu de donnees (lecture du type et des parametres et initialisation de d_part)
 */
template <typename _SIZE_>
void Decouper_32_64<_SIZE_>::lire_partitionneur(Entree& is)
{
  Nom n;
  is >> n;
  Nom type_partitionneur("Partitionneur_");
  type_partitionneur += n;
  Cerr << " Creation of a partitioner of type: " << type_partitionneur << finl;
  deriv_partitionneur_.typer(type_partitionneur);
  // En cas d'echec (si le nom du type est invalide) : exit
  if (! deriv_partitionneur_.non_nul())
    Process::exit();

  // Initialisation des parametres du partitionneur
  Partitionneur_base_32_64<_SIZE_>& part = deriv_partitionneur_.valeur();
  part.associer_domaine(this->domaine());
  is >> part;
}

template <typename _SIZE_>
void Decouper_32_64<_SIZE_>::ecrire_fichier_decoupage() const
{
  Cerr << "Writing of the splitting array at the format IntVect ascii\n"
       << " in the file " << nom_fichier_decoupage_
       << "\n(for each element, number of the destination processor)" << finl;
  SFichier file; // Fichier ascii
  if (! file.ouvrir(nom_fichier_decoupage_))
    {
      Cerr << " Error in the opening of the file." << finl;
      Process::exit();
    }
  file << elem_part_;
  file << nb_parts_tot_;
}

template <typename _SIZE_>
void Decouper_32_64<_SIZE_>::ecrire_fichier_decoupage_som() const
{
  Cerr << "Writing of the splitting array at the format IntVect ascii\n"
       << " in the file " << nom_fichier_decoupage_sommets_
       << "\n(for each node, list of the destination processors)" << finl;
  SFichier file; // Fichier ascii
  if (! file.ouvrir(nom_fichier_decoupage_sommets_))
    Process::exit(" Error in the opening of the file.");

  const Domaine_t& domaine = this->domaine();
  const IntTab_t& elems = domaine.les_elems();
  int nbOfNodesPerElems = elems.dimension_int(1);
  int_t nbSom = domaine.nb_som();
  std::vector<std::set<int_t>> node_part(nbSom);
  for(int i=0; i<elem_part_.size(); i++)
    {
      for (int j = 0; j < nbOfNodesPerElems; j++)
        {
          int_t node = elems(i, j);
          node_part[node].insert(elem_part_[i]);
        }
    }
  file << domaine.nb_som() << finl;
  for(int_t i=0; i<nbSom; i++)
    {
      file << i << " " << node_part[i].size() << " ";
      for(const auto& set_elem: node_part[i])
        file << set_elem << " ";
      file << finl;
    }
}

template <typename _SIZE_>
void Decouper_32_64<_SIZE_>::postraiter_decoupage(const Nom& nom_fichier) const
{
  if (!std::is_same<_SIZE_,int>::value)
    Process::exit("Postprocessing of partitioning is not yet implemented for big (64b) domains!");

  const Domaine& domaine = reinterpret_cast<const Domaine&>(this->domaine());
  // Check and strip ".lata" :
  Nom basename;
  if (nom_fichier.finit_par(".lata"))
    {
      basename = nom_fichier;
      basename.prefix(".lata");
    }
  else if (nom_fichier.finit_par(".med"))
    {
      basename = nom_fichier;
      basename.prefix(".med");
    }
  else
    Process::exit("Decouper: postraiter_decoupage(): the file name for postprocessing the domain should end with extension '.lata' or '.med' !!");

  // Compute dummy field indicating partitioning:
  if (!std::is_same<_SIZE_,int>::value)
    {
      // FOR DEVELOPPERS : remove cast to int below if you are porting this for 64b!
      Cerr << "did you remove cast?" << finl;
      Process::exit(-1);
    }
  const int n = (int)elem_part_.size_reelle();

  DoubleTab data(n);
  for (int i = 0; i < n; i++)
    data(i) = static_cast<double>(elem_part_[i]);

  Noms units, noms_compo;
  units.add("");
  noms_compo.add("I");
  constexpr int IS_FIRST = 1;

  if (nom_fichier.finit_par(".lata"))
    {
      Cerr << "Postprocessing of the splitting at the lata (V2) format: " << nom_fichier << finl;
      Format_Post_Lata post;

      post.initialize_lata(basename, Format_Post_Lata::BINAIRE, Format_Post_Lata::SINGLE_FILE);
      post.ecrire_entete(0.0, 0, IS_FIRST);
      post.ecrire_domaine(domaine, IS_FIRST);
      post.ecrire_temps(0.0);
      post.ecrire_champ(domaine,
                        units,
                        noms_compo,
                        1,           // ncomp,
                        0.0,         //temps,
                        "partition", // id_du_champ,
                        domaine.le_nom(), // id_du_domaine
                        "ELEM",      // localisation,
                        "scalar",    //nature,
                        data         // valeurs
                       );
      post.finir(1);
    }
  else if (nom_fichier.finit_par(".med"))
    {
      Cerr << "Postprocessing of the splitting at the MED format: " << nom_fichier << finl;
      OWN_PTR(Format_Post_base) post;
      post.typer("Format_Post_Med");
      Nom filename(nom_fichier.getPrefix(".med"));
      post->initialize(filename, 1, "SIMPLE");
      post->ecrire_entete(0.0, 0, IS_FIRST);
      post->ecrire_domaine(domaine, IS_FIRST);
      post->ecrire_temps(0.0);
      post->ecrire_champ(domaine,
                         units,
                         noms_compo,
                         -1,           // ncomp,
                         0.0,         //temps,
                         "partition", // id_du_champ,
                         "domain", // id_du_domaine
                         "ELEM",      // localisation,
                         "scalar",    //nature,
                         data         // valeurs
                        );
      post->finir(1);
    }
}

template <typename _SIZE_>
void Decouper_32_64<_SIZE_>::ecrire_sous_domaines(const int nb_parties, const Static_Int_Lists_t* som_raccord) const
{
  DomaineCutter_32_64<_SIZE_> cutter;
  cutter.initialiser(this->domaine(), elem_part_, nb_parties, epaisseur_joint_, liste_bords_periodiques_);
  // Reflexion provisoire:
  // Les joints sont construits dans ecrire_domaines -> construire_sous_domaine
  // Donc apres renumerotation des PEs et donc de elem_part_, il faudrait
  // reinitialiser ? Ou bien, tout renumeroter apres le calcul des joints...
  // Cela parait mieux...
  cutter.ecrire_domaines(nom_domaines_decoup_, format_, reorder_, som_raccord);
}

template<> int Decouper_32_64<int>::print_more_infos_ = 0;
#if INT_is_64_ == 2
template<> int Decouper_32_64<trustIdType>::print_more_infos_ = 0;
#endif

// XD partition interprete decouper -1 Class for parallel calculation to cut a domain for each processor. By default, this keyword is commented in the reference test cases.
// XD attr domaine ref_domaine domaine 0 Name of the domain to be cut.
// XD attr bloc_decouper bloc_decouper bloc_decouper 0 Description how to cut a domain.
// XD bloc_decouper objet_lecture nul 1 Auxiliary class to cut a domain.
// XD attr partitionneur|Partition_tool partitionneur_deriv partitionneur 1 Defines the partitionning algorithm (the effective C++ object used is \'Partitionneur_ALGORITHM_NAME\').
// XD attr larg_joint entier larg_joint 1 This keyword specifies the thickness of the virtual ghost domaine (data known by one processor though not owned by it). The default value is 1 and is generally correct for all algorithms except the QUICK convection scheme that require a thickness of 2. Since the 1.5.5 version, the VEF discretization imply also a thickness of 2 (except VEF P0). Any non-zero positive value can be used, but the amount of data to store and exchange between processors grows quickly with the thickness.
// XD attr nom_zones chaine zones_name 1 Name of the files containing the different partition of the domain. The files will be : NL2 name_0001.Zones NL2 name_0002.Zones NL2 ... NL2 name_000n.Zones. If this keyword is not specified, the geometry is not written on disk (you might just want to generate a \'ecrire_decoupage\' or \'ecrire_lata\').
// XD attr ecrire_decoupage chaine ecrire_decoupage 1 After having called the partitionning algorithm, the resulting partition is written on disk in the specified filename. See also partitionneur Fichier_Decoupage. This keyword is useful to change the partition numbers: first, you write the partition into a file with the option ecrire_decoupage. This file contains the domaine number for each element\'s mesh. Then you can easily permute domaine numbers in this file. Then read the new partition to create the .Zones files with the Fichier_Decoupage keyword.
// XD attr ecrire_lata chaine ecrire_lata 1 Save the partition field in a LATA format file for visualization
// XD attr ecrire_med chaine ecrire_med 1 Save the partition field in a MED format file for visualization
// XD attr nb_parts_tot_ entier nb_parts_tot_ 1 Keyword to generates N .Domaine files, instead of the default number M obtained after the partitionning algorithm. N must be greater or equal to M. This option might be used to perform coupled parallel computations. Supplemental empty domaines from M to N-1 are created. This keyword is used when you want to run a parallel calculation on several domains with for example, 2 processors on a first domain and 10 on the second domain because the first domain is very small compare to second one. You will write Nb_parts 2 and Nb_parts_tot 10 for the first domain and Nb_parts 10 for the second domain.
// XD attr periodique listchaine periodique 1 N BOUNDARY_NAME_1 BOUNDARY_NAME_2 ... : N is the number of boundary names given. Periodic boundaries must be declared by this method. The partitionning algorithm will ensure that facing nodes and faces in the periodic boundaries are located on the same processor.
// XD attr reorder entier reorder 1 If this option is set to 1 (0 by default), the partition is renumbered in order that the processes which communicate the most are nearer on the network. This may slighlty improves parallel performance.
// XD attr single_hdf rien single_hdf 1 Optional keyword to enable you to write the partitioned domaines in a single file in hdf5 format.
// XD attr print_more_infos entier print_more_infos 1 If this option is set to 1 (0 by default), print infos about number of remote elements (ghosts) and additional infos about the quality of partitionning. Warning, it slows down the cutting operations.
template <typename _SIZE_>
Entree& Decouper_32_64<_SIZE_>::interpreter(Entree& is)
{
  Cerr << "Decouper: Splitting of a domain" << finl;
  // Reading parameters
  lire(is);

  // Generating partition
  Partitionneur_base_t& partitionneur = deriv_partitionneur_.valeur();
  partitionneur.declarer_bords_periodiques(liste_bords_periodiques_);
  partitionneur.construire_partition(elem_part_,nb_parts_tot_);

  // Writing out various files (including .zones)
  ecrire();

  Cerr << "End of the interpreter Decouper" << finl;
  return is;
}

template <typename _SIZE_>
Entree& Decouper_32_64<_SIZE_>::lire(Entree& is)
{
  // Lecture du nom du domaine a decouper:
  is >> nom_domaine_;
  Cerr << " Domain name to split : " << nom_domaine_ << finl;
  this->associer_domaine(nom_domaine_);
  // Avant de decouper on imprime des infos
  this->domaine().imprimer();

  Param param(this->que_suis_je());
  int hdf;
  param.ajouter_non_std("partitionneur|partition_tool",(this),Param::REQUIRED);
  param.ajouter("larg_joint",&epaisseur_joint_);
  param.ajouter_condition("value_of_larg_joint_ge_1","The joint thickness must greater or equal to 1.");
  param.ajouter("nom_zones|zones_name",&nom_domaines_decoup_);
  param.ajouter("ecrire_decoupage",&nom_fichier_decoupage_);
  param.ajouter("ecrire_decoupage_sommets",&nom_fichier_decoupage_sommets_);
  param.ajouter("ecrire_lata",&nom_fichier_lata_);
  param.ajouter("ecrire_med",&nom_fichier_med_);
  param.ajouter("nb_parts_tot",&nb_parts_tot_);
  param.ajouter("reorder",&reorder_);
  param.ajouter_flag("single_hdf",&hdf);
  param.ajouter("periodique",&liste_bords_periodiques_);
  param.ajouter("print_more_infos",&print_more_infos_);
  param.lire_avec_accolades_depuis(is);

  if (hdf) format_ = DomainesFileOutputType::HDF5_SINGLE;

  return is;
}

template <typename _SIZE_>
void Decouper_32_64<_SIZE_>::ecrire(const Static_Int_Lists_t* som_raccord)
{
  // Calcul du nombre de parties generees par le partitionneur
  int nb_parties = 0;
  if (elem_part_.size_array() > 0)
    nb_parties = static_cast<int>(max_array(elem_part_)) + 1;  // cast to int, because max among number of procs (or numb of parts)
  nb_parties = Process::mp_max(nb_parties);
  Cerr << "The partitioner has generated " << nb_parties << " parts." << finl;

  // Prise en compte de la directive nb_parts_tot_
  if (nb_parts_tot_ >= 0)
    {
      if (nb_parties > nb_parts_tot_)
        {
          Cerr << "Error: nb_parts_tot_ is less than the number of parts generated by the partitioner."
               << finl;
          Process::exit();
        }
      Cerr << "Number of parts requested : " << nb_parts_tot_ << finl
           << "Generation of " << nb_parts_tot_ - nb_parties << " empty parts." << finl;
      nb_parties = nb_parts_tot_;
    }
  // Force un seul fichier .Zones au dela d'un certain nombre de rangs MPI:
  if (Process::force_single_file(nb_parties, nom_domaines_decoup_+".Zones"))
    format_ = DomainesFileOutputType::HDF5_SINGLE;

  if (nom_fichier_decoupage_ != "?")
    ecrire_fichier_decoupage();

  if (nom_fichier_decoupage_sommets_ != "?")
    ecrire_fichier_decoupage_som();

  if (nom_domaines_decoup_ != "?")
    ecrire_sous_domaines(nb_parties, som_raccord);

  if (nom_fichier_lata_ != "?")
    postraiter_decoupage(nom_fichier_lata_);
  if (nom_fichier_med_ != "?")
    postraiter_decoupage(nom_fichier_med_);

  Cout << finl << "Quality of partitioning --------------------------------------------" << finl;
  trustIdType total_elem = Process::mp_sum(elem_part_.size_reelle());
  Cout << "\nTotal number of elements = " << total_elem << finl;
  Cout << "Number of Domaines : " << nb_parties << finl;

  if (print_more_infos_)
    {
      DoubleVect A(nb_parties);
      A = 0;
      for (int i = 0; i < elem_part_.size_reelle(); i++)
        A(elem_part_[i]) = static_cast<double>(A(elem_part_[i])) + 1;

      if(Process::je_suis_maitre())
        {
          for(int proc=1; proc<Process::nproc(); proc++)
            {
              DoubleVect tmp(nb_parties);
              tmp = 0;
              recevoir(tmp, proc, 0, proc+2005);

              for(int i_part=0; i_part<nb_parties; i_part++)
                A(i_part) += tmp(i_part);
            }

          double mean_element_domaine = static_cast<double>(total_elem/nb_parties);
          if (mean_element_domaine>0)
            {
              double load_imbalance = double(local_max_vect(A) / mean_element_domaine);
              Cout << "Number of cells per Domaine (min/mean/max) : " << local_min_vect(A) << " / " << mean_element_domaine << " / "
                   << local_max_vect(A) << " Load imbalance: " << load_imbalance << "\n" << finl;
            }
        }
      else
        envoyer(A, Process::me(), 0, Process::me()+2005);

      // we could do it as below, but if nb_parties is big, it would involve a lot of collective communication
      // for (int i = 0; i < nb_parties; i++)
      // 	A[i] = Process::mp_sum(A[i]);
    }

  Cerr << "End of the interpreter Decouper" << finl;
  if (reorder_==0 && nb_parties>128)
    {
      Cerr << "Performance tip: You could add \"reorder 1\" option to have less distance between communicating processes on the network." << finl;
      Cerr << "Add also \"Ecrire_lata filename.lata\" to post-process the partition numeration and see the difference." << finl;
    }
}

template <typename _SIZE_>
int Decouper_32_64<_SIZE_>::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  //Motcle motlu;
  if (mot=="partitionneur|partition_tool")
    {
      Cerr << "domaine = " << nom_domaine_ << finl;
      lire_partitionneur(is);
      return 1;
    }
  return -1;
}

template class Decouper_32_64<int>;
#if INT_is_64_ == 2
template class Decouper_32_64<trustIdType>;
#endif
