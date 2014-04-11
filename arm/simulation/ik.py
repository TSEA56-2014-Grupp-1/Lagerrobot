from math import *

links = 100, 100, 50

def rad_to_deg(rad):
    return rad * 180 / pi

def find_p(x, y):
    if x - links[-1] < 60 and y <= 0:
        return (60, y + sqrt(links[-1] ** 2 - (x - 60) ** 2))
    else:
        return (x - links[-1], y)

def dot_product(u, v):
    if len(u) != len(v):
        raise ValueError("Vectors are of different length")

    return sum(u[i] * v[i] for i in xrange(len(u)))

def vector_length(u):
    return sqrt(dot_product(u, u))

def vector_scale(k, u):
    return map(lambda x: k * x, u)

def vector_add(u, v):
    if len(u) != len(v):
        raise ValueError("Vectors are of different length")

    return tuple(u[i] + v[i] for i in xrange(len(u)))

def vector_polar(u):
    return atan2(u[1], u[0]), sqrt(sum(x ** 2 for x in u))

def chain_add(*links):
    x, y = 0, 0
    ref_angle = 0

    for length, angle in links:
        dx = length * cos(angle + ref_angle)
        dy = length * sin(angle + ref_angle)

        x += dx
        y += dy
        ref_angle += angle

    return x, y

def theta_p(x, y):
    #if x < 0:
    #    raise ValueError("X must be positive")

    cos_theta2 = float(x ** 2 + y ** 2 - links[0] ** 2 - links[1] ** 2) / (2 * links[0] * links[1])
    sin_theta2 = -sqrt(1 - cos_theta2 ** 2)

    det_cmatrix = (links[0] + links[1] * cos_theta2) ** 2 + (links[1] ** 2 * sin_theta2 ** 2)

    cos_theta1 = (links[0] + links[1] * cos_theta2) * x + links[1] * sin_theta2 * y
    cos_theta1 /= det_cmatrix

    sin_theta1 = (links[0] + links[1] * cos_theta2) * y - links[1] * sin_theta2 * x
    sin_theta1 /= det_cmatrix

    return atan2(sin_theta1, cos_theta1), atan2(sin_theta2, cos_theta2)

def theta(x, y):
    px, py = find_p(x, y)
    theta1, theta2 = theta_p(px, py)

    jx, jy = links[0] * cos(theta1), links[0] * sin(theta1)

    u = [jx - px, jy - py]
    v = map(lambda x: -x, [x - px, y - py])

    theta3 = acos(dot_product(u, v) / (vector_length(u) * vector_length(v)))

    chain_base = [(links[0], theta1), (links[1], theta2)]
    option1 = chain_add(*(chain_base + [(links[2], theta3)]))
    option2 = chain_add(*(chain_base + [(links[2], -theta3)]))

    option1_length = vector_length(vector_add([x, y], vector_scale(-1, option1)))
    option2_length = vector_length(vector_add([x, y], vector_scale(-1, option2)))

    if option1_length > option2_length:
        theta3 *= -1

    return theta1, theta2, theta3

if __name__ == "__main__":
    print zip(*map(lambda x: map(rad_to_deg, x), theta(100, 100)))
    print zip(*map(lambda x: map(rad_to_deg, x), theta(200, 0)))
    print zip(*map(lambda x: map(rad_to_deg, x), theta(200 * cos(pi / 4), 200 * sin(pi / 4))))
    print zip(*map(lambda x: map(rad_to_deg, x), theta(300 * cos(pi / 4), 300 * sin(pi / 4))))
