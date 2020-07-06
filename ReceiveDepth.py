import numpy as np
import math
import pylsl
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from PIL import Image
import cv2

print("Waiting for Depth Stream")
streams = pylsl.resolve_byprop("type", "Depth Image")
inlet = pylsl.StreamInlet(streams[0])
print("Opened Depth Stream")
sample, ts = inlet.pull_sample()
while sample != None:
    width = 640
    arr = np.array(sample).astype(np.uint16)
    image = arr.reshape(int(len(sample)/width), width)
    modified = image.astype(np.int32)
    depth = Image.fromarray(modified, mode='I')
    depth.save("Saved Images\depth" + str(ts) + ".png", "PNG")
    sample, ts = inlet.pull_sample(timeout=10)
print("Done with Depth")
