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

#include <Precond_local.h>
#include <Matrice_Bloc_Sym.h>

Implemente_instanciable(Precond_local,"Precond_local",Precond_base);

// printOn et readOn
Sortie& Precond_local::printOn(Sortie& s ) const
{
  return s << le_precond_local_.que_suis_je() << finl;
}

Entree& Precond_local::readOn(Entree& is )
{
  is >> le_precond_local_;
  // Pour eviter trop d'affichage (Convergence)
  if (!le_precond_local_.valeur().limpr())
    le_precond_local_.valeur().fixer_limpr(-1);
  return is;
}

void Precond_local::prepare_(const Matrice_Base& la_matrice, const DoubleVect& secmem)
{
  le_precond_local_.valeur().reinit();

  if(sub_type(Matrice_Bloc_Sym,la_matrice))
    {
      const Matrice_Bloc_Sym& m = ref_cast(Matrice_Bloc_Sym, la_matrice);
      m.BlocSymToMatMorseSym(matrice_de_travail_);
      // La matrice est t'elle definie ? On regarde son bloc(0,0)
      const Matrice_Bloc&       bloc     = ref_cast(Matrice_Bloc, m.get_bloc(0,0).valeur());
      const Matrice_Morse_Sym& sous_bloc= ref_cast(Matrice_Morse_Sym, bloc.get_bloc(0,0).valeur());
      int ok = sous_bloc.get_est_definie();
      matrice_de_travail_.set_est_definie(ok);
      matrice_a_resoudre_ = matrice_de_travail_;
    }
  else if (sub_type(Matrice_Bloc,la_matrice))
    {
      const Matrice_Bloc& matrice = ref_cast(Matrice_Bloc,la_matrice);
      const Matrice&      MB00 = matrice.get_bloc(0,0);
      if(!sub_type(Matrice_Morse_Sym,MB00.valeur()))
        {
          Cerr<<"Precond_local::prepare_ MB00 is not of type deriving from matrice_morse_sym "
              <<MB00.valeur().que_suis_je()<<finl;
          exit();
        }
      matrice_a_resoudre_ = MB00.valeur();
    }
  else if (sub_type(Matrice_Morse_Sym,la_matrice))
    {
      matrice_a_resoudre_ = la_matrice;
    }
  else
    {
      Cerr<<"In Precond_local: prepare_, the type of the matrix " << la_matrice.que_suis_je() << finl;
      Cerr<<"is not yet supported." << finl;
      exit();
    }
  Precond_base::prepare_(la_matrice, secmem);
}

int Precond_local::preconditionner_(const Matrice_Base& la_matrice,
                                    const DoubleVect& secmem,
                                    DoubleVect& solution)
{
  const Matrice_Base& m = matrice_a_resoudre_.valeur();
  int ok = precond(m, secmem, solution);
  return ok;
}

int Precond_local::precond(const Matrice_Base& mat,
                           const DoubleVect& secmem,
                           DoubleVect& solution)
{
  if(Process::nproc()>1)
    {
      Cerr << "Precond_local::precond not coded for nproc > 1" << finl;
      exit();
#if 0
      VECT(Descripteur)& desc = solution.structure().descripteur();
      int nb_struct = desc.size();
      if(nb_struct==1)
        res_syst_loc_simple(mat,secmem,solution,champ);
      else if (nb_struct==2)
        res_syst_loc_hybride(mat,secmem,solution,champ);
      else
        {
          Cerr << "Cases still not supported in Precond_local::preconditionner." << finl;
          // PL: Il suffit de generaliser res_syst_loc_hybride comme dans NP une
          // taille nb_struct quelconque...pour supporter P0+P1+Pa (pas le temps de le faire).
          exit();
        }
#endif
      return 0;
    }
  else
    {
      /*
        if (ref_champ_inc_.non_nul())

        return le_precond_local_.resoudre_systeme(mat,secmem,solution,ref_champ_inc_.valeur());
        else
      */
      return le_precond_local_.resoudre_systeme(mat,secmem,solution);
    }
}

