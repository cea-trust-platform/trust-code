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

#include <Ecrire_Champ_MED.h>
#include <Champ_Don_base.h>
#include <Probleme_base.h>
#include <Format_Post.h>

Implemente_instanciable(Ecrire_Champ_MED,"Ecrire_Champ_MED",Interprete);
// XD ecrire_champ_med interprete ecrire_champ_med -1 Keyword to write a field to MED format into a file.
// XD attr nom_dom ref_domaine nom_dom 0 domain name
// XD attr nom_chp ref_field_base nom_chp 0 field name
// XD attr file chaine file 0 file name

/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 *     Imprime l'interprete sur un flot de sortie
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Ecrire_Champ_MED::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 *     Ecrit l'interprete sur un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Ecrire_Champ_MED::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


/*! @brief Fonction principale de l'interprete Ecrire_Champ_MED: erreur si cela echoue.
 *
 * @param (Entree& is) un flot d'entree, a partir duquel on lit les arguments
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Ecrire_Champ_MED::interpreter(Entree& is)
{
  Nom nom_domaine, nom_champ,nom_fic;

  is >> nom_domaine >> nom_champ >> nom_fic;
  Objet_U& obj_champ=objet(nom_champ);

  if (!sub_type(Champ_Don_base, obj_champ))
    {
      Cerr << "The field "<< nom_champ << " is not of type Champ_Don" << finl;
      Process::exit();
    }

  Champ_Don_base& chp = ref_cast(Champ_Don_base,obj_champ);


  /*  EcrMED ecrmed;
      ecrmed.ecrire_domaine(nom_fic,dom,nom_domaine);
      ecrmed.ecrire_champ("CHAMPMAILLE",nom_fic,nom_domaine,"critere",const DoubleTab&val,const Noms& unite,const Nom& type_elem,double time,int compteur)
  */

  Objet_U& obj_dom=objet(nom_domaine);
  const Domaine& dom = ref_cast(Domaine,obj_dom);
  Format_Post post_typer;
  post_typer.typer_direct("format_post_med");
  Format_Post_base& post=ref_cast(Format_Post_base,post_typer.valeur());
  Nom nom_fic2(nom_fic);
  nom_fic2.prefix(".med");
  post.initialize_by_default(nom_fic2);
  int est_le_premier_post=1;
  post.ecrire_entete(0.,0,est_le_premier_post);
  int reprise = 0;
  double t_init = 0.;

  // ecriture domaine
  post.preparer_post(dom.le_nom(),est_le_premier_post,reprise,t_init);
  post.ecrire_domaine(dom, est_le_premier_post);
  double tps=chp.temps();

  post.ecrire_temps(tps);
  post.init_ecriture(tps,-1.,est_le_premier_post,dom);
  chp.corriger_unite_nom_compo();
  // ecriture champ
  //chp.postraiter_champ(dom, tps, nom_champ, "ELEM", post);
  // la nature ne sert pas en MED
  Nom Nature("pas_defini");
  post.ecrire_champ(dom,chp.unites(),chp.noms_compo(),-1,tps,Motcle(chp.le_nom()),dom.le_nom(),Motcle("elem"),Nature,chp.valeurs());
  int fin=1;
  post.finir(fin);
  return is;
}
