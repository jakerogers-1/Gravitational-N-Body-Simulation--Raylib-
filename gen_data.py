"""
Some simple scripts for producing certain .csv files
"""
import random
import csv

UNIVERSE_WIDTH = 1024
UNIVERSE_HEIGHT = 512

def random_universe_256():
    n_bodies = 256
    bodies = []
    for n in range(n_bodies):
        x = random.randint(-UNIVERSE_WIDTH//2, UNIVERSE_WIDTH//2)
        y = random.randint(-UNIVERSE_HEIGHT//2, UNIVERSE_HEIGHT//2)
        name = "body" + str(n)
        mass = 1
        rad = 1
        vx = 0
        vy = 0
        color = "0xFAF9F6FF"

        bodies.append([name, mass, rad, x, y, vx, vy, color])

    with open("./celestial_data/random_universe_256.csv", mode='w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['name','mass','rad','pos_x','pos_y','vel_x','vel_y','color'])
        writer.writerows(bodies)

random_universe_256()