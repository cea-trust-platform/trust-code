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
#include <Decouper.h>
#include <Domaine.h>
#include <DomaineCutter.h>
#include <Postraitement_lata.h>
#include <EChaine.h>
#include <SFichier.h>
#include <Param.h>
#include <communications.h>

Implemente_instanciable(Decouper,"Decouper|Partition",Interprete);

Sortie& Decouper::printOn(Sortie& os) const
{
  exit();
  return os;
}

Entree& Decouper::readOn(Entree& is)
{
  exit();
  return is;
}

// Description:
//  Cherche le domaine de nom "le_nom_domaine" parmi les objets connus
//  de l'interprete et renvoie une ref a ce domaine.
// Exception: si le domaine n'a pas ete trouve, exit.
const Domaine& Decouper::find_domain(const Nom& le_nom_domaine)
{
  const Domaine * domaine_ptr = 0;

  if (objet_existant(le_nom_domaine))
    {
      const Objet_U& objet_domaine = objet(le_nom_domaine);
      if (sub_type(Domaine, objet_domaine))
        domaine_ptr = & ref_cast(Domaine, objet_domaine);
    }

  if (domaine_ptr == 0)
    {
      Cerr << "Error in Decouper::interpreter\n "
           << " Syntax:\n Decouper DOMAINNAME { ... }\n ";
      Cerr << le_nom_domaine << " is not a domain known" << finl;
      Process::exit();
    }
  return *domaine_ptr;
}

// Description: lecture du partitionneur dans le jeu de donnees
//  (lecture du type et des parametres et initialisation de d_part)
static void lire_partitionneur(DERIV(Partitionneur_base) & d_part,
                               const Domaine& domaine, Entree& is)
{
  Nom n;
  is >> n;
  Nom type_partitionneur("Partitionneur_");
  type_partitionneur += n;
  Cerr << " Creation of a partitioner of type: " << type_partitionneur << finl;
  d_part.typer(type_partitionneur);
  // En cas d'echec (si le nom du type est invalide) : exit
  if (! d_part.non_nul())
    Process::exit();

  // Initialisation des parametres du partitionneur
  Partitionneur_base& part = d_part.valeur();
  part.associer_domaine(domaine);
  is >> part;
}

static void ecrire_fichier_decoupage(const Nom& nom_fichier_decoupage,
                                     const IntVect& elem_part,
                                     int& nb_parts_tot)
{
  Cerr << "Writing of the splitting array at the format IntVect ascii\n"
       << " in the file " << nom_fichier_decoupage
       << "\n(for each element, number of the destination processor)" << finl;
  SFichier file; // Fichier ascii
  if (! file.ouvrir(nom_fichier_decoupage))
    {
      Cerr << " Error in the opening of the file." << finl;
      Process::exit();
    }
  file << elem_part;
  file << nb_parts_tot;
}

static void postraiter_decoupage(const Nom& nom_fichier_lata,
                                 const Domaine& domaine,
                                 const IntVect& elem_part)
{
  Cerr << "Postprocessing of the splitting at the lata format: " << nom_fichier_lata << finl;
  Postraitement_lata::Format format = Postraitement_lata::BINAIRE;
  Postraitement_lata::ecrire_entete(nom_fichier_lata,
                                    "Discretisation_inconnue",
                                    format);
  Postraitement_lata::ecrire_zone(nom_fichier_lata,
                                  domaine.zone(0),
                                  format);

  Postraitement_lata::ecrire_temps(nom_fichier_lata,
                                   0.,
                                   format);
  const int n = elem_part.size_reelle();
  DoubleTab data(n);
  for (int i = 0; i < n; i++)
    data(i) = elem_part[i];
  Postraitement_lata::ecrire_champ(nom_fichier_lata,
                                   Nom("partition"),
                                   Postraitement_lata::CHAMP,
                                   'I',
                                   domaine.le_nom(),
                                   "pb",
                                   0., /* temps */
                                   Postraitement_base::ELEMENTS,
                                   data,
                                   format);

  Postraitement_lata::finir_lata(nom_fichier_lata);
}

