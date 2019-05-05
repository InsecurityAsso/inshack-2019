# Neurovision

### Solution

You are given a strange file `neurovision-2d327377b559adb7fc04e0c3ee5c950c` without extension. We start by checking Header with `file` and get : `Hierarchical Data Format (version 5) data` which you might have heard about as `.hdf5`

Looking for clues, we use `strings` and get :
```
...
tensorflow
{"class_name": "Sequential", "config": {"name": "sequential_1", "layers": [{"class_name": "Flatten", "config": {"name": "flatten_1", "trainable": true, "batch_input_shape": [null, 68, 218], "dtype": "float32", "data_format": "channels_last"}}, {"class_name": "Dense", "config": {"name": "dense_1", "trainable": true, "units": 1, "activation": "sigmoid", "use_bias": true, "kernel_initializer": {"class_name": "VarianceScaling", "config": {"scale": 1.0, "mode": "fan_avg", "distribution": "uniform", "seed": null}}, "bias_initializer": {"class_name": "Zeros", "config": {}}, "kernel_regularizer": null, "bias_regularizer": null, "activity_regularizer": null, "kernel_constraint": null, "bias_constraint": null}}]}}
tensorflow
...
```

Obviously, this file might contain a keras model. Let's try to restore it inside a python interactive session :

```python
from keras.models import load_model

model = load_model("../server-files/neurovision-2d327377b559adb7fc04e0c3ee5c950c")
```

```python
>>> model
<keras.engine.sequential.Sequential object at 0x12e053b00>
>>> model.layers
[<keras.layers.core.Flatten object at 0x12e053b38>, <keras.layers.core.Dense object at 0x12e053e48>]
>>> model.layers[0].input_shape
(None, 68, 218)
>>> model.layers[1].output_shape
(None, 1)
```

So this model takes a 2-dimensional array as input an returns a single number. Looks like a single layer neural network. The input looks like some image-like array and the output like a result.

But maybe we can extract some data from it.
```python
>>> model.layers[1].get_weights()
[array([[-4.2567684e-05],
       [-4.2567684e-05],
       [-4.2567684e-05],
       ...,
       [-4.2567684e-05],
       [-4.2567684e-05],
       [-4.2567684e-05]], dtype=float32), array([0.], dtype=float32)]
>>> model.layers[1].get_weights()[0].max()
4.2567684e-05
>>> model.layers[1].get_weights()[0].min()
-4.2567684e-05
```

Looks like it could be some data. Let's set those values either 0 or 255 and reshape them.

```python
weights = (model.layers[1].get_weights()[0] + 1).astype(np.uint8) * 255
data = weights.reshape((68, 218))

img = Image.fromarray(data)
img.save("output.png")
```

And bingo we get the flag !


### Explanation

A single layer layer neural network takes the input values array and multiplies each by its weight. It's just a basic weighted sum which goes through a sigmoid and returns the result.

A black pixel is 0 and a white pixel is 255 so `black pixel * weight = 0`. So if you force the same image to be learnt, you will make the black pixels weights sensitive and only a few of them can lower drasticly the input. But during the training (or overfitting in this case), the neural network stored the image in its weights and and can have a look at it.

[More about it...](https://ml4a.github.io/ml4a/looking_inside_neural_nets/)
