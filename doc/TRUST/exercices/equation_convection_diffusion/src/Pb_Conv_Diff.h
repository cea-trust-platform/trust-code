
#ifndef Pb_Conv_Diff_included
#define Pb_Conv_Diff_included

#include <Probleme_base.h>
#include <Eq_Conv_Diff.h>

class Pb_Conv_Diff : public Probleme_base
{
  Declare_instanciable(Pb_Conv_Diff);
public:
  int nombre_d_equations() const;
  const Equation_base& equation(int i) const ;
  Equation_base& equation(int i);        
  void associer_milieu_base(const Milieu_base& un_milieu);
protected:
  Eq_Conv_Diff eq_conv_diff;
};
#endif
