#
(ligne_contient("mailler")) {
   fin=0;
   prem=1;
   while ((fin != 0) || (prem == 1))
   {
	if ((NF!=0) && (($NF == "}") ||  ($(NF-1) == "}"))) fin--;
        if ($NF == "{") {prem=0;fin++};
	print $0;
	getline;
   }
}

function mot()
{
   mot_ind++
   if (mot_ind>NF) {
      getline
      while (NF==0) getline
      mot_ind=1
   }
return tolower($mot_ind);
}

function ligne_contient(nom)
{
mot_ind=0
for (i=1;i<NF+1;i++)
   if (mot()==nom)
      return 1
return 0
}

