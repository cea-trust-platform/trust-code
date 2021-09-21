# coding: utf-8
"""
Morad Ben Tayeb
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

The main class is TemporalPostProcessing. Using this package allows the following:

- plot the instantaneous evolution of a probe(s) in same/different son file(s)
- Plot the fluctuations evolution of a probe(s) in same/different son file(s) at a given t_start
- plot the dt/residu evolution of a simulation 
- plot the spatial mean evolution of several probes along a segment and/or points from several files
- plot the convergence of the statistical data along a considered probe(s); i.e: several window averaging
- plot the time autocorrelations & calculate the Taylor micro-scale of a probe(s) in same/different son file(s)
- plot the temporal energy spectrum using the Welch method

"""

from .trust_statistical_tools_base import StatisticalPostProcessing
from scipy import interpolate, signal, optimize 
import numpy as np

class TemporalPostProcessing(StatisticalPostProcessing):

    def __init__(self):
        pass

    def __del__(self):
        pass

    def plot(self,x1,y1,label1="",linestyle1="-",linewidth1=0.5,**kwargs):
        """
        
        Method to plot one are multiple data. Calls the StatisticalPostProcessing plot.

        Parameters
        ----------
        x : array

        y : array

        label : str

        linewidth : float

        kwargs : dictionnary 
            To plot multiple data, you can use keywords x1 = ..., y1 = ..., x2 = ..., y2 = ..., ...

        Returns
        -------
        None : None
            Return nothing.

        Examples
        --------

        .. code-block:: python

            obj.plot(
                X, 
                Y, 
                label1 = "Vitesse_x",     
                linestyle1 = '-',        
                linewidth1=0.5,          
                xscale="linear",    # or "log" ...
                yscale="linear",    # or "log" ... 
                title="My title",
                xlabel="distance",
                ylabel="Vitesse_x",
                name = "profX"
                ) 

            # Multi-plot 
            obj.plot(
                x1 = X, y1  = Y, label1 = "Vitesse_X",
                x2 = X2, y2 = Y2, label2 = "Vitesse_Y",
                title = "Velocities", xlabel="distance", ylabel="Velocities", name = "ProfY"
            )

        """
        StatisticalPostProcessing._plot(self,x1,y1,label1=label1,linestyle1=linestyle1,linewidth1=linewidth1,**kwargs)
    
    def instantaneousValues(self,sonFiles,x ,y ,z=None,component="X"):
        """

        Get fields (time, velocities, ...)

        Parameters
        ----------
        sonFiles : list
            List of .son files path
        x : float
            Coordinate.
        y : float
            Coordinate.
        z : float
            Coordinate, Optional. 
        component : str
            "X", "Y", "Z", "magnitude" or "kinetic"

        Returns
        -------
        time : array
            time values
        field : array
            field values
        label : str
            Label of the field (can be used as label in plot method)

        Examples
        --------

        .. literalinclude:: ../../tests/stats/trust_statistical_examples/Temporal/instantaneousValues_example.py


        """
        X, Y, label = StatisticalPostProcessing._getInstantaneous(self,sonFiles ,x ,y ,z,component)
        return(X,Y,label)
    
    def getdtValues(self, sonFiles, entry="dt"): 
        """

        This method plots the data found in the FILE.dt_ev

        Parameters
        ----------- 
        entry : str
            can be one of the following :
                - entry = "dt" . This is the default entry for tracking the time step evolution
                - entry = "facsec"
                - entry = "residu=max|Ri|"
                - entry = "dt_stab"
                - entry = "Ri=max|dV/dt|"
        
        Returns
        -------
        X : array
            An array of time values
        Y : array
            An array of the chosen entry: Residuals, time step, ...

        Examples
        --------

        .. literalinclude:: ../../tests/stats/trust_statistical_examples/Temporal/getdtValues_example.py


        """
        X, Y = StatisticalPostProcessing._getTimeStepEvolution(self,sonFiles,entry)
        return(X,Y)

    def temporalFluctuations(self,sonFiles ,x ,y ,z=None, startTime = None,component = "X"):
        """

        Method that computes fluctuation of a signal

        Parameters
        ----------
        sonFiles : list
            List of .son files path
        x : float
            Coordinate.
        y : float
            Coordinate.
        z : float
            Coordinate, Optional. 
        startTime : float
            Starting time choosen to analyze data
        step_time : float 
            Step time in .son file 
        component : str
            "X", "Y", "Z", "magnitude" or "kinetic"

        Returns
        -------
        time : array
            Time values
        fluc : array
            Fluctuations values
        label : str
            Label of the field (can be used as label in plot method)

        Examples
        --------

        .. literalinclude:: ../../tests/stats/trust_statistical_examples/Temporal/temporalFluctuations_example.py

        """
        X, Y, label = StatisticalPostProcessing._getFluctuation(self,sonFiles ,x ,y , z, startTime, component)
        return(X,Y,label)

    def temporalAverageWindows(self, sonFiles, x ,y ,z=None, component = "X",startTime = None, endTime = None, window = 1,mode = None):
        """

        Window averaging, statistical convergence, etc.
        
        Parameters
        ----------
        sonFiles : list
            List of .son files path
        x : float
            Coordinate.
        y : float
            Coordinate.
        z : float
            Coordinate, Optional. 
        component : str
            "X", "Y", "Z", "magnitude" or "kinetic"
        startTime : float
            Starting time of the statistic analysis
        endTime : float
            Ending time of the statistic analysis
        window : float 
            Time of a single window
        mode : str
            - "average" = window average, 
            - "convergence" = statistical convergence, 
            - "mean" = mean of average window 
            - "difference" = plots absolute difference from the first mean, 
            - "error" = error between accumulated time average and mean 

        Returns
        -------
        X : array
            Array of time values.
        Y : array
            Array that contains the chosen mod: Statistical convergence, window averaging,...
        label : str
            Label of the field (can be used as label in plot method)

        Examples
        --------

        .. literalinclude:: ../../tests/stats/trust_statistical_examples/Temporal/temporalAverageWindows_example.py

        """
        X, Y, label = StatisticalPostProcessing._getAverageWindows(self,sonFiles,x ,y ,z, component, startTime, endTime , window, mode)
        return(X, Y,label)

    def temporalSegmentSpatialMean(self, sonFiles, component = "X"):
        """

        Performs the temporal spatial average on a segment.

        Parameters
        ----------
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None, optional

        Returns
        -------
        X : array
            Time values.
        Y : array
            Spatial average on segment.
        label : str
            Label of the field (can be used as label in plot method)

        Examples
        --------

        .. literalinclude:: ../../tests/stats/trust_statistical_examples/Temporal/temporalSegmentSpatialMean_example.py

        """  
        X, Y, label = StatisticalPostProcessing._getSegmentSpatialMean(self,sonFiles,component)
        return(X,Y,label)

    def temporalAutoCorrelation(self, sonFiles, x ,y ,z=None, digit = 5,
                                component = "X", startTime = None, max_lag = None, 
                                fit = False, mode = "autocorrelation"):
        """

        Performs temporal autocorrelation on signal.

        Parameters
        ----------
        sonFiles : list
            List of .son files path
        x : float
            Coordinate.
        y : float
            Coordinate.
        z : float
            Coordinate, Optional. 
        component : str
            "X", "Y", "Z", or "magnitude" for 2D Z = None, optional
        startTime : float
            Starting time of the statistic analysis
        max_lag : float
            maximum lag for autocorrelation
        fit : Boolean
            When the value is "False", the method does not Returns the parabola fit.
        mode : str
                You can select a mode as this:
                    - mode = ``"autocorrelation"`` (default)
                    - mode = ``"fft"`` returns only Fourier transform of autocorrlation
                    - mode = ``"autocorrelation fft"`` for both
     
        Returns
        -------
        X : array 
            array of time values.
        autocorr : array
            array of signal autocorrelation.
        Xfit : array
            Array of time values, only if fit is ``True``.
        Yfit : array
            Parabola fit, only if fit is ``True``.
        Xfft : array
            Array of frenquencies, Only if ``"fft"`` is in mode.
        Yfft : array
            Fourier transform of autocorrelation,  Only if ``"fft"`` is in mode.
        label : str
            Label of the field (can be used as label in plot method)

        Examples
        --------

        .. literalinclude:: ../../tests/stats/trust_statistical_examples/Temporal/temporalAutoCorrelation_example.py
 

        """

        res = []
        X, Y, label = StatisticalPostProcessing._getTemporalAutocorrelation(self,sonFiles,x ,y , z, component, startTime, max_lag)
        if "autocorrelation" in mode: 
            res = res + [X, Y,label]
        if fit:
            Xfit, Yfit = StatisticalPostProcessing._fitParabola(self,X,Y,digit)
            res = res + [Xfit,Yfit]
        if "fft" in mode:
            Xfft, Yfft = StatisticalPostProcessing._computeFFT(self,X,Y)
            res = res + [Xfft, Yfft,"Fourier transform of autocorrelation"]
        return(res)


    def energySpectrum(self, sonFiles,x ,y ,z=None, component = "X",startTime = None,hanning=False,fft=False):
        """

        Computes the temporal energy spectrum using the Welch method.

        Parameters
        ----------
        sonFiles : list
            List of .son files path
        x : float
            Coordinate.
        y : float
            Coordinate.
        z : float
            Coordinate, Optional. 
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None, optional
        startTime : float
            Starting time of the statistic analysis.

        Returns
        -------
        f : array
            Frequencies array.
        power : array
            Computed Welch method.
        label : str
            Label of the field (can be used as label in plot method)

        Examples
        --------

        .. literalinclude:: ../../tests/stats/trust_statistical_examples/Temporal/energySpectrum_example.py
 
        """
        if hanning:
            X, Y, label, ts, han = StatisticalPostProcessing._getTemporalWelch(self,sonFiles,x ,y ,z, component ,startTime,hanning=True)
            if fft:
                xFFT, yFFT = StatisticalPostProcessing._computeFFT(self,ts,han)
                return(X,Y,label,ts,han,xFFT, yFFT)
            return(X,Y,label,ts,han)
        X, Y, label = StatisticalPostProcessing._getTemporalWelch(self,sonFiles,x ,y ,z, component ,startTime)

        return(X,Y,label)
        
