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

#include <MD_Vector_std.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(MD_Vector_std,"MD_Vector_std",MD_Vector_base2);

MD_Vector_std::MD_Vector_std()
{
  nb_items_seq_tot_ = -1;
  nb_items_seq_local_ = -1;
  nb_items_tot_ = -1;
  nb_items_reels_ = -1;
}

// Description: Constructeur.
//  Si nb_items_reels >= 0, items_to_compute contiendra un bloc avec les items reels,
//   sinon, items_to_compute prendra tous les items (jusqu'a nb_items_tot).
// Parametre: nb_items_tot
// Signification: valeur que prendra nb_items_tot_ (doit etre >= 0)
// Parametre: nb_items_reels
// Signification: valeur que prendra nb_items_reels_ (doit etre >= -1,
//  -1 signifie qu'il n'y a pas d'items "reels" groupes au debut du tableau
// Parametre: pe_voisins
// Signification: liste des processeurs voisins, classes dans l'ordre croissant
//  (les tableaux ArrsOfInt doivent avoir la meme taille).
//  Cette liste n'est pas reprise integralement: on retire les processeurs
//  pour lesquels toutes les trois listes d'items sont vides.
// Parametre: items_to_send
// Signification: pour chaque pe_voisin, liste des items a envoyer (communs et virtuels).
//  count_items_to_send_to_items_ est calcule par une communication a partir de items_to_recv.
// Parametre: items_to_recv
// Signification: liste d'items individuels a recevoir des differents procs
//  (souvent des items communs mais ce n'est pas obligatoire, ex front-tracking)
// Parametre: blocs_to_recv
// Signification: liste de blocs d'items a recevoir (voir MD_Vector_std::blocs_to_recv_)
//  (souvent les items virtuels...)
MD_Vector_std::MD_Vector_std(int nb_items_tot, int nb_items_reels, const ArrOfInt& pe_voisins,
                             const ArrsOfInt& items_to_send, const ArrsOfInt& items_to_recv, const ArrsOfInt& blocs_to_recv)
{
  assert(nb_items_tot >= 0);
  assert(nb_items_reels >= -1 && nb_items_reels <= nb_items_tot);
  nb_items_tot_ = nb_items_tot;
  nb_items_reels_ = nb_items_reels;
  const int nb_voisins = pe_voisins.size_array();
  assert(items_to_send.size() == nb_voisins);
  assert(items_to_recv.size() == nb_voisins);
  assert(blocs_to_recv.size() == nb_voisins);

  // selection: liste des indices des pe a conserver dans pe_voisins
  //  (procs avec qui on echange effectivement des donnees)
  ArrOfInt tmp, selection;
  tmp.set_smart_resize(1);
  selection.set_smart_resize(1);
  int i;
  for (i = 0; i < nb_voisins; i++)
    if (items_to_send[i].size_array() > 0 || items_to_recv[i].size_array() > 0 || blocs_to_recv[i].size_array() > 0)
      {
        tmp.append_array(pe_voisins[i]);
        selection.append_array(i);
      }
  const int nb_voisins2 = tmp.size_array();
  // ** pe_voisins_ **
  pe_voisins_ = tmp;

  {
    ArrsOfInt tmpbis(nb_voisins2);

    for (i = 0; i < nb_voisins2; i++)
      tmpbis[i] = items_to_send[selection[i]];
    items_to_send_.set(tmpbis);

    for (i = 0; i < nb_voisins2; i++)
      tmpbis[i] = items_to_recv[selection[i]];
    items_to_recv_.set(tmpbis);

    for (i = 0; i < nb_voisins2; i++)
      tmpbis[i] = blocs_to_recv[selection[i]];
    blocs_to_recv_.set(tmpbis);
  }

  // Calcul de nb_items_to_items_
  {
    nb_items_to_items_.resize_array(nb_voisins2, Array_base::NOCOPY_NOINIT);
    tmp.resize_array(nproc(), Array_base::NOCOPY_NOINIT);
    tmp = 0;
    for (i = 0; i < nb_voisins2; i++)
      tmp[pe_voisins_[i]] = items_to_recv[selection[i]].size_array();
    ;
    envoyer_all_to_all(tmp, tmp);
    for (i = 0; i < nb_voisins2; i++)
      {
        int n = tmp[pe_voisins_[i]];
        assert(n <= items_to_send_.get_list_size(i));
        nb_items_to_items_[i] = n;
      }
  }
  // Calcul de blocs_items_count_
  {
    blocs_items_count_.resize_array(nb_voisins2, Array_base::NOCOPY_NOINIT);
    for (i = 0; i < nb_voisins2; i++)
      {
        const int nblocs = blocs_to_recv_.get_list_size(i) / 2;
        int count = 0;
        for (int j = 0; j < nblocs; j++)
          {
            int deb = blocs_to_recv_(i, j * 2);
            int fin = blocs_to_recv_(i, j * 2 + 1);
            assert(deb >= 0 && fin > deb && fin <= nb_items_tot_);
            count += fin - deb;
          }
        blocs_items_count_[i] = count;
      }
  }
  // Calcul des blocs d'items sequentiels (items non recus d'un autre proc)
  tmp.resize_array(nb_items_tot, Array_base::NOCOPY_NOINIT);
  {
    // Marquage des items recus a zero
    tmp = 1;
    const ArrOfInt& items = items_to_recv_.get_data();
    int n = items.size_array();
    for (i = 0; i < n; i++)
      {
        assert(tmp[items[i]] == 1); // sinon, c'est qu'on recoit deux fois le meme items
        tmp[items[i]] = 0;
      }
    const ArrOfInt& items2 = blocs_to_recv_.get_data();
    assert(items2.size_array() % 2 == 0);
    n = items2.size_array() / 2;
    for (i = 0; i < n; i++)
      {
        const int start = items2[i * 2];
        const int end = items2[i * 2 + 1];
        for (int j = start; j < end; j++)
          {
            assert(tmp[j] == 1); // sinon, c'est qu'on recoit deux fois le meme items
            tmp[j] = 0;
          }
      }

    // Construction d'une liste de blocs d'items sequentiels (ceux qui sont restes a 1)
    ArrOfInt blocs;
    blocs.set_smart_resize(1);
    int nb_seq = 0;
    for (i = 0; i < nb_items_tot; i++)
      {
        if (tmp[i])
          {
            if (nb_items_reels >= 0 && i >= nb_items_reels)
              {
                // On a un item non recu parmi les items virtuels !
                Cerr << "Error in MD_Vector_std::MD_Vector_std(...) [pe " << me() << "]\n "
                     "an item i(" << i << ") > nb_items_reels(" << nb_items_reels << ") is not received from any other processor" << finl;
                Process::exit();
              }
            append_item_to_blocs(blocs, i);
            nb_seq++;
          }
      }
    blocs_items_to_sum_ = blocs;
    nb_items_seq_local_ = nb_seq;
    nb_items_seq_tot_ = mp_sum(nb_seq);
  }

  // Bloc items to compute:
  // Par defaut: un seul bloc
  if (nb_items_reels_ >= 0)
    {
      // Les operateurs sur les tableaux calculent tous les items reels
      blocs_items_to_compute_.resize(2, Array_base::NOCOPY_NOINIT);
      blocs_items_to_compute_[0] = 0;
      blocs_items_to_compute_[1] = nb_items_reels_;
    }
  else
    {
      // Les operateurs sur les tableaux calculent tout
      blocs_items_to_compute_.resize(2, Array_base::NOCOPY_NOINIT);
      blocs_items_to_compute_[0] = 0;
      blocs_items_to_compute_[1] = nb_items_tot_;
    }
}

