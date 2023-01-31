/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Champs_compris.h>
#include <Champ_base.h>

Implemente_instanciable(Champs_compris, "Champs_compris", Objet_U);

Sortie& Champs_compris::printOn(Sortie& os) const { return os; }

Entree& Champs_compris::readOn(Entree& is) { return is; }

const Champ_base& Champs_compris::get_champ(const Motcle& motcle) const
{
  REF2(Champ_base) ref_champ;
  if (has_champ(motcle, ref_champ)) return ref_champ;
  throw Champs_compris_erreur();
}

bool Champs_compris::has_champ(const Motcle& motcle, REF2(Champ_base)& ref_champ) const
{
  if (motcle=="??")
    {
      Cerr<<"Champs_compris::get_champ()"<<finl;
      Cerr<<"No field can be requested using the identifier \'??\'"<<finl;
      Process::exit();
    }

  for (const auto& itr : liste_champs_)
    {
      ref_champ = itr.valeur();
      if (ref_champ->le_nom() == motcle)  // case insensitive test
        return true;
      else
        {
          const Noms& syno = ref_champ->get_synonyms();
          int nb_syno = syno.size();
          for (int s = 0; s < nb_syno; s++)
            {
              if (syno[s] == motcle)  // case insensitive test
                return true;
            }
          int nb_composantes = ref_champ->nb_comp();
          for (int i = 0; i < nb_composantes; i++)
            {
              if (motcle == ref_champ->nom_compo(i)) // case insensitive test
                return true;
            }
        }
    }
  return false;
}

int new_liste_add_if_not(LIST(Nom)& new_list,const Nom& nom_champ)
{
  Motcle mot(nom_champ);
  for (const auto& itr : new_list)
    {
      if (mot == itr)
        return 0;
    }
  new_list.add(nom_champ);
  return 1;
}

void rebuild_liste_noms(const LIST(REF2(Champ_base))& liste_champs_, const Noms& liste_noms_, Noms& liste_noms_construits_,int info=0)
{
  if (liste_noms_construits_.size()<liste_noms_.size())
    liste_noms_construits_=liste_noms_;
  LIST(Nom) new_liste;
  int size = liste_noms_.size();
  for (int i=0; i<size; i++)
    new_liste.add(liste_noms_[i]);

  Nom nom_champ;
  for (const auto &itr : liste_champs_)
    {
      const Champ_base& ch = itr.valeur();
      nom_champ = ch.le_nom();
      //Cerr<<" ok "<<nom_champ<<finl;
      if (nom_champ != Nom())
        new_liste_add_if_not(new_liste, nom_champ);

      const Noms& syno = ch.get_synonyms();
      int nb_syno = syno.size();
      for (int s = 0; s < nb_syno; s++)
        {
          if (syno[s] != Nom())
            new_liste_add_if_not(new_liste, syno[s]);
        }
      int nb_composantes = ch.nb_comp();
      for (int i = 0; i < nb_composantes; i++)
        {
          nom_champ = (ch.nom_compo(i));
          if (nom_champ != Nom())
            new_liste_add_if_not(new_liste, nom_champ);
        }
    }

  int size_new_liste=new_liste.size();
  int prem=1;
  if (size_new_liste!=liste_noms_construits_.size())
    {
      if (info)
        if (prem)
          {
            prem=0;
          }
      Noms nn_liste_noms_construits_(size_new_liste);
      for (int i=0; i<liste_noms_construits_.size(); i++)
        if (i<size_new_liste)
          nn_liste_noms_construits_[i]=liste_noms_construits_[i];
      liste_noms_construits_ =nn_liste_noms_construits_;
    }

  for (int i=0; i<size_new_liste; i++)
    if (liste_noms_construits_[i]!=new_liste(i))
      {
        if (info)
          {
            if (prem)
              {
                prem=0;
              }
            //Cout<<liste_noms_construits_[i] << " va etre remplace par "<< new_liste(i)<<finl;
          }
        liste_noms_construits_[i]=new_liste(i);
      }
  //if ((prem==0) && (info))
  // Cout<<"liste apres modif "<<liste_noms_construits_<<finl;
  // Cerr<<" ici2 "<<finl;
}




void Champs_compris::ajoute_champ(const Champ_base& champ)
{

  Motcle nom_champ_add = champ.le_nom();
  for (const auto &itr : liste_champs_)
    {
      const Champ_base& ch = itr.valeur();
      const Nom& nom_champ = ch.le_nom();
      if (nom_champ == nom_champ_add) return;
    }

  //liste_champs_.add(champ_ref);
  liste_champs_.add(champ);
  Cerr<<"Champs_compris::ajoute_champ " <<champ.le_nom()<<finl;
  rebuild_liste_noms(liste_champs_,liste_noms_,liste_noms_construits_);

}

void Champs_compris::ajoute_nom_compris( const Nom& mot)
{
  liste_noms_.add(mot);
  rebuild_liste_noms(liste_champs_,liste_noms_,liste_noms_construits_);
}
const Noms Champs_compris::liste_noms_compris() const
{
  Noms liste_noms_prov(liste_noms_construits_);
  rebuild_liste_noms(liste_champs_,liste_noms_,liste_noms_prov,1);
  return liste_noms_prov;
}
/*Noms& Champs_compris::liste_noms_compris()
  {
  abort();
  return liste_noms_;
  }
*/
