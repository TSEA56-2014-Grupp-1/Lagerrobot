import math
import pyglet

import ik

# Create a window to draw in
window = pyglet.window.Window(resizable=False)
window.set_size(800, 600)

# Global containing last valid joint lengths and angles
chain = []

def rad_to_deg(rad):
    """Convert radians to degrees"""

    return rad * 180 / math.pi

def last(seq):
    """Returns the last value of a sequence"""

    item = None
    for item in seq:
        pass

    return item

def find_joints(x, y):
    """Get joint angles based on current a given x and y relative to arm base"""

    try:
        return zip(ik.links, ik.theta(x, y))
    except ValueError:
        return chain

def draw_line(start, end):
    """Draw a line for the tuple position start to end"""

    pyglet.graphics.draw(2, pyglet.gl.GL_LINES, ("v2i", map(int, start + end)))

def chain_add(*links):
    """
    Add the given joint lengths and angles and yield everytime a x and y
    coordinate has been added
    """

    x, y = 0, 0
    ref_angle = 0

    for length, angle in links:
        dx = length * math.cos(angle + ref_angle)
        dy = length * math.sin(angle + ref_angle)

        x += dx
        y += dy
        ref_angle += angle

        yield x, y

def draw_chain(reference, *links):
    """
    Draws links in arm starting at reference point. Links are tuples where the
    first element is link length and the second is joint angle
    """

    last_x, last_y = reference

    for dx, dy in chain_add(*links):
        new_x, new_y = reference[0] + dx, reference[1] + dy
        draw_line((last_x, last_y), (new_x, new_y))
        last_x, last_y = new_x, new_y

@window.event
def on_draw():
    window.clear()

    kwargs = {
        "font_name" : "Verdana",
        "x" : 10,
        "anchor_x" : "left",
        "anchor_y" : "top"
    }

    x, y = last(chain_add(*chain)) or (0, 0)
    pyglet.text.Label(
        "Position: {x}x{y}".format(x=x, y=y),
        y=window.height - 10 , **kwargs).draw()

    for i, (length, angle) in enumerate(chain):
        pyglet.text.Label(
            "Joint {id}: {deg} deg, {len} px".format(
                id=i + 1, deg=rad_to_deg(angle), len=length),
            y=window.height - 30 - i * 20, **kwargs).draw()

    draw_line(
        (window.width // 2 - 50, 0),
        (window.width // 2 - 50, 100))
    draw_line(
        (window.width // 2 + 50, 0),
        (window.width // 2 + 50, 100))
    draw_line(
        (window.width // 2 - 50, 100),
        (window.width // 2 + 50, 100))

    draw_chain((window.width // 2, 100), *chain)

@window.event
def on_mouse_motion(x, y, dx, dy):
    global chain
    chain = list(find_joints(x - window.width // 2, y - 100))

pyglet.app.run()
