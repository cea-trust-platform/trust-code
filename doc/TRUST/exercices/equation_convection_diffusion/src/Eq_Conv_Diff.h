
#ifndef Eq_Conv_Diff_included
#define Eq_Conv_Diff_included

#include <Convection_Diffusion_Concentration.h>
#include <Ref_Constituant_Avec_Vitesse.h>

class Eq_Conv_Diff : public Convection_Diffusion_Concentration
{
  Declare_instanciable(Eq_Conv_Diff);
public :
  void associer_milieu_base(const Milieu_base& un_milieu) override;
  void associer_constituant(const Constituant_Avec_Vitesse& un_constituant);
  int lire_motcle_non_standard(const Motcle& mot, Entree& is) override;
protected :
  REF(Constituant_Avec_Vitesse) le_constituant;
};
#endif