static void ecrire_sous_zones(const Nom& nom_zones_decoup,
                              const Decouper::ZonesFileOutputType format,
                              const Domaine& domaine,
                              IntVect& elem_part,
                              const int nb_parties,
                              const int epaisseur_joint,
                              const int reorder,
                              const Noms& bords_periodiques,
                              const Static_Int_Lists* som_raccord)
{
  DomaineCutter cutter;
  cutter.initialiser(domaine, elem_part, nb_parties, epaisseur_joint,
                     bords_periodiques);
  // Reflexion provisoire:
  // Les joints sont construits dans ecrire_zones -> construire_sous_domaine
  // Donc apres renumerotation des PEs et donc de elem_part, il faudrait
  // reinitialiser ? Ou bien, tout renumeroter apres le calcul des joints...
  // Cela parait mieux...
  cutter.ecrire_zones(nom_zones_decoup, format, elem_part, reorder, som_raccord);
}

// XD partition interprete decouper -1 Class for parallel calculation to cut a domain for each processor. By default, this keyword is commented in the reference test cases.
// XD attr domaine ref_domaine domaine 0 Name of the domain to be cut.
// XD attr bloc_decouper bloc_decouper bloc_decouper 0 Description how to cut a domain.
// XD bloc_decouper objet_lecture nul 1 Auxiliary class to cut a domain.
// XD attr partitionneur|Partition_tool partitionneur_deriv partitionneur 1 Defines the partitionning algorithm (the effective C++ object used is \'Partitionneur_ALGORITHM_NAME\').
// XD attr larg_joint entier larg_joint 1 This keyword specifies the thickness of the virtual ghost zone (data known by one processor though not owned by it). The default value is 1 and is generally correct for all algorithms except the QUICK convection scheme that require a thickness of 2. Since the 1.5.5 version, the VEF discretization imply also a thickness of 2 (except VEF P0). Any non-zero positive value can be used, but the amount of data to store and exchange between processors grows quickly with the thickness.
// XD attr nom_zones|zones_name chaine nom_zones 1 Name of the files containing the different partition of the domain. The files will be : NL2 name_0001.Zones NL2 name_0002.Zones NL2 ... NL2 name_000n.Zones. If this keyword is not specified, the geometry is not written on disk (you might just want to generate a \'ecrire_decoupage\' or \'ecrire_lata\').
// XD attr ecrire_decoupage chaine ecrire_decoupage 1 After having called the partitionning algorithm, the resulting partition is written on disk in the specified filename. See also partitionneur Fichier_Decoupage. This keyword is useful to change the partition numbers: first, you write the partition into a file with the option ecrire_decoupage. This file contains the zone number for each element\'s mesh. Then you can easily permute zone numbers in this file. Then read the new partition to create the .Zones files with the Fichier_Decoupage keyword.
// XD attr ecrire_lata chaine ecrire_lata 1 not_set
// XD attr nb_parts_tot entier nb_parts_tot 1 Keyword to generates N .Zone files, instead of the default number M obtained after the partitionning algorithm. N must be greater or equal to M. This option might be used to perform coupled parallel computations. Supplemental empty zones from M to N-1 are created. This keyword is used when you want to run a parallel calculation on several domains with for example, 2 processors on a first domain and 10 on the second domain because the first domain is very small compare to second one. You will write Nb_parts 2 and Nb_parts_tot 10 for the first domain and Nb_parts 10 for the second domain.
// XD attr periodique listchaine periodique 1 N BOUNDARY_NAME_1 BOUNDARY_NAME_2 ... : N is the number of boundary names given. Periodic boundaries must be declared by this method. The partitionning algorithm will ensure that facing nodes and faces in the periodic boundaries are located on the same processor.
// XD attr reorder entier reorder 1 If this option is set to 1 (0 by default), the partition is renumbered in order that the processes which communicate the most are nearer on the network. This may slighlty improves parallel performance.
// XD attr single_hdf rien single_hdf 1 Optional keyword to enable you to write the partitioned zones in a single file in hdf5 format.
// XD attr print_more_infos entier print_more_infos 1 If this option is set to 1 (0 by default), print infos about number of remote elements (ghosts) and additional infos about the quality of partitionning. Warning, it slows down the cutting operations.
int Decouper::print_more_infos = 0;
Entree& Decouper::interpreter(Entree& is)
{
  Cerr << "Decouper : Splitting of a domain" << finl;
  //lecture des parametres
  lire(is);

  //generation de la partition
  IntVect elem_part;
  Partitionneur_base& partitionneur = deriv_partitionneur.valeur();
  partitionneur.declarer_bords_periodiques(liste_bords_periodiques);
  partitionneur.construire_partition(elem_part,nb_parts_tot);

  //ecriture
  ecrire(elem_part, {});

  Cerr << "End of the interpreter Decouper" << finl;
  return is;
}