void Precond_local::res_syst_loc_simple(const Matrice_Morse_Sym& mat,
                                        const DoubleVect& secmem,
                                        DoubleVect& solution,
                                        const Champ_Inc_base& champ)
{
  exit();
#if 0
  if(a_remplir_)
    {
      a_remplir_=0;
      int n_reel = secmem.size();
      la_matrice_locale_.dimensionner(n_reel,0);
      int ii,jj;
      int cpt=0;
      const IntVect& tab1_tot = mat.tab1_;
      const IntVect& tab2_tot = mat.tab2_;
      const DoubleVect& coeff_tot = mat.coeff_;
      IntVect& tab1_reel = la_matrice_locale_.tab1_;
      IntVect& tab2_reel = la_matrice_locale_.tab2_;
      DoubleVect& coeff_reel = la_matrice_locale_.coeff_;
      tab1_reel(0)=1;
      for(ii=0; ii<n_reel; ii++)
        {
          int dl = tab1_tot(ii);
          int fl = tab1_tot(ii+1);
          for(jj=dl; jj<fl; jj++)
            {
              if(tab2_tot(jj-1)<=n_reel)
                cpt++;
            }
          tab1_reel(ii+1)=cpt+1;
        }
      la_matrice_locale_.dimensionner(n_reel,tab1_reel(n_reel)-1);
      cpt=0;
      for(ii=0; ii<n_reel; ii++)
        {
          int dl = tab1_tot(ii);
          int fl = tab1_tot(ii+1);
          for(jj=dl; jj<fl; jj++)
            {
              if(tab2_tot(jj-1)<=n_reel)
                {
                  tab2_reel(cpt)=tab2_tot(jj-1);
                  coeff_reel(cpt++)=coeff_tot(jj-1);
                }
            }
        }
      la_matrice_locale_.set_est_definie(mat.get_est_definie());
    }
  le_precond_local_.resoudre_systeme(la_matrice_locale_,
                                     secmem,solution,champ);
#endif
}

