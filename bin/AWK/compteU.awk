/Norme du residu/ { residu=$4;getline;ns++;
	if (ns == ncomp) {
	   printf("%d %g\n",0,residu) > compU;
	   it=0;
      	   while ($1 != "Residu") {
	      for (i=1;i<NF+1;i++) printf("%d %g\n",it+i,$i) > compU;
	      it=it+NF;
	      getline;
      	   }
      }
}

/Norm of the residue/ { residu=$5;getline;ns++;
	if (ns == ncomp) {
	   printf("%d %g\n",0,residu) > compU;
	   it=0;
      	   while ($1 != "Final") {
	      for (i=1;i<NF+1;i++) printf("%d %g\n",it+i,$i) > compU;
	      it=it+NF;
	      getline;
      	   }
      }
}

/Convergence/ { nst++
	it = $3
	s+=it
	printf("%d %d\n",nst,it) > convU;
}

END  {
       if (ns==0) { ns=nst }
       if (ns!=0) { 
          print "Iterations of GCP   : ",s;
          print "Number of time step : ",ns;
          print "Iterations/time step: ",s/ns
       }
     }
