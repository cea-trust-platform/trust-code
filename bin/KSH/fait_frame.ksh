#!/bin/bash
# creation du frame
. connect_ftp -no_ftp
v=$TRUST_VERSION
vv=$v
# Verifie si nouvelle version
new_version=""
file=$HOME/HTML/new_version.html
if [ ! -f $file ] || [ ! -f $TRUST_TMP/.new$vv ] || [ $TRUST_ROOT/RELEASE_NOTES -nt $TRUST_TMP/.new$vv ]
then
# onClick=\"window.location.href='file:$HOME/HTML/Etude.html'\"
  # Avertissement licence
  echo "<HTML><CENTER>
<form name=com method=get action=\"\">
<textarea name="text" wrap="virtual" cols=60 rows=20>`cat $TRUST_ROOT/license`</textarea>
</form>
<form name=com method=get action=\"http://localhost:$PORT/Register.cg\">
<INPUT TYPE="submit"  NAME="Prev" VALUE=\"Accept\">
<INPUT TYPE="button" onClick=\"window.close()\" NAME="Next" VALUE=\"Do not accept\">
</CENTER></form></HTML>" > $TRUST_TMP/license.html
  rm -f $HOME/HTML/license.html
  touch $TRUST_TMP/.new$vv
  new_version="<script language="JavaScript">window.open('file:$file','win1','location=no,resizable=yes,scrollbars=yes,width=600,height=500')</script>"
  echo $ECHO_OPTS '<HTML>
<HEAD>
<META NAME="GENERATOR" CONTENT="Adobe PageMill 3.0 Win">
</HEAD>
<BODY>
<FORM>
<H1><CENTER>Release notes</CENTER></H1>' > $file
cat $TRUST_ROOT/RELEASE_NOTES | $TRUST_Awk '{print "<LI>"$0}' >> $file
echo $ECHO_OPTS "</FONT><BR><P><CENTER><INPUT type="button" value=\"See new keywords\" onClick=window.open('file:"$TRUST_ROOT"/IHM/Aide_Mots_Cles.html','win2','location=no,resizable=yes,scrollbars=yes')>
<INPUT type="button" value=\"Close Window\" onClick=window.close()></CENTER></FORM>
</BODY>
</HTML>" >> $file
   fi
prov="TRUST "$v
   echo $ECHO_OPTS "<HTML>
<HEAD>
<TITLE> $prov </TITLE>
</HEAD>
$new_version
<FRAMESET COLS="13%,*">
<FRAME SRC="Raccourcis.html" NAME="LEFT">
<FRAMESET ROWS="*,1%">
<FRAME SRC="TRUST.html" NAME="RIGHT">
<FRAME SRC="about:blank" NAME="DOWN">
</FRAMESET>
</FRAMESET>
</HTML>" > $HOME/HTML/HOME.html
echo > $HOME/HTML/Blanc.html

   echo $ECHO_OPTS "<HTML>
<HEAD>
<TITLE> TRUST $v </TITLE>
</HEAD>
<FRAMESET COLS="13%,*">
<FRAME SRC="Raccourcis.html" NAME="LEFT">
<FRAMESET ROWS="*,1%">
<FRAME SRC="$HOME/.Etude.html" NAME="RIGHT">
<FRAME SRC="Blanc.html" NAME="DOWN">
</FRAMESET>
</FRAMESET>
</HTML>" > $HOME/HTML/Etude.html
HELP="Helps&Tips&Links"
CLASSES="C++ classes"
echo $ECHO_OPTS "<HTML>
<BODY BGCOLOR=#D0D5D9 TEXT=#23238E LINK=#E47833 ALINK=#007FFF VLINK=#007FFF>
<head>
<title> Trio Unitaire </title>
</head>
<body>
<PRE>
<HR><h1> TRUST Version $v</h1><HR>
<img src=$TRUST_ROOT/IHM/animation.gif></PRE>
<h2>    HyperText Documentation  </h2>
<UL>
<LI> <a href=$TRUST_ROOT/IHM/Util_calcul.html> <img src=$TRUST_ROOT/IHM/calcul.gif> Studies</a>" > $HOME/HTML/TRUST.html
# Cas version sans sources
[ -f $TRUST_ROOT/doc/html/index.html ] && echo "<LI> <a href=$TRUST_ROOT/doc/html/index.html><img src=$TRUST_ROOT/IHM/liste.gif> $CLASSES</a>" >> $HOME/HTML/TRUST.html
echo "<LI> <a href=$TRUST_ROOT/IHM/Documents.html><img src=$TRUST_ROOT/IHM/Notice.gif> $HELP</a>
<LI> <a HREF=mailto:$TRUST_MAIL> <img src=$TRUST_ROOT/IHM/mail.gif> Support </a>
</UL>
<BR>
<HR>
</body>
</html>" >> $HOME/HTML/TRUST.html

   echo $ECHO_OPTS "<TITLE>Raccourcis</TITLE>
