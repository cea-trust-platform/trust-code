//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Leap_frog.h>
#include <Pred_Cor.h>
#include <Schema_Adams_Bashforth_order_2.h>
#include <Schema_Adams_Bashforth_order_3.h>
#include <Schema_Adams_Moulton_order_2.h>
#include <Schema_Adams_Moulton_order_3.h>
#include <Schema_Euler_Implicite.h>
#include <Schema_Euler_explicite.h>
#include <Schema_RK_Classique.h>
#include <Schema_RK_Rationnel.h>
#include <Schema_RK_Williamson.h>
void instancie_src_Kernel_Schemas_Temps() {
Cerr << "src_Kernel_Schemas_Temps" << finl;
Leap_frog inst1;verifie_pere(inst1);
Pred_Cor inst2;verifie_pere(inst2);
Schema_Adams_Bashforth_order_2 inst3;verifie_pere(inst3);
Schema_Adams_Bashforth_order_3 inst4;verifie_pere(inst4);
Schema_Adams_Moulton_order_2 inst5;verifie_pere(inst5);
Schema_Adams_Moulton_order_3 inst6;verifie_pere(inst6);
Schema_Euler_Implicite inst7;verifie_pere(inst7);
Schema_Euler_explicite inst8;verifie_pere(inst8);
RK2_Classique inst9;verifie_pere(inst9);
RK3_Classique inst10;verifie_pere(inst10);
RK4_Classique inst11;verifie_pere(inst11);
RK4_Classique_3_8 inst12;verifie_pere(inst12);
RRK2 inst13;verifie_pere(inst13);
RK2 inst14;verifie_pere(inst14);
RK3 inst15;verifie_pere(inst15);
RK4 inst16;verifie_pere(inst16);
}
