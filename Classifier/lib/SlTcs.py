#!/usr/bin/python
import os
import pandas as pd
import csv
from lib.CnnClf import CnnClf
from lib.LabelCtrl import LabelCtrl
from lib.Cluster import Cluster

class SlTcs():
    def __init__(self):
        self.Labels = {};

    def Start(self):        
        # loop to load trainning data and train
        while (True):
            CsvDirs = os.walk("CSVs/") 
            for Path, Dirs, Csvs in CsvDirs:
                print (Path)
                for csv in Csvs:
                    CsvFile = os.path.join(Path, csv)
                    print(CsvFile)

            LC = LabelCtrl ()
            LC.AddLabel ("youtube")
            LC.WriteLabels ()

            break
            
       

