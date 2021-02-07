import cv2
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

from sklearn.decomposition import PCA
from sklearn.preprocessing import MinMaxScaler
from mpl_toolkits.mplot3d import Axes3D

# For sampling the dataset
LENGTH = 2000

# Read data
pikachu = pd.read_csv('data/pikachu_complete.csv')
bulbasaur = pd.read_csv('data/bulbasaur_complete.csv')

# Get a sample of length LENGTH
pikachu = pikachu.sample(n=LENGTH, random_state=1)
bulbasaur = bulbasaur.sample(n=LENGTH, random_state=1)

# Index the wanted columns
pikachu_vals = pikachu[['Red', 'Green', 'Blue']]
bulbasaur_vals = bulbasaur[['Red', 'Green', 'Blue']]

# Upscale the values to the RGB scale.
pikachu_scaler = MinMaxScaler(feature_range=(0, 255))
bulbasaur_scaler = MinMaxScaler(feature_range=(0, 255))

# Create new DataFrame with the scaled values.
pikachu_vals_scaled = pd.DataFrame(pikachu_scaler.fit_transform(
    pikachu_vals), columns=pikachu_vals.columns)
bulbasaur_vals_scaled = pd.DataFrame(bulbasaur_scaler.fit_transform(
    bulbasaur_vals), columns=bulbasaur_vals.columns)


def draw_colors_rectangle(df):
    """
    This function draws the colors.
    """
    # The first two values of np.zeros(...) represent the length of the plot
    # the 3 is because of RGB
    plot_length = 150
    plot = np.zeros((plot_length, LENGTH, 3), dtype="uint8")

    # This loop draws in a rectangle the DataFrame's colors tuple.
    # I'm using sort to sort the colors based on its red color.
    # The reverse is for drawing them from darkest to lightest.
    for idx, val in enumerate(sorted(df.to_numpy(), key=lambda x: (x[0]),
                                     reverse=True)):
        cv2.rectangle(plot, (int(idx), 0), (int(idx+1), plot_length),
                      color=list(val), thickness=-1)

    plt.axis("off")
    plt.imshow(plot)
    plt.show()


def draw_3d_plot(colors):
    r, g, b = zip(*colors)
    r = np.array(r) / 255.0
    g = np.array(b) / 255.0
    b = np.array(b) / 255.0

    x = np.array(colors)
    print(x/255.0)

    fig = plt.figure()
    ax = Axes3D(fig)
    # range is 0 to 1
    ax.scatter(r, g, b, c=x/256.0)
    ax.set_title("Pixel 3D plot")
    ax.set_xlabel('R')
    ax.set_ylabel('G')
    ax.set_zlabel('B')
    fig.set_size_inches(14, 8)
    plt.show()


draw_colors_rectangle(pikachu_vals_scaled)
draw_colors_rectangle(bulbasaur_vals_scaled)
draw_3d_plot(pikachu_vals_scaled.to_numpy())
draw_3d_plot(bulbasaur_vals_scaled.to_numpy())
