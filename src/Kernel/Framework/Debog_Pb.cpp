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
// File:        Debog_Pb.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Octree_Double.h>
#include <Probleme_base.h>
#include <PE_Groups.h>
#include <Debog_Pb.h>
#include <Equation.h>
#include <EChaine.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Param.h>

REF(Debog_Pb) Debog_Pb::instance_debog_;

Implemente_instanciable(Debog_Pb,"Debog_pb",Objet_U);
Implemente_instanciable(Debog_Pb_Wrapper,"Debog",Interprete);
Implemente_ref(Debog_Pb);

Sortie& Debog_Pb::printOn(Sortie& os) const
{
  exit();
  return os;
}

// Description: methode appelee des qu'une erreur est trouvee dans l'espace reel. (utile pour poser un breakpoint dans gdb)
void Debog_Pb::error_function()
{
  if (exit_on_error_)
    {
      Cerr << "Error found in Debog_Pb (see debog.log files)" << finl;
      Process::exit();
    }
}

Entree& Debog_Pb::readOn(Entree& is)
{
  fichier_domaine_ = "dom.debog";
  fichier_faces_ = "faces.debog";
  fichier_debog_ = "DEBOG";

  Param param(que_suis_je());
  param.ajouter("nom_pb", &nom_pb_, Param::REQUIRED);
  param.ajouter("fichier_domaine", &fichier_domaine_);
  param.ajouter("fichier_faces", &fichier_faces_);
  param.ajouter("fichier_debog", &fichier_debog_);
  param.ajouter("seuil_absolu", &seuil_absolu_);
  param.ajouter("seuil_relatif", &seuil_relatif_);
  param.ajouter("ignorer_messages", &msg_a_ignorer_);
  param.ajouter("mode_db", &mode_db_, Param::REQUIRED);
  param.dictionnaire("write", 0);
  param.dictionnaire("read", 1);
  param.ajouter("exit_on_error", &exit_on_error_);
  param.lire_avec_accolades(is);

  Objet_U& obj = Interprete::objet(nom_pb_);
  if (!sub_type(Probleme_base, obj))
    {
      Cerr << "Error in Debog::interpreter: " << obj << " is not a Problem_base object" << finl;
      Process::exit();
    }

  Probleme_base& pb = ref_cast(Probleme_base, obj);
  if (pb.domaine_dis().nombre_de_zones() == 0)
    {
      Cerr<<finl;
      Cerr<<"WARNING, problem in Debog"<<finl;
      Cerr<<"It's maybe because you have put"<<finl;
      Cerr<<"the command line Debog before to have associated"<<finl;
      Cerr<<"the discretization to the problem."<<finl;
    }
  ref_pb_ = pb;

  if (mode_db_ == 0)
    {
      // Ecriture:
      write_geometry_data();
      write_debog_data_file_.ouvrir(fichier_debog_);
      write_debog_data_file_.setf(ios::scientific);
      write_debog_data_file_.precision(20);
    }
  else
    {
      read_geometry_data();
      debog_data_file_.ouvrir(fichier_debog_);
      debog_data_file_.set_error_action(Entree::ERROR_EXIT);
    }

  if (Process::je_suis_maitre()) log_file_.ouvrir("debog.log");

  detailed_log_file_.ouvrir("debog_detail.log");

  instance_debog_ = *this;
  return is;
}


int Debog_Pb::test_ignore_msg(const char* const msg)
{
  return msg_a_ignorer_.rang(msg) >= 0;
}

