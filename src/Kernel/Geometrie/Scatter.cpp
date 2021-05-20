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
// File:        Scatter.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/73
//
//////////////////////////////////////////////////////////////////////////////


#include <Scatter.h>
#include <Domaine.h>
#include <LecFicDistribueBin.h>
#include <Statistiques.h>
#include <DoubleTabs.h>
#include <Connectivite_som_elem.h>
#include <Schema_Comm.h>
#include <Faces_builder.h>
#include <Zone_VF.h>
#include <Reordonner_faces_periodiques.h>
#include <Postraitement_lata.h>
#include <communications.h>
#include <MD_Vector_tools.h>
#include <MD_Vector_std.h>
#include <unistd.h> // PGI
#include <Poly_geom_base.h>

#include <Entree_Brute.h>
#include <hdf5.h>
#include <Comm_Group_MPI.h>
#include <FichierHDFPar.h>
#include <LecFicDiffuse.h>

#include <EFichierBin.h>

extern Stat_Counter_Id interprete_scatter_counter_;

Implemente_instanciable_sans_constructeur(Scatter,"Scatter",Interprete);
Scatter::Scatter()
{}

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
Sortie& Scatter::printOn(Sortie& os) const
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
Entree& Scatter::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description :
// Renvoi le domaine associe
Domaine& Scatter::domaine()
{
  return le_domaine.valeur();
}

static void dump_lata(const Domaine& dom)
{
  const Zone& zone = dom.zone(0);
  const int nb_joints = zone.nb_joints();

  const Comm_Group *cg = 0;
  Nom nom_fichier_lata("espaces_virtuels");
  Postraitement_lata::Format format = Postraitement_lata::BINAIRE;
  Postraitement_lata::ecrire_entete(nom_fichier_lata,
                                    "Discretisation_inconnue",
                                    format, cg);
  Postraitement_lata::ecrire_zone(nom_fichier_lata,
                                  zone,
                                  format, cg);

  Postraitement_lata::ecrire_temps(nom_fichier_lata,
                                   0.,
                                   format, cg);
  const int n = zone.nb_elem();
  DoubleTab data(n);
  for(int ij = 0; ij < nb_joints; ij++)
    {
      const ArrOfInt& t1 = zone.joint(ij).joint_item(Joint::ELEMENT).items_distants();
      data = 0.;
      {
        const int nt1 = t1.size_array();
        for (int i = 0; i < nt1; i++)
          data(t1[i]) += 1;
      }
      Postraitement_lata::ecrire_champ(nom_fichier_lata,
                                       Nom("partition") + Nom(zone.joint(ij).PEvoisin()),
                                       Postraitement_lata::CHAMP,
                                       'I',
                                       dom.le_nom(),
                                       "pb",
                                       0., /* temps */
                                       Postraitement_base::ELEMENTS,
                                       data,
                                       format);
    }
}

// Description:
//  Lit et complete un domaine parallele selon les motcles lus
//  dans le jeu de donnees. Format:
//   Scatter [debug] file_name domain_name
//  On lit les sommets, les elements et les sommets et faces de joint,
//  On construit les espaces distants et virtuels en fonction
//  de l'epaisseur de joint.
Entree& Scatter::interpreter(Entree& is)
{
  // Nom des fichiers de decoupage : nomentree.xxxx
  Nom nomentree;
  is >> nomentree;
  if (Process::nproc()==1)
    {
      Motcle n(nomentree);
      if (n != ";" && n != "unlock;")
        {
          Cerr << "Error ! You ran a sequential calculation and can't use Scatter keyword here. Run a parallel calculation or remove this keyword." << finl;
          exit();
        }
      Cerr << "Scatter: preparing domain structure\n"
           << " (this is workaround for bugged domain operators that don't do it)" << finl;
      Nom nomdomaine;
      is >> nomdomaine;
      Objet_U& obj = objet(nomdomaine);
      if(!sub_type(Domaine, obj))
        {
          Cerr << "obj : " << obj << " is not an object of type Domain !" << finl;
          exit();
        }
      Domaine& dom = ref_cast(Domaine, obj);
      if (n == ";")
        init_sequential_domain(dom);
      else
        uninit_sequential_domain(dom);
      return is;
    }
  // Pour debugger sur linux en parallele
#ifdef linux
  static int gdb_non_lance=1;
  char* TRUST_GDB=getenv("TRUST_GDB");
  if (gdb_non_lance && ((Motcle)nomentree=="DEBUG" || TRUST_GDB!=NULL))
    {
      gdb_non_lance=0;
      if ((Motcle)nomentree=="DEBUG") is >> nomentree;
      if (je_suis_maitre())
        {
          Cerr << "Enter \"return\" to this window after" << finl;
          Cerr << "typing \"cont\" in other gdb windows." << finl;
          Cerr << (int)system ("sh -c read ok") << finl;
        }
      else
        {
          Nom getpidn((int)getpid());
          Nom cmdfile=getpidn;
          Nom command0="echo attach ";
          command0+=getpidn;
          command0+=" > ";
          command0+=cmdfile;
          Cerr << (int)system(command0) << finl;
          command0=" ls -l /proc/";
          command0+=getpidn;
          command0+="/exe | awk '{print $NF}' > execname";
          Cerr << (int)system(command0) << finl;
          Nom command="[ -f /usr/X11R6/bin/xterm ] && x=\"/usr/X11R6/bin/xterm -exec gdb -x \";";
          command+="[ -f /usr/bin/konsole ] && x=\"/usr/bin/konsole -e gdb -x \";";
          command+="$x ";
          command+=cmdfile;
          command+=" `cat execname` ";
          command+=" &";
          Cerr<<"command: " <<command<<finl;
          Cerr << (int)system(command) << finl;
        }
    }
#endif
  barrier();

  if (Process::je_suis_maitre())
    Cerr << "Execution of the Scatter module." << finl;

  statistiques().begin_count(interprete_scatter_counter_);

  // On recupere le domaine:
  Nom nomdomaine;
  is >> nomdomaine;
  Objet_U& obj = objet(nomdomaine);
  if(!sub_type(Domaine, obj))
    {
      Cerr << "obj : " << obj << " is not an object of type Domain !" << finl;
      exit();
    }
  Domaine& dom = ref_cast(Domaine, obj);
  le_domaine = dom;

  // Lecture des fichiers de decoupage :
  barrier();
  if (Process::je_suis_maitre())
    Cerr << "Reading the domain" << finl;

  Noms liste_bords_periodiques;
  lire_domaine(nomentree, liste_bords_periodiques);

  // Verification sanitaire: nombre de processeurs = nombre de zones
  // (on verifie qu'il n'y a pas de joint avec un processeur inexistant)
  {
    const Joints& joints = dom.zone(0).faces_joint();
    const int nb_joints = joints.size();
    int max_pe_voisin = 0;
    for (int i = 0; i < nb_joints; i++)
      {
        const int pe_voisin = joints[i].PEvoisin();
        if (pe_voisin >= max_pe_voisin)
          max_pe_voisin = pe_voisin;
      }
    max_pe_voisin = (int) mp_max(max_pe_voisin);
    double ok=1;
    if (max_pe_voisin >= nproc()) ok=0;
    if (!ok)
      {
        Cerr << "Error in Scatter::interpreter\n"
             << "The domain has been partitioned with at least " << max_pe_voisin << " "
             << "zones whereas the number of processes asked is " << Process::nproc() << "." << finl;
        Cerr << "The number of zones and number of processes must match." << finl;
        exit();
      }
  }

  barrier();
  Cerr << "Calculation of renum_items_communs for the nodes" << finl;
  calculer_renum_items_communs(dom.zone(0).faces_joint(), Joint::SOMMET);

  // Pas encore code: on verifie que les sommets communs ont des coordonnees identiques
  // sur tous les processeurs.
  // check_sommets_joints(dom);

  barrier();
  Cerr << "Construire_structures_paralleles" << finl;
  construire_structures_paralleles(dom, liste_bords_periodiques);

  if (0)
    dump_lata(dom);

  barrier();
  Cerr << "End Distribue_zones" << finl;

  Cerr << "\nQuality of partitioning --------------------------------------------" << finl;
  int total_nb_elem = Process::mp_sum(dom.zone(0).nb_elem());
  Cerr << "\nTotal number of elements = " << total_nb_elem << finl;
  Cerr << "Number of Zones : " << Process::nproc() << finl;
  double min_element_zone = mp_min(dom.zone(0).nb_elem());
  double max_element_zone = mp_max(dom.zone(0).nb_elem());
  double mean_element_zone = total_nb_elem / Process::nproc();
  Cerr << "Min number of elements on a Zone = " << min_element_zone << finl;
  Cerr << "Max number of elements on a Zone = " << max_element_zone << finl;
  Cerr << "Mean number of elements per Zone = " << (int)(mean_element_zone) << finl;
  double load_imbalance = max_element_zone / mean_element_zone;
  Cerr << "Load imbalance = " << load_imbalance << "\n" << finl;

  Elem_geom_base& elem=dom.zone(0).type_elem().valeur();
  if (sub_type(Poly_geom_base,elem))
    {
      ref_cast(Poly_geom_base,elem).compute_virtual_index();
    }
  statistiques().end_count(interprete_scatter_counter_);
  if(Process::me()==0)
    {
      double temps = statistiques().last_time(interprete_scatter_counter_);
      Cerr << "Scatter time : " << temps << finl;
    }

  return is;
}

// Description:
// Merged zones receive joints information from their neighbours
// to ensure that their common items (vertices) appear in the same order
// If it's not the case, the merged zone reorders its common items so that it matches the neighbour's order
// When 2 neighbouring zones have each been merged,
// only the processor with the lowest rank proceeds to reordering
void Scatter::check_consistancy_remote_items(Domaine& dom, const ArrOfInt& mergedZones)
{
  const Joints& joints     = dom.zone(0).faces_joint();
  const int nb_joints = joints.size();

  const DoubleTab& coords = dom.les_sommets();
  ArrOfInt liste_send;
  ArrOfInt liste_recv;
  liste_send.set_smart_resize(1);
  liste_recv.set_smart_resize(1);

  const int& moi = Process::me();
  const int& myZoneWasMerged = mergedZones[moi];

  for (int i_joint = 0; i_joint < nb_joints; i_joint++)
    {

      const int& pe_voisin = joints[i_joint].PEvoisin();
      const int& neighbourZoneWasMerged = mergedZones[pe_voisin];
      if(myZoneWasMerged && neighbourZoneWasMerged)
        {
          if(pe_voisin < moi)
            liste_recv.append_array(pe_voisin);
          else
            liste_send.append_array(pe_voisin);
        }
      else if(myZoneWasMerged && !neighbourZoneWasMerged)
        liste_recv.append_array(pe_voisin);
      else if(!myZoneWasMerged && neighbourZoneWasMerged)
        liste_send.append_array(pe_voisin);
      else
        {
          //nothing to exchange
        }
    }

  VECT(DoubleTab) coord_items_locaux(nb_joints);
  VECT(DoubleTab) coord_items_distants(nb_joints);
  for (int i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const Joint& joint     = joints[i_joint];
      const ArrOfInt& items_communs = joint.joint_item(Joint::SOMMET).items_communs();
      const int nb_items_communs = items_communs.size_array();

      DoubleTab&   coord   = coord_items_locaux[i_joint];
      coord.resize(nb_items_communs, dimension);
      for (int i = 0; i < nb_items_communs; i++)
        for (int j = 0; j < dimension; j++)
          coord(i,j) = coords(items_communs[i], j);
    }

  // Envoi des coordonnees locales au processeur voisin
  {
    Schema_Comm schema_comm;
    schema_comm.set_send_recv_pe_list(liste_send, liste_recv);
    schema_comm.begin_comm();
    for (int i = 0; i < nb_joints; i++)
      {
        const int pe_voisin = joints[i].PEvoisin();
        const int& neighbourZoneWasMerged = mergedZones[pe_voisin];
        if( neighbourZoneWasMerged && !(myZoneWasMerged && pe_voisin<moi) )
          {
            Sortie& buffer = schema_comm.send_buffer(pe_voisin);
            buffer << coord_items_locaux[i];
          }
      }
    schema_comm.echange_taille_et_messages();

    if(myZoneWasMerged)
      {
        for (int i = 0; i < nb_joints; i++)
          {
            const int pe_voisin = joints[i].PEvoisin();
            const int& neighbourZoneWasMerged = mergedZones[pe_voisin];
            if(!(neighbourZoneWasMerged && pe_voisin>moi))
              {
                Entree& buffer = schema_comm.recv_buffer(pe_voisin);
                buffer >> coord_items_distants[i];
              }
          }
      }

    schema_comm.end_comm();
  }

  // check if the vertices in my joints appear in the same order as my neighbour joint
  if(myZoneWasMerged)
    {
      for (int i_joint = 0; i_joint < nb_joints; i_joint++)
        {

          const int pe_voisin = joints[i_joint].PEvoisin();
          const int& neighbourZoneWasMerged = mergedZones[pe_voisin];
          if(neighbourZoneWasMerged && pe_voisin>moi)
            continue;
          ArrOfInt& items_communs = dom.zone(0).faces_joint()[i_joint].set_joint_item(Joint::SOMMET).set_items_communs();
          const ArrOfInt old_items_communs = joints[i_joint].joint_item(Joint::SOMMET).items_communs();
          const int     nb_items      = items_communs.size_array();
          const DoubleTab& coord_voisin = coord_items_distants[i_joint];
          const DoubleTab& my_coord = coord_items_locaux[i_joint];
          assert(my_coord.size_array() ==  coord_voisin.size_array());
          for(int i=0; i<nb_items; i++)
            {
              for(int j=0; j<nb_items; j++)
                {
                  int ok=1;
                  for (int dir=0; dir<Objet_U::dimension; dir++)
                    ok=ok&&(est_egal(coord_voisin(i,dir),my_coord(j,dir)));
                  if (ok)
                    {
                      items_communs[i] = old_items_communs[j];
                      break;
                    }
                }
            }
        }
    }
}


