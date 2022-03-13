/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Raffiner_isotrope_parallele.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Raffiner_isotrope_parallele.h>
#include <LecFicDistribue.h>
#include <EcrFicCollecte.h>
#include <FichierHDFPar.h>
#include <LecFicDiffuse.h>
#include <Statistiques.h>
#include <TRUSTArrays.h>
#include <Schema_Comm.h>
#include <TRUSTTabs.h>
#include <SFichier.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Param.h>
#include <Nom.h>

Implemente_instanciable( Raffiner_isotrope_parallele, "Raffiner_isotrope_parallele", Raffiner_Simplexes ) ;

Sortie& Raffiner_isotrope_parallele::printOn( Sortie& os ) const
{
  Raffiner_Simplexes::printOn( os );
  return os;
}

Entree& Raffiner_isotrope_parallele::readOn( Entree& is )
{
  Raffiner_Simplexes::readOn( is );
  return is;
}


void mon_construire_correspondance_items_par_coordonnees(Joints& joints, const Joint::Type_Item type_item, const DoubleTab& coord_items)
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
        //Scatter::calculer_liste_complete_items_joint(joint, type_item, items);
        items = joint.joint_item(Joint::SOMMET).items_communs();
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
    //    const int moi = Process::me();
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
        int nbsomr=Scatter::Chercher_Correspondance(coord_distants, coord_locaux, corresp, epsilon);
        if (nbsomr)
          {
            Cerr<<"iiiiiiiiiiii"<<finl;
            //	    abort();
          }
        ArrOfInt& items_communs = joint.set_joint_item(type_item).set_items_communs();
        int nb_items_communs_trouves=0;
        for (int k = 0; k < n; k++)
          if (corresp[k]>=0)
            nb_items_communs_trouves++;
          else
            Cerr<<items_communs[k] << " not found in joint voisin "<<PEvoisin<<finl;


        ArrOfInt items_communs_sa(items_communs);
        items_communs.resize_array(nb_items_communs_trouves);

        int l=0;

        for (int k = 0; k < n; k++ )
          {
            if  (corresp[k]>=0)
              items_communs[l++]=items_communs_sa[k];
          }
      }
  }
}


// Description:
// Construction des tableaux joint_item(Joint::SOMMET).items_communs de tous les joints de la zone(0) du domaine dom

void mon_construire_correspondance_sommets_par_coordonnees(Domaine& dom)
{
  mon_construire_correspondance_items_par_coordonnees(dom.zone(0).faces_joint(), Joint::SOMMET, dom.coord_sommets());
}



