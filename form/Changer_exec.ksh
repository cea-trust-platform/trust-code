#!/bin/bash
#$Dtterm -title "Substitute" -e Changer_exec.ihm
file=$TRUST_TMP/change.html
rm -f $file

if [ -f $TRUST_TMP/.EXEC_TRUST ]
then
   # export EXEC_TRUST=`cat $TRUST_TMP/.EXEC_TRUST` sur une Sun fait quitter le script 
   # lorsque appele depuis netscape !! N'importe quoi...
   # le export semble inutile quand meme
   EXEC_TRUST=`cat $TRUST_TMP/.EXEC_TRUST`
else
   # export EXEC_TRUST=$exec
   # idem
   EXEC_TRUST=$exec
fi
# Liste les binaires
if [ "$DIPHA_ROOT" != "" ]
then
   binaries=`ls $DIPHA_ROOT/exec/dipha* $rep_dev/exec*/dipha* $TRUST_ROOT/exec/TRUST* $rep_dev/exec*/TRUST* 2>/dev/null | grep -v "\.a"`
else
   binaries=`ls $TRUST_ROOT/exec/TRUST* $rep_dev/exec*/TRUST* 2>/dev/null | grep -v "\.a"`
fi
echo "<HTML><HEAD><META HTTP-EQUIV=\"pragma\" CONTENT=\"no-cache\"></HEAD><BODY>
<SCRIPT TYPE=\"text/javascript\">
  <!--
  function checkAll(form)
  {
  for (i=0; i<form.binary.length; i++) {
     if (form.binary[i].checked=="true")
        form.path.value=form.binary[i].value;
  }
  }
  //-->
  </SCRIPT><FORM action=\"http://localhost:$PORT/Changer_exec.cg?\" method=get>
<H1><CENTER>Change TRUST executable</CENTER></H1>
<BR><B>Click one available binaries detected:</B>" > $file
CHECKED="CHECKED"
[ -f $EXEC_TRUST ] && E=$EXEC_TRUST && echo "<BR><input type=radio name=binary value=\"$EXEC_TRUST\" $CHECKED onClick=\"checkAll(this.form)\">  $EXEC_TRUST" >> $file && CHECKED=""
let n=0
for binary in $binaries
do
    [ $binary != $EXEC_TRUST ] && echo "<BR><input type=radio name=binary value=\"$binary\" $CHECKED onClick=\"checkAll(this.form)\">  $binary" >> $file && CHECKED="" && [ ${#E} = 0 ] && E=$binary 
done
echo "<BR><BR><B>Or write complete path:</B><BR><input type=text name=path size=60 value=\"$E\"><BR><INPUT type=\"submit\" value=\"Change\" > </CENTER></FORM></BODY></HTML>" >> $file
#void(parent.DOWN.location.replace('http://localhost:$PORT/Changer_exec.cg?binary'));
#onClick=\"setTimeout('window.close()',1000);\"