// Description:
// Does the exact same thing as the readOn of the class Domaine
// but without collective communication
// Necessary when the processors don't have the same numbers of file to read
void Scatter::readDomainWithoutCollComm(Domaine& dom, Entree& fic )
{
  Cerr << "reading vertices..." << finl;
  dom.read_vertices(fic);

  Cerr << "Done !\nreading zones..." << finl;

  Nom accouverte="{";
  Nom accfermee="}";
  Nom virgule=",";
  Motcle nom;
  fic >> nom;
  if(nom!=(const char*)"vide")
    {
      if (nom!=accouverte)
        {
          Cerr << "Error while reading a list." << finl;
          Cerr << "One expected an opened bracket { to start." << finl;
          exit();
        }
      while(1)
        {
          Zone zone_tmp;
          zone_tmp.read_zone(fic);
          dom.add(zone_tmp);
          fic >> nom;
          if(nom==accfermee)
            break;
          if(nom!=virgule)
            {
              Cerr << nom << " one expected a ',' or a '}'" << finl;
              exit();
            }
        }

    }
  Cerr << "Done!" << finl;
}

// Description:
// Merging dom_to_add with dom
void Scatter::mergeDomains(Domaine& dom, Domaine& dom_to_add)
{
  int old_nb_elems = dom.nb_zones() ? dom.zone(0).nb_elem() : 0;
  // merging vertices
  IntVect nums;
  Zone& zone=dom.add(dom_to_add.zone(0));
  zone.associer_domaine(dom);
  dom.ajouter(dom_to_add.coord_sommets(), nums);
  Scatter::uninit_sequential_domain(dom);
  zone.renum(nums);
  zone.renum_joint_common_items(nums, old_nb_elems);
  zone.associer_domaine(dom);

  if(dom.zones().size() > 1)
    {
      int new_nb_elems = dom.zone(0).nb_elem();
      // merging zones
      dom.zones().merge();

      //merging common vertices and remote items
      const int& nb_joints = dom_to_add.zone(0).nb_joints();
      for (int i_joint = 0; i_joint < nb_joints; i_joint++)
        {
          const Joint& joint_to_add  = dom_to_add.zone(0).faces_joint()[i_joint];

          int my_joint_index = 0;
          while(joint_to_add.PEvoisin() != dom.zone(0).faces_joint()[my_joint_index].PEvoisin())
            my_joint_index++;

          const ArrOfInt& sommets_to_add = joint_to_add.joint_item(Joint::SOMMET).items_communs();
          ArrOfInt& items_communs = dom.zone(0).faces_joint()[my_joint_index].set_joint_item(Joint::SOMMET).set_items_communs();
          items_communs.set_smart_resize(1);
          for(int index=0; index<sommets_to_add.size_array(); index++)
            items_communs.append_array(nums[sommets_to_add[index]]);
          items_communs.array_trier_retirer_doublons();

          const ArrOfInt& elements_to_add = joint_to_add.joint_item(Joint::ELEMENT).items_distants();
          ArrOfInt& items_distants = dom.zone(0).faces_joint()[my_joint_index].set_joint_item(Joint::ELEMENT).set_items_distants();
          items_distants.set_smart_resize(1);
          for(int index=0; index<elements_to_add.size_array(); index++)
            items_distants.append_array(new_nb_elems + elements_to_add[index]);
        }
    }
}

// Description:
//  Lit le domaine dans le fichier de nom "nomentree",
//  de type LecFicDistribueBin ou LecFicDistribue
//  Format attendu : Domaine::ReadOn
//  La zone est renommee comme le domaine (pour lance_test_seq_par)
void Scatter::lire_domaine(Nom& nomentree, Noms& liste_bords_periodiques)
{
  // On determine si le fichier est au nouveau format ou a l'ancien
  if (Process::je_suis_maitre())
    Cerr << "Reading geometry from .Zones file(s) ..." << finl;
  barrier(); // Attendre que le message soit affiche

  Domaine& dom = domaine();

  Nom copy(nomentree);
  copy = copy.nom_me(Process::nproc(), "p", 1);

  //bool is_hdf = FichierHDF::is_hdf5(copy);
  LecFicDiffuse test;
  bool is_hdf = test.ouvrir(copy) && FichierHDF::is_hdf5(copy);

  static Stat_Counter_Id stats = statistiques().new_counter(0 /* Level */, "Scatter::lire_domaine", 0 /* Group */);

  statistiques().begin_count(stats);


  ArrOfInt mergedZones(Process::nproc());
  mergedZones = 0;
  if (is_hdf)
    {
      FichierHDFPar fic_hdf;
      //FichierHDF fic_hdf;
      nomentree = copy;
      fic_hdf.open(nomentree, true);

      std::string dname = "/zone_"  + std::to_string(Process::me());
      bool ok = fic_hdf.exists(dname.c_str());
      if(!ok)
        {
          mergedZones = 1;
          for(int i=0; i<Process::nproc(); i++)
            {
              Entree_Brute data_part;
              Domaine part_dom;
              std::string dname_part = dname + "_" + std::to_string(i);

              bool exists = fic_hdf.exists(dname_part.c_str());
              if(exists)
                {
                  Nom dataset_name(dname_part.c_str());
                  fic_hdf.read_dataset(dataset_name, data_part);
                  readDomainWithoutCollComm( part_dom, data_part );
                  mergeDomains(dom, part_dom);

                  // Renseigne dans quel fichier le domaine a ete lu
                  dom.set_fichier_lu(nomentree);
                  data_part >> liste_bords_periodiques;
                }
              else
                break;
            }
        }
      else
        {
          Entree_Brute data;
          Nom dataset_name = dname.c_str();
          fic_hdf.read_dataset(dname.c_str(), data);

          // Feed TRUST objects:
          readDomainWithoutCollComm(dom, data);
          dom.zones().associer_domaine(dom);
          dom.set_fichier_lu(nomentree);
          data >> liste_bords_periodiques;
        }

      fic_hdf.close();
    }
  else
    {

      LecFicDistribueBin fichier_binaire;
      int isSingleZone = fichier_binaire.ouvrir(nomentree);

      if (!isSingleZone)
        {
          mergedZones = 1;
          Nom nomentree_with_suffix=nomentree.nom_me(Process::me());
          for(int i=0; i<Process::nproc(); i++)
            {
              EFichierBin fichier_binaire_part;
              Domaine part_dom;
              std::string tmp = nomentree_with_suffix.getPrefix(".Zones").getString();
              tmp += "_";
              tmp += std::to_string(i);
              tmp += ".Zones";
              Nom nomentree_part(tmp.c_str());
              int ok = fichier_binaire_part.ouvrir(nomentree_part);
              if(ok)
                {
                  readDomainWithoutCollComm( part_dom, fichier_binaire_part );
                  mergeDomains(dom, part_dom);

                  // Renseigne dans quel fichier le domaine a ete lu
                  dom.set_fichier_lu(nomentree);
                  fichier_binaire_part >> liste_bords_periodiques;
                  fichier_binaire_part.close();
                }
              else
                break;
            }
        }
      else
        {
          readDomainWithoutCollComm(dom, fichier_binaire );
          dom.zones().associer_domaine(dom);

          // Renseigne dans quel fichier le domaine a ete lu
          dom.set_fichier_lu(nomentree);
          fichier_binaire >> liste_bords_periodiques;
          fichier_binaire.close();
        }
    }

  envoyer_all_to_all(mergedZones, mergedZones);
  check_consistancy_remote_items( dom, mergedZones );
  dom.zone(0).check_zone();

  // PL : pas tout a fait exact le nombre affiche de sommets, on compte plusieurs fois les sommets des joints...
  int nbsom = mp_sum(dom.les_sommets().dimension(0));
  Cerr << " Number of nodes: " << nbsom << finl;

  init_sequential_domain(dom);

  // merged zones need to reorder faces of periodic borders
  const int myZoneWasMerged = mergedZones[Process::me()];
  if(myZoneWasMerged)
    {
      for(int bord_perio = 0; bord_perio < liste_bords_periodiques.size(); bord_perio++)
        {
          Nom bp_nom = liste_bords_periodiques(bord_perio);
          Bord& bord = dom.zone(0).bord(bp_nom);
          if(bord.nb_faces() == 0)
            continue;

          ArrOfDouble direction_perio(dimension);
          Reordonner_faces_periodiques::chercher_direction_perio(direction_perio, dom, bp_nom);
          IntTab& faces = bord.faces().les_sommets();
          double epsilon = precision_geom;
          Reordonner_faces_periodiques::reordonner_faces_periodiques(dom, faces, direction_perio, epsilon);
        }
    }

  statistiques().end_count(stats);
  barrier();
}

// Description:
//  Construction des structures paralleles du domaine et de la zone
//  (determination des elements distants en fonction de l'epaisseur de joint,
//   determination des sommets distants,
//   creation des sommets et des elements virtuels)
// Precondition:
//  - Les joints(i).faces et joints(i).sommets doivent etre remplis
//  - Les joints(i).joint_item(Joint::SOMMET).items_communs()
//    doivent etre remplis.
void Scatter::construire_structures_paralleles(Domaine& dom, const Noms& liste_bords_periodiques)
{
  // D'abord: supprimer les structures "sequentielles" associees aux sommets et elements lors de la lecture:
  {
    MD_Vector md_nul;
    dom.les_sommets().set_md_vector(md_nul);
    dom.zone(0).les_elems().set_md_vector(md_nul);
  }

  // L'ordre d'appel est important:
  calculer_espace_distant_elements(dom);

  if (liste_bords_periodiques.size() > 0)
    corriger_espace_distant_elements_perio(dom, liste_bords_periodiques);

  calculer_nb_items_virtuels(dom.zone(0).faces_joint(), Joint::ELEMENT);

  // Determination des sommets distants en fonction des elements distants
  calculer_espace_distant_sommets(dom, liste_bords_periodiques);

  // Creation des espaces distants virtuels et items communs pour les tableaux
  // sommets et elements:
  DoubleTab& sommets = dom.les_sommets();
  IntTab& elements = dom.zone(0).les_elems();
  MD_Vector md_sommets, md_elements;
  construire_md_vector(dom, sommets.dimension(0), Joint::SOMMET, md_sommets);
  construire_md_vector(dom, elements.dimension(0), Joint::ELEMENT, md_elements);
  MD_Vector_tools::creer_tableau_distribue(md_sommets, sommets);
  sommets.echange_espace_virtuel();
  construire_espace_virtuel_traduction(md_elements /* type index */,
                                       md_sommets /* type valeur */,
                                       elements);
  // Reordonner les faces de joint (correspondance implicite avec le pe voisin)
  reordonner_faces_de_joint(dom);
}

// Description:
//  Tri des joints dans l'ordre croissant des processeurs
void Scatter::trier_les_joints(Joints& joints)
{
  const int nb_joints = joints.size();
  ArrOfInt pe_voisins(nb_joints);
  int i;
  for (i = 0; i < nb_joints; i++)
    pe_voisins[i] = joints[i].PEvoisin();
  pe_voisins.ordonne_array();
  // Copie la liste des joints
  Joints anciens_joints(joints);
  for (i = 0; i < nb_joints; i++)
    {
      // On traite le processeur pe_voisin:
      const int pe_voisin = pe_voisins[i];
      // Ou est le joint avec ce processeur dans l'ancienne liste ?
      int i_old;
      for (i_old = 0; i_old < nb_joints; i_old++)
        if (anciens_joints[i_old].PEvoisin() == pe_voisin)
          break;
      assert(i_old < nb_joints);
      joints[i] = anciens_joints[i_old];
    }
}


// Description: Methode outil pour retirer les doublons dans un tableau.
static void array_trier_retirer_doublons(ArrOfInt& array)
{
  const int size_ = array.size_array();
  if (size_ == 0)
    return;
  // Tri dans l'ordre croissant
  array.ordonne_array();
  // Retire les doublons
  int new_size_ = 1;
  int last_value = array[0];
  for (int i = 1; i < size_; i++)
    {
      if (array[i] != last_value)
        {
          array[new_size_] = last_value = array[i];
          new_size_++;
        }
    }
  array.resize_array(new_size_);
}


// Description:
//  Retire de "sorted_array" les elements qui figurent dans "sorted_elements".
//  Les deux tableaux doivent etre initialement ordonnes dans l'ordre croissant.
//  Exemple:
//   En entree sorted_array=[1,4,9,10,12,18], sorted_elements=[3,5,9,10,18,25]
//   En sortie sorted_array=[1,4,12]
static void array_retirer_elements(ArrOfInt& sorted_array, const ArrOfInt& sorted_elements_list)
{
  int i_read;      // Index dans sorted_array (en lecture)
  int i_write = 0; // Index dans sorted_array (la ou on ecrit)
  int j = 0;       // Index dans sorted_elements
  const int n = sorted_array.size_array();
  const int m = sorted_elements_list.size_array();
  if (m == 0)
    return;

  int j_value = sorted_elements_list[j];
  for (i_read = 0; i_read < n; i_read++)
    {
      // Tableau trie ?
      assert(i_read == 0 || sorted_array[i_read] > sorted_array[i_read-1]);
      const int i_value = sorted_array[i_read];

      // On avance dans la liste sorted_elements jusqu'a trouver ou depasser
      // l'element i_value
      while ((j_value < i_value) && (j < m))
        {
          j++;
          if (j == m)
            break;
          assert(sorted_elements_list[j] > j_value); // Tableau trie ?
          j_value = sorted_elements_list[j];
        }

      if (j == m || j_value != i_value)
        {
          // i_value ne figure pas dans le tableau sorted_elements, on le garde
          sorted_array[i_write] = i_value;
          i_write++;
        }
    }
  sorted_array.resize_array(i_write);
}



