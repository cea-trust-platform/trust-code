
Example:


**Statistiques Dt_post** dtst {
 
 **t_deb** 0.1 *t_fin* 0.12
 
 **Moyenne** Pression
 
 **Ecart_type** Pression
 
 **Correlation** Vitesse Vitesse
 
 }

will write every dt_post the mean, standard deviation and correlation value:

   if :math:`t<t\_deb` or :math:`t>t\_fin`
   
   .. math:: average: \overline{P(t)} = 0
   .. math:: std\ deviation: <P(t)> = 0
   .. math:: correlation: <U(t).V(t)> = 0
   
   if :math:`t>t\_deb` and :math:`t<t\_fin`
   
   .. math:: average: \overline{P(t)} = \frac{1}{t - t_{deb}} \int_{t_{deb}}^{t}{P(s) ds} 
   .. math:: std\ deviation: <P(t)> = \sqrt{\frac{1}{t - t_{deb}} \int_{t_{deb}}^{t}{[P(s) - \overline{P(t)}]^2 ds} }
   .. math:: correlation: <U(t).V(t)> = \frac{1}{t - t_{deb}} \int_{t_{deb}}^{t}{[U(s) - \overline{U(t)}].[V(s) - \overline{V(t)}] ds}

