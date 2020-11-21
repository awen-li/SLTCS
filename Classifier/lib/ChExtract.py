#!/usr/bin/python
##########################################################################
# Author: Wen Li
# Date:   11/20/2020
# Description: app name and paterns extraction, label update
##########################################################################
import re
import os
import nltk
nltk.download('stopwords');nltk.download('brown');nltk.download('punkt');nltk.download('wordnet')
from nltk.corpus import stopwords
from nltk.stem import WordNetLemmatizer
lemmatizer = WordNetLemmatizer()
from gensim import corpora, models
from lib.LabelCtrl import LabelCtrl
import csv

LV2_LABEL = 2
FLOWLABEL = "data/flow2label.csv"

class ChExtract():
    def __init__(self):
        self.LCtl = LabelCtrl ()
        if os.path.exists (FLOWLABEL):
            os.remove (FLOWLABEL)

    def CleanText(self, Text):
        Text = str (Text)
        Text = re.sub(r'[+|/]', ' and ', Text)
        Text = re.sub(r'[^\w\d,]', ' ', Text)
        Text = Text.lower()
        words = Text.split()
        words = [re.sub(r'[^a-z]', '', word) for word in words if word.isalnum()]
        Text = ' '.join(words)
        return Text      
        
    def Cleaning(self, Text, min_len=4):
        Text = self.CleanText (Text)       
        words = nltk.word_tokenize(Text)
        words = [lemmatizer.lemmatize(word) for word in words]
        words = [word for word in words if word not in stopwords.words('english') and len(word) > min_len]
        return words

    def ExPakcet (self, Packet):
        StringSet = []
        String = []
        for byte in Packet:
            if byte >= 32 and byte <= 126:
                String.append (chr (byte))
            else:
                if len (String) > 10:
                    StringSet.append ("".join(String))
                String = []
        return StringSet

    def ExFlow (self, Flow):
        FlowDoc = []
        for packet in Flow:
            StringSet = self.ExPakcet (packet)
            if len (StringSet) == 0:
                continue
            FlowDoc += StringSet
        return FlowDoc

    def ExGroup (self, Group):
        Documents = []
        for Findex in Group:
            Flow = self.Packets [Findex]
            FlowDoc = self.ExFlow (Flow)
            Documents.append(FlowDoc)
        return Documents

    def RateTopic (self, LdaStr):
        #"0.111*"accept"
        Rate, Topic = LdaStr.split ("*")
        return float(Rate), eval(Topic)

    def CleanDoc (self, Document):
        ClDoc = []
        for Doc in Document:
            Cd = self.Cleaning (Doc)
            if len (Cd) == 0:
                continue
            ClDoc.append (Cd)
        return ClDoc
            
    def LdaProc (self, Documents):
        Documents = self.CleanDoc (Documents)
        Dictionary = corpora.Dictionary(Documents)
        Corpus = [Dictionary.doc2bow(words) for words in Documents]
        Lda = models.ldamodel.LdaModel(corpus=Corpus, id2word=Dictionary, num_topics=1)  
        Topic = Lda.print_topics(num_words=1)[0][1]
        #0.294*"baidu"
        Rate, Tp = self.RateTopic (Topic)
        if Rate < 0.1:
            return None          
        return Tp

    def DumpAppChars (self, Topic, Chars, Path="result/Appcharacteristic.csv"):
        with open(Path, 'a+') as File:
            Writer = csv.writer(File)
            Row = [Topic, Chars]
            Writer.writerow(Row)
        File.close ()
        
    def ExCharacteristis (self, Document, Topic):
        Chars = {}
        for Doc in Document:
            for str in Doc:
                if str.find (Topic) == -1:
                    continue
                Chars[str] = 1
        if len (Chars) == 0:
            return
        Chars = list (Chars.keys())
        Chars = ", ".join (Chars)
        self.DumpAppChars (Topic, Chars)

    def DumpFlowLabels (self, FlowLabels, Path=FLOWLABEL):
        Isexist = os.path.exists (Path)
        with open(Path, 'a+') as File:
            Writer = csv.writer(File)

            if Isexist == False:
                Header = list(("flow", "label"))
                Writer.writerow(Header) 

            for flow, label in FlowLabels.items ():
                row = list((flow, label))
                Writer.writerow(row)
        File.close ()

    def UpdateFlowLabels (self, Flows, group, Documents, Topic):
        FlowLabels = {}
        FlowIndex = 0
        #print (Topic, LV2_LABEL)
        Label = self.LCtl.AddLabel (Topic, LV2_LABEL)     
        for Findex in range (len (group)):
            Fdoc = Documents [Findex]
            IsRelTopic = False
            for str in Fdoc:
                if str.find (Topic) != -1:
                    IsRelTopic = True
                    break
            if IsRelTopic == False:
                continue

            Flow = Flows[group[Findex]]
            FlowLabels[Flow] = Label
            #print (Flow, Label)
        self.DumpFlowLabels (FlowLabels)  

    def CharExtract (self, Flows, Packets, Similars):
        self.Packets = Packets
        for group in Similars:
            if len (group) < 5:
                continue
            Documents = self.ExGroup (group)
            if len (Documents) == 0:
                continue
            Topic = self.LdaProc (Documents)
            if Topic == None:
                continue

            # get partens
            self.ExCharacteristis (Documents, Topic)
            
            # Update Labels
            self.UpdateFlowLabels (Flows, group, Documents, Topic)
            
        self.LCtl.WriteLabels ()
            
                    
    
    
