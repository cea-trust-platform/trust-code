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
#ifndef MED_PROTO_H
#define MED_PROTO_H

/* Interface de l'API MED */

/* Fichier */
extern med_idt
MEDouvrir(char *nom, med_mode_acces mode_acces);
extern med_err
MEDfermer(med_idt fid);
extern med_int
MEDlFichDes(med_idt fid);
extern med_err
MEDfichEntete(med_idt fid, med_fich_info quoi, char str[]);
extern med_err
MEDfichDesEcr(med_idt fid, char *des, med_mode_acces mode);
extern med_err
MEDunvCr(med_idt fid, char *maa);
extern med_err
MEDunvLire(med_idt fid, char *maa,char *nomu);
extern med_err
MEDformatConforme(const char * nomfich);

/* Maillage */
extern med_err
MEDmaaCr(med_idt fid, char *maillage, med_int dim);
extern med_int
MEDdimLire(med_idt fid, char *maillage);
extern med_err
MEDmaaInfo(med_idt fid, int indice, char *maillage, med_int *dim);
extern med_int
MEDnMaa(med_idt fid);
extern med_err
MEDnbnosoEcr(med_idt fid, char *nom_maillage,med_int n);
extern med_int
MEDnbnosoLire(med_idt fid,char *nom_maillage);
extern med_err
MEDnbnoisEcr(med_idt fid, char *nom_maillage,med_int n);
extern med_int
MEDnbnoisLire(med_idt fid,char *nom_maillage);
extern med_err
MEDnbnomaEcr(med_idt fid, char *nom_maillage,med_int n);
extern med_int
MEDnbnomaLire(med_idt fid,char *nom_maillage);

/* EntMaillage */
extern med_err
MEDconnEcr(med_idt fid,char *maa, med_int mdim, med_int *connectivite,med_mode_switch mode_switch,
           med_int nbre,med_mode_acces mode,med_entite_maillage type_ent,
           med_geometrie_element type_geo,med_connectivite type_conn);

extern med_err
MEDconnLire(med_idt fid,char *maa,med_int mdim,med_int *connectivite,med_mode_switch mode_switch,
            med_int * pfltab, med_size psize,
            med_entite_maillage type_ent, med_geometrie_element type_geo,med_connectivite type_conn);
extern med_err
MEDnomEcr(med_idt fid,char *maa, char *nom, med_int n, med_mode_acces mode,
          med_entite_maillage type_ent,med_geometrie_element type_geo);
extern med_err
MEDnomLire(med_idt fid,char *maa, char *nom, med_int n,
           med_entite_maillage type_ent,med_geometrie_element type_geo);
extern med_err
MEDnumLire(med_idt fid,char *maa, med_int *num, med_int n,
           med_entite_maillage type_ent,med_geometrie_element type_geo);
extern med_err
MEDnumEcr(med_idt fid,char *maa, med_int *num, med_int n, med_mode_acces mode,
          med_entite_maillage type_ent,med_geometrie_element type_geo);
extern med_err
MEDcoordEcr(med_idt fid, char *maa, med_int mdim, med_float *coo,
            med_mode_switch mode_coo,med_int n,
            med_mode_acces mode, med_repere type_rep, char *nom, char *unit);
extern med_err
MEDcoordLire(med_idt fid, char *maa, med_int mdim, med_float *coo,
             med_mode_switch mode_coo,med_int numco,
             med_int * pfltab, med_size psize, med_repere *type_rep, char *nom, char *unit);

extern med_int
MEDnEntMaa(med_idt fid, char *maa, med_table quoi, med_entite_maillage type_ent,
           med_geometrie_element type_geo, med_connectivite type_conn);
extern med_err
MEDpolygoneConnEcr(med_idt fid, char *maa, med_int *index, med_int ni, med_int *con,
                   med_entite_maillage type_ent, med_connectivite type_conn);
extern med_err
MEDpolygoneInfo(med_idt fid, char *maa, med_entite_maillage type_ent,med_connectivite type_conn,
                med_int *consize);
extern med_err
MEDpolygoneConnLire(med_idt fid, char *maa, med_int *index, med_int ni, med_int *con,
                    med_entite_maillage type_ent, med_connectivite type_conn);
extern med_err
MEDpolyedreConnEcr(med_idt fid,char *maa,med_int *indexp,med_int np,med_int *indexf,med_int nf,
                   med_int *con,med_connectivite type_conn);
extern med_err
MEDpolyedreInfo(med_idt fid, char *maa,med_connectivite type_conn,med_int *nf,
                med_int *consize);
extern med_err
MEDpolyedreConnLire(med_idt fid,char *maa,med_int *indexp,med_int np,med_int *indexf,med_int nf,
                    med_int *con,med_connectivite type_conn);

/* Resultat */
extern med_err MEDchampCr(med_idt fid, char *champ, med_type_champ type, char *comp,
                          char *unit,med_int ncomp);

extern med_err
MEDchampEcr(med_idt fid, char *maa, char *cha,unsigned char *val,med_mode_switch interlace,med_int nbelem,med_int ngauss,
            med_int numco, char * profil, med_mode_acces mode, med_entite_maillage type_ent,
            med_geometrie_element type_geo, med_int numdt,char * dt_unit, med_float dt, med_int numo);

extern med_err
MEDchampLire(med_idt fid,char *maa, char *cha, unsigned char *val,med_mode_switch interlace,med_int numco,
             char *profil,med_entite_maillage type_ent, med_geometrie_element type_geo,
             med_int numdt, med_int numo);