// Description: method used to dump/restore a descriptor in a file
//  Each process writes a different descriptor. See MD_Vector_tools::restore_vector_with_md()
Entree& MD_Vector_std::readOn(Entree& is)
{
  MD_Vector_base2::readOn(is);
  Param p(que_suis_je());
  ArrOfInt items_send_index, items_send_data, items_index, items_data, blocs_index, blocs_data;
  p.ajouter("pe_voisins", &pe_voisins_);
  p.ajouter("items_to_send_index", &items_send_index);
  p.ajouter("items_to_send_data", &items_send_data);
  p.ajouter("nb_items_to_items", &nb_items_to_items_);
  p.ajouter("items_to_recv_index", &items_index);
  p.ajouter("items_to_recv_data", &items_data);
  p.ajouter("blocs_to_recv_index", &blocs_index);
  p.ajouter("blocs_to_recv_data", &blocs_data);
  p.ajouter("blocs_items_count", &blocs_items_count_);
  p.lire_avec_accolades(is);
  if (items_send_data.size_array())
    items_to_send_.set_index_data(items_send_index, items_send_data);
  if (items_data.size_array())
    items_to_recv_.set_index_data(items_index, items_data);
  if (blocs_data.size_array())
    blocs_to_recv_.set_index_data(blocs_index, blocs_data);
  return is;
}

// Description: method used to dump/restore a descriptor in a file
//  Each process writes a different descriptor. See MD_Vector_tools::dump_vector_with_md()
Sortie& MD_Vector_std::printOn(Sortie& os) const
{
  MD_Vector_base2::printOn(os);
  os << "{" << finl;
  os << "pe_voisins" << space << pe_voisins_ << finl;
  os << "items_to_send_index" << space << items_to_send_.get_index() << finl;
  os << "items_to_send_data" << space << items_to_send_.get_data() << finl;
  os << "nb_items_to_items" << space << nb_items_to_items_ << finl;
  os << "items_to_recv_index" << space << items_to_recv_.get_index();
  os << "items_to_recv_data" << space << items_to_recv_.get_data();
  os << "blocs_to_recv_index" << space << blocs_to_recv_.get_index() << finl;
  os << "blocs_to_recv_data" << space << blocs_to_recv_.get_data();
  os << "blocs_items_count" << space << blocs_items_count_ << finl;
  os << "}" << finl;
  return os;
}
