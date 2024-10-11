#ifndef Constituant_Avec_Vitesse_included
#define Constituant_Avec_Vitesse_included

#include <Champ_Inc_base.h>
#include <Constituant.h>

class Constituant_Avec_Vitesse : public Constituant
{
  Declare_instanciable(Constituant_Avec_Vitesse);
public :
  int   initialiser(const double temps) override;
  void  set_param(Param& param) override;
  void  mettre_a_jour(double temps) override;
  const Champ_Don_base& vit_convection_constituant() const;
  bool has_vit_convection_constituant() const { return C_.non_nul(); }
  const Champ_base& vitesse_pour_transport() ;
  void discretiser(const Probleme_base& pb, const Discretisation_base& dis) override;
protected :
  OWN_PTR(Champ_Don_base) C_;
private :
  OWN_PTR(Champ_Inc_base) vitesse_transport;
};
#endif
