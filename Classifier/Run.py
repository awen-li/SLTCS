#!/usr/bin/python
import sys
import os
from lib.SlTcs import SlTcs


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

    SlSysten = SlTcs ()
    SlSysten.Start ();


if __name__ == "__main__":
   main(sys.argv[1:])
