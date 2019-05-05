"""Generate data, train Tensorflow model and freeze it."""


import keras
import numpy as np
from PIL import Image


source_image = Image.open("source.png").convert('L')
image_data = np.asarray(source_image)
width, height = source_image.size

w = (image_data / 255).astype(np.int8)
b = np.count_nonzero(w)
w[w<=0] = -1

model = keras.models.Sequential([
    keras.layers.Flatten(input_shape=(height, width)),
    keras.layers.Dense(1, activation='sigmoid')
])

model.compile(optimizer='rmsprop', loss='mse')

weights = model.layers[1].get_weights()
weights[0] = w.reshape(height*width, 1).astype(np.float32) / (b*14)
model.layers[1].set_weights(weights)

model.save("../server-files/neurovision-2d327377b559adb7fc04e0c3ee5c950c")

print("[+] Model generated")