<H1>TrioU </H1>
<DL><p>
    <HR>
    <DT><A HREF="file:$HOME/HTML/TRUST.html" TARGET="RIGHT" onMouseOver=\"status='Main page of TRUST'\">Main</A>
    <HR>
    <DT><A HREF="file:$HOME/HTML/new_version.html" TARGET="RIGHT" onMouseOver=\"status='What is new in TRUST'\">What's new</A>
    <HR>
    <DT><A HREF="ftp://$TRUST_FTP/$PUBLIC/index.html" TARGET="RIGHT" onMouseOver=\"status='Ftp TRUST'\">Downloads</A>
    <HR>" > $HOME/HTML/Raccourcis.html
if [ $TRUST_ATELIER != -1 ]
then
   echo $ECHO_OPTS " <DT><A HREF="file:$rep_dev/.Fichiers_atelier.html" TARGET="RIGHT" onMouseOver=\"status='Workbench to compile your own TRUST binary'\">MyTrioU </A>
<HR>" >> $HOME/HTML/Raccourcis.html
fi
echo $ECHO_OPTS "<SCRIPT LANGUAGE=\"JavaScript\">
function v(url)
{
var aff=window.open (url,'methods','location=no,toolbar=no,directories=no,menubar=no,resizable=yes,scrollbars=yes,status=yes,width=800,height=800');
aff.focus();
}
</SCRIPT>
 <DT><A HREF="file:$HOME/.Etude.html" TARGET="RIGHT" onMouseOver=\"status='Access to your last study with TRUST'\">MyStudy</A>
    <HR>
 <DT><A HREF="file:$TRUST_ROOT/form/Term_root.ksh" TARGET="RIGHT" onMouseOver=\"status='Open a new terminal'\">Xterm</A>
    <HR>" >> $HOME/HTML/Raccourcis.html
# Cas version sans sources
[ -f $TRUST_ROOT/doc/html/index.html ] && echo "
    <DT><A HREF="file:$TRUST_ROOT/doc/html/index.html" TARGET="RIGHT" onMouseOver=\"status='Browse html doc'\">$CLASSES</A>
    <HR>" >> $HOME/HTML/Raccourcis.html 
echo "<DT><A HREF="file:$TRUST_ROOT/doc/TRUST/User_Manual_TRUST.pdf" TARGET="RIGHT" onMouseOver=\"status='TRUST Keywords'\">User's manual</A>
    <HR>
    <DT><A HREF="file:$TRUST_ROOT/doc/TRUST/Models_Equations_TRUST.pdf" TARGET="RIGHT" onMouseOver=\"status='TRUST Equations'\">Description note</A>
    <HR> 
    <DT><A HREF="file:$TRUST_ROOT/doc/TRUST/NT_Valid_2009_lin.pdf" TARGET="RIGHT" onMouseOver=\"status='TRUST Validation'\">Validation note</A>
    <HR> 
    <DT><A HREF="file:$TRUST_ROOT/IHM/Documents.html" TARGET="RIGHT" onMouseOver=\"status='$HELP'\">$HELP</A>
    <HR>       
</DL><p>" >> $HOME/HTML/Raccourcis.html
