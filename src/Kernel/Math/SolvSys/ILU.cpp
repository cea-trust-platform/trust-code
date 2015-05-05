/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        ILU.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <ILU.h>
#include <Matrice_Bloc.h>
#include <Matrice_Morse_Sym.h>
#include <SPV2.h>
#include <Param.h>

Implemente_instanciable(ILU,"ILU",Precond_base);
//
// printOn et readOn

Sortie& ILU::printOn(Sortie& s ) const
{
  s << " { type "<<precond_ ;
  s << " filling "<< lfil_ << " } " ;
  return s;
}

Entree& ILU::readOn(Entree& is )
{
  precond_ = _TYPE_ILU_;
  lfil_ = _LFIL_ILU_;

  Param param(que_suis_je());
  param.ajouter("type", &precond_, Param::OPTIONAL);
  param.dictionnaire("0", PRECOND_NUL);
  param.dictionnaire("1", PRECOND_GAUCHE);
  param.dictionnaire("2", PRECOND_DROITE);
  param.dictionnaire("3", PRECOND_GAUCHE_DROITE);
  param.ajouter("filling", &lfil_, Param::OPTIONAL);
  param.lire_avec_accolades(is);

  return is;
}
void MorseSymToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M)
{
  M = MS;
  Matrice_Morse mattmp(MS);
  int i, ordre;
  ordre = M.ordre();
  M.transpose(mattmp);
  for (i = 0; i < ordre; i++)
    M(i, i) = 0.;
  M = mattmp + M;
}


void ILU::prepare_(const Matrice_Base& la_matrice, const DoubleVect& v)
{
  if (get_status() != READY)
    {
      if(sub_type(Matrice_Morse_Sym,la_matrice))
        {
          Cerr<<"ILU ERROR : one is not able to carry out a ILU preconditioning"
              <<" on a symetric matrix"<<finl;
          exit();
        }
      else if(sub_type(Matrice_Morse,la_matrice))
        {
          const Matrice_Morse& m = ref_cast(Matrice_Morse,la_matrice);
          factorisation(m, v);
        }
      else if(sub_type(Matrice_Bloc,la_matrice))
        {
          const Matrice_Bloc& matrice = ref_cast(Matrice_Bloc,la_matrice);
          const Matrice& bloc0 = matrice.get_bloc(0,0);
          if(sub_type(Matrice_Morse_Sym,bloc0.valeur()))
            {
              /*
                Cerr<<"ILU ERROR : one is not able to carry out a ILU preconditioning"
                <<" on a symetric matrix"<<finl;
                exit();
              */
              const Matrice_Morse_Sym ms=ref_cast(Matrice_Morse_Sym,bloc0.valeur());
              Matrice_Morse m2;
              MorseSymToMorse(ms,m2);
              factorisation(m2,v);
            }
          else if(sub_type(Matrice_Morse,bloc0.valeur()))
            {
              const Matrice_Morse& m = ref_cast(Matrice_Morse,bloc0.valeur());
              factorisation(m, v);
            }
        }
      else
        {
          Cerr<<"ILU : Warning : preconditioning can be carry out only for linear systems based on Matrice_Morse or Matrice_Bloc type matrixes"<<finl;
          exit();
        }
    }
  Precond_base::prepare_(la_matrice, v);
}

// Description:
//    Calcule la solution du systeme lineaire: A * solution = b
//    avec la methode de relaxation ILU.
int ILU::preconditionner_(const Matrice_Base& la_matrice,
                          const DoubleVect& u,
                          DoubleVect& s)
{
  int n = u.size();
  return ilu(n,u,s);
}

