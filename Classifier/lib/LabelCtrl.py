#!/usr/bin/python
import os
import pandas as pd
import csv


LABEL_FILE = "labelctrl.csv"

class LabelCtrl():
    def __init__(self):
        self.Labels = self.LoadLabels ();
         
    def LoadLabels (self):
        Labels = {}
        if not os.path.exists(LABEL_FILE):
            return Labels   
        
        PDF = pd.read_csv(LABEL_FILE)       
        for PIndex, PRow in PDF .iterrows():
            Labels[PRow['label']] = PRow['label_id']
        return Labels

    def WriteLabels (self):
        with open(LABEL_FILE, 'w', encoding='utf-8') as LabelFile:
            Writer = csv.writer(LabelFile)
            
            Header = list(("label", "label_id"))
            Writer.writerow(Header)  
            for item in self.Labels.items ():
                if item != None:
 
                    print (item)
                    row = list(item)
                    Writer.writerow(row)
        LabelFile.close()

    def AddLabel (self, Label):    
        ID = self.Labels.get (Label)
        if ID != None:
            return
        self.Labels[Label] = len (self.Labels) + 1
        
            
       

