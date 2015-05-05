C/////////////////////////////////////////////////////////////////////////////
C/
C/ File:        Lec_Ideas.f
C/ Directory:   $TRUST_ROOT/Geometrie
C/
C/////////////////////////////////////////////////////////////////////////////

C-------------------------------------------------
C-------------------------------------------------
C
C     LECTURE DU FICHIER UNIVERSEL ISSU DE IDEAS
C
C-------------------------------------------------
C-------------------------------------------------

	SUBROUTINE LIREIDEAS 
     *                       (NAME, 
     *                        NDIM, 
     *                        X, 
     *                        Y, 
     *                        Z, 
     *                        TETRA, 
     *                        FACB, 
     *                        GROUP, 
     *                        NNOEUDS,
     *                        NELEM,
     *                        NTETRA, 
     *                        NFACB, 
     *                        NGROUPE,
     *                        NGELEM,
     *                        WORK,
     *                        CAS)
C
C --------------------------------------------------------------------
C | Nom     | E/S/I |  Type  | Role                                  |
C --------------------------------------------------------------------
C | NDIM    |   E   | Entier | Dimension du probleme                 |
C | NELEM   |   E   | Entier | Nombre d'elements du maillage         |
C | NGROUPE |   E   | Entier | Nombre de groupes dans le .unv d'IDEAS|
C | NNOEUDS |   E   | Entier | Nombre de noeuds du maillage          |
C | NGELEM  |   E   | Entier | Nombre d'element par groupe           |
C | NAME    |   E   | Carac. | Nom du fichier universel              |
C | GROUP   |  E/S  | Entier | Table des groupes (no du groupe, surf)|
C | X,Y,Z   |  E/S  | Reels  | Table des coordonnees                 |
C | GRNAME  |  I/S  | Carac. | Noms de groupe                        |
C | WORK    |  E/I  | Entier | Tableau de travail                    |
C | Ligne   |   I   | Entier | Numero de la ligne dans un groupe     |
C | Nogroupe|   I   | Entier | Numero du groupe                      |
C | Nb_ligne|   I   | Entier | Nombre de ligne par groupe dans .unv  |
C | Reste   |   I   | Entier | Nombre restant d'element d'un groupe  |
C | Num     |   I   | Entier | Variable bidon                        |
C | ID      |   I   | Entier | Variable bidon                        |
C | Nbnoeud |   I   | Entier | Nombre de noeuds par element          |
C | i,j,k   |   I   | Entier | Compteur de boucle                    |
C | n       |   I   | Entier | Entier pour reperage unv              |
C | n*      |   I   | Entier | Variable bidon                        |
C | CAS     |   I   | Entier | Type d'entetes pour les groupes       |
C | Ch      |   I   | Carac. | Chaine de caractere pour reperage unv |
C --------------------------------------------------------------------
C
	Implicit None
C
C---> Debut des declarations
C
	Integer NDIM
	Integer NELEM
	Integer NTETRA
	Integer NFACB
	Integer NGROUPE
	Integer NNOEUDS
	Integer NGELEM(1:*)
	Integer TETRA(1:NTETRA,1:4)
	Integer FACB(1:NFACB,1:3)
	Integer GROUP(1:NGROUPE,1:NFACB)
	Integer Ligne
	Integer WORK(*)
	Integer Nogroupe
	Integer Nb_ligne
	Integer Reste
	Integer ID
	Integer Nbnoeud
	Integer CAS
	Integer Temp
	Integer Surplus
        Integer Total (100)
	Integer i, j, k, n
        Integer N1,N2,N3,N4,N5,N6
	Real*8  X(*), Y(*), Z(*)
        integer itetra,ifacb
C
	Character*80 NAME
        Character*30 GRNAME
	Character*6  Ch
C
C---> Fin des declarations
C
C
C---> Ouverture du fichier universel et du fichier des noms de groupes
C
	OPEN (Unit=1, FILE=NAME, Status='old')
	OPEN (Unit=2, FILE='nom.groupe', Status='unknown')
C
C--->
C---> Recherche de la table des noeuds (index 2411 dans Ideas)
C--->
C
	REWIND (1)
C
 10	Read(1,1000,END=99) Ch
	If (Ch .NE. '    -1') GOTO 10 
C 
 20	Read(1,*,END=99) n 
C 
	If(n.EQ.-1) Then 
	   Goto 20 
	Elseif (n .NE. 2411) Then 
	   Goto 10 
	Endif 