void Precond_local::res_syst_loc_hybride(const Matrice_Morse_Sym& mat,
                                         const DoubleVect& secmem,
                                         DoubleVect& solution,
                                         const Champ_Inc_base& champ)
{
  exit();
#if 0
  if(a_remplir_)
    {
      a_remplir_=0;
      {
        const Zone_dis_base& zdisb = champ.zone_dis_base();
        const Zone& zone = zdisb.zone();
        int nb_som = zone.nb_som();
        int nb_elem = zone.nb_elem();
        int nb_elem_tot = zone.nb_elem_tot();

        la_matrice_locale_.dimensionner(nb_elem+nb_som,0);
        int ii,jj;
        int cpt=0;
        const IntVect& tab1_tot = mat.tab1_;
        const IntVect& tab2_tot = mat.tab2_;
        const DoubleVect& coeff_tot = mat.coeff_;
        IntVect& tab1_reel = la_matrice_locale_.tab1_;
        IntVect& tab2_reel = la_matrice_locale_.tab2_;
        DoubleVect& coeff_reel = la_matrice_locale_.coeff_;
        tab1_reel(0)=1;
        for(ii=0; ii<nb_elem; ii++)
          {
            int dl = tab1_tot(ii);
            int fl = tab1_tot(ii+1);
            for(jj=dl; jj<fl; jj++)
              {
                int i_col = tab2_tot(jj-1);
                if( (i_col<=nb_elem) ||
                    ((i_col>=nb_elem_tot+1) && (i_col<=(nb_elem_tot+nb_som))) )
                  cpt++;
              }
            tab1_reel(ii+1)=cpt+1;
          }
        for(ii=nb_elem_tot; ii<(nb_elem_tot+nb_som); ii++)
          {
            int dl = tab1_tot(ii);
            int fl = tab1_tot(ii+1);
            for(jj=dl; jj<fl; jj++)
              {
                if(tab2_tot(jj-1)<=(nb_elem_tot+nb_som))
                  cpt++;
              }
            tab1_reel((ii-nb_elem_tot)+nb_elem+1)=cpt+1;
          }
        la_matrice_locale_.dimensionner(nb_elem+nb_som,tab1_reel(nb_elem+nb_som)-1);
        cpt=0;
        for(ii=0; ii<nb_elem; ii++)
          {
            int dl = tab1_tot(ii);
            int fl = tab1_tot(ii+1);
            for(jj=dl; jj<fl; jj++)
              {
                int i_col = tab2_tot(jj-1);
                if(i_col<=nb_elem)
                  {
                    tab2_reel(cpt)=tab2_tot(jj-1);
                    coeff_reel(cpt++)=coeff_tot(jj-1);
                  }
                if( (i_col>=nb_elem_tot+1) &&
                    (i_col<=(nb_elem_tot+nb_som)) )
                  {
                    tab2_reel(cpt)=tab2_tot(jj-1)-nb_elem_tot+nb_elem;
                    coeff_reel(cpt++)=coeff_tot(jj-1);
                  }
              }
          }
        for(ii=nb_elem_tot; ii<(nb_elem_tot+nb_som); ii++)
          {
            int dl = tab1_tot(ii);
            int fl = tab1_tot(ii+1);
            for(jj=dl; jj<fl; jj++)
              {
                if(tab2_tot(jj-1)<=(nb_elem_tot+nb_som))
                  {
                    tab2_reel(cpt)=tab2_tot(jj-1)-nb_elem_tot+nb_elem;
                    coeff_reel(cpt++)=coeff_tot(jj-1);
                  }
              }
          }
        la_matrice_locale_.set_est_definie(mat.get_est_definie());
        {
          int i,j,k,l;

          const ArrOfInt& items_tot = solution.get_items_communs_tot();
          for(i=0; i<items_tot.size_array(); i++)
            {
              k = items_tot[i]-nb_elem_tot+nb_elem;
              for(j=tab1_reel[k]+1; j<tab1_reel[k+1]; j++)
                {
                  coeff_reel(j-1) = 0. ;
                }
              // On parcourt les lignes au-dessus pour annuler la colonne k
              for(j=0; j<k; j++)
                {
                  for(l=tab1_reel[j]+1; l<tab1_reel[j+1]; l++)
                    {
                      if((tab2_reel[l-1]-1)==k)
                        {
                          coeff_reel(l-1) = 0. ;
                        }
                    }
                }
            }
        }
      }
    }
  VECT(Descripteur)& desc = solution.structure().descripteur();
  int nb_struct = desc.size();
  int ind_struct;
  int taille=0;
  for(ind_struct=0; ind_struct<nb_struct; ind_struct++)
    {
      Descripteur& desci=desc[ind_struct];
      taille+=desci.nb();
    }
  DoubleTab sol_loc(taille);
  DoubleTab sec_loc(taille);
  int cpt=0;
  for(ind_struct=0; ind_struct<nb_struct; ind_struct++)
    {
      Descripteur& desci=desc[ind_struct];
      int i;
      int deb=desci.deb();
      int fin=deb+desci.nb();
      for(i=deb; i<fin; i++)
        {
          sol_loc[cpt] = solution[i];
          sec_loc[cpt] = secmem[i];
          cpt++;
        }
    }
  le_precond_local_.resoudre_systeme(la_matrice_locale_,
                                     sec_loc,sol_loc,champ);
  cpt=0;
  for(ind_struct=0; ind_struct<nb_struct; ind_struct++)
    {
      Descripteur& desci=desc[ind_struct];
      int i;
      int deb=desci.deb();
      int fin=deb+desci.nb();
      for(i=deb; i<fin; i++)
        solution[i]=sol_loc[cpt++];
    }
  solution.echange_espace_virtuel();
#endif
}
