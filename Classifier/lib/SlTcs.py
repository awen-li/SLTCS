#!/usr/bin/python
##########################################################################
# Author: Wen Li
# Date:   11/18/2020
# Description: self-learning traffic classification system entry
##########################################################################

import os
import csv
import time
import numpy as np
import pandas as pd
from lib.CnnClf import CnnClf
from lib.LabelCtrl import LabelCtrl
from lib.Cluster import LshCluster
from lib.ChExtract import ChExtract


CNN_MODEL = "result/CnnModel"
FLOWLABEL = "data/flow2label.csv"

class SlTcs():
    def __init__(self, IsOffline=False):
        self.LCtl = LabelCtrl ()
        self.Flow2Label = {}
        self.LoadFlow2Labels ()
        self.IsOffline = IsOffline

    def LoadPackets (self, FileName):
        RawData = pd.read_csv(FileName, header=0)
        Data = RawData.values

        Flows   = Data[0::, 0]
        Apps    = Data[0::, 1]
        Packets = Data[0::, 2:22]
        #print (Flows)
        #print (Apps)
        #print (Packets)

        return Flows, Apps, Packets

    def UpdateFlows (self, Flows):
        for flow in Flows:
            self.Flows.append (flow)
        return

    def LoadFlow2Labels (self, File = FLOWLABEL):
        if not os.path.exists (File):
            return None
        DF = pd.read_csv(File, header=0)
        for Index, Row in DF.iterrows():
            self.Flow2Label[Row['flow']] = Row['label']
        return

    def UpdateLabels (self, Flows, Apps):
        Index = 0
        for app in Apps:
            flow = Flows [Index]
            flowLabel = self.Flow2Label.get (flow)
            if flowLabel is not None:
                Label = flowLabel
            else: 
                Label = self.LCtl.AddLabel (app) 
            self.Labels.append (Label)
            Index += 1
        return

    def UpdatePackets (self, Packets):
        for Flow in Packets:  
            NumFlow = []        
            for Pkt in Flow:
                NumPkt  = []
                NumData = Pkt.split()
                for Elm in NumData:
                    Num = int(Elm , 16)
                    NumPkt.append (Num)
                NumFlow.append (np.array(NumPkt))
            self.Packets.append (np.array(NumFlow))
        return

    def Reshape (self):
        TrainSet    = np.array(self.Packets)
        TrainLabels = np.array(self.Labels).flatten()

        TrainSet = TrainSet/255
        TrainSet = TrainSet.astype('float32')
        TrainSet = TrainSet.reshape(len(self.Packets), 20, 256, 1)
        return TrainSet, TrainLabels

    def LoadAllCsvs (self):
        self.Flows   = []
        self.Packets = []
        self.Labels  = []

        # reload all training packets
        CsvDirs = os.walk("CSVs/") 
        for Path, Dirs, Csvs in CsvDirs:
            for csv in Csvs:
                if csv.find("label") != -1:
                    continue
                    
                CsvFile = os.path.join(Path, csv)
                Flows, Apps, Packets = self.LoadPackets (CsvFile)

                self.UpdateLabels (Flows, Apps)
                self.UpdateFlows (Flows)
                self.UpdatePackets (Packets)
        self.LCtl.WriteLabels ()

    def Level1Classify (self):
        self.LoadAllCsvs ()
        TrainSet, TrainLabels = self.Reshape ()
         
        if os.path.exists (CNN_MODEL):
            CnnModel = CnnClf (self.LCtl.GetLabelNum ())
            CnnModel.LoadModel ()    
            PredLabels = CnnModel.Predict (TrainSet)
            LabelLen = len (PredLabels)
            for Index in range (LabelLen):
                if self.LCtl.LabelId2Type (PredLabels[Index]) > self.LCtl.LabelId2Type (TrainLabels[Index]):             
                    print ("Label update: ", TrainLabels[Index], " -> ", PredLabels[Index])
                    TrainLabels[Index] = PredLabels[Index]

        CnnModel = CnnClf (self.LCtl.GetLabelNum ())
        CnnModel.Train (TrainSet, TrainLabels, ModelPath=CNN_MODEL)
        
    def Level2Classify (self):
        LC = LshCluster (self.Flows)
        Visited  = {}
        Similars = []
        FlowNum = len (self.Flows)
        for Index in range (FlowNum):
            if Visited.get (Index) != None:
                continue
            
            Sim = LC.QuerySimilars (Index)
            Visited [Index] = 1
            for s in Sim:
                Visited [s] = 1
            Sim.append (Index)
            Similars.append (Sim)
            #print (Index, " <Similars> ", len (Sim))
        return Similars

    def CharExtract (self, Similars):
        CE = ChExtract ()
        CE.CharExtract (self.Flows, self.Packets, Similars)
    
    def Start(self):
        Num = 0
        # loop to load trainning data and train
        while (True):
            if self.IsOffline == False:
                time.sleep(600)
            
            # 1-level flow clustering
            print ("=> Level 1 classifier..")
            self.Level1Classify ()

            # 2-level flow clustering
            print ("=> Level 2 classifier..")
            Similars = self.Level2Classify ()

            # feature extraction and update labels
            print ("=> Feature extraction and update labels..")
            self.CharExtract (Similars)

            if self.IsOffline == True:
                break
            
       

