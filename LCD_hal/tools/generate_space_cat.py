"""Draw original pixel-art animation frames locally (Pillow; no AI image API).

Run from any directory: python tools/generate_space_cat.py
Then: python tools/convert_animation.py assets/space_cat
"""

from pathlib import Path
import math

from PIL import Image, ImageDraw


OUTPUT = Path(__file__).resolve().parents[1] / "assets" / "space_cat"
BG = "#081020"
INK = "#20243e"
WHITE = "#e9f4ee"
SHADE = "#9ab9c9"
ORANGE = "#f4ac62"
DARK_ORANGE = "#cb794c"
PINK = "#f58e95"
TEAL = "#72e4dd"


def draw_frame(frame):
    image = Image.new("RGB", (96, 96), BG)
    draw = ImageDraw.Draw(image)
    phase = frame * 2 * math.pi / 16
    bob = round(2 * math.sin(phase))
    wave = round(3 * math.sin(phase))
    blink = frame in (10, 11)

    # Small, deliberately sparse star field. Background is opaque in every frame.
    for index, (x, y) in enumerate(((6, 15), (17, 38), (84, 41), (11, 72),
                                   (87, 79), (33, 9), (68, 88), (44, 84))):
        bright = (frame + index * 3) % 16 < 8
        draw.point((x, y), fill=WHITE if bright else "#385672")
        if bright and index % 3 == 0:
            draw.line((x - 1, y, x + 1, y), fill=TEAL)
            draw.line((x, y - 1, x, y + 1), fill=TEAL)

    # A little ringed planet behind the astronaut.
    draw.ellipse((68, 8, 85, 23), fill=INK)
    draw.ellipse((69, 9, 84, 22), fill="#8472c9")
    draw.rectangle((72, 12, 81, 14), fill="#c7b4ed")
    draw.line((63, 22, 88, 12), fill="#e0af87", width=3)
    draw.line((64, 23, 88, 14), fill="#9c6f85", width=1)

    def rect(box, fill):
        x0, y0, x1, y1 = box
        draw.rectangle((x0, y0 + bob, x1, y1 + bob), fill=fill)

    def ellipse(box, fill, outline=None, width=1):
        x0, y0, x1, y1 = box
        draw.ellipse((x0, y0 + bob, x1, y1 + bob), fill=fill,
                     outline=outline, width=width)

    def line(points, fill, width=1):
        draw.line([(x, y + bob) for x, y in points], fill=fill, width=width)

    def polygon(points, fill):
        draw.polygon([(x, y + bob) for x, y in points], fill=fill)

    # Curled tail, life-support backpack, and orange thruster flicker.
    tail = round(2 * math.cos(phase))
    line(((59, 64), (68, 66), (74, 62), (74 + tail, 55)), INK, 7)
    line(((59, 63), (68, 65), (73, 61), (73 + tail, 55)), ORANGE, 4)
    rect((28, 48, 36, 66), INK)
    rect((29, 49, 35, 64), SHADE)
    rect((29, 52, 31, 60), "#6d829f")
    flame = 3 + frame % 3
    polygon(((30, 66), (35, 66), (33, 70 + flame), (31, 69)), "#fa9653")
    polygon(((31, 66), (34, 66), (33, 70)), "#ffe4a1")

    # Legs and boots.
    polygon(((36, 64), (46, 64), (45, 76), (35, 79), (31, 75)), INK)
    polygon(((37, 65), (44, 65), (43, 74), (35, 77), (34, 74)), WHITE)
    rect((33, 74, 42, 77), SHADE)
    polygon(((49, 64), (59, 64), (62, 75), (58, 79), (48, 76)), INK)
    polygon(((51, 65), (57, 65), (59, 74), (56, 77), (50, 74)), WHITE)
    rect((51, 74, 59, 77), SHADE)

    # Rounded spacesuit torso and a tiny telemetry panel.
    ellipse((31, 45, 64, 72), INK)
    ellipse((33, 46, 62, 70), WHITE)
    ellipse((35, 60, 61, 70), SHADE)
    rect((39, 51, 55, 63), INK)
    rect((40, 52, 54, 62), "#6d829f")
    rect((42, 54, 52, 56), TEAL)
    rect((42, 59, 44, 60), ORANGE)
    rect((48, 59, 51, 60), WHITE)
    rect((36, 66, 59, 67), INK)

    # Left arm floats downward; the other paw waves on a smooth loop.
    line(((34, 51), (28, 59), (25, 57)), INK, 9)
    line(((34, 51), (28, 57), (25, 56)), WHITE, 5)
    ellipse((20, 52, 28, 60), INK)
    ellipse((21, 53, 27, 59), ORANGE)
    line(((59, 51), (68, 47), (72, 40 + wave)), INK, 9)
    line(((59, 50), (67, 46), (71, 40 + wave)), WHITE, 5)
    ellipse((67, 32 + wave, 76, 42 + wave), INK)
    ellipse((68, 33 + wave, 75, 41 + wave), ORANGE)
    rect((70, 35 + wave, 71, 37 + wave), PINK)
    rect((73, 34 + wave, 74, 36 + wave), PINK)

    # Helmet shell and dark blue visor, with warm orange cat ears inside.
    ellipse((24, 18, 68, 53), INK)
    ellipse((26, 19, 66, 51), SHADE)
    ellipse((27, 19, 65, 49), WHITE)
    ellipse((29, 21, 63, 47), "#24425d")
    polygon(((32, 34), (32, 21), (42, 28), (51, 27), (59, 20), (60, 36)), INK)
    polygon(((34, 33), (34, 24), (42, 30), (51, 29), (57, 23), (58, 35)), ORANGE)
    polygon(((35, 26), (36, 32), (40, 30)), PINK)
    polygon(((56, 26), (52, 30), (56, 32)), PINK)
    ellipse((32, 27, 60, 46), DARK_ORANGE)
    ellipse((33, 27, 59, 44), ORANGE)
    rect((44, 28, 46, 31), DARK_ORANGE)
    rect((49, 28, 50, 30), DARK_ORANGE)
    ellipse((38, 36, 54, 44), "#ffe4ba")
    if blink:
        line(((37, 35), (41, 35)), INK, 1)
        line(((51, 35), (55, 35)), INK, 1)
    else:
        rect((38, 33, 40, 36), INK)
        rect((52, 33, 54, 36), INK)
        rect((38, 33, 38, 33), WHITE)
        rect((52, 33, 52, 33), WHITE)
    rect((35, 38, 38, 39), PINK)
    rect((55, 38, 58, 39), PINK)
    polygon(((44, 37), (48, 37), (46, 39)), "#a95760")
    line(((46, 39), (46, 41), (43, 41)), INK)
    line(((46, 41), (49, 41)), INK)
    line(((32, 35), (35, 36)), "#ffe4ba")
    line(((57, 36), (61, 35)), "#ffe4ba")
    # Reflective visor highlights and two external ear-cup connectors.
    line(((31, 25), (33, 23), (39, 22)), "#a5ece9", 2)
    rect((59, 40, 60, 42), "#80c4d1")
    rect((24, 33, 27, 39), INK)
    rect((25, 34, 27, 38), TEAL)
    rect((65, 33, 68, 39), INK)
    rect((65, 34, 67, 38), TEAL)
    rect((39, 48, 54, 50), INK)
    rect((41, 48, 52, 49), TEAL)
    return image


def main():
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for frame in range(16):
        draw_frame(frame).save(OUTPUT / f"frame_{frame:02d}.png")
    print(f"Drew 16 original 96x96 pixel-art frames in {OUTPUT}")


if __name__ == "__main__":
    main()
