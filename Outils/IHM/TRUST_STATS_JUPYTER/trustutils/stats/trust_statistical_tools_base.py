# coding: utf-8
"""

Morad Ben Tayeb
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to deeply analyze the son files of TRUST.
This work is based on the files package (a TRUST package that reads the son files).

"""

from .. import files as tf
import matplotlib.pyplot as plt
import numpy as np
from scipy import interpolate, signal, optimize
import os
import sys
import re


class StatisticalPostProcessing:
    """

    This is the main class of the Statistical StatisticalPostProcessing code of TRUST.
    See also its main derived classes : 

    - SpatialPostProcessing 
    - TemporalPostProcessing

    """

    COUNT = 0  # counting figures
    COUNT_CURVE_FILES = 0  # To count .curve files
    COUNT_PARAB = 0
    save_path = "./"

    def __init__(self):
        """Constructor of the class StatisticalPostProcessing.
        """
        pass

    def __del__(self):
        """Destructor
        """
        pass

    ###########
    ### Spatial
    ###########

    def _checkName(self, name):
        """

        Used by SpatialPostProcessing.
        Check if the file name is already used

        """
        if name in self.L_NAME:
            print("Warning: This name already exist - {0}".format(name))

    def _getValuesFromCurve(self, name):
        """

        Takes a .curve file and return arrays.
        Used by SpatialPostProcessing.

        Parameters
        ---------
        name : str
            name of the curve file.
            
        Returns
        -------
        Two arrays: the first one corresponds to the first column and the second one to the second column.
        """
        curveFile = self._getFile(name)
        L = np.loadtxt(curveFile)
        return L.T

    def _getFieldsFromOKC(self, name, maxrows=100):
        """

        Takes a .okc file and return arrays of fields.
        Used by SpatialPostProcessing.

        Parameters
        ---------
        name : str
            name of the .okc file.
            
        Returns
        -------
        Four arrays : array.
            Returns X, Y, Z and field. 
        """
        okcFile = self._getFile(name, ext=".okc")
        i = 0
        # Looks to the first raw of data
        # and ignores raised warning
        while i < maxrows:
            try:
                L = np.loadtxt(okcFile, skiprows=i)
                break
            except:
                i = i + 1
        if i >= maxrows:
            raise Exception("First row of data not found")

        return L.T

    ############
    ### Temporal
    ############

    def _getTypeSon(self, sonFile):
        """
        
        This calls SonFile from files package.
        Used by TemporalPostProcessing.

        Parameters
        ----------
        sonFile : str
            A path to a son file
        
        Returns
        -------
        The type of son file : SEGMENT, POINT...

        """
        header = tf.SonFile(sonFile, None)._populateFromHeader()
        for h in header:
            if h[0] == "Type":
                return h[1]
        return None

    def _getFieldSon(self, sonFile):
        """
        This calls SonFile from files package
        To get the field from a file. 
        Used by TemporalPostProcessing.

        Parameters
        ----------
        sonFile : str
            A path to a son file
        
        Returns
        -------
        Field:  pressure, velocity...

        """
        header = tf.SonFile(sonFile, None)._populateFromHeader()
        for h in header:
            if h[0] == "Champ":
                return h[1]
        return None

    def _concatenateSonFiles(self, sonFiles):
        """
        This calls SonFile from files package.
        Concatenates SonFile objects, each one corresponding to one .son file
        Used by TemporalPostProcessing.

        Parameters
        ----------
        sonFiles : list
            A list of .son (or.dt_ev) files
        
        Returns
        -------
        L_SonFiles : list 
            List of SonFile objects 

        """
        L_SonFiles = []
        for pth in sonFiles:
            L_SonFiles += [tf.SonFile(pth, None)]
        return L_SonFiles

    def _concatenateDTEVFiles(self, sonFiles):
        """ 
        This calls DTEVFile from files package.
        Concatenates DTEVFile objects, each one corresponding to one .dt_ev file
        Used by TemporalPostProcessing.

        Parameters
        ----------
        sonFile : list
            A list of paths to dt_ev files

        Returns
        -------
        DTEVFiles : list 
            List of DTEVFile objects 
        """
        DTEVFiles = []
        for pth in sonFiles:  # loop on all the paths
            DTEVFiles += [tf.DTEVFile(pth, None)]  # concatenate them
        return DTEVFiles

    def _concatenateValues(self, obj, entry):
        """

        Concatenates the X (e.g. time) and Y (e.g. Pressure) component values 
        Used by TemporalPostProcessing.

        Parameters
        ----------
        obj : list of objects
            Each one of the object on the list correspond to one file (.son file or .dt_ev file)
        entry : str
            Entry (e.g. "VITESSE_X (X=0.14,Y=0.115)")

        Returns
        -------
        valX,valY : numpy arrays
            Two lists of values concatenated
        """
        tol_son = 1e-6
        valX = []
        valY = []
        i = 1
        for sf in obj:  # loop on the objects (.son files)
            val = list(sf.getValues(entry))
            if i == 1:
                if val[0][-1] - val[0][-2] < tol_son:
                    val[0] = np.delete(val[0], -1)
                    val[1] = np.delete(val[1], -1)
            else:
                l = len(val[0])
                for k in range(l):
                    if val[0][k] > valX[-1] and val[0][k] - valX[-1] > tol_son:
                        index_corr = k
                        break
                if index_corr > 0:
                    val[0] = np.delete(val[0], range(index_corr))
                    val[1] = np.delete(val[1], range(index_corr))

            valX = np.concatenate((valX, val[0]))
            valY = np.concatenate((valY, val[1]))
            i += 1
        return (valX, valY)

    def _is3D(self, sonFile):
        """

        Used by TemporalPostProcessing.

        Parameters
        ----------
        sonFile : str
            A path to a son file

        Returns
        -------
        bool :boolean
            True if the .son file is a 3D result

        """
        L = tf.SonFile(sonFile, None).getEntries()
        for entry in L:
            if "Z=" in entry:
                return True
        return False

    def _delDouble(self, time, field):
        """
        Used by TemporalPostProcessing.
        return an increasing monotonic time table by erasing all non-monotonic time values in a concatenated son file
        Not used anymore.
        """
        while np.argwhere(time[1:] <= time[:-1]).shape[0] != 0:
            find = np.argwhere(time[1:] <= time[:-1])
            time = np.delete(time, find[0][0] + 1)
            field = np.delete(field, find[0][0] + 1)
        return (time, field)

    def _extract_coor_from_entry(self, entry):
        """

        Parsing entry to get the coordinates
        Used by TemporalPostProcessing.

        Parameters
        ---------
        entry : str

        Returns
        -------
        vec : array
            Coordinates

        """

        is3D = "Z=" in entry
        coordinates = re.findall(r"[\d]*[.][\d]+|[\d]+", entry)
        if is3D:
            vec = np.array([float(coordinates[0]), float(coordinates[1]), float(coordinates[2])])
        else:
            vec = np.array([float(coordinates[0]), float(coordinates[1])])
        return vec

    def _findClosestProbe(self, sonFiles, x, y, z, component):
        """

        Find the closest probe to the coordinates (x,y,z)
        Used by TemporalPostProcessing.

        Parameters
        ----------
        sonFiles : list
            A list of .son (or.dt_ev) files
        x,y,z : float
            Input vector given by the user
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None

        Returns
        -------
        res : str
            Closest probe entry (e.g. "VITESSE_X (X=0.14,Y=0.115)")

        """
        component = component.upper()
        tol = 10e12
        is3D = self._is3D(sonFiles[0])

        if is3D:
            vec_user = np.array([x, y, z])
        else:
            vec_user = np.array([x, y])

        Entries = tf.SonFile(sonFiles[0], None).getEntries()
        field = self._getFieldSon(sonFiles[0])
        # print(Entries)
        for entry in Entries:
            if field + "_" + component in entry:  # On verifie si on a VITESSE_X dans entry
                vec = self._extract_coor_from_entry(entry)
                if np.linalg.norm(vec_user - vec) < tol:  # On verifie si il y a des coordonnees plus proches
                    tol = np.linalg.norm(vec_user - vec)
                    res = entry
            elif field + " " in entry:  # Sinon on regarde d'autre champs (pression, etc.)
                entry_no_field = entry.replace(field, "")
                vec = self._extract_coor_from_entry(entry_no_field)
                if np.linalg.norm(vec_user - vec) < tol:
                    tol = np.linalg.norm(vec_user - vec)
                    res = entry
        return res

    def _getValues(self, sonFiles, x, y, z, component):
        """

        Method to get values from coordinates input
        Used by TemporalPostProcessing.

        Parameters
        ----------
        sonFiles : list
            A list of .son (or.dt_ev) files
        x ,y ,z : float
            Coordinates to plot, z is equal to None for a 2D case 
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None, optional

        Returns
        -------
        values : numpy arrays
            time, fields
        entry : str
            (e.g. "VITESSE_X (X=0.14,Y=0.115)")
        """
        component = component.upper()
        self._checkComponent(component)

        is3D = self._is3D(sonFiles[0])
        L_SonFiles = StatisticalPostProcessing._concatenateSonFiles(self, sonFiles)
        if component == "MAGNITUDE" or component == "KINETIC":  # Cas particulier si on veut la norme
            entry1 = StatisticalPostProcessing._findClosestProbe(self, sonFiles, x, y, z, component="X")  # On recupere composante en X
            entry2 = StatisticalPostProcessing._findClosestProbe(self, sonFiles, x, y, z, component="Y")  # On recupere composante en Y
            Coor = entry1.split(" ")[-1]

            # On concatene les valeurs (en cas de reprise)
            time, val1 = StatisticalPostProcessing._concatenateValues(self, L_SonFiles, entry1)
            _, val2 = StatisticalPostProcessing._concatenateValues(self, L_SonFiles, entry2)

            if is3D:  # On verifie bien que l'on est 2D ou 3D
                entry3 = StatisticalPostProcessing._findClosestProbe(self, sonFiles, x, y, z, component="Z")
                _, val3 = StatisticalPostProcessing._concatenateValues(self, L_SonFiles, entry3)
            else:
                val3 = 0
            if component == "MAGNITUDE":
                mag_val = StatisticalPostProcessing._computeNorm(self, val1, val2, val3)
            if component == "KINETIC":
                mag_val = StatisticalPostProcessing._computeKinetic(self, val1, val2, val3)
            return (time, mag_val, component + " " + Coor)

        else:  # Cas general
            entry = StatisticalPostProcessing._findClosestProbe(self, sonFiles, x, y, z, component)  # On recupere l'entry le plus proche
            val = [[], []]
            val[0], val[1] = StatisticalPostProcessing._concatenateValues(self, L_SonFiles, entry)  # On concatene le temps et les valeurs
            return (val[0], val[1], entry)

    def _getSegment(self, sonFiles, component):
        """
        Get segment entries
        Used by TemporalPostProcessing.
        """
        self._checkComponent(component)
        Entries = tf.SonFile(sonFiles[0], None).getEntries()
        field = self._getFieldSon(sonFiles[0])
        seg = []
        for entry in Entries:
            if "_" + component in entry:
                seg.append(entry)

            elif field + " " in entry:
                seg.append(entry)
        return seg

    def _getValuesFromSeg(self, sonFiles, segEntries):
        """
        From a list of entries, get the values
        Used by TemporalPostProcessing.
        """
        L_SonFiles = StatisticalPostProcessing._concatenateSonFiles(self, sonFiles)
        val = []

        time, field = StatisticalPostProcessing._concatenateValues(self, L_SonFiles, segEntries[0])
        val.append(field)
        for entry in segEntries[1:]:
            _, field = StatisticalPostProcessing._concatenateValues(self, L_SonFiles, entry)
            val.append(field)
        return (time, np.array(val))

    def _checkComponent(self, component):
        """
        Method to check component. Raise an error when an invalid component is in input.
        Used by TemporalPostProcessing.

        Parameters
        ---------
        component : str
            Checks component

        """
        if component != "X" and component != "Y" and component != "Z" and component != "MAGNITUDE" and type(component) != str:
            raise Exception("Invalid component input : component = {0}. Must be 'X', 'Y', 'Z', or 'magnitude'".format(component))

    def _checkStartTime(self, startTime):
        """
        Check if the starting time is correctly specified and is not None
        Used by TemporalPostProcessing.
        """
        if type(startTime) != float and type(startTime) != int:
            raise TypeError("Invalid type of startTime: {0}".format(type(startTime)))

    def _computeFluc(self, field):
        """

        Computing fluctuations of a signal
        
        Parameters
        ----------
        field : array

        Returns
        ------
        Fluctuations
        """
        return field - np.mean(field)

    def _getInstantaneous(self, sonFiles, x, y, z=None, component="X"):
        """

        Get fields (time, velocities, ...)
        
        Parameters
        ----------- 
        x ,y ,z : float
            Coordinates, z is equal to None for a 2D case 
        component : str
            "X", "Y", "Z", or "magnitude"

        Returns
        -------
        time : array
            time values
        field : array
            field values

        """
        time, field, entry = StatisticalPostProcessing._getValues(self, sonFiles, x, y, z, component)
        return (time, field, entry)

    def _fluc(self, sonFiles, x, y, z, component, startTime):
        """
        
        Computes fluctuation.

        Parameters
        ----------- 
        x ,y ,z : float
            Coordinates, z is equal to None for a 2D case 
        component : str
            "X", "Y", "Z", or "magnitude"
        
        Returns
        ------
        time, fluc : numpy arrays

        """
        StatisticalPostProcessing._checkStartTime(self, startTime)
        time, field, entry = StatisticalPostProcessing._getValues(self, sonFiles, x, y, z, component)
        if startTime < time[0] or startTime > time[-1]:
            raise Exception("startTime is out of bounds: startTime = {0}, time starts at {1} and ends at {2} ".format(startTime, time[0], time[-1]))
        index = np.argwhere(time > startTime)[0][0]
        fluc = self._computeFluc(field[index:])  # fluctuation
        return (time[index:], fluc, entry)

    def _getFluctuation(self, sonFiles, x, y, z=None, startTime=None, component="X"):
        """

        Method that computes fluctuation of a signal

        Parameters
        ----------- 
        x ,y ,z : float
            Coordinates, z is equal to None for a 2D case 
        startTime : float
            Starting time choosen to analyze data
        step_time : float 
            Step time in .son file 
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None, optional

        Returns
        ------
        time : array
            Time values
        fluc : array
            Fluctuations values
        entry : str 
            Label
        """
        component = component.upper()
        time, fluc, entry = StatisticalPostProcessing._fluc(self, sonFiles, x, y, z, component, startTime)
        return (time, fluc, entry)

    def _getTimeStepEvolution(self, sonFiles, entry="dt"):
        """

        This method plots the data found in the FILE.dt_ev

        Parameters
        ----------
        entry : str
            can be one of the following :
            - entry = "dt" . This is the default entry for tracking the time step evolution
            - entry = "facsec"
            - entry = "residu=max|Ri|"
            - entry = "dt_stab"
            - entry = "Ri=max|dV/dt|"

        Returns
        -------
        time : array
            Array of time values 
        dtev : array
            Array of time step, or others...
            
        """
        dtev_obj = StatisticalPostProcessing._concatenateDTEVFiles(self, sonFiles)
        ent = dtev_obj[0].getEntries()
        if len(ent) == 0:
            raise Exception("No entry found. Please check that your file is .dt_ev ")

        if entry not in ent:
            raise KeyError("Wrong key {0}. Key must be one of the following : {1}".format(entry, ent))

        time, dtev = StatisticalPostProcessing._concatenateValues(self, dtev_obj, entry)

        return (time, dtev)

    def _getAverageWindows(self, sonFiles, x, y, z=None, component="X", startTime=None, endTime=None, window=1, mode=None):
        """

        Parameters
        ----------
        sonFiles : list
            list of .son files path
        x ,y ,z : float
            Coordinates to plot, z is equal to None for a 2D case 
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None, optional
        startTime : float
            Starting time of the statistic analysis
        endTime : float
            Ending time of the statistic analysis
        window : float 
            Time of a single window
        mode : str
            average = window average, convergence = statistical convergence, mean = mean of average window 
            difference = plots absolute difference from the first mean, error = error between accumulated time average and mean 
        
        Returns
        -------
        x : array
            Time.
        y : array
            Field of the chosen mode.
        entry : str 
            Label.
        
        """
        component = component.upper()
        StatisticalPostProcessing._checkStartTime(self, startTime)

        nb_windows = int((endTime - startTime) / window)

        time, field, entry = StatisticalPostProcessing._getValues(self, sonFiles, x, y, z, component)

        if "convergence" in mode:
            x_ax, table_wind_av, _ = self._buildTablesStatsConvEvol(time, field, nb_windows, startTime, window)
            return (x_ax, table_wind_av, entry)

        elif "difference" in mode:
            x_ax, _, difference_table_wind_av = self._buildTablesStatsConvEvol(time, field, nb_windows, startTime, window)
            return (x_ax, difference_table_wind_av, entry)
        elif "average" in mode:
            x_ax, table_wind_av, _ = self._buildTables(time, field, nb_windows, startTime, window)
            return (x_ax, table_wind_av, entry)
        elif "mean" in mode:
            x_ax, table_wind_av, _ = self._buildTables(time, field, nb_windows, startTime, window)
            x = np.array([x_ax[0], x_ax[-1]])
            mean = np.mean(table_wind_av)
            y = np.array([mean, mean])
            return (x, y, entry)
        elif "error" in mode:
            x_ax, table_wind_av, _ = self._buildTablesStatsConvEvol(time, field, nb_windows, startTime, window)
            _, avg, _ = self._buildTables(time, field, nb_windows, startTime, window)
            mean = np.mean(avg)
            y = np.abs(table_wind_av - mean) * 100  # error percentage
            return (x_ax, y, entry)
        else:
            raise Exception("Invalid input")

    def _baseTables(self, time, field, nb_windows, startTime, window):
        """

        Used in _buildTables and _buildTablesStatsConvEvol methods. Building tables to be used in those methods.

        Parameters
        ----------
        time : array
            array of time values
        field : array
            contains values of the analyzed field
        nb_windows : int
            number of windows
        startTime : float
            Starting time of the simulation
        window : real
            time window 
        Returns
        ------
        time_table : array
            Time table
        time_index_table : array
        
        nb_time_probes_table : array

        table_wind_av : array
            table of window averaging
        """
        # building tables
        st = startTime
        time_table = np.zeros(nb_windows + 1)
        time_index_table = np.zeros(nb_windows + 1)
        nb_time_probes_table = np.zeros(nb_windows)
        table_wind_av = np.zeros(nb_windows)
        # finding index
        time_index_table[0] = np.argwhere(time >= startTime)[0][0]
        time_table[0] = startTime
        for i in range(nb_windows):
            st = st + window
            time_table[i + 1] = st
            time_index_table[i + 1] = np.argwhere(time >= st)[0][0]

        for i in range(nb_windows):
            nb_time_probes_table[i] = time_index_table[i + 1] - time_index_table[i] + 1

        return (time_table, time_index_table, nb_time_probes_table, table_wind_av)

    def _buildTables(self, time, field, nb_windows, startTime, window):
        """
        Parameters
        ----------
        time : array
            array of time values
        field : array
            contains values of the analyzed field
        nb_windows : int
            number of windows
        startTime : float
            Starting time of the simulation
        window : real
            time window 

        Returns
        ------
        x_ax : array
            Time table
        table_wind_av : array
            table of window average 
        table_average : 
            table of field average

        """
        # Building tables for window averaging
        time_table, time_index_table, nb_time_probes_table, table_wind_av = self._baseTables(time, field, nb_windows, startTime, window)
        for i in range(nb_windows):
            summ = 0.0
            average_time = 0.0
            for k in time_index_table[i : i + 2]:
                summ = summ + field[int(k)]

            average_time = summ / nb_time_probes_table[i]
            table_wind_av[i] = average_time
        # st = startTime
        st = time_table[0]
        index_stat_all = np.argwhere(time >= st)[0][0]
        table_average = np.mean(field[index_stat_all:])
        x_ax = time_table[1:]
        return (x_ax, table_wind_av, table_average)

    def _buildTablesStatsConvEvol(self, time, field, nb_windows, startTime, window):
        """

        Building tables for statistical convergence

        Parameters
        ----------
        time : array
            array of time values
        field : array
            contains values of the analyzed field
        nb_windows : int
            number of windows
        startTime : float
            Starting time of the simulation
        window : real
            time window 

        Returns
        -------
        x_ax : array
            Time table
        table_wind_av : array
            table of window average 
        difference_table_wind_av : 
            table of difference windows average
        """
        # Building tables for statistical convergence
        time_table, time_index_table, nb_time_probes_table, table_wind_av = self._baseTables(time, field, nb_windows, startTime, window)

        summ = 0
        # average_time = 0
        for k in [time_index_table[0], time_index_table[1]]:
            summ = summ + field[int(k)]

        average_time = summ / nb_time_probes_table[0]
        table_wind_av[0] = average_time
        number_acc_points = nb_time_probes_table[0]
        for i in range(1, nb_windows):
            number_acc_points_2 = number_acc_points + nb_time_probes_table[i]
            summ = table_wind_av[i - 1] * number_acc_points
            average_time = 0.0

            for k in time_index_table[i : i + 2]:
                summ = summ + field[int(k)]

            average_time = summ / number_acc_points_2
            table_wind_av[i] = average_time
            number_acc_points = number_acc_points + nb_time_probes_table[i]

        x_ax = time_table[1:]
        difference_table_wind_av = np.abs(table_wind_av - table_wind_av[0])
        return (x_ax, table_wind_av, difference_table_wind_av)

    def _getSegmentSpatialMean(self, sonFiles, component="X"):
        """
        Method to compute the segment spatial mean.

        Parameters
        ----------
        sonFiles : str 
            List of .son files
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None, optional

        Returns
        -------
        time : array
            Time values
        avg : array
            Segment spatial mean
        entry : str
            Label

        """

        component = component.upper()
        is3D = StatisticalPostProcessing._is3D(self, sonFiles[0])
        if component != "MAGNITUDE":
            segEntries = StatisticalPostProcessing._getSegment(self, sonFiles, component)
            time, segValues = StatisticalPostProcessing._getValuesFromSeg(self, sonFiles, segEntries)
            avg = np.mean(segValues, axis=0)

        else:
            segEntriesX = StatisticalPostProcessing._getSegment(self, sonFiles, "X")
            time, segValuesX = StatisticalPostProcessing._getValuesFromSeg(self, sonFiles, segEntriesX)

            segEntriesY = StatisticalPostProcessing._getSegment(self, sonFiles, "Y")
            _, segValuesY = StatisticalPostProcessing._getValuesFromSeg(self, sonFiles, segEntriesY)
            lenght = len(segEntriesX)

            if is3D:
                segEntriesZ = StatisticalPostProcessing._getSegment(self, sonFiles, "Z")
                _, segValuesZ = StatisticalPostProcessing._getValuesFromSeg(self, sonFiles, segEntriesZ)
            else:
                segValuesZ = np.zeros(lenght)
            segMag = []
            for i in range(lenght):
                segMag.append(StatisticalPostProcessing._computeNorm(self, segValuesX[i], segValuesY[i], segValuesZ[i]))
            segMag = np.array(segMag)
            avg = np.mean(segMag, axis=0)
        entry = component + " spatial mean"
        return (time, avg, entry)

    def _getTemporalAutocorrelation(self, sonFiles, x, y, z=None, component="X", startTime=None, max_lag=None):
        """
        Parameters
        -----------
        x ,y ,z : float
            Coordinates, z is equal to None for a 2D case 
        component : str
            "X", "Y", "Z", or "magnitude" for 2D Z = None, optional
        startTime : float
            Starting time of the statistic analysis
        max_lag : float
            maximum lag for autocorrelation

        Returns
        ------
        X : array 
            array of time values
        autocorr : array
            array of signal autocorrelation
        """
        component = component.upper()
        StatisticalPostProcessing._checkStartTime(self, startTime)

        time, field, entry = StatisticalPostProcessing._fluc(self, sonFiles, x, y, z, component, startTime)

        X, autocorr = StatisticalPostProcessing._computeAutocorrelation(self, time, field, max_lag)

        return (X, autocorr, entry)

    def _getTemporalWelch(self, sonFiles, x, y, z=None, component="X", startTime=None, hanning=False):
        """

        Computes the temporal energy spectrum using the Welch method
        
        Parameters
        ----------
        x ,y ,z : float
            Coordinates to plot, z is equal to None for a 2D case 
        component : str
            "X", "Y", "Z", or "magnitude" for 2D plot Z = None, optional
        startTime : float
            Starting time of the statistic analysis
        Returns
        ------
        f : array
            Frequencies array
        power : array
            Computed Welch method
        entry : str
            label
        """

        component = component.upper()
        StatisticalPostProcessing._checkStartTime(self, startTime)

        time, fluc, entry = StatisticalPostProcessing._fluc(self, sonFiles, x, y, z, component, startTime)

        if hanning:
            f, power, ts, Yhan = StatisticalPostProcessing._computeWelch(self, time, fluc, hanning)
            return (f, power, entry, ts, Yhan)
        else:
            f, power = StatisticalPostProcessing._computeWelch(self, time, fluc, hanning)
            return (f, power, entry)

    ######################
    ### Temporal & Spatial
    ######################

    def _plot(self, x1, y1, label1="", linestyle1="-", linewidth1=0.5, color1=None, **kwargs):
        """

        Method to plot one are multiple data.
        Used by SpatialPostProcessing and TemporalPostProcessing.

        Parameters
        ---------
        x1 : array

        y1 : array

        label : str

        linewidth : float

        To plot multiple data, you can use keywords x1 = ..., y1 = ..., x2 = ..., y2 = ..., ...
        
        Returns
        -------
        None

        """
        plt.style.use("classic")
        plt.rc("xtick", labelsize=14)  # Font size
        plt.rc("ytick", labelsize=14)
        plt.rcParams.update({"font.size": 14})

        plt.plot(x1, y1, label=label1, linestyle=linestyle1, linewidth=linewidth1, color=color1)
        i = 2
        varb = kwargs.get("x{0}".format(i))
        # Multiplot
        while type(varb) != type(None):
            x = kwargs.get("x{0}".format(i))
            y = kwargs.get("y{0}".format(i))
            label = kwargs.get("label{0}".format(i))
            linestyle = kwargs.get("linestyle{0}".format(i))
            linewidth = kwargs.get("linewidth{0}".format(i))
            color = kwargs.get("color{0}".format(i))
            if linewidth == None:
                linewidth = 0.5
            plt.plot(x, y, label=label, linestyle=linestyle, linewidth=linewidth, color=color)
            i = i + 1
            varb = kwargs.get("x{0}".format(i))

        # Keywords management
        xlabel = kwargs.get("xlabel")
        ylabel = kwargs.get("ylabel")
        title = kwargs.get("title")
        xscale = kwargs.get("xscale")
        yscale = kwargs.get("yscale")
        xmin = kwargs.get("xmin")
        xmax = kwargs.get("xmax")
        ymin = kwargs.get("ymin")
        ymax = kwargs.get("ymax")
        name = kwargs.get("name")
        save_path = kwargs.get("save_path")
        loc = kwargs.get("loc")
        borderpad = kwargs.get("label_size")
        format = kwargs.get("format")

        # Setting default values
        if title == None:
            title = ""
        if save_path == None:
            save_path = "results/"
        if (xscale) == (None):
            xscale = "linear"
        if (yscale) == (None):
            yscale = "linear"
        plt.autoscale(tight=True)
        if xmin != None:
            plt.xlim(xmin=xmin)
        if xmax != None:
            plt.xlim(xmax=xmax)
        if ymin != None:
            plt.ylim(ymin=ymin)
        if ymax != None:
            plt.ylim(ymax=ymax)
        if loc == None:
            loc = "upper right"
        if borderpad == None:
            borderpad = 0.4
        if format == None:
            format = "png"

        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.title(title)
        plt.xscale(xscale)
        plt.yscale(yscale)
        plt.grid(True, which="both", ls="-", color="0.65")
        plt.legend(loc=loc, borderpad=borderpad, fontsize=11)
        if save_path != None and not os.path.exists(save_path):
            os.mkdir(save_path)
        if (name) == None:
            StatisticalPostProcessing.COUNT += 1
            plt.savefig(
                save_path + "fig_" + str(StatisticalPostProcessing.COUNT) + "." + format, format=format, bbox_inches="tight",
            )
        else:
            plt.savefig(save_path + name + "." + format, format=format, bbox_inches="tight")
        plt.close()
        return ()

    def _computeNorm(self, X, Y, Z):
        """

        Computing norm of a vector
        Used by SpatialPostProcessing and TemporalPostProcessing.

        Parameters
        ----------
        X,Y,Z: numpy arrays 

        Return
        ------
        N : array
            array of norm 

        """
        return np.sqrt(X ** 2 + Y ** 2 + Z ** 2)

    def _computeKinetic(self, X, Y, Z):
        """
        Computing kinetic energy
        Used by SpatialPostProcessing and TemporalPostProcessing.
        Parameters
        ----------
        X,Y,Z: numpy arrays 

        Returns
        -------
        K : array
            kinetic energy
        """
        return 0.5 * (X ** 2 + Y ** 2 + Z ** 2)

    def _nextpow2(self, x):
        """
        Exponent of next higher power of 2
        Used by SpatialPostProcessing and TemporalPostProcessing.

        Parameters
        ---------
        x : array 
            array of values

        Return
        ------
        y : array 
            next higher power of 2 of of every element of array

        """
        return np.ceil(np.log2(x))

    def _computeWelch(self, X, Y, hanning=False):
        """

        Computing Welch
        Used by SpatialPostProcessing and TemporalPostProcessing.

        Parameters
        ---------
        X : array

        Y : array

        return
        ------
        f : array

        power : array
        """
        m = len(X)
        n = 2 ** self._nextpow2(m)
        dts = (X[-1] - X[0]) / (n - 1)
        ts = np.arange(X[0], X[-1], dts)
        ldts = len(ts)
        Esp = interpolate.interp1d(X, Y, kind="linear")
        f, power = signal.welch(Esp(ts), fs=1.0 / dts, nperseg=ldts)
        # print(power)
        if hanning:
            H = signal.hanning(m)
            Yhan = Y * H
            eSpecHan = interpolate.interp1d(X, Yhan)  # linear interpolation
            return (f, power, ts, eSpecHan(ts))

        return (f, power)

    def _computeAutocorrelation(self, X, Y, max_lag=None):
        """

        Used by SpatialPostProcessing and TemporalPostProcessing.

        Parameters
        ---------
        X : array

        Y : array

        Returns
        -------
        t : array

        autocorr : array

        """
        steps = self._getSteps(X)
        if max_lag == None:  # When no max_lag is specified
            max_lag = X[-1] - X[0]  # We compute max_lag as this

        mean_step = np.mean(steps)  # compute the average of the steps
        max_lag = int(max_lag / mean_step)

        lags, autocorr, _, _ = plt.xcorr(Y, Y, maxlags=int(max_lag))  # autocorrelation
        plt.close()  # close all matplotlib windows
        index = np.argwhere(lags >= 0)[0][0]
        lags = lags[index:]
        autocorr = autocorr[index:]
        t = lags * mean_step
        return (t, autocorr)

    def _computeFFT(self, X, Y):
        """

        Used by SpatialPostProcessing and TemporalPostProcessing.

        Returns
        -------
        xf : array

        fft : array
            Fourier transform

        """
        steps = self._getSteps(X)
        step_time = np.mean(steps)
        z_c = np.fft.fft(Y)  # Transformee de fourier
        xf = np.linspace(0.0, 0.5 / step_time, int(len(z_c) / 2))
        return (xf, np.abs(z_c[: int(len(z_c) / 2)]))

    def _checkPath(self, file_path):
        """
        Used by SpatialPostProcessing and TemporalPostProcessing.
        Check file path. Raise an error if the file is not found
        """
        if not (os.path.exists(file_path)):
            raise FileNotFoundError('The file does not exist or not found: path = "{0}"'.format(file_path))

    def _getSteps(self, X):
        """
        Return array of step from an array X.
        Used by SpatialPostProcessing and TemporalPostProcessing.
        """
        L = [X[i + 1] - X[i] for i in range(len(X) - 1)]
        return L

    def _fitParabola(self, X, Y, digit=5):
        """

        Perform a parabola fitting.

        Parameters
        ----------
        x : array
            time values
        autocorr : array 
            array of signal autocorrelation

        Returns
        ------
        x : array
            x axis values
        y : array
            parabola fit 

        """
        aaa = X
        num_fit1 = 0
        num_fit2 = 3
        xdata = aaa[num_fit1 : num_fit2 + 1]
        ydata = Y[num_fit1 : num_fit2 + 1]
        p = np.polyfit(xdata, ydata, 2)  # try to fit a parabola
        a, b, c = p  # retrieving coefficient
        func = lambda x: a * x ** 2 + b * x + c
        sol = optimize.root(func, 0, method="hybr")
        # print(sol.x) # for debug
        StatisticalPostProcessing.COUNT_PARAB += 1
        f = open("{0}_Roots_{1}.txt".format(sys.argv[0], StatisticalPostProcessing.COUNT_PARAB), "a")
        f.write("{0}\n".format(round(sol.x[0], digit)))
        f.close()
        x = np.linspace(0, max(aaa), 1000)
        y = np.polyval(p, x)  # evaluate parabola
        index = np.argwhere(y <= 0)[0][0] + 5
        return (x[:index], y[:index])
