import datetime
import os

import pandas as pd
import tensorflow as tf
from sklearn.metrics import classification_report
from sklearn.model_selection import train_test_split

LENGTH = 2000
ts = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")

# TensorBoard directory
log_dir = "/tmp/tensorboard/{}".format(ts)
tensorboard_callback = tf.keras.callbacks.TensorBoard(
    log_dir=log_dir, histogram_freq=1)

# Select a subset of the data
pikachu = pd.read_csv('data/pikachu_df.csv').sample(n=LENGTH, random_state=1)
bulbasaur = pd.read_csv(
    'data/bulbasaur_df.csv').sample(n=LENGTH, random_state=1)

# Create the DataFrame
X = pd.concat([pikachu, bulbasaur])
y = X.pop('Class')


# Use 30% of data for testing
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.3, random_state=0)


model = tf.keras.Sequential(
    tf.keras.layers.Dense(units=1,
                          input_shape=[3],
                          activation='sigmoid')
)

model.compile(
    optimizer=tf.optimizers.Adam(learning_rate=0.01),
    loss=tf.keras.losses.BinaryCrossentropy(),
    metrics="accuracy"
)

model.fit(X_train, y_train,
          epochs=10,
          validation_split=0.1,
          callbacks=[tensorboard_callback],
          )


# Predict and assess
y_pred = model.predict(X_test)
preds = [1 if x > 0.50 else 0 for x in y_pred]
print(classification_report(y_test, preds))

# Convert the model
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Save the model to disk
model_path = "model/pika_bulba{}.tflite".format(ts)
open(model_path, "wb").write(tflite_model)

print("Model is {} bytes".format(os.path.getsize(model_path)))
