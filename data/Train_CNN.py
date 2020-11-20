import numpy as np
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras import datasets,layers

import pdb

#import matplotlib.pyplot as plt



facebook=np.loadtxt("./data/facebook.csv",delimiter=',',dtype=str)
hangouts=np.loadtxt("./data/hangouts.csv",delimiter=',',dtype=str)
skype=np.loadtxt("./data/skype.csv",delimiter=',',dtype=str)
other=np.loadtxt("./data/other.csv",delimiter=',',dtype=str)


X_train=[]
y_train=[]
for i in range(len(facebook)):
    image=[]
    for j in range(1,len(facebook[i])-1):
        hexs=facebook[i][j].split(" ")
        bytes=[]
        for hexx in hexs:
            if hexx!="":
                bytes.append([int(hexx,16)])
        bytes=np.array(bytes)
        image.append(bytes)
    image=np.array(image)
    X_train.append(image)
    y_train.append(1)   # 1 for facebook

for i in range(len(hangouts)):
    image=[]
    for j in range(1,len(hangouts[i])-1):
        hexs=hangouts[i][j].split(" ")
        bytes=[]
        for hexx in hexs:
            if hexx!="":
                bytes.append([int(hexx,16)])
        bytes=np.array(bytes)
        image.append(bytes)
    image=np.array(image)
    X_train.append(image)
    y_train.append(2)   # 2 for hangouts

for i in range(len(skype)):
    image=[]
    for j in range(1,len(skype[i])-1):
        hexs=skype[i][j].split(" ")
        bytes=[]
        for hexx in hexs:
            if hexx!="":
                bytes.append([int(hexx,16)])
        bytes=np.array(bytes)
        image.append(bytes)
    image=np.array(image)
    X_train.append(image)
    y_train.append(3)   # 3 for skype

for i in range(len(other)):
    image=[]
    for j in range(1,len(other[i])-1):
        hexs=other[i][j].split(" ")
        bytes=[]
        for hexx in hexs:
            if hexx!="":
                bytes.append([int(hexx,16)])
        bytes=np.array(bytes)
        image.append(bytes)
    image=np.array(image)
    X_train.append(image)
    y_train.append(0)   # 0 for other


X_train=np.array(X_train)
y_train=np.array(y_train)

X_train,X_test,y_train,y_test=train_test_split(X_train,y_train,test_size=0.3,random_state=5)

train_ds=tf.data.Dataset.from_tensor_slices((X_train,y_train)).batch(32)
test_ds=tf.data.Dataset.from_tensor_slices((X_test,y_test)).batch(32)

model = tf.keras.Sequential([
  layers.Conv2D(16, 3, padding='same', activation='relu', input_shape=(10,200,1)),
  layers.MaxPooling2D(),
  layers.Conv2D(32, 3, padding='same', activation='relu'),
  layers.MaxPooling2D(),
  layers.Conv2D(64, 3, padding='same', activation='relu'),
  layers.MaxPooling2D(),
  layers.Flatten(),
  layers.Dense(256, activation='relu'),
  layers.Dense(4),
  layers.Softmax()
])

model.summary()

model.compile(optimizer='adam',
              loss=tf.keras.losses.SparseCategoricalCrossentropy(),
              metrics=['accuracy'])

history = model.fit(train_ds, epochs=25,
                    validation_data=test_ds)



pdb.set_trace()


