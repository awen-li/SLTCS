#!/usr/bin/python
import sys
import os
import pandas as pd
from lib.CnnClf import CnnClf
from lib.LabelCtrl import LabelCtrl



def LoadData (FileName):
    RawData = pd.read_csv("data/" + FileName, header=0)
    Data = RawData.values

    Features = Data[0::, 1::]
    Labels   = Data[::, 0]
    return Features, Labels

def main(argv):
    if (not os.path.exists("result")):
        os.makedirs("result")

    if (not os.path.exists("CSVs")):
        print ("data directory does not exist!!!!")
        sys.exit(2)

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
    
    
   

if __name__ == "__main__":
   main(sys.argv[1:])
