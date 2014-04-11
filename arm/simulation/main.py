import math
import pyglet

import ik

window = pyglet.window.Window(resizable=False)
window.set_size(800, 600)

chain = [(100, math.pi / 2), (100, 0)]

def find_joints(x, y):
    try:
        return zip(ik.links, ik.theta(x, y))
    except ValueError:
        return chain

def draw_arc(center, radius, start, end, points):
    pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
        ("v2i", (100, 100)))
    k = (start - end) / points
    for i in range(points):
        angle = start + k * i
        pyglet.graphics.draw(
            1, pyglet.gl.GL_POINTS,
            ("v2i",
                (center[0] + int(radius * math.cos(angle)),
                (center[1] - int(radius * math.sin(angle))))))

def draw_line(start, end):
    pyglet.graphics.draw(2, pyglet.gl.GL_LINES, ("v2i", start + end))

def draw_chain(reference, *links):
    x, y = reference
    ref_angle = 0

    for length, angle in links:
        dx = int(length * math.cos(angle + ref_angle))
        dy = int(length * math.sin(angle + ref_angle))

        draw_line((x, y), (x + dx, y + dy))

        x += dx
        y += dy
        ref_angle += angle

@window.event
def on_draw():
    window.clear()

    kwargs = {
        "font_name" : "Verdana",
        "x" : 10,
        "anchor_x" : "left",
        "anchor_y" : "top"
    }

    x, y = ik.chain_add(*chain)
    pyglet.text.Label(
        "Position: {x}x{y}".format(x=x, y=y),
        y=window.height - 10 , **kwargs).draw()

    for i, (length, angle) in enumerate(chain):
        pyglet.text.Label(
            "Joint {id}: {deg} deg, {len} px".format(
                id=i + 1, deg=ik.rad_to_deg(angle), len=length),
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

    draw_arc((window.width // 2, 100), sum(ik.links), 0, math.pi / 2, 50)
    draw_arc((window.width // 2, 100), sum(ik.links), 0, -math.pi / 2, 50)

    draw_chain((window.width // 2, 100), *chain)

#@window.event
#def on_resize(width, height):
#    on_draw()
#    print window.width

@window.event
def on_mouse_motion(x, y, dx, dy):
    global chain
    chain = list(find_joints(x - window.width // 2, y - 100))

pyglet.app.run()