C 
C---> Lecture de la table des Noeuds, Remplissage des tableaux 
C 
	If (NDIM.EQ.2) Then
	   Do i = 1, NNOEUDS 
	      Read(1,2200) N1, N2, N3, Temp 
 	      Read(1,2210) X(i), Y(i)
	   Enddo 
	Else
	   Do i = 1, NNOEUDS 
	      Read(1,2200) N1, N2, N3, Temp 
 	      Read(1,2210) X(i), Y(i), Z(i) 
	   Enddo 
	Endif
C 
C---> 
C---> Recherche de la table des elements (index 2412 dans Ideas) 
C---> 
C 
	REWIND (1) 
C        
 30	Read(1,1000,END=99) Ch 
	If (Ch .NE. '    -1') GOTO 30 
C        
 40	Read(1,*,END=99) n 
C        
	If(n.EQ.-1) Then 
	   Goto 40 
	Elseif (n .NE. 2412) Then 
	   Goto 30 
	Endif 
C 
C---> Remplissage du tableau des elements - TETRA pour les tetraedres 
C     et FACB pour les faces de bord 
C 
	Surplus = NTETRA + NFACB
	write(*,*) Surplus,NELEM
C       
        itetra=0
        ifacb=0
	Do i = 1, NELEM 
	   Read(1,3200) Temp, ID, N1, N2, N3, Nbnoeud
c	   print *,temp,Nbnoeud
	   If (Nbnoeud.EQ.4) Then 
              itetra=itetra+1
	     
	      Read(1,3210) (TETRA(itetra,j),j=1,Nbnoeud)
c	      print *,temp, (TETRA(itetra,j),j=1,Nbnoeud)
	   Elseif (Nbnoeud.EQ.3) Then 
              ifacb=ifacb+1
	      Read(1,3220) (FACB(ifacb,j),j=1,Nbnoeud) 
	   Elseif (Nbnoeud.LT.3) Then
               Read(1,3220) N1,N2,N3
	       Read(1,3230) N1,N2
	   Endif 
	Enddo 
C
	if (itetra.ne.ntetra) then
	   print *,' Pb a la lecture des tetras'
	   STOP
	   endif
	 if (ifacb.ne.nfacb) then
	   print *,' Pb a la lecture des tetras'
	   STOP
	   endif
	   
       
 12   Continue 
C
C---> 
C---> Recherche de la table des groupes (index 2429 ou 2417 dans Ideas) 
C---> 
C 
	REWIND (1) 
C        
 50	Read(1,1000,END=99) Ch 
	If (Ch .NE. '    -1') GOTO 50 
C        
 60	Read(1,*,END=99) n 
C        
	If(n.EQ.-1) Then 
	   Goto 60 
	Elseif ((n .NE. 2429).AND.(n .NE. 2417).AND.(n .NE. 2467)) Then 
	   Goto 50 
	Elseif ((n .EQ. 2429).OR.(n .EQ. 2417).OR.(n .EQ. 2467)) Then 
	   Goto 61 
	Endif 
 61	Continue 
C 
C---> Differents cas sont possibles: 
C--->     Entete simple pour plusieurs groupes - CAS = 1 
C--->     Entete simple pour 1 seul groupe     - CAS = 2 
C--->     Entete multiple                      - CAS = 3 

	write(6,*) 'mon cas est ,******************************'
	write(6,*) CAS
	
	If (CAS.EQ.1) Then 
C 
C---> Remplissage des groupes 
C 
C--------------------------------------------------------------- 
C---> ATTENTION, le format differe entre l'index 2417 et 2429 !! 
C--------------------------------------------------------------- 
C 
	   If ((n.EQ.2429).OR.(n.EQ.2430)) Then 
	      Do i= 1, NGROUPE 
C 
C---> Lecture de l'entete 
C 
		 Read(1,1050) Nogroupe,N1,N2,N3,N4,N5,N6,NGELEM(i) 
		 Read(1,1110) GRNAME
                 Write(2,1110) GRNAME
C 
C---> Calcul du nombre de lignes 
C 
		 Nb_ligne = NGELEM(i)*2/8
		 write(*,*) Nb_ligne
		 Reste = NGELEM(i)*2 - Nb_ligne*8 
C 
C---> Boucle sur le nombre de ligne et lecture dans un tableau de travail 
C 
		 Do j = 1, Nb_ligne 
		    Ligne = j 
		    Read(1,1050) (WORK(8*(Ligne-1)+k),k=1,8) 
		    write(6,1050) (WORK(8*(Ligne-1)+k),k=1,8)
		 Enddo 
