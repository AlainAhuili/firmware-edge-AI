"""Smoke test: core libraries import and a tiny Keras model converts to TFLite."""
import os

os.environ.setdefault("TF_CPP_MIN_LOG_LEVEL", "2")

import matplotlib
import numpy as np
import pandas as pd
import sklearn
import tensorflow as tf

print(f"numpy {np.__version__} | pandas {pd.__version__} | scikit-learn {sklearn.__version__}")
print(f"matplotlib {matplotlib.__version__} | tensorflow {tf.__version__}")

model = tf.keras.Sequential([
    tf.keras.Input((4,)),
    tf.keras.layers.Dense(8, activation="relu"),
    tf.keras.layers.Dense(1),
])
tflite_model = tf.lite.TFLiteConverter.from_keras_model(model).convert()
print(f"TFLite conversion OK: {len(tflite_model)} bytes")
