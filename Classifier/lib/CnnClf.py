#!/usr/bin/python

import numpy as np
import tensorflow as tf

from tensorflow import keras
from tensorflow.keras import datasets, layers
from sklearn.model_selection import train_test_split

class CnnClf():
    def __init__(self, ClassNum, ChannelNum=1, RowNum=20, ColNum=256, LearnRate=1e-3):
        self.ClassNum   = ClassNum
        self.LearnRate  = LearnRate
        self.BatchSize  = 32

        self.CnnModel = tf.keras.Sequential([
          layers.Conv2D(16, 3, padding='same', activation='relu', input_shape=(RowNum, ColNum, ChannelNum)),
          layers.MaxPooling2D(),
          layers.Conv2D(32, 3, padding='same', activation='relu'),
          layers.MaxPooling2D(),
          layers.Conv2D(64, 3, padding='same', activation='relu'),
          layers.MaxPooling2D(),
          layers.Flatten(),
          layers.Dense(128, activation='relu'),
          layers.Dense(ClassNum),
          layers.Softmax()
        ])
        #self.CnnModel.summary()
    
    def Train(self, Train, Label, ModelPath="result/CnnModel", Epochs=10):
        X_train, X_valid, Y_train, Y_valid = train_test_split(Train, Label, test_size=0.2, random_state=5)
        TrainSet = tf.data.Dataset.from_tensor_slices((X_train, Y_train)).batch(self.BatchSize)
        ValidSet = tf.data.Dataset.from_tensor_slices((X_valid, Y_valid)).batch(self.BatchSize)

        self.CnnModel.compile(optimizer='adam',
                              loss=tf.keras.losses.SparseCategoricalCrossentropy(),
                              metrics=['accuracy'])

        history = self.CnnModel.fit(TrainSet, epochs=Epochs, validation_data=ValidSet)

        self.CnnModel.save(ModelPath, include_optimizer=False)
                    
    def Predict(self, X):
        return self.CnnModel.predict_classes([X])

    def LoadModel (self, ModelPath="result/CnnModel"):
        self.CnnModel = keras.models.load_model(ModelPath)
        self.CnnModel.compile(optimizer='adam',
                              loss=tf.keras.losses.SparseCategoricalCrossentropy(),
                              metrics=['accuracy'])