C 
C---> Les derniers elements sont lus et stockes 
C 
		 If (Reste.NE.0) Then 
		    Read(1,1050) (WORK(Nb_ligne*8+k),k=1,Reste) 
		 Endif  
C 
C---> On stocke dans le tableau GROUP 1 terme sur 2 
C 
                 Total(i) = 0
		 Do j = 1, NGELEM(i) 
		    If ((WORK(2*j).LE.Surplus)
     1             .AND.(WORK(2*j).GT.NTETRA)) Then 
                       Total(i) = Total(i) + 1
		       GROUP(i,Total(i)) = WORK(2*j)-NTETRA
C                       write(6,*) GROUP(i,Total(i))
		    Endif 
		 Enddo 
	      Enddo
C
C---> Mise a jour des nombres d'elements par groupe (on enleve les noeuds et les segments)
C
              Do i = 1, NGROUPE
                 NGELEM(i) = Total(i)
              Enddo
              
C              Do i = 1, NGROUPE
C              	Do j= 1 , NGELEM(i) 
C              		write(6,*) GROUP(i,j)
C              	Enddo
C              Enddo		
            elseif (n.eq.2467) then
	    Do i= 1, NGROUPE 
C 
C---> Lecture de l'entete 
C 
		 Read(1,1050) Nogroupe,N1,N2,N3,N4,N5,N6,NGELEM(i) 
		 Read(1,1110) GRNAME
                 Write(2,1110) GRNAME
		 print *,GRNAME
C 
C---> Calcul du nombre de lignes 
C 
		 Nb_ligne = NGELEM(i)/2
		 Reste = NGELEM(i) - Nb_ligne*2 
C 
C---> Boucle sur le nombre de ligne et lecture dans un tableau de travail 
C 
		 Do j = 1, Nb_ligne 
		    Ligne = j 
		    Read(1,1050) (WORK(8*(Ligne-1)+k),k=1,8) 
		 Enddo 
C 
C---> Les derniers elements sont lus et stockes 
C 
		 
		 If (Reste.NE.0) Then 
		    Read(1,1050) (WORK(Nb_ligne*8+k),k=1,Reste*4) 
		 Endif  
C 
C---> On stocke dans le tableau GROUP 1 terme sur 2 
                 surplus=NELEM-NTETRA-NFACB
                 Total(i) = 0
		 Do j = 1, NGELEM(i)
		    If ((WORK(4*(j-1)+2).GE.Surplus)) then
c     1             .AND.(WORK(2*j).GT.NTETRA)) Then 
c		    if (1.eq.1) then
                       Total(i) = Total(i) + 1
		       GROUP(i,Total(i)) = WORK(4*(j-1)+2)-surplus
		       Endif 
		 Enddo 
	      Enddo
C
C---> Mise a jour des nombres d'elements par groupe (on enleve les noeuds et les segments)
C
              Do i = 1, NGROUPE
                 NGELEM(i) = Total(i)
              Enddo
              
C              Do i = 1, NGROUPE
C              	Do j= 1 , NGELEM(i) 
C              		write(6,*) GROUP(i,j)
C              	Enddo
C              Enddo		
              
	   Else 
	      Do i= 1, NGROUPE 
C 
C---> Lecture de l'entete 
C 
		 Read(1,1060) Nogroupe, N1, N2, N3, N4, N5, NGELEM(i) 
		 Read(1,1110) GRNAME 
                 Write(2,1110) GRNAME
C 
C---> Calcul du nombre de lignes 
C 
		 Nb_ligne = NGELEM(i)*2./8 
		 Reste = NGELEM(i)*2 - Nb_ligne*8 
C 
C---> Boucle sur le nombre de ligne et lecture dans un tableau de travail 
C 
		 Do j = 1, Nb_ligne 
		    Ligne = j 
		    Read(1,1050) (WORK(8*(Ligne-1)+k),k=1,8) 
		 Enddo 
C 
C---> Les derniers elements sont lus et stockes 
C 
		 If (Reste.NE.0) Then 
		    Read(1,1050) (WORK(Nb_ligne*8+k),k=1,Reste) 
		 Endif  
C 
C---> On stocke dans le tableau GROUP 1 terme sur 2 
C 
		 Total(i) = 0
		 Do j = 1, NGELEM(i) 
		    If ((WORK(2*j).LE.Surplus)
     1             .AND.(WORK(2*j).GT.NTETRA)) Then 
		       Total(i) = Total(i) + 1
		       GROUP(i,Total(i)) = WORK(2*j) -NTETRA
		    Endif 
		 Enddo 
	      Enddo 