void Debog_Pb::goto_msg(const char *const message)
{
  assert(mode_db_ == 1);
  Nom dummy;
  Nom n;
  int num;
  Nom msg(message); // copie sans les espaces avant/apres
  while (msg.debute_par(" "))
    msg.suffix(" ");
  while (msg.finit_par(" "))
    msg.prefix(" ");
  while (1)
    {
      // read until we find a "msg" word:
      do
        {
          debog_data_file_ >> dummy;
        }
      while (dummy != "msg");

      debog_data_file_ >> dummy;
      if (dummy != ":")
        {
          Cerr << "Error in Debog_Pb::goto_msg(" << msg << ")\n invalid data in DEBOG file" << finl;
          exit();
        }
      num = -1;
      debog_data_file_ >> num;
      debog_data_file_ >> dummy;
      if (dummy != ":")
        {
          Cerr << "Error in Debog_Pb::goto_msg(" << msg << ")\n invalid data in DEBOG file" << finl;
          exit();
        }
      n = "";
      while (1)
        {
          debog_data_file_ >> dummy;
          if (dummy == "FinMsg")
            break;
          n += dummy;
          n += " ";
        }
      if (n.debute_par(msg))
        break;
      if (Process::je_suis_maitre())
        {
          log_file_ << "Skipping message " << n << " (looking for " << msg << ")" << finl;
        }
    }
  if (Process::je_suis_maitre())
    log_file_ << "Reading message " << num << "(file) " << debog_msg_count_ << "(current) " << msg << finl;

  detailed_log_file_ << "Reading message " << num << "(file) " << debog_msg_count_ << "(current) " << msg << finl;
}

void Debog_Pb::write_geometry_data()
{
  if (Process::nproc() > 1)
    {
      Cerr << "Error in Debog.cpp: cannot write geometry data in parallel." << finl;
      Process::exit();
    }
  const Domaine& dom = ref_pb_.valeur().domaine();
  const Zone_dis_base& zd = ref_pb_.valeur().domaine_dis().zone_dis(0).valeur();
  const Zone_VF& zvf = ref_cast(Zone_VF, zd);
  {
    SFichier f(fichier_domaine_);
    f.precision(20);
    f << dom;
  }
  {
    SFichier f(fichier_faces_);
    f.precision(20);
    f << zvf.elem_faces();
    f << zvf.xv();
    f << zvf.face_voisins();
    if (zvf.xa().dimension(0) > 0)
      f << zvf.xa();
  }
  register_item(dom.md_vector_sommets(), "SOM");
  register_item(dom.zone(0).md_vector_elements(), "ELEM");
  register_item(zvf.md_vector_faces(), "FACE");
  if (zvf.md_vector_aretes().non_nul())
    register_item(zvf.md_vector_aretes(), "ARETES");
}

void Debog_Pb::register_item(const MD_Vector& md, const Nom& id)
{
  known_md_.add(md);
  renum_id_.add(id);
}

void Debog_Pb::add_renum_item(const DoubleTab& coord_ref, const DoubleTab& coord_par, const MD_Vector& md, const Nom& id)
{
  register_item(md, id);

  const double epsilon = Objet_U::precision_geom;

  // On cree un octree contenant les points du domaine de reference
  // et on cherche chaque point du domaine local dans cet octree.
  Octree_Double octree;
  octree.build_nodes(coord_ref, 1 /* include_virtual_items */);

  // Add a new renum array:
  renum_array_.add(IntVect());
  IntVect& renum = renum_array_[renum_array_.size()-1];
  MD_Vector_tools::creer_tableau_distribue(md, renum, Array_base::NOCOPY_NOINIT);

  if (renum.size_totale() != coord_par.dimension_tot(0))
    {
      Cerr << "Internal error in Debog::add_renum_item: coordinate array does not match md descriptor" << finl;
      Process::exit();
    }
  const int dim = coord_par.dimension(1);
  ArrOfDouble center(dim);
  ArrOfInt elements;
  elements.set_smart_resize(1);
  const int n = renum.size_totale();
  for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < dim; j++)
        center[j] = coord_par(i,j);
      octree.search_elements_box(center, epsilon, elements);
      // la premiere recherche renvoie tous les sommets potentiellement proches.
      // il faut faire ensuite un test sur chaque sommet:
      octree.search_nodes_close_to(center, coord_ref, elements, epsilon);
      const int k = elements.size_array();
      if (k != 1)
        {
          Cerr << "Debog::add_renum_item: Error. Id=" << id
               << "\n Item with following coordinates was found " << k << " times within epsilon=" << epsilon
               << "\n in the reference geometry: " << center << finl;
          Process::exit();
        }
      // renum[i] is the index of the i-th local item in the reference geometry
      renum[i] = elements[0];
    }
}