// Si un joint avec le "pe" existe, renvoie son indice,
// sion cree un nouveau joint et renvoie son indice.
static int ajouter_joint(Zone& zone, int pe)
{
  Joints& joints = zone.faces_joint();
  const int i_joint = joints.size();

  {
    for (int i = 0; i < i_joint; i++)
      if (joints[i].PEvoisin() == pe)
        return i;
  }

  Joint& joint = joints.add(Joint());
  joint.nommer(Nom("Joint_")+Nom(pe));
  joint.associer_zone(zone);
  int ep = (i_joint > 0) ? joints[0].epaisseur() : 1;
  joint.affecte_epaisseur(ep);
  joint.affecte_PEvoisin(pe);

  // Initialiser tous les tableaux des joints supplementaires.
  // Note BM: pour bien faire, il faudrait initialiser uniquement
  // certains tableaux (ceux qui sont deja initialises pour les
  // joints existants), mais c'est plus complique a faire...
  {
    for (int t = 0; t < 5; t++)
      {
        Joint::Type_Item type;
        switch(t)
          {
          case 0:
            type = Joint::SOMMET;
            break;
          case 1:
            type = Joint::ELEMENT;
            break;
          case 2:
            type = Joint::FACE;
            break;
          case 3:
            type = Joint::ARETE;
            break;
          case 4:
            type = Joint::FACE_FRONT;
            break;
          default:
            Cerr << "Error in Scatter.cpp : ajouter_joint" << finl;
            // Pour eviter le warning suivant sur gcc 3.4:
            // Scatter.cpp:416: warning: 'type' might be used uninitialized in this function
            type = Joint::SOMMET;
            Process::exit();
          }
        Joint_Items& data = joint.set_joint_item(type);
        data.set_items_communs();
        data.set_items_distants();
        data.set_nb_items_virtuels(0);
        data.set_renum_items_communs().resize(0,2);
      }
  }

  return i_joint;
}


// Description:
//  Determination des items distants en fonction d'une liste d'items
//  a envoyer et de listes d'items communs.
//  exemple:
//   calculer_espace_distant_sommets
//   calculer_espace_distant_faces
//  Pour les sommets: les "items_to_send" sont les sommets des elements distants,
//   Si le processeur A veut que le processeur B connaisse le sommet i,
//   il faut que le processeur qui possede le sommet l'envoie a B.
//   Le processeur qui "possede" le sommet est le plus petit parmi les PEs
//   qui partagent ce sommet (item commun) (requis pour pouvoir faire
//   echange_item_commun et echange_espace_virtuel en une seule passe).
//   De plus, si plusieurs processeurs demandent a envoyer le meme sommet
//   au meme processeur, il ne faut l'inserer qu'une seule fois dans l'espace
//   distant.
//  Parametre: joints
//  Signification: les joints dans lesquels on veut calculer un espace distant
//  Parametre: nb_items_reels
//  Signification: le nombre d'items reels (sommets, faces, ...)
//  Parametre: items_to_send
//  Signification: un vecteur de "nproc()" tableaux, pour chaque processeur,
//   la liste des items qu'on veut lui envoyer (exemple:tous les sommets des
//   elements distants, ou toutes les faces)
//  Parametre: type_item
//  Signification: les items dont on veut calculer l'espace distant
// Precondition:
//  Methode a appeler simultanement par tous les processeurs.
void Scatter::calculer_espace_distant(Zone&                  zone,
                                      const int           nb_items_reels,
                                      const VECT(ArrOfInt) & items_to_send,
                                      const Joint::Type_Item type_item)
{
  assert(items_to_send.size() == Process::nproc());

  Process::Journal() << "Scatter::calculer_espace_distant type_item="
                     << (int)type_item << finl;

  Joints& joints = zone.faces_joint();

  // D'abord, on determine pour tous les items le numero du PE proprietaire:
  //  Pour chaque item du domaine:
  //   colonne 0 : indice du item sur le PE proprietaire (index_on_pe_owner)
  //   colonne 1 : numero du PE proprietaire (le plus petit pe qui partage l'item)
  IntTab num_global_items(nb_items_reels, 2);
  {
    int i;
    const int moi = Process::me();
    for (i = 0; i < nb_items_reels; i++)
      {
        num_global_items(i, 0) = i;
        num_global_items(i, 1) = moi;
      }
    const int nb_joints = joints.size();
    for (int i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        const Joint&   joint              = joints[i_joint];
        const int   pe_voisin          = joint.PEvoisin();
        const IntTab& renum_items_communs= joint.joint_item(type_item).renum_items_communs();
        const int   nb_items_communs   = renum_items_communs.dimension(0);
        for (i = 0; i < nb_items_communs; i++)
          {
            const int num_item_distant = renum_items_communs(i, 0);
            const int num_item_local   = renum_items_communs(i, 1);
            const int pe_actuel = num_global_items(num_item_local, 1);
            if (pe_voisin < pe_actuel)
              {
                num_global_items(num_item_local, 0) = num_item_distant;
                num_global_items(num_item_local, 1) = pe_voisin;
              }
          }
      }
  }

  Schema_Comm schema_comm;
  const int nproc = Process::nproc();

  // Premiere etape : on envoie au processeur proprietaire des items
  // la liste des items qu'il faut envoyer et a quel processeur il
  // faut les envoyer.
  // Si le processeur A doit envoyer l'element E au processeur B,
  // et que cet element utilise un item S qui appartient au processeur C,
  // alors on envoie a C le message :
  // "tu dois mettre le item S dans l'espace distant du processeur B"

  // On prepare un schema de communication entre les voisins:
  //  Processeur emetteur : le processeur qui possede l'element distant,
  //  Processeur recepteur: le processeur qui possede un item de l'element.
  // Ces processeurs sont voisins par les joints existants.
  const int nb_joints = joints.size();
  ArrOfInt liste_voisins(nb_joints);
  //int i_joint;
  for (int i_joint = 0; i_joint < nb_joints; i_joint++)
    liste_voisins[i_joint] = joints[i_joint].PEvoisin();

  schema_comm.set_send_recv_pe_list(liste_voisins, liste_voisins, 1 /* me_to_me */);
  schema_comm.begin_comm();
  {
    // et pour chaque item a envoyer, on envoie au processeur qui possede l'item (pe_item_owner):
    //  - l'indice local du item chez lui (item_distant),
    //  - le numero du processeur a qui il doit l'envoyer (pe_destination)
    const int nb_procs = Process::nproc();
    for (int pe_destination = 0; pe_destination < nb_procs; pe_destination++)
      {
        const ArrOfInt& items    = items_to_send[pe_destination];
        const int     nb_items = items.size_array();
        for (int i_item = 0; i_item < nb_items; i_item++)
          {
            const int item          = items[i_item];
            const int item_distant  = num_global_items(item, 0);
            const int pe_item_owner = num_global_items(item, 1);
            // On envoie le numero du item distant et dans quel joint il
            // faut le mettre.
            // Si pe_joint == pe_destination, litem est forcement deja
            // connu par l'autre processeur, inutile de l'envoyer
            if (pe_item_owner != pe_destination)
              schema_comm.send_buffer(pe_item_owner) << item_distant << pe_destination;
          }
      }
  }

  // Echange des messages
  schema_comm.echange_taille_et_messages();

  // Reception des items distants. On lit tous les buffers et on
  // range les items dans "items_distants" par processeur destination.
  // Pour chaque processeur voisin, la liste des items distants a envoyer:
  VECT(ArrOfInt) items_distants(nproc);
  {
    // Pour append_array
    for (int pe = 0; pe < nproc; pe++)
      items_distants[pe].set_smart_resize(1);
  }

  // Boucle sur tous les processeurs (pe_source) qui m'ont envoye des messages:
  // On boucle sur les processeurs voisins, plus moi-meme:
  for (int i_source = 0; i_source < nb_joints + 1; i_source++)
    {
      const int pe_source =
        (i_source < nb_joints) ? liste_voisins[i_source] : Process::me();

      Entree& buffer = schema_comm.recv_buffer(pe_source);
      // Boucle "tant que le buffer n'est pas vide"
      while(1)
        {
          int item_distant; // Indice du item distant
          int pe_distant;   // Numero du pe a qui il faut envoyer le item
          buffer >> item_distant >> pe_distant;
          if (buffer.eof())
            break;
          assert(pe_distant != Process::me());
          ArrOfInt& array = items_distants[pe_distant];
          array.append_array(item_distant);
        }
    }
  schema_comm.end_comm();

  // On retire les doublons et les items deja connus par le processeur voisin:
  {
    // Liste des joints correspondant a chaque pe
    ArrOfInt joint_of_pe(nproc);
    joint_of_pe = -1;
    for (int i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        const int pe = joints[i_joint].PEvoisin();
        joint_of_pe[pe] = i_joint;
      }
    // Liste des items deja connus par le processeur voisin (items communs)
    // tries dans l'ordre croissant
    ArrOfInt items_communs_tri;
    items_communs_tri.set_smart_resize(1);
    for (int pe = 0; pe < nproc; pe++)
      {
        ArrOfInt& items = items_distants[pe];
        // Retirer les doublons:
        array_trier_retirer_doublons(items);
        // Retirer les items deja connus:
        const int i_joint = joint_of_pe[pe];
        if (i_joint >= 0)
          {
            items_communs_tri =
              joints[i_joint].joint_item(type_item).items_communs();
            items_communs_tri.ordonne_array();
            array_retirer_elements(items, items_communs_tri);
          }
        else
          {
            // Pas d'item commun avec ce pe.
          }
      }
  }

  // Des espaces distants peuvent etre crees sur des processeurs avec
  // qui il n'existe pas encore de joint. On ajoute les nouveaux joints.
  {
    ArrOfInt nouveaux_voisins;
    nouveaux_voisins.set_smart_resize(1);
    int i;
    for (i = 0; i < nproc; i++)
      if (items_distants[i].size_array() > 0)
        nouveaux_voisins.append_array(i);

    // On ajoute les nouveaux joints
    ajouter_joints(zone, nouveaux_voisins);
    Process::Journal() << " News joints created : (ArrOfInt) "
                       << nouveaux_voisins << finl;
  }

  Joints& joints_non_const = zone.faces_joint();
  const int nb_new_joints = joints_non_const.size();
  // Remplissage des tableaux d'items distants
  for (int i_joint = 0; i_joint < nb_new_joints; i_joint++)
    {
      Joint& joint = joints_non_const[i_joint];
      const int pe = joint.PEvoisin();
      ArrOfInt& joint_items_distants = joint.set_joint_item(type_item).set_items_distants();
      joint_items_distants = items_distants[pe];
      Process::Journal() << " Joint with PE:" << pe
                         << " Number of remote items : "
                         << joint_items_distants.size_array() << finl;
    }
  // Remplissage du nombre d'items virtuels
  calculer_nb_items_virtuels(joints_non_const, type_item);
}
inline Nom endian()
{
  int x = 1;
  if(*(char *)&x == 1)
    return "little-endian";
  else
    return "big-endian";
}
// Description:
//  Ajoute des joints avec tous les pe de pe_voisins.
//  Pour que l'ensemble des joints soit symetrique,
//  on en cree aussi un joint sur le processeur destination:
//   Si A ajoute un joint avec B, alors B ajoute un joint avec A.
//  On trie les joints par ordre croissant du numero de PE.
//  ATTENTION: les joints sont donc reordonnes !
//  On met dans pe_voisins la liste des joints effectivement crees.
// Precondition:
//  Methode a appeler simultanement par tous les processeurs.
void Scatter::ajouter_joints(Zone& zone,
                             ArrOfInt& pe_voisins)
{
  Joints& joints = zone.faces_joint();
  ArrOfInt liste_pe;
  pe_voisins.set_smart_resize(1);
  liste_pe.set_smart_resize(1);
  // Rendre les joints symetriques (si A->B alors B->A) :
  {
    // On met dans liste pe la "transposee" de la liste des pe_voisins:
    // liste des processeurs chez qui mon numero est dans "pe_voisins".
    reverse_send_recv_pe_list(pe_voisins, liste_pe);
    const int n = liste_pe.size_array();
    // On concatene les deux listes.
    for (int i = 0; i < n; i++)
      pe_voisins.append_array(liste_pe[i]);
    array_trier_retirer_doublons(pe_voisins);
    liste_pe.resize_array(0);
  }
  // On retire de pe_voisins les pe pour lesquels un joint existe deja
  {
    const int n = joints.size();
    liste_pe.resize_array(n);
    for (int i = 0; i < n; i++)
      liste_pe[i] = joints[i].PEvoisin();
    array_retirer_elements(pe_voisins, liste_pe);
  }
  // Ajouter les nouveaux joints et trier par ordre croissant
  // Aujourd'hui (2/11/2005) Liste::inserer ne permet pas d'inserer
  // en debut de liste. Donc inutilisable. Methode bourrin:
  {
    const int n = pe_voisins.size_array();
    for (int i = 0; i < n; i++)
      ajouter_joint(zone, pe_voisins[i]);
    trier_les_joints(joints);
  }
}

