#!/usr/bin/python
import sys, getopt
import os
from lib.SlTcs import SlTcs

def Remove (File):
    if not os.path.exists (File):
        return
    os.remove (File)
    
def main(argv):
    Action = ""
    Offline = False
    
    try:
        opts, args = getopt.getopt(argv,"hcf",["q="])
    except getopt.GetoptError:
        print ("Run.py -c <clear>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ("Run.py -c <clear>")
            sys.exit()
        elif opt in ("-c", "--clear"):
            Action = "clear";
        elif opt in ("-f", "--offline"):
            Offline = True;
    
    if (not os.path.exists("result")):
        os.makedirs("result")

    if (not os.path.exists("CSVs")):
        print ("data directory does not exist!!!!")
        sys.exit(2)

    if Action == "clear":
        Remove ("data/flow2label.csv")
        Remove ("result/Appcharacteristic.csv")
        Remove ("result/CnnModel")

    SlSysten = SlTcs (Offline)
    SlSysten.Start ();


if __name__ == "__main__":
   main(sys.argv[1:])