Entree&  Raffiner_isotrope_parallele::interpreter(Entree& is)
{
  int form=0, format_hdf = 0;
  Nom org,newd;
  Param param(que_suis_je());
// XD Raffiner_isotrope_parallele interprete Raffiner_isotrope_parallele 1 Refine parallel mesh in parallel
  param.ajouter("name_of_initial_zones",&org,Param::REQUIRED); // XD_ADD_P chaine name of initial Zones
  param.ajouter("name_of_new_zones",&newd,Param::REQUIRED); // XD_ADD_P chaine name of new Zones
  param.ajouter("ascii",&form);  // XD_ADD_P flag writing Zones in ascii format
  param.ajouter_flag("single_hdf",&format_hdf); // XD_ADD_P flag writing Zones in hdf format
  param.lire_avec_accolades(is);
  // Force un fichier unique au dela d'un certain nombre de rangs MPI:
  if (Process::force_single_file(Process::nproc(), org+".Zones"))
    format_hdf = 1;
  int binaire=!form;
  if (form && format_hdf)
    {
      Cerr << "Raffiner_isotrope_parallele::interpreter(): options 'ascii' and 'single_hdf' are mutually exclusive!" << finl;
      Process::exit(1);
    }
  Domaine dom_org;
  Noms liste_bords_periodiques;
  org+=".Zones";

  Nom copy(org);
  copy = copy.nom_me(Process::nproc(), "p", 1);
  //bool is_hdf = FichierHDF::is_hdf5(copy);
  LecFicDiffuse test;
  bool is_hdf = test.ouvrir(copy) && FichierHDF::is_hdf5(copy);

  if (!is_hdf)
    {
      LecFicDistribue    fichier;
      fichier.set_bin(binaire);
      fichier.ouvrir(org);
      fichier >> dom_org;
      dom_org.set_fichier_lu(org);
      fichier >> liste_bords_periodiques;
    }
  else
    {
      FichierHDFPar fic_hdf;
      //FichierHDF fic_hdf;
      org = copy;
      fic_hdf.open(org, true);
      Entree_Brute data;

      fic_hdf.read_dataset("/zone", Process::me(), data);

      // Feed TRUST objects:
      data >> dom_org;
      dom_org.set_fichier_lu(org);
      data >> liste_bords_periodiques;

      fic_hdf.close();
    }

  Scatter::uninit_sequential_domain(dom_org);
  Domaine dom_new(dom_org);

  refine_domain(dom_org,dom_new);


  static Stat_Counter_Id stats = statistiques().new_counter(0 /* Level */, "Mailler_Parallel", 0 /* Group */);

  if (nproc() > 1)
    {
      Scatter::uninit_sequential_domain(dom_new);
      int nb_sommet_avant_completion=dom_new.nb_som();
      Scatter::trier_les_joints(dom_new.zone(0).faces_joint());
      statistiques().begin_count(stats);


      mon_construire_correspondance_sommets_par_coordonnees(dom_new);

      if (0)
        {
          Nom debug(newd);
          debug+="_debug.Zones";
          EcrFicCollecte os;
          os.set_bin(binaire);

          os.ouvrir(debug);
          if (!binaire)
            {
              os.setf(ios::scientific);
              os.precision(Objet_U::format_precision_geom);
            }

          os << dom_new;

          os << liste_bords_periodiques;

        }
      // else
      {

        Scatter::construire_correspondance_sommets_par_coordonnees(dom_new);
        Scatter::calculer_renum_items_communs(dom_new.zone(0).faces_joint(), Joint::SOMMET);

        statistiques().end_count(stats);
        double maxtime = mp_max(statistiques().last_time(stats));
        Cerr << "Scatter::construire_correspondance_sommets_par_coordonnees fin, time:"
             << maxtime
             << finl;

        statistiques().begin_count(stats);

        Scatter::construire_structures_paralleles(dom_new, liste_bords_periodiques);

        statistiques().end_count(stats);
        maxtime = mp_max(statistiques().last_time(stats));
        Cerr << "Scatter::construire_structures_paralleles, time:" << maxtime << finl;


        int ecrit=1;
        if (ecrit)
          {
            int nb_elem_reel=dom_new.zone(0).nb_elem();
            Scatter::uninit_sequential_domain(dom_new);
            dom_new.zone(0).les_elems().resize( nb_elem_reel,dom_new.zone(0).les_elems().dimension(1));
            dom_new.les_sommets().resize(nb_sommet_avant_completion,dimension);

            Scatter::uninit_sequential_domain(dom_new);
            newd+=".Zones";

            if( !format_hdf )
              {
                EcrFicCollecte os;
                os.set_bin(binaire);
                os.ouvrir(newd);
                if (!binaire)
                  {
                    os.setf(ios::scientific);
                    os.precision(Objet_U::format_precision_geom);
                  }
                os << dom_new;
                os << liste_bords_periodiques;
              }
            else
              {
                Sortie_Brute os_hdf;
                os_hdf << dom_new;
                os_hdf << liste_bords_periodiques;
                FichierHDFPar fic_hdf;
                newd = newd.nom_me(Process::nproc(), "p", 1);
                fic_hdf.create(newd);
                fic_hdf.create_and_fill_dataset_MW("/zone", os_hdf);
                fic_hdf.close();
              }
          }
      }
    }
  else
    {
      Scatter::init_sequential_domain(dom_new);
    }

  return is;
}
