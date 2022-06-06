C-------------------------------------------------
C-------------------------------------------------
C
C     LECTURE DU FICHIER UNIVERSEL ISSU DE IDEAS
C     Premier passage pour obtenir les tailles des
C     differents tableaux.
C
C-------------------------------------------------
C-------------------------------------------------

        SUBROUTINE LIREPREM
     *                      (NAME,
     *                       NDIM,
     *                       NNOEUDS,
     *                       NELEM,
     *                       NTETRA,
     *                       NFACB,
     *                       NSEG,
     *                       NGROUPE,
     *                       CAS)
C
C --------------------------------------------------------------------
C | Nom     | E/S/I |  Type  | Role                                  |
C --------------------------------------------------------------------
C | NDIM    |   E   | Entier | Dimension du probleme                 |
C | NELEM   |   E   | Entier | Nombre d'elements du maillage         |
C | NTETRA  |   E   | Entier | Nombre de tetraedres du maillage      |
C | NFACB   |   E   | Entier | Nombre de faces de bord du maillage   |
C | NSEG    |   E   | Entier | Nombre de segments de bord du maillage|
C | NGROUPE |   E   | Entier | Nombre de groupes dans le .unv d'IDEAS|
C | NNOEUDS |   E   | Entier | Nombre de noeuds du maillage          |
C | NAME    |   E   | Carac. | Nom du fichier universel              |
C | Compteur|   I   | Entier | Compteur pour determiner les valeurs  |
C | n       |   I   | Entier | Entier pour reperage unv              |
C | nb      |   I   | Entier | Nombre d'entetes                      |
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
      Integer NGROUPE
      Integer NNOEUDS
      Integer NTETRA
      Integer NFACB
      Integer NSEG
      Integer n
      Integer nb
      Integer CAS
      Integer TEMP
      Integer ID,N1,N2,N3,Nbnoeud,Compteur
C
      Character*80 NAME
      Character*6  Ch
C
C---> Fin des declarations
C
C
C---> Ouverture du fichier universel
C
      OPEN (Unit=1, FILE=NAME, Status='old')
C
C--->
C---> Recherche de la table des noeuds (index 2411 dans Ideas)
C--->
C
      REWIND (1)
C
10    Read(1,1000,END=99) Ch
      If (Ch .NE. '    -1') GOTO 10
C
20    Read(1,*,END=99) n
C
      If(n.EQ.-1) Then
         Goto 20
      Elseif (n .NE. 2411) Then
         Goto 10
      Endif
C
C---> Lecture de la table des Noeuds
C
      Compteur = 0
      Read(1,1000) Ch
      Do While (Ch.NE.'    -1')
         Compteur = Compteur + 1
         Read(1,1000) Ch
      Enddo
C
      NNOEUDS = Compteur / 2
C
C--->
C---> Recherche de la table des elements (index 2412 dans Ideas)
C--->
C
      REWIND (1)
C
30    Read(1,1000,END=99) Ch
      If (Ch .NE. '    -1') GOTO 30
C
40    Read(1,*,END=99) n
C
      If(n.EQ.-1) Then
         Goto 40
      Elseif (n .NE. 2412) Then
         Goto 30
      Endif
C 
C---> Lecture des elements
C
      NTETRA = 0
      NFACB = 0
      NSEG = 0
C     
      Read(1,3200) TEMP, ID, N1, N2, N3, Nbnoeud
      Do While (TEMP.NE.-1)
         If (Nbnoeud.EQ.4) NTETRA = NTETRA + 1
         If (Nbnoeud.EQ.3) NFACB  = NFACB  + 1
         If (Nbnoeud.EQ.2) Then
            NSEG   = NSEG   + 1
            Read(1,3200) N1, N2, N3
         Endif
         Read(1,3200) TEMP, ID, N1, N2, N3, Nbnoeud
      Enddo
C
      NELEM = NTETRA + NFACB + NSEG
C
C--->
C---> Recherche de la table des groupes (index 2429 ou 2417 dans Ideas)
C--->
C
      REWIND (1)
C
C---> Premiere lecture pour voir le type des entetes (simples ou multiples)
C
      nb = 0
 50   Read(1,1000,END=55) Ch
      If (Ch .NE. '    -1') GOTO 50
C
 60   Read(1,*,END=55) n
C
      If(n.EQ.-1) Then
         Goto 60
      Elseif ((n .EQ. 2429).OR.(n .EQ. 2417).OR.(n .EQ. 2467)) Then
         nb = nb+1
         Goto 50
      Elseif ((n .NE. 2429).AND.(n .NE. 2417).AND.(n .NE. 2467)) Then
         Goto 50
      Endif
 55   Continue
C
C---> Lecture des groupes
C
      If (nb.EQ.1) Then
C
C---> tous les groupes sont ecrits en 1 seule fois (1 seule entete)
C
         Compteur = 0
C
         REWIND (1)
C
 70      Read(1,1000,END=99) Ch
         If (Ch .NE. '    -1') GOTO 70
C     
 80      Read(1,*,END=99) n
C     
         If(n.EQ.-1) Then
            Goto 80
         Elseif ((n .NE. 2417).AND.(n .NE. 2429).AND.(n .NE. 2467)) Then
            Goto 70
         Endif
C
C---> On a trouve le debut de la zone des groupes
C---> On les lit
C         
         Read(1,1000) Ch
         Do While (Ch.NE.'    -1')
            If (Ch.NE.'      ') Then
               Compteur = Compteur + 1
            Endif
            Read(1,1000) Ch
         Enddo
C
         If (Compteur .NE. 1) Then
            CAS = 1
         Else
            CAS = 2
         Endif
C
         NGROUPE = Compteur
      Else
C
C---> Les groupes ont tous des entetes propres
C
         CAS = 3
         NGROUPE = nb
      Endif
C
 99   Continue
C
      Close(1)
C
      Return
C
C---> FORMATS
C
 1000 FORMAT(A6)
 3200 FORMAT(6I10)

      END
