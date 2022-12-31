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

#include <Traitement_particulier_NS_Pression_VDF.h>
#include <Navier_Stokes_std.h>
#include <Milieu_base.h>
#include <Zone_Cl_VDF.h>
#include <TRUSTTrav.h>
#include <Zone_VDF.h>

Implemente_instanciable_sans_constructeur(Traitement_particulier_NS_Pression_VDF,"Traitement_particulier_NS_Pression_VDF",Traitement_particulier_NS_Pression);

Traitement_particulier_NS_Pression_VDF::Traitement_particulier_NS_Pression_VDF()
{
}

Sortie& Traitement_particulier_NS_Pression_VDF::printOn(Sortie& is) const
{
  return is;
}

Entree& Traitement_particulier_NS_Pression_VDF::readOn(Entree& is)
{
  return is;
}

inline int Traitement_particulier_NS_Pression_VDF::comprend_champ(const Motcle& mot) const
{
  if (mot == "Pression_porosite") return 1 ;
  return 0 ;
}

int Traitement_particulier_NS_Pression_VDF::a_pour_Champ_Fonc(const Motcle& mot,
                                                              REF(Champ_base)& ch_ref) const
{
  if (mot == "Pression_porosite")
    {
      ch_ref = ch_p;
      return 1 ;
    }
  return 0 ;
}

Entree& Traitement_particulier_NS_Pression_VDF::lire(Entree& is)
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
                const Zone_dis_base& zdis=mon_equation->inconnue().zone_dis_base();
                const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdis);
                //                  const Probleme_base& pb = mon_equation->probleme();
                const int nb_elem = zone_VDF.nb_elem() ;

                ch_p.associer_zone_dis_base(zdis);
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


void Traitement_particulier_NS_Pression_VDF::post_traitement_particulier()
{
  if (c_pression == 1 ) post_traitement_particulier_calcul_pression();
}

void Traitement_particulier_NS_Pression_VDF::post_traitement_particulier_calcul_pression()
{
  const Zone_VDF& zvdf=ref_cast(Zone_VDF, mon_equation->zone_dis().valeur());
  const DoubleVect& porosite_face = mon_equation->milieu().porosite_face();
  int i;
  int nb_face = zvdf.nb_faces();
  Champ_Inc gradient_P;
  Champ_Inc la_pression = mon_equation->pression();
  Operateur_Div divergence = mon_equation->operateur_divergence();
  Operateur_Grad gradient = mon_equation->operateur_gradient();
  SolveurSys solveur_pression_ = mon_equation->solveur_pression();

  DoubleTab& pression=la_pression.valeurs();
  DoubleTrav inc_pre(pression);
  DoubleTrav secmem(pression);
  gradient.calculer(la_pression.valeurs(),gradient_P.valeurs());

  //on veut BM-1Bt(spi*Pression)
  DoubleTab& grad=gradient_P.valeurs();
  mon_equation->solv_masse().appliquer(grad);
  DoubleTab& grad_temp = grad;
  for(i=0; i<nb_face; i++)
    {
      grad_temp(i) /=porosite_face(i);
    }
  divergence.calculer(grad_temp, secmem);
  secmem *= -1; // car div =-B
  solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre);
  Cerr <<"inc_pre " << inc_pre << finl;
}
