import random
import os

import numpy as np
import pandas as pd

import matplotlib.pyplot as plt
import seaborn as sns
%matplotlib inline

from pylab import rcParams
rcParams['figure.figsize'] = 15, 10

from sklearn.model_selection import train_test_split
from imblearn.over_sampling import SMOTE

import tensorflow as tf
from tensorflow.keras.callbacks import EarlyStopping

from sklearn.metrics import confusion_matrix, classification_report, accuracy_score, precision_recall_curve, roc_auc_score

df = pd.read_csv("./test/weatherAUS.csv")

df = df.drop(['Date', 'Rainfall', 'Location', 'Evaporation', 'Sunshine', 'WindGustDir', 'WindGustSpeed', 'WindDir3pm', 'WindSpeed9am', 'WindSpeed3pm','WindDir9am', 'Cloud9am', 'Cloud3pm', 'RainTomorrow'], axis=1)

df['RainToday'] = df['RainToday'].replace(['No'], 0)
df['RainToday'] = df['RainToday'].replace(['Yes'], 1)

df.dropna(inplace = True)

normalized_df=(df-df.min())/(df.max()-df.min())

X = normalized_df.drop("RainToday", axis = 1)
y = normalized_df.RainToday
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2, stratify  = y, random_state = 42)

sm = SMOTE(random_state = 666)

X_train_s, y_train_s = sm.fit_resample(X_train, y_train.ravel())

model = tf.keras.Sequential([                      
    tf.keras.layers.Dense(64, activation='relu', input_shape=(8,)),
    tf.keras.layers.Dense(64, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(32, activation='relu'),
    tf.keras.layers.Dense(32, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(16, activation='relu'),
    tf.keras.layers.Dense(8, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid'),
])

early_stopping = EarlyStopping(
    min_delta=0.001,
    patience=20,
    restore_best_weights=True,
)

model.compile(
    optimizer=tf.keras.optimizers.Adam(epsilon=0.01),
    loss='binary_crossentropy',
    metrics=['binary_accuracy']
)

history = model.fit(
    X_train_s, y_train_s, 
    batch_size=16,
    validation_data=(X_test, y_test),
    epochs=1000,
    verbose=1,
    callbacks = [early_stopping]
)

acc = history.history['binary_accuracy']
val_acc = history.history['val_binary_accuracy']
loss = history.history['loss']
val_loss = history.history['val_loss']
epochs = range(len(acc))

plt.plot(epochs,acc)
plt.plot(epochs,val_acc)

plt.figure()

plt.plot(epochs,loss)
plt.plot(epochs,val_loss)

model.save('rainafall_detect.h5')
