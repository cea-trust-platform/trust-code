/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Solv_rocALUTION.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/0
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_rocALUTION.h>
#include <Matrice_Morse.h>
#include <Matrice_Morse_Sym.h>
#include <DoubleVect.h>
#include <EChaine.h>
#include <Motcle.h>
#include <SFichier.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_rocALUTION,"Solv_rocALUTION",SolveurSys_base);

// printOn
Sortie& Solv_rocALUTION::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_rocALUTION::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

Solv_rocALUTION::Solv_rocALUTION(const Solv_rocALUTION& org)
{
  // on relance la lecture ....
  EChaine recup(org.get_chaine_lue());
  readOn(recup);
}

// Lecture et creation du solveur
void Solv_rocALUTION::create_solver(Entree& entree)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  lecture(entree);
  EChaine is(get_chaine_lue());

  Motcle accolade_ouverte("{"), accolade_fermee("}");
  Nom solver, precond, motlu;
  is >> solver;   // On lit le solveur en premier puis les options du solveur: PETSC ksp { ... }
  is >> motlu; // On lit l'accolade
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the parameters of the solver " << solver << " { ... }" << finl;
      Cerr << "We expected " << accolade_ouverte << " instead of " << motlu << finl;
      exit();
    }
  Motcles les_solveurs(2);
  {
    les_solveurs[0] = "GCP";
    les_solveurs[1] = "GMRES";
  }
  int rang=les_solveurs.search(solver);
  switch(rang)
    {
    case 0:
      {
        break;
      }
    case 1:
      {
        break;
      }
    default:
      {
        Cerr << solver << " solver not recognized for rocALUTION:" << finl << les_solveurs << finl;
        Process::exit();
      }
    }
  // Lecture des parametres du solver:
  Motcles les_parametres_solveur(3);
  {
    les_parametres_solveur[0] = "impr";
    les_parametres_solveur[1] = "seuil"; // Seuil absolu (atol)
    les_parametres_solveur[2] = "precond";
  }
  is >> motlu;
  while (motlu!=accolade_fermee)
    {
      switch (les_parametres_solveur.search(motlu))
        {
        case 0:
          {
            break;
          }
        case 1:
          {
            break;
          }
        case 2:
          {
            break;
          }
        default:
          {
            Cerr << solver << " keyword not recognized for solver:" << finl << les_parametres_solveur << finl;
            Process::exit();
          }
        }
    }
#endif
}

int Solv_rocALUTION::resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
    // Build matrix
    const Matrice_Morse& csr = ref_cast(Matrice_Morse, a);
  int N = csr.get_tab1().size_array()-1;
  int nnz = csr.get_coeff().size_array();
  ArrOfInt tab1_c(csr.get_tab1()); // Passage Fortran->C
  for (int i=0;i<tab1_c.size_array();i++)
      tab1_c(i)--;
  int* csr_row_ptr = (int*)tab1_c.addr();
  ArrOfInt tab2_c(csr.get_tab2()); // Passage Fortran->C
  for (int i=0;i<tab2_c.size_array();i++)
      tab2_c(i)--;
  int* csr_row_col = (int*)tab2_c.addr();
  double* csr_val = (double*)csr.get_coeff().addr();

    // Save matrix
    if (Process::nproc() > 1) Process::exit("Error, matrix market format is not available yet in parallel.");
    Nom filename(Objet_U::nom_du_cas());
    filename += "_matrix";
    filename += ".mtx";
    SFichier mtx(filename);
    mtx.precision(14);
    mtx.setf(ios::scientific);
    int rows = csr.get_tab1().size_array()-1;
    mtx << "%%MatrixMarket matrix coordinate real " << (sub_type(Matrice_Morse_Sym, a) ? "general" : "symmetric") << finl;
    Cerr << "Matrix (" << rows << " lines) written into file: " << filename << finl;
    mtx << "%%matrix" << finl;
    mtx << rows << " " << rows << " " << csr_row_ptr[rows] << finl;
    for (int row=0; row<rows; row++)
        for (int j=csr_row_ptr[row]; j<csr_row_ptr[row+1]; j++)
            mtx << row+1 << " " << csr_row_col[j]+1 << " " << csr_val[j] << finl;

    mat.SetDataPtrCSR(&csr_row_ptr, &csr_row_col, &csr_val, "a", nnz, N, N);
    Cout << "Provisoire mat.GetN()=" << mat.GetN() << finl;
    delete csr_row_ptr;
    delete csr_row_col;
    delete csr_val;

  mat.Info();
  // Move objects to accelerator
  mat.MoveToAccelerator();
  sol.MoveToAccelerator();
  rhs.MoveToAccelerator();
  res.MoveToAccelerator();

  // Allocate vectors
  sol.Allocate("sol", mat.GetN());
  rhs.Allocate("rhs", mat.GetM());
  res.Allocate("res", mat.GetN());

  // Linear Solver
  CG<LocalMatrix<double>, LocalVector<double>, double> ls;

  // Preconditioner
  SGS<LocalMatrix<double>, LocalVector<double>, double> p;
  //p.SetRelaxation(1.6); // ToDo omega
  rhs.Info();
  
  // Build rhs and initial solution:
  assert(mat.GetN()==b.size_array());
  assert(mat.GetN()==x.size_array());
  double* ptr_x = x.addr();
  sol.SetDataPtr(&ptr_x, "sol", x.size_array());
  delete ptr_x;
  double* ptr_b = (double*)b.addr();
  rhs.SetDataPtr(&ptr_b, "rhs", b.size_array());
  delete ptr_b;
  // Set solver operator
  ls.SetOperator(mat);
  ls.SetPreconditioner(p);
  ls.Build();
  ls.Verbose(2); // Verbosity output

  // Tolerances:
  double atol = 1.e-5;
  double rtol = 1.e-5;
  double div_tol = 1e3;
  ls.InitTol(atol, rtol, div_tol);
  ls.InitMaxIter(300);
  //ls.InitMinIter(20);

  // Print matrix info
  mat.Info();
  ls.Print();
  p.Print();

  // Start time measurement
  double tick, tack;
  tick = rocalution_time();

  // Solve A x = rhs
  ls.Solve(rhs, &sol);
  if (ls.GetSolverStatus()==3) Process::exit("Divergence for solver.");
  if (ls.GetSolverStatus()==4) Cout << "Maximum number of iterations reached." << finl;

  // Stop time measurement
  tack = rocalution_time();
  Cout << "Solver execution:" << (tack - tick) / 1e6 << " sec" << finl;

  int nb_iter = ls.GetIterationCount();
  // Clear solver ?
  ls.Clear();

    // Check residual again e=||Ax-rhs||
  mat.Apply(sol, &res);
  res.ScaleAdd(-1.0, rhs);
  Cout << "||Ax - rhs||_2 = " << res.Norm() << finl;
  if (res.Norm()>atol)
  {
      Cerr << "Solution not correct !" << finl;
      Process::exit();
  }
  return nb_iter;
#else
    return -1;
#endif
}
