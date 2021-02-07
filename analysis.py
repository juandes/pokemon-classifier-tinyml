import cv2
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


def draw_colors_histogram():
    """
    Draws the leading colors plot.
    :param hist: histogram of color labels
    """
    # the first two values of np.zeros(...) represent the length of the plot
    # the 3 is because of RGB
    plot_width = 700
    plot_length = 150
    plot = np.zeros((plot_length, plot_width, 3), dtype="uint8")
    start = 0

    colors = []

    for i in range(plot_width):
        colors.append((100, 74, 9))

    for idx, val in enumerate(sorted(colors, key=lambda x: x[0])):
        print(idx, val)
        cv2.rectangle(plot, (int(idx), 0), (int(idx+1), plot_length),
                      color=val, thickness=-1)
        print(val)

    plt.axis("off")
    plt.imshow(plot)
    plt.show()


def draw_3d_plot(colors):
    r, g, b = zip(*colors)
    r = np.array(r) / 255.0
    g = np.array(b) / 255.0
    b = np.array(b) / 255.0

    x = np.array(colors)

    fig = plt.figure()
    ax = Axes3D(fig)
    ax.scatter(r, g, b, c=x/255.0)
    ax.set_title("Pixel 3D plot")
    fig.set_size_inches(14, 8)
    plt.show()


draw_3d_plot(colors)


np.array(colors)