int ILU::factoriser(const Matrice_Morse& mat,
                    const DoubleVect& b)
{
  int ie=1;                                // niveau d'erreur
  if (precond_!=0)
    {
      int n1 = mat.nb_lignes();
      int n2 = mat.nb_coeff()+(2*lfil_*n1);        // nombre de termes non nuls dans LU

      //Cerr<<"Calculation of matrix preconditioning by LU decomposition..."<<finl;
      int  iw = n2 + 2;
      ju.resize_array(n1+1);
      jlu.resize_array(iw);
      alu.resize_array(iw);
      double to = 1.e-4;                // filtrage des coefficents de L U
      //   ArrOfDouble wu(n1+1);                // tableau de travail pour ilut
      // ArrOfDouble wl(n1);                // tableau de travail pour ilut
      // ArrOfInt jr(n1);                        // tableau de travail pour ilut
      // ArrOfInt jwu(n1);                        // tableau de travail pour ilut
      // ArrOfInt jwl(n1);                        // tableau de travail pour ilut
      //       F77NAME(ILUT)(&n1, mat.coeff_, mat.tab2_, mat.tab1_, &lfil_,
      //                     &to, alu.addr(), jlu.addr(), ju.addr(),
      //                     &iw, wu.addr(),
      //                     wl.addr(), jr.addr(), jwl.addr(), jwu.addr(), &ie);

      ArrOfDouble wbis(n1+1);
      ArrOfInt jwbis(2*n1);

      F77NAME(ILUTV2)(&n1, mat.coeff_.addr(), mat.tab2_.addr(), mat.tab1_.addr(), &lfil_,
                      &to, alu.addr(), jlu.addr(), ju.addr(),
                      &iw, wbis.addr(),
                      jwbis.addr(), &ie);
      switch(ie)
        {
        case 0 :
          break;
        case -1 :
          Cerr << "Error in ilut 'matrix may be wrong' dixit SAAD" << finl;
          exit();
        case  -2  :
          Cerr << "Error in ilut : overflow in L " << finl;
          exit();
        case   -3  :
          Cerr << "Error in ilut : overflow in U " << finl;
          exit();
        case    -4   :
          Cerr << "Illegal value for lfil : it may be a memory trouble " << finl;
          exit();
        case    -5  :
          Cerr << "Empty line met " << finl;
          exit();
        default :
          Cerr << "Pivot null met ! at step " << ie << finl;;
          exit();
        }
    }
  return 0;
}

int ILU::factorisation(const Matrice_Morse& mat,const DoubleVect& secmem)
{
  if (nproc()==1)
    {
      return factoriser(mat,secmem);
    }
  if(1)
    {
      //dimensionnenment de la matrice MLOC
      int n_reel = secmem.size();
      //      int n_tot = secmem.size_array();
      MLOC.dimensionner(n_reel,0);
      int ii,jj;
      int cpt=0;
      const IntVect& tab1_tot = mat.tab1_;
      const IntVect& tab2_tot = mat.tab2_;
      //      const DoubleVect& coeff_tot = mat.coeff_;
      IntVect& tab1_reel = MLOC.tab1_;
      IntVect& tab2_reel = MLOC.tab2_;
      //      DoubleVect& coeff_reel = MLOC.coeff_;
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
      MLOC.dimensionner(n_reel,tab1_reel(n_reel)-1);
      cpt=0;
      for(ii=0; ii<n_reel; ii++)
        {
          int dl = tab1_tot(ii);
          int fl = tab1_tot(ii+1);
          for(jj=dl; jj<fl; jj++)
            {
              if(tab2_tot(jj-1)<=n_reel)
                {
                  tab2_reel(cpt++)=tab2_tot(jj-1);
                  //coeff_reel(cpt++)=coeff_tot(jj-1);
                }
            }
        }
    }
  // copie des coeffs
  {
    int ii,jj;
    int cpt=0;
    int n_reel = secmem.size();

    const IntVect& tab1_tot = mat.tab1_;
    const IntVect& tab2_tot = mat.tab2_;
    const DoubleVect& coeff_tot = mat.coeff_;
    //    IntVect& tab1_reel = MLOC.tab1_;
    //    IntVect& tab2_reel = MLOC.tab2_;
    DoubleVect& coeff_reel = MLOC.coeff_;

    for(ii=0; ii<n_reel; ii++)
      {
        int dl = tab1_tot(ii);
        int fl = tab1_tot(ii+1);
        for(jj=dl; jj<fl; jj++)
          {
            if(tab2_tot(jj-1)<=n_reel)
              {
                //tab2_reel(cpt)=tab2_tot(jj-1);
                coeff_reel(cpt++)=coeff_tot(jj-1);
              }
          }
      }
  }

  return factoriser(MLOC,secmem);
}

int ILU::ilu(int n, const DoubleVect& u, DoubleVect& s)
{
  F77NAME(LUSOLV2)(&n,u.addr(),s.addr(),alu.addr(),jlu.addr(),ju.addr());
  return 1;
}

int ILU::ilut(int n, const DoubleVect& u, DoubleVect& s)
{
  F77NAME(LUTSOLV2)(&n,u.addr(),s.addr(),alu.addr(),jlu.addr(),ju.addr());
  return 1;
}