// Description:
//  Methode generique pour calculer l'espace distant d'un type d'items geometrique
//  (sommet, face, arete) en fonction de l'espace distant des elements:
//  Les "type_item" distants (pour type_item = sommet face ou arete) sont
//  les "type_item" attaches aux elements distants.
//  Exemple : les sommets distants sont tous les sommets de tous les elements
//  distants.
// Voir aussi:
//  Scatter::calculer_espace_distant_sommets
//  Scatter::calculer_espace_distant_faces
// Parametre: zone
// Signification: bah, la zone quoi...
// Parametre: type_item
// Signification: le type des items dont on veut calculer l'espace distant
// Parametre: connectivite_elem_item
// Signification: le tableau qui donne pour chaque element de la zone les
//  indices des items de cet element. On n'utilise que la partie reele du
//  tableau (logiquement, la partie virtuelle n'existe pas encore).
//  (exemple: zone().les_elems() pour type_item==SOMMET ou zone_VF().face_sommets()
//   pour type_item==FACE)
// Parametre: nb_items_reels
// Signification: le nombre de "type_item" reels
// Parametre: items_lies
// Signification: si le tableau est non vide, il doit etre de taille nb_items_reels.
//  Dans ce cas, il permet de forcer la propriete suivante :
//   "si l'item i est distant, alors l'item items_lies[i] est distant aussi".
//  Ce tableau est utilise pour inclure les sommets periodiques virtuels associes.
//  (voir calculer_espace_distant_sommets).
// Precondition: les elements distants et les items_communs du type_item
//  doivent avoir ete remplis.
static void calculer_espace_distant_item(Zone& la_zone,
                                         const Joint::Type_Item type_item,
                                         const IntTab& connectivite_elem_item,
                                         const int nb_items_reels,
                                         const ArrOfInt& items_lies)
{
  const Joints& joints                 = la_zone.faces_joint();
  const int   nb_joints              = joints.size();
  const int   nproc                  = Process::nproc();
  const int   nb_items_par_element   = connectivite_elem_item.dimension(1);
  // Les type_item a envoyer a chaque processeur:
  VECT(ArrOfInt) items_to_send(nproc);
  // Un tableau temporaire;
  ArrOfInt liste_items;
  liste_items.set_smart_resize(1);

  // Est-ce qu'il y a des items lies ?
  const int flag_items_lies = (items_lies.size_array() > 0);
  assert(flag_items_lies == 0 || items_lies.size_array() == nb_items_reels);


  for (int i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const Joint&     joint          = joints[i_joint];
      const int     pe_voisin      = joint.PEvoisin();
      const ArrOfInt& esp_dist_elems = joint.joint_item(Joint::ELEMENT).items_distants();
      const int     nb_elems_dist  = esp_dist_elems.size_array();
      liste_items.resize_array(0);
      // On met dans liste_items tous les items de tous les elements
      // qui sont dans esp_dist_elems:
      for (int i_elem = 0; i_elem < nb_elems_dist; i_elem++)
        {
          const int elem = esp_dist_elems[i_elem];
          for (int i_item = 0; i_item < nb_items_par_element; i_item++)
            {
              const int item = connectivite_elem_item(elem, i_item);
              if (item>-1)
                {
                  liste_items.append_array(item);
                  // Si un item est lie a l'item courant, on envoie aussi l'item lie.
                  if (flag_items_lies)
                    {
                      const int item_lie = items_lies[item];
                      if (item_lie != item)
                        {
                          assert(item_lie >= 0 && item_lie < nb_items_reels);
                          assert(items_lies[item_lie] == item_lie); // chaine de liaisons interdite
                          liste_items.append_array(item_lie);
                        }
                    }
                }
            }
        }
      array_trier_retirer_doublons(liste_items);
      // Ces items doivent etre envoyes au processeur voisin:
      items_to_send[pe_voisin] = liste_items;
    }
  // Calcul des espaces distants en fonction de "items_to_send"
  Scatter::calculer_espace_distant(la_zone, nb_items_reels, items_to_send, type_item);
}

// Description:
//  En fonction de l'espace distant des elements, calcule l'espace
//  distant des sommets. Pour chaque joint, on envoie au processeur voisin
//  l'ensemble des sommets de tous les elements du joint.
//  C'est le processeur proprietaire du sommet
//  (plus petit pe qui le possede) qui le met dans son espace distant.
//  Attention, on cree de nouveaux joints.
//  On remplit les tableaux
//   dom.zone(0).faces_joint(i).joint_item(Joint::SOMMET).items_distants();
void Scatter::calculer_espace_distant_sommets(Domaine& dom, const Noms& liste_bords_perio)
{
  if (Process::je_suis_maitre())
    Cerr << "Scatter::calculer_espace_distant_sommets : start" << finl;

  Zone&          zone                  = dom.zone(0);
  const IntTab& connectivite_elem_som = zone.les_elems();
  const int   nb_sommets_reels      = dom.nb_som();

  ArrOfInt renum_som_perio(nb_sommets_reels);
  // Initialisation du tableau renum_som_perio
  for (int i = 0; i < nb_sommets_reels; i++)
    renum_som_perio[i] = i;
  Reordonner_faces_periodiques::renum_som_perio(dom, liste_bords_perio, renum_som_perio,
                                                0 /* ne pas calculer pour les sommets virtuels */);

  calculer_espace_distant_item(zone,
                               Joint::SOMMET,
                               connectivite_elem_som,
                               nb_sommets_reels,
                               renum_som_perio);
}

// Description:
//  Idem que Scatter::calculer_espace_distant_sommets pour les faces
void Scatter::calculer_espace_distant_faces(Zone& zone,
                                            const int nb_faces_reelles,
                                            const IntTab& elem_faces)
{
  if (Process::je_suis_maitre())
    Cerr << "Scatter::calculer_espace_distant_faces : start" << finl;

  ArrOfInt tableau_vide;

  calculer_espace_distant_item(zone,
                               Joint::FACE,
                               elem_faces,
                               nb_faces_reelles,
                               tableau_vide);
}

// Description:
//  Idem que Scatter::calculer_espace_distant_sommets pour les aretes
void Scatter::calculer_espace_distant_aretes(Zone& zone,
                                             const int nb_aretes_reelles,
                                             const IntTab& elem_aretes)
{
  if (Process::je_suis_maitre())
    Cerr << "Scatter::calculer_espace_distant_aretes : start" << finl;
  ArrOfInt tableau_vide;
  calculer_espace_distant_item(zone,
                               Joint::ARETE,
                               elem_aretes,
                               nb_aretes_reelles,
                               tableau_vide);
}

// Description:
//  On suppose que chaque joint[i].joint_item(type_item).items_communs()
//  contient les indices locaux des items de joint communs dans le meme
//  ordre sur les deux processeurs (local et voisin)
//  On remplit renum_items_communs :
//   colonne 0=contenu du tableau items_communs sur le PE voisin
//   colonne 1=contenu du tableau items_communs sur le PE local
void Scatter::calculer_renum_items_communs(Joints& joints,
                                           const Joint::Type_Item type_item)
{
  // Il suffit d'envoyer au voisin le tableau _faces dans l'ordre
  // pour qu'il ait les indices des faces sur l'autre pe.

  const int nb_joints = joints.size();
  int       i_joint;
  Schema_Comm  schema_comm;
  ArrOfInt liste_voisins(nb_joints);
  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    liste_voisins[i_joint] = joints[i_joint].PEvoisin();
  schema_comm.set_send_recv_pe_list(liste_voisins, liste_voisins);

  schema_comm.begin_comm();

  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const Joint& joint     = joints[i_joint];
      const int  pe_voisin = joint.PEvoisin();
      const ArrOfInt& items_communs =
        joint.joint_item(type_item).items_communs();
      schema_comm.send_buffer(pe_voisin) << items_communs;
    }

  schema_comm.echange_taille_et_messages();

  // Le tableau items communs recu du pe voisin:
  ArrOfInt items_communs_voisin;
  items_communs_voisin.set_smart_resize(1);

  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      Joint&           joint         = joints[i_joint];
      const int     pe_voisin     = joint.PEvoisin();
      const ArrOfInt& items_communs = joint.joint_item(type_item).items_communs();
      const int     nb_items      = items_communs.size_array();
      schema_comm.recv_buffer(pe_voisin) >> items_communs_voisin;

      assert(nb_items == items_communs_voisin.size_array());

      IntTab& renum_items_communs = joint.set_joint_item(type_item).set_renum_items_communs();
      renum_items_communs.resize(nb_items, 2);
      // L'indice de la face de joint sur l'autre PE est dans tmp(i,1)
      for (int i = 0; i < nb_items; i++)
        {
          renum_items_communs(i,0) = items_communs_voisin[i];
          renum_items_communs(i,1) = items_communs[i];
        }
    }

  schema_comm.end_comm();
}

// Description: construction d'un MD_Vector_std a partir des informations de joint du domaine
//  pour le type d'item demande.
void Scatter::construire_md_vector(const Domaine& dom, int nb_items_reels, const Joint::Type_Item type_item, MD_Vector& md_vector)
{
  const Joints& joints  = dom.zone(0).faces_joint();
  const int nb_joints = joints.size();

  ArrOfInt pe_voisins(nb_joints);
  VECT(ArrOfInt) items_to_send(nb_joints);
  VECT(ArrOfInt) items_to_recv(nb_joints);
  VECT(ArrOfInt) blocs_to_recv(nb_joints);

  // drapeau indique si l'item (commun) est recu d'un processeur
  ArrOfBit flags(nb_items_reels);
  flags = 0;

  int nitems_tot = nb_items_reels;
  const int moi = me();

  for (int i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const int pe = joints[i_joint].PEvoisin();
      pe_voisins[i_joint] = pe;
      const Joint_Items& joint = joints[i_joint].joint_item(type_item);
      {
        // Traitement des items communs
        const ArrOfInt& items_communs = joint.items_communs();
        const int n = items_communs.size_array();

        // Les joints doivent arriver dans l'ordre croissant du numero de pe
        // sinon l'algo suivant ne marche pas:
        assert((i_joint == 0) || (pe > joints[i_joint-1].PEvoisin()));
        if (pe > moi)
          {
            // Je dois envoyer ces items au processeur voisin
            ArrOfInt& dest = items_to_send[i_joint];
            dest.set_smart_resize(1);
            for (int i = 0; i < n; i++)
              {
                const int item = items_communs[i];
                if (!flags[item])
                  {
                    // item pas recu d'un processeur
                    dest.append_array(item);
                  }
              }
          }
        else
          {
            // Je recois cet item d'un autre processeur
            ArrOfInt& dest = items_to_recv[i_joint];
            dest.set_smart_resize(1);
            for (int i = 0; i < n; i++)
              {
                const int item = items_communs[i];
                if (!flags.testsetbit(item))
                  {
                    // item pas encore recu d'un processeur
                    dest.append_array(item);
                  }
              }
          }
      }
      // Traitement des items distants et virtuels
      {
        const int nitems_virt = joint.nb_items_virtuels();
        ArrOfInt& dest = blocs_to_recv[i_joint];
        if (nitems_virt > 0)
          {
            dest.resize_array(2, Array_base::NOCOPY_NOINIT);
            // Definition du bloc d'items virtuels pour le processeur voisin
            dest[0] = nitems_tot;
            dest[1] = nitems_tot + nitems_virt;
            nitems_tot += nitems_virt;
          }
      }
      {
        const ArrOfInt& items_distants = joint.items_distants();
        const int n = items_distants.size_array();
        ArrOfInt& dest = items_to_send[i_joint];
        const int index = dest.size_array();
        dest.resize_array(index + n, Array_base::COPY_NOINIT); // copier les anciennes valeurs !
        dest.inject_array(items_distants, n, index /* dest index */, 0 /* src index */);
      }
    }

  MD_Vector_std md(nitems_tot, nb_items_reels, pe_voisins, items_to_send, items_to_recv, blocs_to_recv);
  md_vector.copy(md);

  // Verification que le md_vector est valide (tailles en send correspondant aux tailles en recv)
  if (comm_check_enabled())
    {
      IntVect toto;
      MD_Vector_tools::creer_tableau_distribue(md_vector, toto);
      toto = 0;
      toto.echange_espace_virtuel();
    }
}

// Fonction tri (selon la premiere colonne du tableau)
static True_int fct_tri_table_inverse(const void *ptr1, const void *ptr2)
{
  const int i1 = *(int*)ptr1;
  const int i2 = *(int*)ptr2;
#ifdef INT_is_64_
  if (i1 == i2)
    return 0;
  if (i1>i2)
    return 1;
  return -1;
#else
  return i1 - i2;
#endif

}

// .DESCRIPTION
//  Cette classe fournit les outils pour construire l'espace
//  virtuel d'un tableau contenant des indices d'entites geometriques
//  (sommets, elements, faces). Elle gere en particulier la
//  renumerotation des elements virtuels.
class Traduction_Indice_Global_Local
{
public:
  Traduction_Indice_Global_Local() {};
  void initialiser(const MD_Vector& md_items);
  void reset();
  void traduire_indice_local_vers_global(const ArrOfInt& indices_locaux, ArrOfInt& indices_globaux, int n) const;
  int traduire_indice_global_vers_local(const ArrOfInt& indices_globaux, ArrOfInt& indices_locaux) const;
  int traduire_espace_virtuel(IntVect& tableau) const;

  static int chercher_table_inverse(const IntTab& table_inverse,
                                    const int sommet_global);

private:
  // Metadata des indices qu'on va renumeroter :
  MD_Vector md_items_;
  int premier_indice_global_;
  // Tableau distribue (avec espaces virtuels et items communs)
  // contenant pour toutes les entites a indexer  (reelles et virtuelles)
  // un indice global.
  // (si type_table_==SOMMETS, table_[i] est l'indice global du sommet i)
  IntVect table_;
  // Table permettant d'inverser la numerotation, classee par ordre
  // croissant de l'indice global :
  // * colonne 0 : l'indice global de l'entite
  // * colonne 1 : l'indice local de l'entite
  IntTab table_inverse_;
};

// Description:
//  Initialise le dictionnaire
// Precontition:
//  Les espaces distants des entites utilisees doivent avoir ete calculees
void Traduction_Indice_Global_Local::initialiser(const MD_Vector& md_items)
{
  md_items_ = md_items;

  // Construction de "table" : on cree un numero global pour les entites reelles
  // (indice de l'entite + nombre total d'entites sur les processeurs de rang inferieur)
  // puis on echange l'espace virtuel de ce tableau, on obtient pour chaque entite
  // reelle ou virtuelle son numero global.

  table_.reset();
  MD_Vector_tools::creer_tableau_distribue(md_items, table_);

  const int nb_entites = md_items.valeur().get_nb_items_tot();
  const int decal = mppartial_sum(nb_entites);
  premier_indice_global_ = decal;

  int i;
  for (i = 0; i < nb_entites; i++)
    table_[i] = i + decal;
  table_.echange_espace_virtuel();

  // Construction de la table_inverse contenant les indices non triviaux
  // (pour lesquels table_[i] != i + decal apres l'echange)
  // tri par ordre croissant du numero global.
  const int nb_entites_tot = table_.size_totale();
  table_inverse_.resize(0, 2);
  table_inverse_.set_smart_resize(1);
  for (i = 0; i < nb_entites_tot; i++)
    {
      if (table_[i] != i + decal)
        table_inverse_.append_line(table_[i], i);
    }
  // insure se plaint .. regarder si il a raison
  if (table_inverse_.size_array()>0)
    qsort(table_inverse_.addr(),
          table_inverse_.dimension(0),
          2 * sizeof(int),
          fct_tri_table_inverse);
}

