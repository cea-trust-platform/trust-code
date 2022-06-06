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

#ifndef Debog_Pb_included
#define Debog_Pb_included

#include <MD_Vector_composite.h>
#include <TRUSTTabs_forward.h>
#include <Ref_Probleme_base.h>
#include <Interprete_bloc.h>
#include <MD_Vector_tools.h>
#include <TRUSTTab_parts.h>
#include <EcrFicCollecte.h>
#include <VectMD_Vector.h>
#include <MD_Vector_std.h>
#include <TRUSTVects.h>
#include <EFichier.h>
#include <Motcle.h>
#include <Debog.h>
#include <Noms.h>

class Debog_Pb;
Declare_ref(Debog_Pb);

class Debog_Pb : public Objet_U
{
  Declare_instanciable(Debog_Pb);
public:
  void set_nom_pb_actuel(const Nom& nom);
  void verifier_matrice(const char *const msg, const Matrice_Base&, const MD_Vector& md_lignes, const MD_Vector& md_colonnes);
  void verifier_Mat_elems(const char *const msg, const Matrice_Base& la_matrice);

  static REF(Debog_Pb)& get_debog_instance() { return instance_debog_; }

  template <typename _TYPE_>
  typename std::enable_if< (std::is_convertible<_TYPE_, double>::value) || (std::is_convertible<_TYPE_, int>::value),void >::type
  verifier(const char *const msg, _TYPE_, _TYPE_ *refvalue = 0);

  template <typename _TYPE_>
  void verifier(const char *const msg, const TRUSTVect<_TYPE_>&, TRUSTVect<_TYPE_> *refvalue = 0);

protected:
  Debog_Pb(const Debog_Pb&) : Objet_U() { throw; /* pas delete car utiliser dans Objet_U */ }

  int test_ignore_msg(const char *const msg);
  void goto_msg(const char *const msg);
  void write_geometry_data();
  void register_item(const MD_Vector& md, const Nom& id);
  void add_renum_item(const DoubleTab& coord_ref, const DoubleTab& coord_par, const MD_Vector& md, const Nom& id);
  void read_geometry_data();
  const IntVect& find_renum_vector(const MD_Vector&, Nom& id) const;

  template <typename _TYPE_>
  void ecrire_partie(const TRUSTVect<_TYPE_>& arr);

  template <typename _TYPE_>
  void ecrire_gen(const char* const msg, const TRUSTVect<_TYPE_>& arr, int num_deb = -1);

  template <typename _TYPE_>
  inline void verifier_partie_std(const TRUSTVect<_TYPE_>& reference, const TRUSTVect<_TYPE_>& arr, TRUSTVect<_TYPE_> *arr_ref = 0);

  template <typename _TYPE_>
  inline void verifier_partie(const TRUSTVect<_TYPE_>& reference, const TRUSTVect<_TYPE_>& arr, TRUSTVect<_TYPE_> *arr_ref = 0);

  template <typename _TYPE_>
  inline void verifier_gen(const char * const msg, const TRUSTVect<_TYPE_>& arr, TRUSTVect<_TYPE_> *arr_ref = 0);

  void error_function();


  // Nombre de messages ecrits dans le fichier
  int debog_msg_count_ = 0;
  Nom nom_pb_;
  REF(Probleme_base) ref_pb_;
  Nom fichier_domaine_;
  Nom fichier_faces_;
  Nom fichier_debog_; // fichier contenant les valeurs a relire
  double seuil_absolu_ = 1e-10;
  double seuil_relatif_ = 1e-10;
  int noeuds_doubles_ignores_ = 0; // Noeuds doubles
  // Liste de messages a ignorer (sauter ces messages si on les trouve dans le fichier debog)
  Motcles msg_a_ignorer_;
  // Mode: 0->ecrire, 1->lire et comparer
  int mode_db_ = 0;
  // Faut-il faire exit des qu'on trouve une erreur ?
  int exit_on_error_ = 0;

  // Liste des descripteurs connus (on ignore tous les tableaux qui n'auront pas
  //  leur descripteur dans known_md_)
  VECT(MD_Vector) known_md_;
  // Dans le meme ordre, identificateur des structures (som, elem, face, etc)
  Noms renum_id_;
  // Dans le meme ordre, tableaux de renumerotation (pour chaque items du calcul,
  // indice de cet items dans le tableau sequentiel), tableau valide uniquement
  // pendant la phase lecture/comparaison. renum_array_[i] a le meme descripteur parallele
  // que known_md_[i]
  IntVects renum_array_;

  // Fichier pour la phase d'ecriture
  SFichier write_debog_data_file_;
  // Fichier dans lequel on lit les valeurs lors de la phase lecture/comparaison
  EFichier debog_data_file_;
  // Fichier de log general (ecrit par le maitre)
  SFichier log_file_;
  // Fichier detaille (ecrit par tout le monde)
  EcrFicCollecte detailed_log_file_;

  Nom nom_pb_actuel_;

  static REF(Debog_Pb) instance_debog_;
};

// This is the interpreter (what we use in a data set TRUST).. it will instanciate a Debog_Pb object
class Debog_Pb_Wrapper : public Interprete
{
  Declare_instanciable(Debog_Pb_Wrapper);
public:
  Entree& interpreter(Entree& is) override;
};

#include <Debog_Pb.tpp>

#endif /* Debog_Pb_included */