Entree& Decouper::lire(Entree& is)
{
  // Lecture du nom du domaine a decouper:
  is >> nom_domaine;
  Cerr << " Domain name to split : " << nom_domaine << finl;
  ref_domaine = find_domain(nom_domaine);
  // Avant de decouper on imprime des infos
  ref_domaine->imprimer();

  Param param(que_suis_je());
  param.ajouter_non_std("partitionneur|partition_tool",(this),Param::REQUIRED);
  param.ajouter("larg_joint",&epaisseur_joint);
  param.ajouter_condition("value_of_larg_joint_ge_1","The joint thickness must greater or equal to 1.");
  param.ajouter("nom_zones|zones_name",&nom_zones_decoup);
  param.ajouter("ecrire_decoupage",&nom_fichier_decoupage);
  param.ajouter("ecrire_lata",&nom_fichier_lata);
  param.ajouter("nb_parts_tot",&nb_parts_tot);
  param.ajouter("reorder",&reorder);
  param.ajouter_flag("single_hdf",&format_hdf);
  param.ajouter("periodique",&liste_bords_periodiques);
  param.ajouter("print_more_infos",&Decouper::print_more_infos);
  param.lire_avec_accolades_depuis(is);
  return is;
}


void Decouper::ecrire(IntVect& elem_part, const Static_Int_Lists* som_raccord)
{
  // Calcul du nombre de parties generees par le partitionneur
  int nb_parties = 0;
  if (elem_part.size_array() > 0)
    nb_parties = max_array(elem_part) + 1;
  nb_parties = mp_max(nb_parties);
  Cerr << "The partitioner has generated " << nb_parties << " parts." << finl;

  // Prise en compte de la directive nb_parts_tot
  if (nb_parts_tot >= 0)
    {
      if (nb_parties > nb_parts_tot)
        {
          Cerr << "Error: nb_parts_tot is less than the number of parts generated by the partitioner."
               << finl;
          exit();
        }
      Cerr << "Number of parts requested : " << nb_parts_tot
           << "\nGeneration of " << nb_parts_tot - nb_parties << " empty parts." << finl;
      nb_parties = nb_parts_tot;
    }
  // Force un seul fichier .Zones au dela d'un certain nombre de rangs MPI:
  if (Process::force_single_file(nb_parties, nom_zones_decoup+".Zones"))
    format_hdf = 1;

  if (nom_fichier_decoupage != "?")
    ecrire_fichier_decoupage(nom_fichier_decoupage, elem_part, nb_parts_tot);

  if (nom_zones_decoup != "?")
    {
      Decouper::ZonesFileOutputType typ;
      if (format_binaire) typ = BINARY_MULTIPLE;
      if (format_hdf) typ = HDF5_SINGLE;
      ecrire_sous_zones(nom_zones_decoup, typ,
                        ref_domaine.valeur(), elem_part, nb_parties, epaisseur_joint, reorder,
                        liste_bords_periodiques, som_raccord);
    }

  if (nom_fichier_lata != "?")
    postraiter_decoupage(nom_fichier_lata, ref_domaine.valeur(), elem_part);

  Cout << "\nQuality of partitioning --------------------------------------------" << finl;
  int total_elem = Process::mp_sum(elem_part.size_reelle());
  Cout << "\nTotal number of elements = " << total_elem << finl;
  Cout << "Number of Zones : " << nb_parties << finl;

  if (Decouper::print_more_infos)
    {
      DoubleVect A(nb_parties);
      A = 0;
      for (int i = 0; i < elem_part.size_reelle(); i++)
        A(elem_part[i]) = A(elem_part[i]) + 1;

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


          double mean_element_zone = total_elem/nb_parties;
          if (mean_element_zone>0)
            {
              double load_imbalance = double(local_max_vect(A) / mean_element_zone);
              Cout << "Number of cells per Zone (min/mean/max) : " << local_min_vect(A) << " / " << mean_element_zone << " / "
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
  if (reorder==0 && nb_parties>128)
    {
      Cerr << "Performance tip: You could add \"reorder 1\" option to have less distance between communicating processes on the network." << finl;
      Cerr << "Add also \"Ecrire_lata filename\" to post-process the partition numeration and see the difference." << finl;
    }
}

int Decouper::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="partitionneur|partition_tool")
    {
      Cerr<<"domaine = "<<nom_domaine<<finl;
      const Domaine& domaine = find_domain(nom_domaine);
      lire_partitionneur(deriv_partitionneur,domaine,is);
      return 1;
    }
  else
    {
      Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
      exit();
    }
  return -1;
}