void Traduction_Indice_Global_Local::reset()
{
  md_items_.detach();
  table_.reset();
  table_inverse_.reset();
}

// Description:
//  Cherche i tel que table_inverse(i, 0) == sommet_global,
//  et renvoie table_inverse(i, 1) (l'indice local du sommet).
//  Si le sommet n'est pas trouve dans la table, renvoie -1.
//  La table_inverse doit etre triee par ordre croissant de la colonne 0.
//  La table_inverse ne doit pas avoir d'espace virtuel.
int Traduction_Indice_Global_Local::chercher_table_inverse(const IntTab& table_inverse,
                                                           const int sommet_global)
{
  // Algorithme : recherche par dichotomie:
  int imin = 0;
  int imax = table_inverse.dimension(0) - 1;
  // Si un seul element dans la table, on ne passe pas dans while
  //        (donc initialisation a table_inverse(0, 0))
  // Sinon, si aucun element, il ne faut pas que valeur == sommet_global,
  //        sinon, valeur quelconque, elle sera ecrasee dans le while.
  int valeur;
  if (imax == 0)
    valeur = table_inverse(0, 0);
  else
    valeur = sommet_global - 1;

  while (imax > imin)
    {
      const int milieu = (imin + imax) >> 1; // (min+max)/2
      valeur = table_inverse(milieu, 0);
      const int compare = valeur - sommet_global;
      if (compare < 0)
        imin = milieu + 1;
      else if (compare > 0)
        imax = milieu - 1;
      else
        imin = imax = milieu;
    }
  int resu = -1;
  valeur = table_inverse(imin, 0);
  if (valeur == sommet_global)
    resu = table_inverse(imin, 1);
  return resu;
}

// Description:
//  Transforme les indices locaux en indices globaux a l'aide la
//  "table_" (voir initialiser). On fait :
//  Pour debut <= i < debut+nb
//   indices_globaux[i] = table_[indices_locaux[i]]
//   si indices_locaux[i] < 0 alors indices_globaux[i] = -1
void Traduction_Indice_Global_Local::traduire_indice_local_vers_global(
  const ArrOfInt& indices_locaux,
  ArrOfInt& indices_globaux,
  int nb_items_a_traiter) const
{
  for (int i = 0; i < nb_items_a_traiter; i++)
    {
      const int i_loc = indices_locaux[i];
      const int i_glob = (i_loc < 0) ? -1 : table_[i_loc];
      indices_globaux[i] = i_glob;
    }
}

// Description:
//  Pour debut <= i < debut+nb
//   indices_locaux[i] = chercher l'indice local de "indices_globaux[i]"
// Parametre: indices_globaux
// Signification: le tableau des indices globaux a traduire
// Parametre: indices_locaux
// Signification: en sortie, les indices locaux ou -1 si l'indice global
//                n'a pas ete trouve.
// Valeur de retour: nombre d'indices non trouves (indices globaux qui
// ne correspondent a aucun indice local).
int Traduction_Indice_Global_Local::traduire_indice_global_vers_local(
  const ArrOfInt& indices_globaux,
  ArrOfInt& indices_locaux) const
{
  assert(indices_globaux.size_array() == indices_locaux.size_array());
  int i;
  int nb_erreurs = 0;
  const int nb_indices = indices_globaux.size_array();
  const int size_table = table_.size_array();
  for (i = 0; i < nb_indices; i++)
    {
      const int i_glob = indices_globaux[i];
      int i_loc;
      if (i_glob < 0)
        {
          // Indice negatif, on considere que c'est normal,
          // c'est un marqueur "indice vide".
          i_loc = -1;
        }
      else
        {
          // On teste si l'item n'est pas renumerote
          i_loc = i_glob - premier_indice_global_;
          if (i_loc < 0 || i_loc >= size_table || table_[i_loc] != i_glob)
            {
              // non, il faut inverser la table:
              i_loc = chercher_table_inverse(table_inverse_, i_glob);
            }
          if (i_loc < 0)
            nb_erreurs++;
        }
      indices_locaux[i] = i_loc;
    }
  return nb_erreurs;
}

// Description:
//  A partir d'un tableau dont la structure d'espace virtuel est initialisee
//  (descripteurs elements distants et virtuels, items communs)
//  et contenant des indices compatibles avec le contenu des tables
//  (indices de sommets ou d'elements selon type_table_),
//  on remplit les elements virtuels du "tableau" en fonction des elements
//  distants et on traduit les indices en indices locaux.
//  (exemple, voir construire_espace_virtuel_elements et
//   construire_espace_virtuel_faces).
// Valeur de retour: nombre d'indices qui n'ont pas pu etre traduits
//  (par exemple, le sommet reference n'existe pas sur le processeur voisin)
int Traduction_Indice_Global_Local::traduire_espace_virtuel(IntVect& tableau) const
{
  // On cree une copie du tableau, dans laquelle on met les indices globaux:
  IntVect indices_globaux;
  // Ne copier que la structure
  indices_globaux.copy(tableau, ArrOfInt::NOCOPY_NOINIT);

  const int nb_items_reels    = tableau.size_reelle();
  const int nb_items_tot      = tableau.size_totale();
  const int nb_items_virtuels = nb_items_tot - nb_items_reels;

  // On traduit les items reels en indices globaux:
  traduire_indice_local_vers_global(tableau, indices_globaux, nb_items_reels);
  // On remplit les cases virtuelles
  indices_globaux.echange_espace_virtuel();

  // On retraduit uniquement les items virtuels du "tableau" en indices locaux:
  ArrOfInt src;
  ArrOfInt dest;
  src.ref_array(indices_globaux, nb_items_reels /*debut*/, nb_items_virtuels /*taille*/);
  dest.ref_array(tableau, nb_items_reels /*debut*/, nb_items_virtuels /*taille*/);
  const int nb_erreurs = traduire_indice_global_vers_local(src, dest);
  return nb_erreurs;
}

// Description:
//  Construit la structure items_communs + espaces virtuels d'un tableau contenant
//  des indices d'items geometriques, indexe par un autre type d'item geometrique.
//  Exemple: tableau indexe par md_indice, contenant des indices md_valeur:
//     type_indice  type_valeur   exemple de tableau:
//      element      sommet       zone.les_elems()
//      face         sommet       faces_sommets
//      element      face         elem_faces
//      face         element      faces_voisins
//      element      element      ?
//      element      arete        elem_aretes
//  Nb_valeurs_max est le nombre d'items reels de type "type_valeur".
void Scatter::construire_espace_virtuel_traduction(const MD_Vector& md_indice,
                                                   const MD_Vector& md_valeur,
                                                   IntTab& tableau,
                                                   const int error_is_fatal)
{
  if (tableau.dimension_tot(0) != md_indice.valeur().get_nb_items_reels()
      && (tableau.dimension_tot(0) != md_indice.valeur().get_nb_items_tot()))
    {
      Cerr << "[PE " << Process::me()
           << "] Error in Scatter::construire_espace_virtuel_traduction\n"
           << " the array does not have the good dimension on input" << finl;
      exit();
    }
  // Construction du dictionnaire indice global/indice local
  // pour les valeurs du tableau
  Traduction_Indice_Global_Local dictionnaire_indices;
  dictionnaire_indices.initialiser(md_valeur);

  // Construit la structure d'espaces virtuels du "tableau"
  if (!(tableau.get_md_vector() == md_indice))
    MD_Vector_tools::creer_tableau_distribue(md_indice, tableau, Array_base::COPY_NOINIT);

  // Remplissage des valeurs vituelles du "tableau"
  const int nb_erreurs =
    dictionnaire_indices.traduire_espace_virtuel(tableau);

  if (nb_erreurs > 0 && error_is_fatal)
    {
      Cerr << "[PE " << Process::me()
           << "] Error in Scatter::construire_espace_virtuel_traduction\n"
           << " some indices of values were not found in\n"
           << " the local area : it missing virtual items"
           << finl;
      exit();
    }
}


// Description:
//  Reordonne les faces de joint de sorte qu'elles apparaissent dans le meme
//  ordre sur chaque couple de processeur voisin. En pratique, pour un couple
//  pe1 < pe2, pe1 envoie ses faces de joint a pe2 et pe2 les traduit en indices
//  de sommets locaux. Les faces de joint du PE2 ne sont donc pas utilisees.
// Precondition:
//  Les sommets distants doivent avoir ete calcules (utilisation du dictionnaire)
void Scatter::reordonner_faces_de_joint(Domaine& dom)
{
  // Construction du dictionnaire indice global/indice local
  // pour les sommets du domaine:
  Traduction_Indice_Global_Local dictionnaire_indices;
  dictionnaire_indices.initialiser(dom.les_sommets().get_md_vector());

  Schema_Comm schema_comm;
  Joints&      joints    = dom.zone(0).faces_joint();
  const int nb_joints = joints.size();
  const int moi       = Process::me();
  int       i_joint;

  // Remplissage des listes de destinataires:
  //  on envoie aux voisins de rang superieur et on recoit des voisins de rang inf.
  ArrOfInt send_list;
  ArrOfInt recv_list;
  send_list.set_smart_resize(1);
  recv_list.set_smart_resize(1);

  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const int pe_voisin = joints[i_joint].PEvoisin();
      if (pe_voisin > moi)
        send_list.append_array(pe_voisin);
      else
        recv_list.append_array(pe_voisin);
    }

  schema_comm.set_send_recv_pe_list(send_list, recv_list);

  schema_comm.begin_comm();
  // Envoi des faces de joint, traduites en indices de sommets globaux
  IntTab faces_num_global;
  faces_num_global.set_smart_resize(1);
  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const Joint&   joint         = joints[i_joint];
      const int   pe_voisin     = joint.PEvoisin();
      if (pe_voisin > moi)
        {
          const IntTab& faces_sommets = joint.faces().les_sommets();
          if (faces_sommets.dimension(0) > 0)
            {
              faces_num_global.resize(faces_sommets.dimension(0), faces_sommets.dimension(1));
              dictionnaire_indices.traduire_indice_local_vers_global(faces_sommets,
                                                                     faces_num_global,
                                                                     faces_sommets.size_array());
            }
          else
            faces_num_global.resize(0);

          schema_comm.send_buffer(pe_voisin) << faces_num_global;
        }
    }
  schema_comm.echange_taille_et_messages();
  // Reception des faces de joint et traduction en indices locaux
  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      Joint&       joint     = joints[i_joint];
      const int pe_voisin = joint.PEvoisin();
      if (pe_voisin < moi)
        {
          IntTab& faces_sommets = joint.faces().les_sommets();
          schema_comm.recv_buffer(pe_voisin) >> faces_num_global;
          if (faces_sommets.dimension(0) != faces_num_global.dimension(0))
            {
              Cerr << "[PE " << moi
                   << "] Error in Scatter::reordonner_faces_de_joint:\n"
                   << " the number of joint faces is not identical to the PE "
                   << pe_voisin << finl;
              exit();
            }
          const int nb_erreurs =
            dictionnaire_indices.traduire_indice_global_vers_local(faces_num_global,
                                                                   faces_sommets);
          if (nb_erreurs > 0)
            {
              Cerr << "[PE " << moi
                   << "] Error in Scatter::reordonner_faces_de_joint:\n"
                   << " The faces of the joint with PE " << pe_voisin
                   << " use of unknown nodes" << finl;
              exit();
            }
        }
    }
  schema_comm.end_comm();
}

// Description: Methode outil: renvoie une liste complete de tous les
// sommets de joint (sommets des faces + sommets isoles), triee et
// sans doublons
static void calculer_liste_complete_sommets_joint(const Joint& joint, ArrOfInt& liste_sommets)
{
  liste_sommets = joint.joint_item(Joint::SOMMET).items_communs();
#if 0
  liste_sommets.set_smart_resize(1);
  // On prend tous les sommets des faces de joint:
  const IntTab& som_faces = joint.faces().les_sommets();
  liste_sommets = ref_cast(ArrOfInt,som_faces);
  // On ajoute tous les sommets isoles :
  const ArrOfInt& som_isoles = joint.sommets();
  const int n = som_isoles.size_array();
  for (int i = 0; i < n; i++)
    liste_sommets.append_array(som_isoles[i]);
  // Retirer les doublons de la liste
  array_trier_retirer_doublons(liste_sommets);
#endif
}

inline int arete_de_sommets_Si_et_Sj(const int& Si, const int& Sj, const int& arete, const IntTab& aretes_som)
{
  if ( (aretes_som(arete,0) == Si && aretes_som(arete,1) == Sj)
       || (aretes_som(arete,1) == Si && aretes_som(arete,0) == Sj) )
    return 1;
  else
    return 0;
}

