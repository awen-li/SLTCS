#!/usr/bin/python
import os
import pandas as pd
import csv
from snapy import MinHash, LSH


class LshCluster():
    def __init__(self, Content, nGram=3, PrenutNum=16, BandNum=8, MinJaccard=0.1):
        self.nGram      = nGram
        self.BandNum    = BandNum
        self.PrenutNum  = PrenutNum
        self.MinJaccard = MinJaccard
        self.Seed       = 3
        self.CreateLsh (Content)

    def Transform(self, Contexts):
        NewContexts = []
        for ctx in Contexts:
            ctx = ctx.strip()
            ctx = ctx.replace("_", "")
            ctx = ctx.lower()
            NewContexts.append(ctx) 
     
        return NewContexts

    def CreateLsh(self, InContext):
        Labels = range(len(InContext))
        InContext = self.Transform (InContext)
        Hash   = MinHash(InContext, n_gram=self.nGram, permutations=self.PrenutNum, hash_bits=64, seed=self.Seed)
        self.Lsh = LSH(Hash, Labels, no_of_bands=self.BandNum)
        self.MaxIndex = len (self.Lsh.adjacency_list(min_jaccard=self.MinJaccard))

    def QuerySimilars(self, Index):
        if Index >= self.MaxIndex:
            return []
        Results = self.Lsh.query(Index, min_jaccard=self.MinJaccard)
        return Results

