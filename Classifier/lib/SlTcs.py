#!/usr/bin/python
import os
import numpy as np
import pandas as pd
import csv
from lib.CnnClf import CnnClf
from lib.LabelCtrl import LabelCtrl
from lib.Cluster import LshCluster

CNN_MODEL = "result/CnnModel"

class SlTcs():
    def __init__(self):
        self.LCtl = LabelCtrl ()
        self.Flow2Label = {}
        self.LoadFlow2Labels ()

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

    def LoadFlow2Labels (self, File = "data/flow2label.csv"):
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
                self.LCtl.InsertLabel (app, Label)
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
        print (1, TrainSet.shape, TrainLabels.shape)
            
        TrainSet = TrainSet/255
        TrainSet = TrainSet.astype('float32')
        TrainSet = TrainSet.reshape(len(self.Packets), 20, 256, 1)
        print (2, TrainSet.shape, TrainLabels.shape)
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
        
        CnnModel = CnnClf (self.LCtl.GetLabelNum ())
        if os.path.exists (CNN_MODEL):
            CnnModel.LoadModel ()    
            PredLabels = CnnModel.Predict (TrainSet)
            LabelLen = len (PredLabels)
            for Index in range (LabelLen):
                if self.LCtl.LabelId2Type (PredLabels[Index]) > self.LCtl.LabelId2Type (TrainLabels[Index]):             
                    print ("Label update: ", TrainLabels[Index], " -> ", PredLabels[Index])
                    TrainLabels[Index] = PredLabels[Index]

        CnnModel.Train (TrainSet, TrainLabels, ModelPath=CNN_MODEL)
        
    def Level2Classify (self):
        LC = LshCluster (self.Flows)
        Visited = {}
        FlowNum = len (self.Flows)
        for Index in range (FlowNum):
            if Visited.get (Index) != None:
                continue

            if Index == 511:
                continue
            
            Similars = LC.QuerySimilars (Index)
            Visited [Index] = 1
            for s in Similars:
                Visited [s] = 1
            Similars.append (Index)
            print (Index, " <Similars> ", len (Similars))

    
    def Start(self):
        Num = 0
        # loop to load trainning data and train
        while (True):
            self.Level1Classify ()

            # 2-level flow clustering
            self.Level2Classify ()

            # feature extraction and update labels

            break
            
       