// Description: Methode outil: renvoie une liste complete de tous les
// aretes de joint (aretes des faces + aretes isolees), triee et
// sans doublons
static void calculer_liste_complete_aretes_joint(const Joint& joint, ArrOfInt& liste_aretes)
{
  // Construction de la liste des aretes communes liste_aretes
  liste_aretes.set_smart_resize(1);
  ///////////////////////////////////////////////////////
  // Recherche des aretes de joint sur les faces de joint
  ///////////////////////////////////////////////////////
  int nb_faces_joint=joint.faces().nb_faces();
  int nb_som_faces=joint.faces().nb_som_faces();
  const IntTab& sommet=joint.faces().les_sommets();
  const Domaine& dom=joint.zone().domaine();
  const DoubleTab& coord=dom.coord_sommets();
  const IntTab& aretes_som=joint.zone().aretes_som();
  ArrOfInt aretes(1);
  int compteur=0;
  DoubleTab positions(1,Objet_U::dimension);
  ArrOfInt som_faces(nb_faces_joint*nb_som_faces);
  // On parcourt 2 a 2 les sommets de chaque face du joint
  for (int face=0; face<nb_faces_joint; face++)
    for (int i=0; i<nb_som_faces; i++)
      {
        int Si = sommet(face,i);
        som_faces(face*nb_som_faces+i)=Si;
        for (int j=i; j<nb_som_faces; j++)
          {
            int Sj = sommet(face,j);
            // Calcul du point C entre 2 sommets Si et Sj
            for (int comp=0; comp<Objet_U::dimension; comp++)
              positions(0,comp)=0.5*(coord(Si,comp)+coord(Sj,comp));
            dom.zone(0).chercher_aretes(positions,aretes);
            // Si on trouve une arete dont le centre coincide avec le point C
            // et dont les sommets sont identiques a Si et Sj, on ajoute l'arete a la liste
            if (aretes(0)>=0 && arete_de_sommets_Si_et_Sj(Si, Sj, aretes[0], aretes_som))
              {
                compteur++;
                liste_aretes.append_array(aretes[0]);
              }
          }
      }
  Process::Journal() << "common edges found on faces of joint with " << joint.PEvoisin() << " :" << compteur << finl;
  /////////////////////////////////////////////////////////////////////////
  // Recherche des aretes de joint isolees sur les sommets de joints isoles
  /////////////////////////////////////////////////////////////////////////
  // joint.sommets() contient parfois tous les sommets !
  // Donc on construit un tableau som_isoles
  ArrOfInt som_isoles;
  // Met tous les sommets dans som_isoles (isoles+issus des faces de joint):
  calculer_liste_complete_sommets_joint(joint, som_isoles);
  // On trie som_faces et on supprime les doublons
  array_trier_retirer_doublons(som_faces);
  // Supprime tous les sommets de som_isoles contenus dans som_faces
  array_retirer_elements(som_isoles, som_faces);
  // Supprime les sommets des faces de joint
  const int n = som_isoles.size_array();
  Process::Journal() << "number of isolated nodes: " << n << finl;
  Process::Journal() << "number of nodes of faces of joint: " << 3*sommet.dimension(0) << finl;

  compteur=0;
  // On parcourt 2 a 2 les sommets isoles
  for (int i = 0; i < n; i++)
    for (int j = i; j < n; j++)
      {
        // Calcul du point C entre 2 sommets Si et Sj
        int Si = som_isoles(i);
        int Sj = som_isoles(j);
        for (int comp=0; comp<Objet_U::dimension; comp++)
          positions(0,comp)=0.5*(coord(Si,comp)+coord(Sj,comp));
        dom.zone(0).chercher_aretes(positions,aretes);
        // Si on trouve une arete dont le centre coincide avec le point C
        // et dont les sommets sont identiques a Si et Sj, on ajoute l'arete a la liste
        if (aretes(0)>=0 && arete_de_sommets_Si_et_Sj(Si, Sj, aretes[0], aretes_som))
          {
            compteur++;
            liste_aretes.append_array(aretes[0]);
          }
      }
  Process::Journal() << "common edges found isolated on joint with " << joint.PEvoisin() << " :" << compteur << finl;
  // Retirer les doublons de la liste
  array_trier_retirer_doublons(liste_aretes);
}

static void calculer_liste_complete_items_joint(const Joint& joint, const Joint::Type_Item type_item, ArrOfInt& liste_items)
{
  switch(type_item)
    {
    case Joint::SOMMET:
      calculer_liste_complete_sommets_joint(joint, liste_items);
      break;
    case Joint::ARETE:
      calculer_liste_complete_aretes_joint(joint, liste_items);
      break;
    default:
      Cerr << "Error in Scatter::calculer_liste_complete_items_joint" << finl;
      Cerr << "Type of item not expected." << finl;
      Process::exit();
    }
}

// Description:
//  Les algorithmes actuels pour le periodique (assembleur P1B, OpDivElem P1B)
//  ont besoin que pour chaque face virtuelle periodique, la face opposee soit
//  aussi virtuelle. Ceci n'est pas assure a la sortie de la methode
//  calculer_elements_distants. Cette methode ajoute aux elements distants les
//  elements manquants pour assurer cette condition:
//  Si un element est distant pour un PE donne est voisin d'une face periodique,
//  on ajoute a l'espace distant l'element adjacent a la face opposee.
void Scatter::corriger_espace_distant_elements_perio(Domaine& dom,
                                                     const Noms& liste_bords_periodiques)
{
  if (Process::je_suis_maitre())
    Cerr << "Correction of remote spaces of the elements for the periodic faces" << finl;

  Zone& zone = dom.zone(0);
  const int nb_elem = zone.nb_elem();
  const IntTab& les_elems = zone.les_elems();

  // Ce tableau contiendra pour un bord periodique donne:
  //  si l'element i est adjacent a une face de ce bord,
  //  element_oppose[i] est le numero de l'element adjacent a la face
  //  opposee sur ce bord.
  // -1 sinon.
  ArrOfInt element_oppose(nb_elem);

  Static_Int_Lists connectivite_som_elem;
  const int nb_sommets = dom.nb_som();
  construire_connectivite_som_elem(nb_sommets,
                                   les_elems,
                                   connectivite_som_elem,
                                   0 /* ne pas inclure les sommets virtuels */);

  const int nb_som_face = zone.type_elem().nb_som_face();
  ArrOfInt une_face(nb_som_face);
  ArrOfInt elems_voisins;
  elems_voisins.set_smart_resize(1);

  const int nb_joints = zone.nb_joints();
  const int nb_bords_perio = liste_bords_periodiques.size();

  // Marqueurs des elements distants existants:
  ArrOfBit marqueurs_elements_distants(nb_elem);
  marqueurs_elements_distants = 0;

  // Le fait d'ajouter un element dans un espace distant pour un bord donne
  // peut entrainer un probleme sur un autre bord (l'element oppose de l'element
  // ainsi ajoute peut manquer pour une autre direction de periodicite).
  // Il faut dont iterer jusqu'a ce que plus rien ne bouge
  int nb_elements_ajoutes = 0;
  do
    {
      nb_elements_ajoutes = 0;
      for (int i_bord_perio = 0; i_bord_perio < nb_bords_perio; i_bord_perio++)
        {
          const Nom& nom_bord = liste_bords_periodiques[i_bord_perio];
          const Bord& bord = zone.bord(nom_bord);
          const IntTab& faces_sommets = bord.les_sommets_des_faces();
          const int nb_faces = bord.nb_faces();

          // Premiere etape, reperer les element opposes pour ce bord periodique
          // On boucle sur la premiere moitie de la frontiere.
          // ATTENTION: on suppose que les faces de bords sont ordonnees : d'abord toutes les faces
          // d'un cote du domaine periodique, puis, dans le meme ordre, les faces opposees.
          element_oppose = -1;

          for (int i_face = 0; i_face < nb_faces / 2; i_face++)
            {
              // Pour chaque face, trouver l'element adjacent a cette face et a la face opposee
              // Boucle sur la face et la face opposee
              int elem0 = -1; // Les deux elements opposes de cette face periodique
              int elem1 = -1;
              for (int quel_cote = 0; quel_cote < 2; quel_cote++)
                {
                  const int face = i_face + quel_cote * nb_faces / 2;
                  int i;
                  for (i = 0; i < nb_som_face; i++)
                    une_face[i] = faces_sommets(face, i);
                  find_adjacent_elements(connectivite_som_elem, une_face, elems_voisins);
                  const int n = elems_voisins.size_array();
                  if (n != 1)
                    {
                      Cerr << "Error in Scatter::corriger_espace_distant_elements_perio: \n"
                           << " The face " << i_face << " of boundary " << nom_bord << " has "
                           << n << " neighbors." << finl;
                      Process::exit();
                    }
                  if (quel_cote == 0)
                    elem0 = elems_voisins[0];
                  else
                    elem1 = elems_voisins[0];
                }
              element_oppose[elem0] = elem1;
              element_oppose[elem1] = elem0;
            }
          // Deuxieme etape: parcourir les elements distants. Si un element distant est
          // dans les elements jumeaux, ajouter l'autre jumeau dans les elements distants
          for (int i_joint = 0; i_joint < nb_joints; i_joint++)
            {
              ArrOfInt& elements_distants = zone.joint(i_joint).set_joint_item(Joint::ELEMENT).set_items_distants();
              int n = elements_distants.size_array();
              // Marquer les elements distants existants:
              int i;
              for (i = 0; i < n; i++)
                {
                  const int elem = elements_distants[i];
                  marqueurs_elements_distants.setbit(elem);
                }
              elements_distants.set_smart_resize(1);
              for (i = 0; i < n; i++)
                {
                  const int elem = elements_distants[i];
                  const int elem_oppose = element_oppose[elem];
                  if (elem_oppose >= 0 && (!marqueurs_elements_distants.testsetbit(elem_oppose)))
                    {
                      elements_distants.append_array(elem_oppose);
                      nb_elements_ajoutes++;
                    }
                }
              // Remettre a zero le tableau de marqueurs:
              n = elements_distants.size_array();
              for (i = 0; i < n; i++)
                {
                  const int elem = elements_distants[i];
                  marqueurs_elements_distants.clearbit(elem);
                }
            }
        }

    }
  while (nb_elements_ajoutes > 0);
  // Dernier tri des elements distants dans l'ordre croissant
  for (int i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      ArrOfInt& elements_distants = zone.joint(i_joint).set_joint_item(Joint::ELEMENT).set_items_distants();
      elements_distants.set_smart_resize(0);
      elements_distants.ordonne_array();
    }
}

