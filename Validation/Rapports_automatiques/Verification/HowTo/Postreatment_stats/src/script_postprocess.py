# -*- coding: utf-8 -*-
"""
@author : Morad BenTayeb
CEA Saclay

Mars 2021
"""

import time 
from trustutils.stats import TemporalPostProcessing

if __name__=='__main__':
    start = time.time()

    # File managment 
    dtev1 = "../sim_1/Cas.dt_ev"
    dtev2 = "../sim_2/Cas.dt_ev"
    dtev3 = "../sim_3/Cas.dt_ev"

    dtev = [dtev1,dtev2,dtev3]

    son_vit1 = "../sim_1/Cas_SONDE_VIT.son"
    son_vit2 = "../sim_2/Cas_SONDE_VIT.son"
    son_vit3 = "../sim_3/Cas_SONDE_VIT.son"

    son_vit = [son_vit1,son_vit2,son_vit3]
    
    son_vitPoint1 = "../sim_1/Cas_SONDE_VIT_POINT.son"
    son_vitPoint2 = "../sim_2/Cas_SONDE_VIT_POINT.son"
    son_vitPoint3 = "../sim_3/Cas_SONDE_VIT_POINT.son"

    son_vit_point = [son_vitPoint1,son_vitPoint2,son_vitPoint3]

    son_rho1 = "../sim_1/Cas_SONDE_RHO.son"
    son_rho2 = "../sim_2/Cas_SONDE_RHO.son"
    son_rho3 = "../sim_3/Cas_SONDE_RHO.son"
    
    son_rho = [son_rho1,son_rho2,son_rho3]

    son_press1 = "../sim_1/Cas_SONDE_PRESS.son"
    son_press2 = "../sim_2/Cas_SONDE_PRESS.son"
    son_press3 = "../sim_3/Cas_SONDE_PRESS.son"

    son_press = [son_press1,son_press2,son_press3]

    # Object (the only instance) 
    obj = TemporalPostProcessing()
    
    # DTEV plots
    t, dt = obj.getdtValues(dtev)
    obj.plot(t,dt,"dt",title = "Time step evolution",xlabel="time",ylabel="time step")

    # Instantaneous and fluctuation velocity 
    t, vit, labelx = obj.instantaneousValues(son_vit,0,0,0,component="x")
    t2, vit2,labelx2 = obj.instantaneousValues(son_vit_point,0,0,0)

    obj.plot(
        x1 = t, y1 = vit, label1 = labelx,
        x2 = t2, y2 = vit2, label2 = labelx2,
        title = "Instantaneous", xlabel = "Time", ylabel = "Velocities"
    )


    t, fluc, label = obj.temporalFluctuations(son_vit,0,0,0,startTime=0.5)
    obj.plot(t,fluc,label,title = "Fluctuation",xlabel="time",ylabel="Velocity fluctuation")

    # Density 
    t, rho, label = obj.instantaneousValues(son_rho,0,0,0)
    obj.plot(t,rho,label,title = "Density",xlabel="time",ylabel="Density")

    # Pressure
    t, press1, label1 = obj.instantaneousValues(son_press,4.69e-2,0.,2.34e-2)
    t2, press2, label2 = obj.instantaneousValues(son_press,4.69e-2,2e-2,2.34e-2)

    obj.plot(
        x1 = t, y1 = press1, label1 = label1,
        x2 = t2, y2 = press2, label2=label2,
        title = "Instantaneous", xlabel = "Time", ylabel = "Pressure"
    )


    t, press, label = obj.temporalFluctuations(son_press,4.69e-2,0.,2.34e-2,startTime=0.5)
    obj.plot(t,press,label,title = "Pressure",xlabel="time",ylabel="Pressure")


    t, press, label = obj.temporalSegmentSpatialMean(son_press)
    obj.plot(t,press,label,title = "Pressure",xlabel="time",ylabel="Pressure")
    
    # Average windows
    t1 , avg1, label1 = obj.temporalAverageWindows(son_press,0,0,0,component = "magnitude", startTime = 0.5, endTime = 1.5, window = 0.1, mode = "average")
    t2 , avg2, label2 = obj.temporalAverageWindows(son_press,0,0,0,component = "magnitude", startTime = 0.5, endTime = 1.5, window = 0.1, mode = "mean")
    t3 , avg3, label3 = obj.temporalAverageWindows(son_press,0,0,0,component = "magnitude", startTime = 0.5, endTime = 1.5, window = 0.1, mode = "convergence")

    obj.plot(
        x1 = t1, y1 = avg1, label1 = "window averaging ",
        x2 = t2, y2 = avg2, label2 = "mean ",
        x3 = t3, y3 = avg3, label3 = "statistical convergence ",
        title = "Instantaneous "+ label1, xlabel = "Time", ylabel = "Windows averaging"
    )
    
    # Autocorrelation
    t, autocorr, label, t2, parab  = obj.temporalAutoCorrelation(son_vit,0,0,0,component = "magnitude", startTime = 0.5, max_lag = .1, fit = True)

    obj.plot(
        t, autocorr, label,
        x2 = t2, y2 = parab, label2 = "Parabola",
        title = "Autocorrolation" ,xlabel="time" ,ylabel="Autocorrelation",
    )

    
    freq, spec, label = obj.energySpectrum(son_vit,0,0,0, startTime = 0.5)
    lin = 0.1 * (freq ** (-5/3))

    obj.plot(
        x1 = freq, y1 = lin, label1 = "-5/3" ,
        x2 = freq, y2 = spec ,label2=label ,
        title = "Welch", xlabel="frequencies" ,ylabel="Autocorrelation", xscale = "log", yscale = "log"
    )