void Debog_Pb::read_geometry_data()
{
  const Domaine& dom = ref_pb_.valeur().domaine();
  const Zone_dis_base& zd = ref_pb_.valeur().domaine_dis().zone_dis(0).valeur();
  const Zone_VF& zvf = ref_cast(Zone_VF, zd);
  {
    DoubleTab coord_som_seq; // sommets
    DoubleTab xp_seq; // centres des elements
    // Il faut passer dans un groupe monoprocesseur pour Domaine::readOn:
    {
      DERIV(Comm_Group) group;
      ArrOfInt liste_procs(1); // Liste de 1 processeur contenant le proc 0
      PE_Groups::create_group(liste_procs, group, 1);
      if (PE_Groups::enter_group(group.valeur()))
        {
          EFichier f(fichier_domaine_);
          Domaine dom_seq;
          f >> dom_seq;
          coord_som_seq = dom_seq.coord_sommets();
          const Elem_geom_base& elem = dom_seq.zone(0).type_elem().valeur();
          elem.calculer_centres_gravite(xp_seq);
          PE_Groups::exit_group();
        }
    }
    // Tous les processeurs recoivent les coordonnees des sommets de reference
    // (detruire le descripteur sinon printOn refuse d'envoyer le vecteur)
    coord_som_seq.set_md_vector(MD_Vector());
    envoyer_broadcast(coord_som_seq, 0);
    // Idem avec les centres des elements
    xp_seq.set_md_vector(MD_Vector());
    envoyer_broadcast(xp_seq, 0);

    // Calculer les renumerotations:
    const DoubleTab& coord_som = dom.coord_sommets();
    const MD_Vector& md_som = dom.coord_sommets().get_md_vector();
    add_renum_item(coord_som_seq, coord_som, md_som, "SOM");
    const DoubleTab& xp = zvf.xp();
    // Le tableau xp n'a pas de structure parallele, on la prend dans les elements...
    const MD_Vector& md_elem = dom.zone(0).les_elems().get_md_vector();
    add_renum_item(xp_seq, xp, md_elem, "ELEM");
  }
  {
    DoubleTab xv_seq;
    DoubleTab xa_seq;
    int avec_aretes = 1;
    if (Process::je_suis_maitre())
      {
        EFichier f(fichier_faces_);
        f.precision(20);
        {
          IntTab elem_faces;
          f >> elem_faces; // non utilise
        }
        f >> xv_seq;
        {
          IntTab face_voisins;
          f >> face_voisins; // non utilise
        }
        f.set_error_action(Entree::ERROR_EXCEPTION);
        try
          {
            f >> xa_seq;
          }
        catch (Entree_Sortie_Error)
          {
            avec_aretes = 0;
          }
      }
    envoyer_broadcast(xv_seq, 0);
    const DoubleTab& xv = zvf.xv();
    const MD_Vector& md_face = zvf.md_vector_faces();
    add_renum_item(xv_seq, xv, md_face, "FACE");

    envoyer_broadcast(avec_aretes, 0);
    if (avec_aretes)
      {
        envoyer_broadcast(xa_seq, 0);
        const DoubleTab& xa = zvf.xa();
        const MD_Vector& md_arete = zvf.md_vector_aretes();
        add_renum_item(xa_seq, xa, md_arete, "ARETE");
      }
  }
}

enum Debog_Exceptions { RENUM_ARRAY_NOT_FOUND };

const IntVect& Debog_Pb::find_renum_vector(const MD_Vector& mdv, Nom& id) const
{
  const int n = renum_array_.size();
  for (int i = 0; i < n; i++)
    {
      const IntVect& renum = renum_array_[i];
      if (renum.get_md_vector() == mdv)
        {
          id = renum_id_[i];
          return renum;
        }
    }
  throw RENUM_ARRAY_NOT_FOUND;
}

void Debog_Pb::set_nom_pb_actuel(const Nom& nom)
{
  nom_pb_actuel_ = nom;
}