// Description:
// Remplissage du tableau "espace_distant()" des elements dans les joints.
// C'est ici qu'on determine les elements de joint en fonction de l'epaisseur de joint.
// Le tableau espace_distant contient les
// indices locaux des elements distants (a envoyer aux processeurs voisins)
// Pour un joint d'epaisseur 1, ce sont tous les elements voisins d'un
// sommet de joint (sommet sur un face de joint ou sommet isole).
// Pour un joint d'epaisseur n>1, ce sont tous les elements voisins d'un
// sommet d'un element du joint d'epaisseur n-1.
// Le voisinage s'entend sur le domaine global (toutes zones confondues)
// Historique: premiere version B.Mathieu le 16/01/2007.
//   Il existe une methode qui determine les elements distants au moment du decoupage
//   (DomaineCutter::construire_elements_distants_ssdom).
//   La methode ci-dessous a ete validee par comparaison avec la methode du decoupeur.
//   Les sorties ont ete verifiees pour des epaisseurs jusqu'a 5 sur des maillages tetra.
// La difficulte de l'algorithme est d'obtenir les elements virtuels d'epaisseur > 1 qui se
// trouvent sur des sous-domaines qui ne sont pas en contact direct avec le sous-domaine
// local. Difficulte resolue par l'algorithme ci-dessous.
void Scatter::calculer_espace_distant_elements(Domaine& dom)
{
  Zone&        zone         = dom.zone(0);
  const int nbjoints    = zone.nb_joints();
  const int nb_som_elem  = zone.nb_som_elem();
  const IntTab& les_elems    = zone.les_elems();
  const int nproc        = Process::nproc();
  // PL on doit avoir la meme epaisseur_joint sur chaque processeur pour l'algorithme suivant
  // utilisant un echange de donnees avec schema_comm.begin_comm() schema_comm.end_comm()
  // sinon il y'a un blocage en mode debug a cause de la sortie de la ligne 1866 (voir cas Quasi_Comp_Coupl_Incomp)
  //const int epaisseur_joint = (nb_joints > 0) ? zone.joint(0).epaisseur() : 1;
  const int epaisseur_joint = (int) mp_max((nbjoints > 0) ? zone.joint(0).epaisseur() : 1);

  if (Process::je_suis_maitre())
    Cerr << "Calculation of remote space of elements : thickness " << epaisseur_joint << finl;

  // L'algorithme repose sur la construction progressive du tableau "liste_sommets".
  // liste_sommets(pe) contient a un instant donne la liste des sommets possedes par me()
  // dont le "pe" veut connaitre les elements voisins.
  // Chaque pe commence par reclamer ses elements voisins directs, c'est a dire les elements
  // voisins des sommets de joint. On sait qu'il voudra ensuite les elements voisins des elements
  // ainsi trouves, donc on ajoutera aux listes les sommets des elements distants trouves
  // a l'iteration precedente.
  VECT(ArrOfInt) liste_sommets(nproc);

  // Pour chaque processeur, la liste des elements locaux qu'il faut lui envoyer
  VECT(ArrOfInt) elements_distants(nproc);
  {
    // smart_resize car on va faire append_array sur ces tableaux
    for (int i = 0; i < nproc; i++)
      {
        liste_sommets[i].set_smart_resize(1);
        elements_distants[i].set_smart_resize(1);
      }
  }

  Static_Int_Lists som_elem;
  {
    if (Process::je_suis_maitre())
      Cerr << "Nodes-elements connectivity ..." << finl;
    const int nb_sommets = dom.nb_som();
    construire_connectivite_som_elem(nb_sommets,
                                     les_elems,
                                     som_elem,
                                     0 /* ne pas inclure les sommets virtuels */);
  }

  ArrOfInt liste_pe_voisins(nbjoints);

  // Initialisation de liste_pe_voisins et
  // initialisation de la liste de sommets: chaque processeur requiert tous les elements
  // voisins de ses sommets de joint. Le processeur local sait que pour l'epaisseur 1,
  // chaque processeur voisin par un joint veut connaitre tous les elements voisins
  // des sommets de joint. Donc on met dans liste_sommets(pe) les sommets de joint avec ce pe,
  // se sorte a lui envoyer les elements locaux voisins de ces sommets.
  {
    for (int i_joint = 0; i_joint < nbjoints; i_joint++)
      {
        const Joint& joint = zone.joint(i_joint);
        const int pe = joint.PEvoisin();
        liste_pe_voisins[i_joint] = pe;
        const ArrOfInt& sommets_joint = joint.joint_item(Joint::SOMMET).items_communs();
        liste_sommets[pe] = sommets_joint;
      }
  }

  // On va avoir besoin d'un schema de communication ou chaque processeur envoie et recoit
  // des donnees a ses voisins directs (voisins par un sommet)
  Schema_Comm schema_comm;
  schema_comm.set_send_recv_pe_list(liste_pe_voisins, liste_pe_voisins);

  // On va avoir besoin d'un acces rapide a la liste des processeurs qui partagent un
  // sommet et a l'indice de ce sommet sur ce processeur.
  // Contenu de la structure:
  //  data_sommets_communs.get_list_size(sommet) = 2*le nombre de procs qui partagent le sommet
  //  data_sommets_communs(sommet, 2*i) = numero du pe voisin
  //  data_sommets_communs(sommet, 2*i+1) = indice du sommet sur ce pe.
  Static_Int_Lists data_sommets_communs;
  // Remplissage : la structure n'est utilisee que si l'epaisseur de joint est > 1
  if (epaisseur_joint > 1)
    {
      ArrOfInt count(dom.nb_som());
      // Etape 1 : avec combien de processeurs chaque sommet est-il partage ?
      for (int ijoint = 0; ijoint < nbjoints; ijoint++)
        {
          const Joint& joint = zone.joint(ijoint);
          const ArrOfInt& sommets_joint = joint.joint_item(Joint::SOMMET).items_communs();
          const int n = sommets_joint.size_array();
          for (int i = 0; i < n; i++)
            {
              const int som = sommets_joint[i];
              count[som] += 2; // On stocke 2 entiers par sommets commun
            }
        }
      data_sommets_communs.set_list_sizes(count);
      count = 0;
      // Etape 2 : remplissage de la structure:
      for (int ijoint = 0; ijoint < nbjoints; ijoint++)
        {
          const Joint& joint = zone.joint(ijoint);
          const int pe = joint.PEvoisin();
          const IntTab& renum_sommets = joint.joint_item(Joint::SOMMET).renum_items_communs();
          const int n = renum_sommets.dimension(0);
          for (int i = 0; i < n; i++)
            {
              // Indice du sommet partage sur le pe voisin
              const int i_sommet_distant = renum_sommets(i, 0);
              // Indice du sommet partage sur ma zone locale
              const int i_sommet_local = renum_sommets(i, 1);
              const int j = count[i_sommet_local]++;
              data_sommets_communs.set_value(i_sommet_local, j*2, pe);
              data_sommets_communs.set_value(i_sommet_local, j*2+1, i_sommet_distant);
            }
        }
    }

  // Boucle sur l'epaisseur de joint:
  // A l'entree de la boucle, on suppose que liste_sommets contient, pour chaque processeur
  // qui requiert des elements virtuels, la liste des sommets de me() dont il veut les voisins.
  for (int epaisseur = 1; ; epaisseur++)
    {
      if (Process::je_suis_maitre())
        Cerr << " Calculation of the thickness " << epaisseur << finl;

      // Pour chaque liste de sommets, mettre dans les elements distants du meme processeur
      // les elements voisins des sommets de la liste.
      int pe;
      for (pe = 0; pe < nproc; pe++)
        {
          ArrOfInt& elems_dist = elements_distants[pe];
          const ArrOfInt& sommets = liste_sommets[pe];
          elems_dist.resize_array(0);
          const int nb_som_liste = sommets.size_array();
          for (int isom = 0; isom < nb_som_liste; isom++)
            {
              const int som = sommets[isom];
              if (som<0)
                continue;
              const int nb_elem_som = som_elem.get_list_size(som);
              for (int ielem = 0; ielem < nb_elem_som; ielem++)
                {
                  const int elem = som_elem(som, ielem);
                  elems_dist.append_array(elem);
                }
            }
          array_trier_retirer_doublons(elems_dist);
        }

      // La suite est la mise a jour de liste_sommets pour l'iteration suivante.
      // Inutile de le faire si on est a la derniere iteration:
      if (epaisseur == epaisseur_joint)
        break;

      // Mettre dans les listes de sommets les sommets des elements distants trouves
      for (pe = 0; pe < nproc; pe++)
        {
          ArrOfInt& sommets = liste_sommets[pe];
          const ArrOfInt& elems_dist = elements_distants[pe];
          sommets.resize_array(0);
          const int nb_elems_dist = elems_dist.size_array();
          for (int ielem = 0; ielem < nb_elems_dist; ielem++)
            {
              const int elem = elems_dist[ielem];
              for (int isom = 0; isom < nb_som_elem; isom++)
                {
                  const int som = les_elems(elem, isom);
                  sommets.append_array(som);
                }
            }
          array_trier_retirer_doublons(sommets);
        }
      // Parcourir les listes de sommets. Pour chaque sommet, s'il est de joint,
      // envoyer aux processeurs possedant ce sommet une requete "le processeur i
      // veut tous les voisins de ce sommet".
      // Ne pas envoyer la requete au processeur "i" pour la liste "i": il connait
      // deja ses propres elements !
      schema_comm.begin_comm();
      // Premiere phase de communication: empiler les donnees a envoyer dans des buffers
      for (pe = 0; pe < nproc; pe++)
        {
          const ArrOfInt& sommets = liste_sommets[pe];
          const int nb_som_liste = sommets.size_array();
          for (int isom = 0; isom < nb_som_liste; isom++)
            {
              const int i_sommet_local = sommets[isom];
              if (i_sommet_local<0)
                continue;
              const int nb_pe_voisins = data_sommets_communs.get_list_size(i_sommet_local) / 2;
              for (int i = 0; i < nb_pe_voisins; i++)
                {
                  const int pe_voisin = data_sommets_communs(i_sommet_local, i*2);
                  // Indice du sommet sur le processeur voisin.
                  const int i_sommet_distant = data_sommets_communs(i_sommet_local, i*2+1);
                  if (pe_voisin != pe)
                    {
                      // Envoyer au processeur "pe_voisin" le message : "le processeur PE a besoin
                      // des elements voisins du sommet i_sommet_distant"
                      schema_comm.send_buffer(pe_voisin) << pe << i_sommet_distant;
                    }
                }
            }
        }
      schema_comm.echange_taille_et_messages();
      for (int i_pevoisin = 0; i_pevoisin < nbjoints; i_pevoisin++)
        {
          const int pe_voisin = liste_pe_voisins[i_pevoisin];
          Entree& buffer = schema_comm.recv_buffer(pe_voisin);
          for (;;)
            {
              int pe2, sommet;
              // On recupere le message "le processeur PE a besoin des elements voisins de SOMMET".
              buffer >> pe2 >> sommet;
              if (buffer.eof())
                break;
              liste_sommets[pe2].append_array(sommet);
            }
        }
      schema_comm.end_comm();
      // Supprimer les doublons dans les listes de sommets
      for (pe = 0; pe < nproc; pe++)
        {
          ArrOfInt& sommets = liste_sommets[pe];
          array_trier_retirer_doublons(sommets);
        }
    }

  // Creation des nouveaux joints si besoin, et stockage des elements distants dans les joints
  {
    ArrOfInt voisins;
    voisins.set_smart_resize(1);

    for (int pe = 0; pe < nproc; pe++)
      if (elements_distants[pe].size_array() > 0)
        voisins.append_array(pe);
    ajouter_joints(zone, voisins);

#ifdef CHECK_ALGO_ESPACE_VIRTUEL
    // On n'utilise pas les espaces virtuels calcules ci-dessus, on se
    // contente de les comparer aux espaces virtuels calcules lors du decoupage
    // par l'algorithme sequentiel.
    // Pour l'instant, l'algorithme parallele a l'air de fonctionner sans probleme
    // je desactive ce test. (Benoit Mathieu)
    int erreur = 0;
    const int nbjoints = zone.nbjoints();
    for (int i = 0; i < nbjoints; i++)
      {
        Joint& joint = zone.joint(i);
        const int pe = joint.PEvoisin();
        if (!(joint.joint_item(Joint::ELEMENT).items_distants() == elements_distants[pe]))
          {
            Cerr << "Error in Scatter, PE " << Process::me() << finl;
            Process::Journal() << "Error scatter, remote elements pe " << pe << finl
                               << " Splitting algorithm: " << joint.joint_item(Joint::ELEMENT).items_distants()
                               << " Scatter algorithm  : " << elements_distants[pe] << finl;

            erreur = 1;
          }
      }
    if (mp_sum(erreur))
      Process::exit();
#else
    // Stockage du resultat
    const int nb_joints = zone.nb_joints();
    for (int i = 0; i < nb_joints; i++)
      {
        Joint& joint = zone.joint(i);
        const int pe = joint.PEvoisin();
        joint.set_joint_item(Joint::ELEMENT).set_items_distants() = elements_distants[pe];
      }
#endif
  }
}

static int fct_cmp_coord_dimension = -1;
static double fct_cmp_coord_epsilon = -1.;

static inline True_int fct_cmp_coordonnees(const double * s1, const double *s2)
{
  const int dim = fct_cmp_coord_dimension;
  const double epsilon = fct_cmp_coord_epsilon;
  assert(dim==2 || dim==3);
  if (s1[0] < s2[0] - epsilon)
    return -1;
  else if (s1[0] > s2[0] + epsilon)
    return 1;
  else if (s1[1] < s2[1] - epsilon)
    return -1;
  else if (s1[1] > s2[1] + epsilon)
    return 1;
  else if (dim<3)
    return 0;
  else if (s1[2] < s2[2] - epsilon)
    return -1;
  else if (s1[2] > s2[2] + epsilon)
    return 1;
  else
    return 0;
}

static const DoubleTab * fct_cmp_coord_tableau = 0;
// Fonction de comparaison pour le tri d'un tableau d'indirection vers
// des coordonnees de sommets. Fonction appelee par qsort.
// ptr1 et ptr2 sont des adresses de deux entiers dans le tableau "index"
// Les coordonnees comparees sont dans le tableau *fct_cmp_coord_tableau
static True_int fct_cmp_index_coord(const void * ptr1, const void * ptr2)
{
  const int i1 = *(const int *)ptr1;
  const int i2 = *(const int *)ptr2;
  const double * s1 = & (*fct_cmp_coord_tableau)(i1,0);
  const double * s2 = & (*fct_cmp_coord_tableau)(i2,0);
  const True_int resu = fct_cmp_coordonnees(s1, s2);
  return resu;
}

// Description:
//  Construit le tableau "correspondance" tel que
//  Pour 0 <= i < sommets2.size_array(),
//   Si sommet2(i) existe dans le tableau sommet1, alors
//      sommets2(i, ...) == sommets1(correspondance[i], ...)
//   Sinon
//      correspondance[i] = -1
//  L'egalite est verifiee a epsilon pres en absolu (soit abs(x1-x2)<epsilon)
//  L'algorithme est generalement en n1*log(n1) + n2*log(n1)
//  (recherche basee sur un quicksort).
//  En cas d'echec du tri, on utilise un algorithme en n1*n2.
//  Les tableaux sommets1 et sommets2 doivent etre de dimension 2
//  Le tableau correspondance doit etre de taille sommets2.size_array().
// Valeur de retour: nombre de sommets de sommets2 non trouves dans le tableau sommets1.
int Scatter::Chercher_Correspondance(const DoubleTab& sommets1, const DoubleTab& sommets2,
                                     ArrOfInt& correspondance, const double epsilon)
{
  // Methode non threadsafe a cause des static. On teste
  if (fct_cmp_coord_tableau)
    {
      Cerr << "Thread error in Chercher_Correspondance" << finl;
      exit();
    }
  const int nb_sommets1 = sommets1.dimension(0);
  const int nb_sommets2 = sommets2.dimension(0);
  // Precondition necessaire pour fct_cmp_index_coord
  assert(sommets1.nb_dim() == 2);
  assert(sommets2.nb_dim() == 2);
  assert(correspondance.size_array() == nb_sommets2);
  if (nb_sommets1 < 1)
    {
      correspondance = -1;
      return nb_sommets2;
    }

  fct_cmp_coord_tableau   = &sommets1;
  fct_cmp_coord_dimension = sommets1.dimension(1);
  fct_cmp_coord_epsilon   = epsilon;

  // Tableau d'indirection trie, tel que les coordonnees sommets1(index[i], .) soient
  // tiees dans l'ordre lexicographique.
  ArrOfInt index(nb_sommets1);
  {
    int i;
    for (i = 0; i < nb_sommets1; i++)
      index[i] = i;
  }

  // Tri du tableau d'index
  // On trie les sommets par ordre lexicographique des coordonnees.
  // Comme on fait un test a epsilon pres, le tri peut echouer:
  //  Si x=1, y=1.01, z=1.02 et epsilon=0.01, on a
  //   x==y (a epsilon pres)
  //   y==z (a epsilon pres)
  //  mais x!=z
  // Donc la recherche par dichotomie peut echouer par la suite.

  qsort(index.addr(), nb_sommets1, sizeof(int),
        fct_cmp_index_coord);

  // Construction du tableau de correspondance tel que
  //   sommet1(correspondance[i], ...) == sommet2(i, ...)
  int nb_sommets_non_trouves = 0;
  int nb_echec_dichotomie = 0;
  {
    int i;
    for (i = 0; i < nb_sommets2; i++)
      {
        const double * s2 = & sommets2(i,0);
        int num_sommet = -1;

        // D'abord recherche du sommet dans le tableau sommets1 par dichotomie (bsearch)
        int imin = 0;
        int imax = nb_sommets1 - 1;
        int resu_cmp = -1;
        int k = -1;
        while (imax > imin)
          {
            const int milieu = (imin + imax) >> 1; // (min+max)/2
            k = index[milieu];
            const double * s1 = & sommets1(k, 0);
            resu_cmp = fct_cmp_coordonnees(s1, s2);
            switch(resu_cmp)
              {
              case -1:
                imin = milieu + 1;
                break; // s1<s2
              case 1 :
                imax = milieu - 1;
                break; // s1>s2
              default:
                imin = imax = milieu;
                break; // s1==s2
              }
          }
        if (resu_cmp != 0)
          {
            k = index[imin];
            const double * s1 = & sommets1(k, 0);
            resu_cmp = fct_cmp_coordonnees(s1, s2);
          }
        if (resu_cmp == 0)
          {
            num_sommet = k;
          }
        else
          {
            nb_echec_dichotomie++;
            // Si echec, c'est peut-etre que le tableau n'est pas ordonne correctement
            // => recherche du sommet en parcourant tout le tableau
            int j;
            for (j = 0; j < nb_sommets1; j++)
              {
                const double * s1 = & sommets1(j,0);
                resu_cmp = fct_cmp_coordonnees(s1, s2);
                if (resu_cmp == 0)
                  break;
              }
            if (j < nb_sommets1)
              num_sommet = j;
            else
              nb_sommets_non_trouves++;
          }

        correspondance[i] = num_sommet;
      }
  }
  fct_cmp_coord_dimension = 0;
  fct_cmp_coord_epsilon   = -1.;
  fct_cmp_coord_tableau   = 0;

  if (nb_echec_dichotomie > 0)
    Process::Journal() << "Chercher_Correspondance Dichotomy failure rate "
                       << nb_echec_dichotomie << " / " << nb_sommets2 << finl;

  return nb_sommets_non_trouves;
}

