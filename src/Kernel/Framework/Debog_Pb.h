/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Debog_Pb.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Debog_Pb_included
#define Debog_Pb_included

#include <Debog.h>
#include <DoubleVect.h>
#include <EFichier.h>
#include <VectMD_Vector.h>
#include <EcrFicCollecte.h>
#include <Motcle.h>
#include <Noms.h>
#include <IntVects.h>
#include <Ref_Probleme_base.h>

class DoubleTab;

class Debog_Pb;
Declare_ref(Debog_Pb);

class Debog_Pb : public Objet_U
{
  Declare_instanciable(Debog_Pb);
public:
  void verifier(const char* const msg, double, double *refvalue = 0);
  void verifier(const char* const msg, const DoubleVect&, DoubleVect *refvalue = 0);
  void verifier(const char* const msg, int, int *refvalue = 0);
  void verifier(const char* const msg, const IntVect&, IntVect *refvalue = 0);
  void set_nom_pb_actuel(const Nom& nom);
  void verifier_matrice(const char * const msg, const Matrice_Base&,
                        const MD_Vector& md_lignes, const MD_Vector& md_colonnes);
  void verifier_Mat_elems(const char* const msg, const Matrice_Base& la_matrice);

  static REF(Debog_Pb)& get_debog_instance()
  {
    return instance_debog_;
  }
protected:
  Debog_Pb(const Debog_Pb&);

  int test_ignore_msg(const char* const msg);
  void goto_msg(const char * const msg);
  void write_geometry_data();
  void register_item(const MD_Vector& md, const Nom& id);
  void add_renum_item(const DoubleTab& coord_ref,
                      const DoubleTab& coord_par,
                      const MD_Vector& md,
                      const Nom& id);
  void read_geometry_data();
  void ecrire_partie(const DoubleVect& arr);
  void ecrire_partie(const IntVect& arr);
  void ecrire_gen(const char* const msg, const DoubleVect& arr, int num_deb = -1);
  void ecrire_gen(const char* const msg, const IntVect& arr, int num_deb = -1);
  const IntVect& find_renum_vector(const MD_Vector&, Nom& id) const;
  void verifier_partie_std(const DoubleVect& reference, const DoubleVect& arr, DoubleVect *arr_ref = 0);
  void verifier_partie_std(const IntVect& reference, const IntVect& arr, IntVect *arr_ref = 0);
  void verifier_partie(const DoubleVect& reference, const DoubleVect& arr, DoubleVect *arr_ref = 0);
  void verifier_partie(const IntVect& reference, const IntVect& arr, IntVect *arr_ref = 0);
  void verifier_gen(const char * const msg, const DoubleVect& arr, DoubleVect *arr_ref = 0);
  void verifier_gen(const char * const msg, const IntVect& arr, IntVect *arr_ref = 0);
  void error_function();


  // Nombre de messages ecrits dans le fichier
  int debog_msg_count_;
  Nom nom_pb_;
  REF(Probleme_base) ref_pb_;
  Nom fichier_domaine_;
  Nom fichier_faces_;
  Nom fichier_debog_; // fichier contenant les valeurs a relire
  double seuil_absolu_;
  double seuil_relatif_;
  // Liste de messages a ignorer (sauter ces messages si on les trouve dans le fichier debog)
  Motcles msg_a_ignorer_;
  // Mode: 0->ecrire, 1->lire et comparer
  int mode_db_;
  // Faut-il faire exit des qu'on trouve une erreur ?
  int exit_on_error_;

  // Liste des descripteurs connus (on ignore tous les tableaux qui n'auront pas
  //  leur descripteur dans known_md_)
  VECT(MD_Vector) known_md_;
  // Dans le meme ordre, identificateur des structures (som, elem, face, etc)
  Noms renum_id_;
  // Dans le meme ordre, tableaux de renumerotation (pour chaque items du calcul,
  // indice de cet items dans le tableau sequentiel), tableau valide uniquement
  // pendant la phase lecture/comparaison. renum_array_[i] a le meme descripteur parallele
  // que known_md_[i]
  VECT(IntVect) renum_array_;

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
#endif
