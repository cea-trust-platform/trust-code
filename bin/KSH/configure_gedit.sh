#!/bin/bash
# Configure gedit

lang_file=TRUST.lang
style_file=TRUST.xml
keywords=$TRUST_ROOT/doc/TRUST/Keywords.txt

# test of existing libgtksourceview
if [ "`ldconfig -p | grep gtksourceview-4`" != "" ]
then
    path=~/.local/share/gtksourceview-4.0
elif [ "`ldconfig -p | grep gtksourceview-3`" != "" ]
then
    path=~/.local/share/gtksourceview-3.0
elif [ "`ldconfig -p | grep gtksourceview-2`" != "" ]
then
    path=~/.local/share/gtksourceview-2.0
else
    echo "No libgtksourceview found !!!"
    echo "gtksourceview must be installed for configuring gedit highlighting !"
    echo "STOP"
    exit 1
fi

echo "Configuring gedit into $path"
echo ""

# if OK, create directories
mkdir -p $path/styles
mkdir -p $path/language-specs

path_lang_file=$path/language-specs/$lang_file
path_style_file=$path/styles/$style_file

# Write a new lang file
echo '<?xml version="1.0" encoding="UTF-8"?>
<language id="trust" _name="TRUST" version="2.0" _section="Scripts">
    <metadata>
        <property name="globs">*.data;*.geo</property>
    </metadata>
    <styles>
        <style id="keywords"       _name="Keywords"      map-to="def:string"/>
        <style id="comments"       _name="Comments"      map-to="def:comment"/>
        <style id="comments2"      _name="Comments2"     map-to="def:comment"/>
        <style id="chiffre"        _name="Chiffre"       map-to="latex:display-math"/>
        <style id="point_virgule"  _name="Point_virgule" map-to="def:identifier"/>
    </styles>
    <default-regex-options case-sensitive="false"/>
    <definitions>
        <context id="trust">
            <include>
                <context id="keywords" style-ref="keywords">
                    <keyword>TAG</keyword>
                </context>
                <context id="comments" style-ref="comments">
                    <start>\/\* </start>
                    <end> \*\/</end>
                </context>
                <context id="comments2" style-ref="comments2">
                    <start>\# </start>
                    <end> \#</end>
                </context>
                <context id="floating-point-number" style-ref="chiffre">
                    <match extended="true">
                    (?&lt;![\w\.])
                    [+-]?
                    ((\.[0-9]+ | [0-9]+\.[0-9]*) ([ed][+-]?[0-9]*)? |
                    ([0-9]+[ed][+-]?[0-9]*))
                    (?![\w\.])
                    </match>
                </context>
                <context id="decimal" style-ref="chiffre">
                    <match extended="true">
                    (?&lt;![\w\.])
                    [+-]? ([1-9][0-9]*|0)
                    (?![\w\.])
                    </match>
                </context>
                <context id="point_virgule" style-ref="point_virgule">
                    <match>[;]</match>
                </context>
            </include>
        </context>
    </definitions>
</language>'                            > $path_lang_file.tmp

# Grab the TRUST keywords
KeywordsTRUST=`$TRUST_Awk '!/\|/ {k=k" "$1} /\|/ {gsub("\\\|"," ",$0);k=k" "$0} END {print k}' $keywords`
KeywordsTRUST=`echo $KeywordsTRUST | awk '{gsub(" ","<k2>aaa<k1>",$0);print $0}'`
sed -i "s/TAG/$KeywordsTRUST/"              $path_lang_file.tmp
sed -i "s/<k1>/<keyword>/g"                 $path_lang_file.tmp
sed -i "s#<k2>#</keyword>\n#g"              $path_lang_file.tmp
sed -i "s#aaa#                    #g"       $path_lang_file.tmp
sed -i "s#<=#\&lt;=#"                       $path_lang_file.tmp



if [ ! -f $path_lang_file ]
then
    mv -f $path_lang_file.tmp $path_lang_file
    echo "$path_lang_file updated..."
else
    if [ "`diff $path_lang_file $path_lang_file.tmp`" != "" ]
    then
        mv -f $path_lang_file $path_lang_file.save
        echo "Renaming old language file in $path_lang_file.save"
        mv -f $path_lang_file.tmp $path_lang_file
        echo "$path_lang_file updated..."
        echo ""
    else
        rm -f $path_lang_file.tmp
    fi
fi




# Write style file
echo '<?xml version="1.0" encoding="UTF-8"?>
<style-scheme id="trust" _name="TRUST" version="1.0">
  <author>TRUST support team</author>
  <_description>To highlight TRUST keywords</_description>
  <!-- Palette -->
  <color name="blue"       value="#0000FF"/>
  <color name="red"        value="#FF0000"/>
  <color name="yellow"     value="#FFFF00"/>
  <color name="magenta"    value="#FF00FF"/>
  <color name="green"      value="#2E8B57"/>
  <color name="purple"     value="#A020F0"/>
  <!-- Bracket Matching -->
  <style name="bracket-match"               foreground="#white" background="#magenta" bold="true"/>
  <!-- not coded for the moment -->
  <style name="bracket-mismatch"            foreground="#white" background="#red" bold="true"/>
  <!-- Search Matching -->
  <style name="search-match"                foreground="#000000" background="yellow"  bold="true"/>  
  <!-- TRUST specific styles -->
  <style name="trust:keywords"              foreground="red"      bold="true"/>
  <style name="trust:comments"              foreground="blue"     bold="false"/>
  <style name="trust:comments2"             foreground="blue"     bold="false"/>
  <style name="trust:chiffre"               foreground="purple"   bold="false"/>
  <style name="trust:point_virgule"         foreground="green"    bold="true"/>
  <style name="line-numbers"/>
</style-scheme>' > $path_style_file.tmp

if [ ! -f $path_style_file ]
then
    mv -f $path_style_file.tmp $path_style_file
    echo "$path_style_file updated..."
else 
    if [ "`diff $path_style_file $path_style_file.tmp`" != "" ]
    then
        mv -f $path_style_file $path_style_file.save
        echo "Renaming old language file in $path_style_file.save"
        mv -f $path_style_file.tmp $path_style_file
        echo "$path_style_file updated..."
        echo ""
    else
        rm -f $path_style_file.tmp
    fi
fi


# Apply configuration
echo "To finish installation:
- open gedit,
- select the TRUST language in the drop down menu at the bottom right of your window,
- go to 'Edit/Preferences/Font & Colors',
- add a new style with the file $path_style_file,
- close all your sessions of gedit, then re-open a session."


