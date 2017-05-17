#include <Pb_Conv_Diff.h>

Implemente_instanciable(Pb_Conv_Diff,"Probleme_Convection_Diffusion",Probleme_base);
// XD probleme_convection_diffusion Pb_base probleme_convection_diffusion -1 not_set
// XD   attr convection_diffusion  convection_diffusion convection_diffusion 1 not_set
Sortie& Pb_Conv_Diff::printOn(Sortie& os) const
{                                                
  return Probleme_base::printOn(os);
} 

Entree& Pb_Conv_Diff::readOn(Entree& is) 
{                                                
  return Probleme_base::readOn(is);
}

int Pb_Conv_Diff::nombre_d_equations() const
{
  return 1;
}

const Equation_base& Pb_Conv_Diff::equation(int i) const
{
  assert (i==0);
  return eq_conv_diff;
}

Equation_base& Pb_Conv_Diff::equation(int i)
{
  assert (i==0);
  return eq_conv_diff;
}

void Pb_Conv_Diff::associer_milieu_base(const Milieu_base& un_milieu)
{
  eq_conv_diff.associer_milieu_base(un_milieu);
}