// Description:
//  Methode obsolete (utilisee avec l'ancien decoupeur).
//
// Description:
//  On suppose que le domaine contient les structures suivantes:
//   * zone(0).faces_joint[i].sommets()            (les sommets de joint isoles)
//   * zone(0).faces_joint[i].faces().les_sommets  (les sommets des faces de joint)
//  et que chaque processeur contient une portion d'un domaine complet.
//  On remplit les tableaux joint_item(Joint::Type_Item).items_communs pour tous les joints de la
//  zone 0 du domaine et renum_items_communs.
//  Ordre d'apparition des items : ordre croissant de l'indice du
//   sommet sur le processeur de rang le plus petit
//  Cette methode doit etre appelee simultanement sur tous les processeurs
//  du groupe et ne s'applique qu'aux items ARETE et SOMMET
//  L'algorithme utilise les coordonnees des items pour retrouver la
//  correspondance entre les items de joint.

void Scatter::construire_correspondance_items_par_coordonnees(Joints& joints, const Joint::Type_Item type_item, const DoubleTab& coord_items)
{
  switch(type_item)
    {
    case Joint::SOMMET:
      ;
      break;
    case Joint::ARETE:
      ;
      break;
    default:
      Cerr << "Scatter::construire_correspondance_items_par_coordonnees unusable for item "
           << (int)type_item
           << finl;
      Process::exit();
    }
  const int dim = Objet_U::dimension;
  const int nb_joints = joints.size();

  // Indices des items de joints dans le domaine sur mon processeur
  VECT(ArrOfInt)  indices_items_locaux(nb_joints);
  // Indices des items de joints dans le domaine sur le processeur voisin
  VECT(ArrOfInt)  indices_items_distants(nb_joints);
  // Coordonnees des items correspondants (dans le meme ordre que indices_items_xxx)
  VECT(DoubleTab) coord_items_locaux(nb_joints);
  VECT(DoubleTab) coord_items_distants(nb_joints);

  // Remplissage des tableaux indices_items_locaux
  // et coord_items_locaux
  {
    int i_joint;
    for (i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        const Joint& joint = joints[i_joint];
        ArrOfInt& items = indices_items_locaux[i_joint];
        // Remarque: **LISTE_TRI** les indices_items_locaux sont
        //  tries dans l'ordre croissant:
        calculer_liste_complete_items_joint(joint, type_item, items);

        const int n       = items.size_array();
        DoubleTab&   coord   = coord_items_locaux[i_joint];
        int i, j;
        coord.resize(n, dim);
        for (i = 0; i < n; i++)
          for (j = 0; j < dim; j++)
            coord(i,j) = coord_items(items[i], j);
      }
  }

  // Envoi des indices et coordonnees locaux au processeur voisin
  {
    Schema_Comm schema_comm;
    ArrOfInt liste_pe_voisins(nb_joints);
    int i;
    for (i = 0; i < nb_joints; i++)
      liste_pe_voisins[i] = joints[i].PEvoisin();
    schema_comm.set_send_recv_pe_list(liste_pe_voisins, liste_pe_voisins);
    schema_comm.begin_comm();
    for (i = 0; i < nb_joints; i++)
      {
        const int pe = liste_pe_voisins[i];
        Sortie& buffer = schema_comm.send_buffer(pe);
        buffer << indices_items_locaux[i];
        buffer << coord_items_locaux[i];
      }
    schema_comm.echange_taille_et_messages();
    for (i = 0; i < nb_joints; i++)
      {
        const int pe = liste_pe_voisins[i];
        Entree& buffer = schema_comm.recv_buffer(pe);
        buffer >> indices_items_distants[i];
        buffer >> coord_items_distants[i];
      }
    schema_comm.end_comm();
  }

  // Boucle sur les joints
  {
    // Cette fois, on modifie les joints (remplissage de renum_virt_loc)
    const int moi = Process::me();
    for (int i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        Joint&           joint           = joints[i_joint];
        const int     PEvoisin        = joint.PEvoisin();
        const ArrOfInt& indices_locaux  = indices_items_locaux[i_joint];
        //ArrOfInt &       indices_distants= indices_items_distants[i_joint];
        const DoubleTab& coord_locaux    = coord_items_locaux[i_joint];
        const DoubleTab& coord_distants  = coord_items_distants[i_joint];
        const int n = indices_locaux.size_array();
        /*
          if (n != indices_distants.size_array()) {
          Cerr << "Error in Scatter::remplir_renum_virt_loc on PE "
          << Process::me();
          Cerr << "\n Joint with PE " << joint.PEvoisin()
          << "\n Number of items " << type_item << " on my joint : " << n;
          Cerr << "\n Number of items " << type_item << " on the remote joint : "
          << indices_distants.size_array()
          << finl;
          exit();
          } */
        // Recherche des correspondances entre items
        ArrOfInt corresp(n);
        const double epsilon = Objet_U::precision_geom;
        Chercher_Correspondance(coord_distants, coord_locaux, corresp, epsilon);

        int nb_items_communs_trouves=0;
        for (int k = 0; k < n; k++)
          if (corresp[k]>=0) nb_items_communs_trouves++;

        ArrOfInt& items_communs = joint.set_joint_item(type_item).set_items_communs();
        items_communs.resize_array(nb_items_communs_trouves);
        items_communs = -1;
        int i=0;
        for (int k = 0; k < n; k++)
          {
            const int i_local = indices_locaux[k];
            // Le j-ieme item distant est identique au k-ieme item local
            const int j = corresp[k];

            // Pas trouve ? Erreur possible
            if (j < 0)
              {
                Cerr << "Error in Scatter::remplir_renum_virt_loc on PE " << moi << finl
                     << "The item of type " << (int)type_item << " number " << i_local << " with coordinates ";
                for (int k2 = 0; k2 < dim; k2++)
                  Cerr << coord_locaux(i, k2) << " ";
                Cerr << finl << "was not found in the joint with the PE " << PEvoisin << finl;
                if (type_item==3)
                  {
                    Cerr << "The searching algorithm of the isolated edges on a joint" << finl;
                    Cerr << "does not work yet in some cases. Two isolated nodes of a joint (example below" << finl;
                    Cerr << "joint between 0 and 2) can be those of an edge not belonging to this joint (below" << finl;
                    Cerr << "the edge belongs to the joint 0-1 but not 0-2):" << finl;
                    //Cerr << "  ________    " << finl;
                    //Cerr << "1\ 2/1\2 /1\  " << finl;
                    //Cerr << "__\/___\/___\ " << finl;
                    //Cerr << " 0/\ 0 /\ 0   " << finl;
                    //Cerr << " / 0\ / 0\    " << finl;
                    Cerr << "  ________      " << finl;
                    Cerr << "1\\ 2/1\\2 /1\\ " << finl;
                    Cerr << "__\\/___\\/___\\" << finl;
                    Cerr << " 0/\\ 0 /\\ 0   " << finl;
                    Cerr << " / 0\\ / 0\\    " << finl;
                    Cerr << finl;
                    Cerr << "One way to by-pass this problem is to split again your domain with" << finl;
                    Cerr << "different options of splitting or with another splitter to do not fall" << finl;
                    Cerr << "on the same configuration." << finl;
                  }
                exit();
              }
            else
              {
                if (moi < PEvoisin)
                  {
                    // items communs dans l'ordre des items de joint locaux:
                    items_communs[i] = i_local;
                    // On verifie que c'est bien l'ordre croissant de l'indice local
                    // (voir **LISTE_TRI**)
                    assert(i==0 || items_communs[i] > items_communs[i-1]);
                  }
                else
                  {
                    // items communs dans l'ordre des items sur le voisin:
                    assert(items_communs[j] < 0);
                    items_communs[j] = i_local;
                  }
                i++;
              }
          }
        assert(i==nb_items_communs_trouves);
      }
    // Remplissage de renum_items_communs:
    calculer_renum_items_communs(joints, type_item);
  }
}

// Description:
// Construction des tableaux joint_item(Joint::SOMMET).items_communs de tous les joints de la zone(0) du domaine dom

void Scatter::construire_correspondance_sommets_par_coordonnees(Domaine& dom)
{
  construire_correspondance_items_par_coordonnees(dom.zone(0).faces_joint(), Joint::SOMMET, dom.coord_sommets());
}

// Description
// Construction des tableaux joint_item(Joint::ARETE).items_communs de tous les joints de la zone

void Scatter::construire_correspondance_aretes_par_coordonnees(Zone_VF& zvf)
{
  construire_correspondance_items_par_coordonnees(zvf.zone().faces_joint(), Joint::ARETE, zvf.xa());
}

static void init_simple_md_vector(MD_Vector_std& md, const int n)
{
  md.nb_items_tot_ = n;
  md.nb_items_reels_ = n;
  md.nb_items_seq_tot_ = n;
  md.nb_items_seq_local_ = n;
  md.blocs_items_to_sum_.resize_array(2, Array_base::NOCOPY_NOINIT);
  md.blocs_items_to_sum_[0] = 0;
  md.blocs_items_to_sum_[1] = n;
  md.blocs_items_to_compute_.resize_array(2, Array_base::NOCOPY_NOINIT);
  md.blocs_items_to_compute_[0] = 0;
  md.blocs_items_to_compute_[1] = n;
}

/// Description
//  Pour un item geometrique "type_item", remplit le champ nb_items_virtuels_
//  des joints en fonction du nombre d'items distants :
//  Le nombre d'items virtuels sur un joint i du processeur j est le
//  nombre d'items distants du joint j sur le processeur i.
void Scatter::calculer_nb_items_virtuels(Joints& joints,
                                         const Joint::Type_Item type_item)
{
  Schema_Comm schema_comm;
  const int nb_joints = joints.size();
  ArrOfInt liste_voisins(nb_joints);
  int i_joint;
  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    liste_voisins[i_joint] = joints[i_joint].PEvoisin();

  // On envoie le nombre d'items distants au PE voisin
  schema_comm.set_send_recv_pe_list(liste_voisins, liste_voisins);
  schema_comm.begin_comm();
  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const Joint&        joint = joints[i_joint];
      const Joint_Items& items = joint.joint_item(type_item);
      const int n  = items.items_distants().size_array();
      const int pe = joint.PEvoisin();
      schema_comm.send_buffer(pe) << n;
    }
  // Echange des messages
  schema_comm.echange_taille_et_messages();
  // Le PE voisin recoit ce nombre d'items et le stocke.
  for (i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      Joint&        joint = joints[i_joint];
      Joint_Items& items = joint.set_joint_item(type_item);
      const int pe = joint.PEvoisin();
      int n;
      schema_comm.recv_buffer(pe) >> n;
      items.set_nb_items_virtuels(n);
    }
  schema_comm.end_comm();
}

// Description: cree des descripteurs sequentiels pour les tableaux sommets
//  et elements du domaine (necessaire car Scatter n'est pas appele pour les
//  domaines en sequentiel)
void Scatter::init_sequential_domain(Domaine& dom)
{
  MD_Vector_std mdstd;
  MD_Vector md;
  const int nbsom = dom.nb_som();
  init_simple_md_vector(mdstd, nbsom);
  md.copy(mdstd);
  dom.les_sommets().set_md_vector(md);

  const int nb_zones = dom.nb_zones();
  for (int i = 0; i < nb_zones; i++)
    {
      const int nb_elem = dom.zone(i).nb_elem();
      init_simple_md_vector(mdstd, nb_elem);
      md.copy(mdstd);
      dom.zone(i).les_elems().set_md_vector(md);
    }
}

// Description: methode utilisee par les interpretes qui modifient le domaine
//  (sequentiel), detruit les descripteurs des sommets et elements pour permettre
//  la modification de ces tableaux.
void Scatter::uninit_sequential_domain(Domaine& dom)
{
  MD_Vector md; // descripteur nul
  dom.les_sommets().set_md_vector(md);
  const int nb_zones = dom.nb_zones();
  for (int i = 0; i < nb_zones; i++)
    {
      dom.zone(i).les_elems().set_md_vector(md);
    }
}