// Description:
//  md_lignes: descripteur des indices de lignes (cad descripteur du vecteur b dans A*x=b)
//  mb_colonnes: idem, indices colonnes (cad descripteur du vecteur x dans A*x=b)
void Debog_Pb::verifier_matrice(const char *const msg, const Matrice_Base& matrice, const MD_Vector& md_lignes, const MD_Vector& md_colonnes)
{
  // Attention: cette implementation ne marche que pour les types md_colonnes enregistres dans renum_array_
  //  (cad types simples sommets, elements, faces, aretes, et pas les MD_Vector_composite)
  // et uniquement pour linesize==1 (une ligne et une colonne de la matrice par item)

  // Pour chaque colonne i de la matrice, ecriture du produit matrice * vecteur_i
  //  avec vecteur_i[j] = (i==j);
  DoubleVect vecteur_i;
  DoubleVect tmp;
  MD_Vector_tools::creer_tableau_distribue(md_colonnes, vecteur_i);
  MD_Vector_tools::creer_tableau_distribue(md_lignes, tmp);
  Nom id;

  // Boucle sur les items sequentiels du vecteur x de A*x=b
  const int nb_colonnes = md_colonnes.valeur().nb_items_seq_tot();

  for (int i = 0; i < nb_colonnes; i++)
    {
      int renum_i;
      if (mode_db_ == 0)
        {
          renum_i = i;
        }
      else
        {
          // Recherche l'indice local correspondant a l'indice sequentiel i
          // Si i est un item virtuel on le prend quand meme: vecteur_i aura son espace virtuel a jour
          // Si i n'existe pas sur ce processeur, on met -1
          renum_i = -1;
          // Renum_colonnes[j] est l'indice dans le vecteur de reference sequentiel de l'item j sur ce processeur
          const IntVect& renum_colonnes = find_renum_vector(md_colonnes, id);
          const int sz = renum_colonnes.size_totale();
          for (int j = 0; j < sz; j++)
            {
              if (renum_colonnes[j] == i)
                {
                  renum_i = j;
                  break;
                }
            }
        }
      if (renum_i >= 0)
        vecteur_i[renum_i] = 1.;
      matrice.multvect(vecteur_i, tmp);
      Nom msg2(msg);
      msg2 += " produit avec base ";
      msg2 += Nom(i);
      verifier(msg2, tmp);
      if (renum_i >= 0)
        vecteur_i[renum_i] = 0.;
    }
}

void Debog_Pb::verifier_Mat_elems(const char* const msg, const Matrice_Base& la_matrice)
{
  // Cherche le md_vecteur des elements

  const int n = renum_id_.size();
  int i;
  for (i = 0; i < n; i++)
    {
      if (renum_id_[i] == "ELEM")
        break;
    }
  if (i == n)
    {
      Cerr << "Error in Debog_Pb::verifier_Mat_elems no registered MD_Vector for ELEM type" << finl;
      exit();
    }
  const MD_Vector& md = known_md_[i];
  verifier_matrice(msg, la_matrice, md, md);
}

////////////////////////////////////////////////////////////////

Sortie& Debog_Pb_Wrapper::printOn(Sortie& os) const
{
  Process::exit();
  return os;
}

Entree& Debog_Pb_Wrapper::readOn(Entree& is)
{
  Process::exit();
  return is;
}

Entree& Debog_Pb_Wrapper::interpreter(Entree& is)
{
  Nom nom_pb, nom_fic, nom_fic_faces, seuil;
  is >> nom_pb;
  int mode_db;
  is >> nom_fic >> nom_fic_faces >> seuil >> mode_db;

  Nom n("Debog_pb debog Lire debog { nom_pb ");
  n += nom_pb;
  n += " fichier_domaine ";
  n += nom_fic;
  n += " fichier_faces ";
  n += nom_fic_faces;
  n += " fichier_debog DEBOG";
  n += " seuil_absolu ";
  n += Nom(seuil);
  n += " seuil_relatif ";
  n += Nom(seuil);
  n += " mode_db ";
  if (mode_db == 0) n += "write";
  else n += "read";
  n += "  }";

  if (je_suis_maitre()) Cerr << "Debog_Pb_Wrapper: convert to new syntax: " << finl << n << finl;

  EChaine echaine(n);
  Interprete_bloc::interprete_courant().interpreter_bloc(echaine, Interprete_bloc::BLOC_EOF, 0 /* verifie_sans_interpreter */);
  return is;
}