C
C---> Mise a jour des nombres d'elements par groupe (on enleve les noeuds et les segments)
C
              Do i = 1, NGROUPE
                 NGELEM(i) = Total(i)
              Enddo
	   Endif 
C 
C---> Fin du cas no1 (i.e. 1 entete pour plusieurs groupes) 
C 
C 
C---> Debut du cas no2 (1 entete pour 1 seul groupe) 
C 
	Elseif (CAS.EQ.2) Then 
C 
C--------------------------------------------------------------- 
C---> ATTENTION, le format differe entre l'index 2417 et 2429 !! 
C--------------------------------------------------------------- 
C 
	   If ((n.EQ.2429).OR.(n.EQ.2429)) Then 
C 
C---> Lecture de l'entete 
C 
	      Read(1,1050) Nogroupe, N1, N2, N3, N4, N5, N6, NGELEM(1) 
	      Read(1,1110) GRNAME
              Write(2,1110) GRNAME
C 
C---> Calcul du nombre de lignes 
C 
	      Nb_ligne = NGELEM(1)*2./8 
	      Reste = NGELEM(1)*2 - Nb_ligne*8 
C
C---> Boucle sur le nombre de ligne et lecture dans un tableau de travail 
C        
	      Do j = 1, Nb_ligne 
		 Ligne = j 
		 Read(1,1050) (WORK(8*(Ligne-1)+k),k=1,8) 
	      Enddo 
C 
C---> Les derniers elements sont lus et stockes 
C 
	      If (Reste.NE.0) Then 
		 Read(1,1050) (WORK(Nb_ligne*8+k),k=1,Reste) 
	      Endif  
C 
C---> On stocke dans le tableau GROUP 1 terme sur 2 
C 
	      Total(1) = 0
	      Do j = 1, NGELEM(1) 
		 If ((WORK(2*j).LE.Surplus).AND.
     1               (WORK(2*j).GT.NTETRA)) Then 
		    Total(1) = Total(1) + 1
		    GROUP(1,Total(1)) = WORK(2*j) -NTETRA
		 Endif 
	      Enddo 
C
C---> Mise a jour des nombres d'elements par groupe (on enleve les noeuds et les segments)
C
              NGELEM(1) = Total(1)
C
 	   Else 
C 
C---> Lecture de l'entete 
C 
 	      Read(1,1060) Nogroupe, N1, N2, N3, N4, N5, NGELEM(1) 
 	      Read(1,1110) GRNAME
              Write(2,1110) GRNAME
C 
C---> Calcul du nombre de lignes 
C 
 	      Nb_ligne = NGELEM(1)*2./8 
 	      Reste = NGELEM(1)*2 - Nb_ligne*8 
C 
C---> Boucle sur le nombre de ligne et lecture dans un tableau de travail 
C 
 	      Do j = 1, Nb_ligne 
 		 Ligne = j 
 		 Read(1,1050) (WORK(8*(Ligne-1)+k),k=1,8) 
 	      Enddo 
C 
C---> Les derniers elements sont lus et stockes 
C 
 	      If (Reste.NE.0) Then 
 		 Read(1,1050) (WORK(Nb_ligne*8+k),k=1,Reste) 
 	      Endif  
C 
C---> On stocke dans le tableau GROUP 1 terme sur 2 
C
 	      Total(1) = 0
 	      Do j = 1, NGELEM(1) 
		 If ((WORK(2*j).LE.Surplus)
     1       .AND.(WORK(2*j).GT.NTETRA)) Then 
 		    Total(1) = Total(1) + 1
 		    GROUP(1,Total(1)) = WORK(2*j) -NTETRA
 		 Endif 
 	      Enddo 
C
C---> Mise a jour des nombres d'elements par groupe (on enleve les noeuds et les segments)
C
 	      NGELEM(1) = Total(1)
 	   Endif 
C 
C---> Fin du cas no2 
C 
 	Elseif (CAS .EQ. 3) Then 
C 
C---> Debut du cas no3 
C 
C--------------------------------------------------------------- 
C---> ATTENTION, le format differe entre l'index 2417 et 2429 !! 
C--------------------------------------------------------------- 
C 
 	   If ((n.EQ.2429).OR.(n.EQ.2430)) Then 
 	      Do i= 1, NGROUPE 
