/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Traitement_particulier_NS_Pression_VEF.h>
#include <Assembleur_base.h>
#include <Milieu_base.h>
#include <Domaine_Cl_VEF.h>

Implemente_instanciable_sans_constructeur(Traitement_particulier_NS_Pression_VEF,"Traitement_particulier_NS_Pression_VEF",Traitement_particulier_NS_Pression);

Traitement_particulier_NS_Pression_VEF::Traitement_particulier_NS_Pression_VEF()
{
}

Sortie& Traitement_particulier_NS_Pression_VEF::printOn(Sortie& is) const
{
  return is;
}

Entree& Traitement_particulier_NS_Pression_VEF::readOn(Entree& is)
{
  return is;
}

inline int Traitement_particulier_NS_Pression_VEF::comprend_champ(const Motcle& mot) const
{
  if (mot == "Pression_porosite") return 1 ;
  return 0 ;
}

int Traitement_particulier_NS_Pression_VEF::a_pour_Champ_Fonc(const Motcle& mot,
                                                              REF(Champ_base)& ch_ref) const
{
  if (mot == "Pression_porosite")
    {
      ch_ref = ch_p;
      return 1 ;
    }
  return 0 ;
}

Entree& Traitement_particulier_NS_Pression_VEF::lire(Entree& is)
{
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;

  is >> motbidon ;

  Motcles les_mots(1);
  {
    les_mots[0] = "Pression_porosite";
  }


  if (motbidon == accouverte)
    {
      // is >> motlu;

      while(motlu != accfermee)
        {
          is >> motlu;
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                Cerr << " Lire Pression_porosite " << finl;
                const Domaine_dis_base& zdis=mon_equation->inconnue().domaine_dis_base();
                const Domaine_VEF& domaine_VEF=ref_cast(Domaine_VEF, zdis);
                //                  const Probleme_base& pb = mon_equation->probleme();
                const int nb_elem = domaine_VEF.nb_elem() ;

                ch_p.associer_domaine_dis_base(zdis);
                ch_p.nommer("Pression_porosite");
                ch_p.fixer_nb_comp(1);
                ch_p.fixer_nb_valeurs_nodales(nb_elem);
                ch_p.fixer_unite("Pa.m3/kg");
                c_pression = 1 ;
                champs_compris_.ajoute_champ(ch_p);
                break ;

              }
            default :
              {
                if (motlu == accfermee)
                  {
                    break ;
                  }
                else
                  {
                    Cerr << "Erreur dans la lecture de Traitement_particulier_Brech_VEF" << finl;
                    Cerr << "Les mots cles possibles sont : calcul_flux ou Richardson " << finl;
                    Cerr << "Vous avez lu :" << motlu << finl;
                    exit();
                    break;
                  }
              }
            }
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Erreur dans la lecture de Traitement_particulier_NS_Brech_VEF 1 ";
          Cerr << "On attendait une } et pas " << motlu << finl;
          exit();
        }
    }

  return is;
}


void Traitement_particulier_NS_Pression_VEF::post_traitement_particulier()
{

  if (c_pression == 1 ) post_traitement_particulier_calcul_pression() ;

}

void Traitement_particulier_NS_Pression_VEF::post_traitement_particulier_calcul_pression()
{
  const Domaine_VEF& zvef=ref_cast(Domaine_VEF, mon_equation->domaine_dis());
  const DoubleVect& porosite_face = mon_equation->milieu().porosite_face();
  int i,comp;
  int nb_face = zvef.nb_faces();
  Operateur_Div divergence = mon_equation->operateur_divergence();
  Operateur_Grad gradient = mon_equation->operateur_gradient();
  SolveurSys solveur_pression_ = mon_equation->solveur_pression();

  DoubleTab& pression=mon_equation->pression().valeurs();
  DoubleTab& vitesse=mon_equation->vitesse().valeurs();
  DoubleTab gradP(vitesse);
  DoubleTab inc_pre(pression);
  DoubleTab secmem(pression);
  DoubleTab grad_temp(vitesse);

  inc_pre  = 0. ;
  secmem = 0. ;
  gradP = 0. ;
  grad_temp = 0. ;

  gradient.calculer(mon_equation->pression().valeurs(),gradP);

  //on veut BM-1Bt(psi*Pression)
  mon_equation->solv_masse()->appliquer(gradP);

  for(i=0; i<nb_face; i++)
    {
      for (comp=0; comp<dimension; comp++)
        grad_temp(i,comp) = gradP(i,comp)/porosite_face(i);

    }
  //mon_equation->solv_masse().appliquer(grad_temp);
  divergence.calculer(grad_temp, secmem);

  secmem *= -1; // car div =-B
  // Correction du second membre d'apres les conditions aux limites :
  mon_equation->assembleur_pression()->modifier_secmem(secmem);

  //solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre,mon_equation->pression());
  solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre);
  DoubleVect& la_pression_porosite = ch_p.valeurs();
  la_pression_porosite = pression;
  //  Cerr << " la_pression_porosite = pression " << la_pression_porosite  << finl;
  //   Cerr << " inc_pre " << inc_pre << finl;
  la_pression_porosite += inc_pre;
}
