
Example:


**Statistiques_en_serie Dt_integr** dtst {
 
 **Moyenne** Pression
 
 }


will calculate and write every dtst seconds the mean value:

   .. math:: (n+1)dt\_integr > t > n . dt\_integr, \overline{P(t)} = \frac{1}{t - n . dt_{integr}} \int_{n . dt_{integr}}^{t}{P(t) dt} 