C 
C---> Lecture de l'entete 
C 
 		 Read(1,1050) Nogroupe,N1,N2,N3,N4,N5,N6,NGELEM(i) 
 		 Read(1,1110) GRNAME
                 Write(2,1110) GRNAME
C 
C---> Calcul du nombre de lignes 
C 
 		 Nb_ligne = NGELEM(i)*2./8 
 		 Reste = NGELEM(i)*2 - Nb_ligne*8 
C 
C---> Boucle sur le nombre de ligne et lecture dans un tableau de travail 
C 
 		 Do j = 1, Nb_ligne 
 		    Ligne = j 
 		    Read(1,1050) (WORK(8*(Ligne-1)+k),k=1,8) 
 		 Enddo 
C 
C---> Les derniers elements sont lus et stockes 
C 
 		 If (Reste.NE.0) Then 
 		    Read(1,1050) (WORK(Nb_ligne*8+k),k=1,Reste) 
 		 Endif  
C 
C---> On stocke dans le tableau GROUP 1 terme sur 2 
C 
 		 Total(i) = 0
 		 Do j = 1, NGELEM(i) 
		    If ((WORK(2*j).LE.Surplus)
     1             .AND.(WORK(2*j).GT.NTETRA)) Then 
 		       Total(i) = Total(i) + 1
 		       GROUP(i,Total(i)) = WORK(2*j) -NTETRA
 		    Endif 
 		 Enddo 
C 
C---> On lit les trois prochaines lignes (entetes) 
C 
 		 If (i.NE.(NGROUPE)) Then 
 		    Read(1,1000,END=99) Ch 
 		    Read(1,1000,END=99) Ch 
 		    Read(1,*,END=99) N1 
 		 Endif 
 	      Enddo 
C
C---> Mise a jour des nombres d'elements par groupe (on enleve les noeuds et les segments)
C
              Do i = 1, NGROUPE
                 NGELEM(i) = Total(i)
              Enddo
 	   Else 
 	      Do i= 1, NGROUPE 
C 
C---> Lecture de l'entete 
C 
 		 Read(1,1060) Nogroupe, N1, N2, N3, N4, N5, NGELEM(i) 
 		 Read(1,1110) GRNAME
                 Write(2,1110) GRNAME
C 
C---> Calcul du nombre de lignes 
C 
 		 Nb_ligne = NGELEM(i)*2./8 
 		 Reste = NGELEM(i)*2 - Nb_ligne*8 
C 
C---> Boucle sur le nombre de ligne et lecture dans un tableau de travail 
C 
 		 Do j = 1, Nb_ligne 
 		    Ligne = j 
 		    Read(1,1050) (WORK(8*(Ligne-1)+k),k=1,8) 
 		 Enddo 
C 
C---> Les derniers elements sont lus et stockes 
C 
 		 If (Reste.NE.0) Then 
 		    Read(1,1050) (WORK(Nb_ligne*8+k),k=1,Reste) 
 		 Endif  
C 
C---> On stocke dans le tableau GROUP 1 terme sur 2 
C 
 		 Total(i) = 0
 		 Do j = 1, NGELEM(i) 
		    If ((WORK(2*j).LE.Surplus)
     1             .AND.(WORK(2*j).GT.NTETRA)) Then 
 		       Total(i) = Total(i) + 1
 		       GROUP(i,Total(i)) = WORK(2*j) -NTETRA
 		    Endif 
 		 Enddo 
C 
C---> On lit les trois prochaines lignes (entetes) 
C 
 		 If (i.NE.(NGROUPE)) Then 
 		    Read(1,1000,END=99) Ch 
 		    Read(1,1000,END=99) Ch 
 		    Read(1,*,END=99) N1 
 		 Endif 
 	      Enddo 
C
C---> Mise a jour des nombres d'elements par groupe (on enleve les noeuds et les segments)
C
              Do i = 1, NGROUPE
                 NGELEM(i) = Total(i)
              Enddo
 	   Endif 
 	Endif 
C 
C---> Fin du cas no3 
C	  
 99	Continue 
C        
	Close(1) 
	Close(2) 
C 
	Return 
C 
C---> FORMATS 
C 
 1000	FORMAT(A6) 
 1050	FORMAT(8I10) 
 1060	FORMAT(7I10) 
 1110	FORMAT(A20) 
 2200	FORMAT(4I10) 
 2210	FORMAT(1P3D25.16) 
 3200	FORMAT(6I10) 
 3210	FORMAT(8I10) 
 3220	FORMAT(3I10)
 3230	FORMAT(2I10)
C        
	END 