extern med_err
MEDchampInfo(med_idt fid,int indice,char *champ,
             med_type_champ *type,char *comp,char *unit,
             med_int ncomp);

extern med_int
MEDnChamp(med_idt fid, int indice);

extern med_int
MEDnVal(med_idt fid, char *champ, med_entite_maillage typ_ent,
        med_geometrie_element typ_geo,med_int numdt, med_int numo);


/* Famille  */
extern med_err
MEDfamEcr(med_idt fid,char *maa, med_int *fam, med_int n,
          med_entite_maillage type_ent, med_geometrie_element type_geo);
extern med_err
MEDfamLire(med_idt fid,char *maa, med_int *fam, med_int n,
           med_entite_maillage type_ent,med_geometrie_element type_geo);
extern med_err
MEDfamCr(med_idt fid,char* maa,char *famille,med_int numero,
         med_int *attr_ident, med_int *attr_val,char *attr_desc,med_int n_attr,
         char *groupe , med_int n_groupe);
extern med_int
MEDnFam(med_idt fid,char *maa, int indice, med_dim_famille quoi);

extern med_err
MEDfamInfo(med_idt fid,char *maa,int indice, char *famille,
           med_int *numero,
           med_int *attr_ident, med_int *attr_val, char *attr_desc,
           med_int *n_attr,char *groupe ,med_int *n_groupe);

/* Equivalence    */
extern med_err
MEDequivCr(med_idt fid,char *maa, char *eq, char *desc);
extern med_err
MEDequivLire(med_idt fid, char *maa, char *eq, med_int *corr, med_int n,
             med_entite_maillage typ_ent,med_geometrie_element typ_geo);
extern med_err
MEDequivEcr(med_idt fid, char *maa, char *eq, med_int *corr, med_int n,
            med_mode_acces mode, med_entite_maillage typ_ent, med_geometrie_element typ_geo);
extern med_err
MEDequivInfo(med_idt fid, char *maa, int ind, char *eq, char *des);
extern med_int
MEDnEquiv(med_idt fid, char *maa);
extern med_int
MEDnCorres(med_idt fid,char *maa,char *eq,med_entite_maillage typ_ent,
           med_geometrie_element typ_geo);


/* Routines de niveau intermediaire */
extern med_int
MEDnEntites(med_idt fid,char *maa,med_entite_maillage typ_ent,
            med_connectivite typ_con);

extern med_err
MEDnoeudsLire(med_idt fid,char *maa,med_int mdim, med_float *coord,
              med_mode_switch mode_coo,
              med_repere *repere,char *nomcoo, char *unicoo,char *nom,
              med_booleen *inom,med_int *num,med_booleen *inum,med_int *fam,
              med_int nnoeuds);

extern med_err
MEDnoeudsEcr(med_idt fid,char *maa,med_int mdim,med_float *coord,
             med_mode_switch mode_coo,
             med_repere repere,char *nomcoo, char *unicoo,char *nom,
             med_booleen inom,med_int *num,med_booleen inum,med_int *fam,
             med_int nnoeuds,med_mode_acces mode);
extern med_err
MEDelementsEcr(med_idt fid,char *maa,med_int mdim,med_int *connectivite,med_mode_switch mode_switch,
               char *nom,med_booleen inom,med_int *num,med_booleen inum,
               med_int *fam,med_int nele,med_entite_maillage typ_ent,
               med_geometrie_element typ_geo,med_connectivite typ_conn,med_mode_acces mode);
extern med_err
MEDelementsLire(med_idt fid,char *maa,med_int mdim,med_int *connectivite,med_mode_switch mode_switch,
                char *nom,med_booleen *inom,med_int *num,med_booleen *inum,
                med_int *fam,med_int nele,med_entite_maillage typ_ent,
                med_geometrie_element typ_geo,med_connectivite typ_conn);

/* Routines de haut niveau    */

/*(? On enleve le reste ?)*/

extern med_err
MEDfamMaaInfo(med_idt fid,char *maa,med_int *nfam,med_int *nattc,
              med_int *ngroc);

extern med_err
MEDfamMaaLire(med_idt fid,char *maa,
              med_int *numfam,med_int *attide,
              med_int *attval,char *attdes,int *indatt,char *gro,int *indgro,
              med_int nfamilles);

extern med_err
MEDfamMaaCr(med_idt fid,char *maa,
            med_int *numfam,med_int *attide,
            med_int *attval,char *attdes,int *indatt,char *gro,int *indgro,
            med_int nfamilles);

/* Routines concernant les profils */

extern med_err
MEDprofilEcr(med_idt fid,med_int *pflval,med_int n,char *nom);

extern med_int
MEDnValProfil(med_idt fid, char *nom);

extern med_err
MEDprofilLire(med_idt fid, med_int *pflval, char *nom);

/* Routines concernant les pas de temps/ numeros d'ordre */

extern med_int
MEDnPasdetemps(med_idt fid,char *cha,med_entite_maillage type_ent,
               med_geometrie_element type_geo);

extern med_err
MEDpasdetempsInfo(med_idt fid,char *champ
                  ,med_entite_maillage type_ent, med_geometrie_element type_geo,
                  int indice, char *maa, med_int * ngauss, med_int * numdt, char * dt_unit, med_float * dt,
                  med_int * numo);

#endif /* MED_PROTO_H */




