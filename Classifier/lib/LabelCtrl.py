#!/usr/bin/python
import os
import pandas as pd
import csv


LABEL_FILE = "labelctrl.csv"

LV1_LABEL  = 1
LV2_LABEL  = 2

class LabelAttr ():
    def __init__(self, Label, Id, Type):
        self.Label = Label
        self.Id    = Id
        self.Type  = Type

class LabelCtrl():
    def __init__(self):
        self.Labels = self.LoadLabels ();
         
    def LoadLabels (self):
        Labels = {}
        if not os.path.exists(LABEL_FILE):
            return Labels   
        
        PDF = pd.read_csv(LABEL_FILE)       
        for PIndex, PRow in PDF .iterrows():
            Label = PRow['label']
            Id    = PRow['label_id']
            Type  = PRow['type']
            Labels[Label] = LabelAttr (Label, Id, Type)
        return Labels

    def WriteLabels (self):
        with open(LABEL_FILE, 'w', encoding='utf-8') as LabelFile:
            Writer = csv.writer(LabelFile)
            
            Header = list(("label", "label_id", "type"))
            Writer.writerow(Header)  
            for label, labelAttr in self.Labels.items ():
                row = list((labelAttr.Label, labelAttr.Id, labelAttr.Type))
                Writer.writerow(row)
        LabelFile.close()

    def AddLabel (self, Label, Type=LV1_LABEL):    
        LabelAttr = self.Labels.get (Label)
        if LabelAttr != None:
            return LabelAttr.Id
        ID = len (self.Labels)
        self.Labels[Label] = LabelAttr (Label, ID, Type)
        return ID

    def InsertLabel (self, Label, ID, Type=LV1_LABEL):
        self.Labels[Label] = LabelAttr (Label, ID, Type)
        return ID

    def UpdateType (self, Label, Type):
        LabelAttr = self.Labels.get (Label)
        if LabelAttr == None:
            return
        LabelAttr.Type = Type

    def LabelId2Type (self, Id):
        for Label, Attr in self.Labels.items ():
            if Attr.Id != Id:
                continue
            return Attr.Type
        assert (0)
        return -1

    def GetLabelNum (self):
        return len (self.Labels)
        
            
       

