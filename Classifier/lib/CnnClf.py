#!/usr/bin/python

import numpy as np
import tensorflow as tf
from tensorflow.keras import datasets, layers

class CnnClf():
    def __init__(self, ChannelNum, ClassNum, RowNum=20, ColNum=256, LearnRate=1e-3):
        self.ChannelNum = ChannelNum
        self.ClassNum   = ClassNum
        self.LearnRate  = LearnRate

        self.CnnModel = tf.keras.Sequential([
          layers.Conv2D(16, 3, padding='same', activation='relu', input_shape=(RowNum, ColNum, self.ChannelNum)),
          layers.MaxPooling2D(),
          layers.Conv2D(32, 3, padding='same', activation='relu'),
          layers.MaxPooling2D(),
          layers.Conv2D(64, 3, padding='same', activation='relu'),
          layers.MaxPooling2D(),
          layers.Flatten(),
          layers.Dense(128, activation='relu'),
          layers.Dense(num_classes),
          layers.Softmax()
        ])

        self.LossFunc  = tf.keras.losses.SparseCategoricalCrossentropy()
        self.Optimizer = tf.keras.optimizers.Adam(self.lr)
    
    def Train(self, TrainSet, ModelPath="result/model_target", Epochs=10, NewTrain=True):
        @tf.function
        def train_step(X, Y):
            with tf.GradientTape() as tape: 
                Pred = self.model(X, training=True)
                Loss = self.LossFunc(Y, Pred)
            Gradients = tape.gradient(Loss, self.model.trainable_variables)
            self.Optimizer.apply_gradients(zip(Gradients, self.model.trainable_variables))
            
        if not NewTrain:
            self.model.load_weights(ModelPath)
            print("\nWeights loaded!")
        else:
            for ep in range(Epochs):
                print("{}: Epochs {}/{} . . .".format(self.name, ep+1, Epochs))
                for X, Y in TrainSet:
                    train_step(X, Y)
            self.model.save_weights(ModelPath)
        return
                    
    def Predict(self, X):
        return tf.argmax(self.model(X, training=False), 1)

    def Score(self, X, Y):
        X = tf.cast(X, tf.float64)
        Acc = tf.keras.metrics.Accuracy()
        Acc.reset_states()
        Pred = self.Predict(X)
        Acc.update_state(Pred, Y)
        return Acc.result().numpy()

