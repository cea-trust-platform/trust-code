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
// File:        Format_Post_Lata_V1.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Format_Post_Lata_V1_included
#define Format_Post_Lata_V1_included

#include <Format_Post_Lata.h>

// .DESCRIPTION        : classe d'ecriture des postraitement au format lata V1
//   (Trio_U version 1.4.9 et anterieures). Fournie pour compatibilite
class Format_Post_Lata_V1 : public Format_Post_Lata
{
  Declare_instanciable_sans_constructeur(Format_Post_Lata_V1);
public:
  // Methodes declarees dans la classe de base (interface commune a tous
  // les formats de postraitment de champs):
  void reset() override;

  int ecrire_entete(double temps_courant,int reprise,const int est_le_premier_post) override;
  int finir(int& est_le_dernier_post) override;
  int ecrire_domaine(const Domaine& domaine,const int est_le_premier_post) override;
  virtual int ecrire_bords(const Nom&     id_du_domaine,
                           const Motcle& type_faces,
                           const DoubleTab& sommets,
                           const IntTab& faces_sommets,
                           const IntTab& faces_num_bord,
                           const Noms&    bords_nom);

  int ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo,
                   int ncomp,double temps_,double temps_courant,
                   const Nom&   id_du_champ,
                   const Nom&   id_du_domaine,
                   const Nom&   localisation,
                   const Nom&   nature,
                   const DoubleTab& data) override;

  int ecrire_item_int(const Nom&   id_item,
                      const Nom&   id_du_domaine,
                      const Nom&   id_zone,
                      const Nom&   localisation,
                      const Nom&   reference,
                      const IntVect& data,
                      const int reference_size) override;

  /*   virtual int ecrire_noms(const Nom  & id_de_la_liste, */
  /*                              const Nom  & id_du_domaine, */
  /*                              const Nom  & localisation, */
  /*                              const Noms & liste_noms); */

  static int ecrire_domaine_lata_V1(const Nom&        id_domaine,
                                    const Motcle&     type_elem,
                                    const int      dimension,
                                    const DoubleTab& sommets,
                                    const IntTab&     elements,const Status& stat,const Nom& basename, const Format&
                                    format, const Options_Para& option);

  static int ecrire_entete_lata_V1(const Nom& basename,const Options_Para& option,const int est_le_premier_post);
  static int finir_lata_V1(const Nom& basename,const Options_Para& option,const int est_le_dernier_post);
  static int ecrire_champ_lata_V1(const Nom&   id_du_champ,
                                  const Nom&   id_du_domaine,
                                  const Nom&   localisation,
                                  const DoubleTab& data,const double temps,const Nom& basename,const Format& format, const
                                  Options_Para& option);

  static int ecrire_faces_lata_V1(const Nom& basename,
                                  const Format& format,
                                  const Options_Para& option,
                                  const Nom&   id_du_domaine,
                                  const IntTab& faces_sommets,
                                  const IntTab& elem_faces,
                                  const int nb_sommets);
  Format_Post_Lata_V1();

protected:
  // Variable temporaire utilisee pour ecrire les faces a l'ancien format
  // (voir ecrire_inttab() et Postraitement_Champs::ecrire_faces()
  const IntTab *ref_faces_sommets;
  int ref_nb_som;
};
#endif
