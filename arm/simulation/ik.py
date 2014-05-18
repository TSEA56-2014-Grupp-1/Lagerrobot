from math import *

# Length of links
links = 100, 100, 50

def find_p(x, y):
    """Choose a point P depending on how close to origin x and y is"""

    if x - links[-1] < 60 and y <= 10:
        return (60, y + sqrt(links[-1] ** 2 - (x - 60) ** 2))
    else:
        return (x - links[-1], y)

def theta_p(x, y):
    """
    Calculate the angles for the first two joints in order to reach the point P
    """

    cos_theta2 = float(x ** 2 + y ** 2 - links[0] ** 2 - links[1] ** 2)
    cos_theta2 /= 2 * links[0] * links[1]
    sin_theta2 = -sqrt(1 - cos_theta2 ** 2)

    det_cmatrix = links[0] ** 2 + 2 * links[1] * cos_theta2 + links[1] ** 2

    cos_theta1 = (links[0] + links[1] * cos_theta2) * x
    cos_theta1 += links[1] * sin_theta2 * y
    cos_theta1 /= det_cmatrix

    sin_theta1 = (links[0] + links[1] * cos_theta2) * y
    sin_theta1 -= links[1] * sin_theta2 * x
    sin_theta1 /= det_cmatrix

    return atan2(sin_theta1, cos_theta1), atan2(sin_theta2, cos_theta2)

def theta(x, y):
    """
    Calculate the angles for all three joints in order to reach the given
    coorinate
    """

    # Find a coordinate P that can be reached with only the first two joints
    px, py = find_p(x, y)

    # Calculate joint angles for the point P
    theta1, theta2 = theta_p(px, py)

    # Calculate theta 3 based on theta 1 and 2
    sin_theta3 = y - py
    cos_theta3 = x - px

    theta3 = atan2(sin_theta3, cos_theta3) - theta2 - theta1

    return theta1, theta2, theta3
