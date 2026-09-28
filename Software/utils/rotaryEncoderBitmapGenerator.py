#Thanks ChatGPT

import math

WIDTH  = 32
HEIGHT = 32
PAGES  = HEIGHT // 8

CENTER_X = WIDTH // 2
CENTER_Y = HEIGHT // 2

SHAFT_RADIUS = 14
SHAFT_OUTLINE = 2
KEYWAY_WIDTH = 2

def empty_bitmap():
    return [[0 for _ in range(WIDTH)] for _ in range(HEIGHT)]

def draw_shaft(img):
    # Draw shaft
    for y in range(HEIGHT):
        for x in range(WIDTH):
            dx = x - CENTER_X + 0.5
            dy = y - CENTER_Y + 0.5
            d = math.sqrt(dx*dx + dy*dy)

            if SHAFT_RADIUS - SHAFT_OUTLINE <= d <= SHAFT_RADIUS + SHAFT_OUTLINE:
                img[y][x] = 1

def draw_keyway(img, frame):
    angle = frame * (2 * math.pi / 6)

    # Direction along the keyway
    ux = math.cos(angle)
    uy = math.sin(angle)

    # Perpendicular direction
    vx = -uy
    vy = ux

    for t in range(-SHAFT_RADIUS + 2, SHAFT_RADIUS - 1):
        for w in range(-KEYWAY_WIDTH, KEYWAY_WIDTH + 1):
            x = int(CENTER_X + t * ux + w * vx)
            y = int(CENTER_Y + t * uy + w * vy)

            if 0 <= x < WIDTH and 0 <= y < HEIGHT:
                img[y][x] = 1

def to_ssd1306_bytes(img):
    data = []
    for page in range(PAGES):
        for x in range(WIDTH):
            byte = 0
            for bit in range(8):
                y = page * 8 + bit
                if y < HEIGHT and img[y][x]:
                    byte |= (1 << bit)
            data.append(byte)
    return data

def emit_c_array(name, data):
    print(f"const __code uint8_t {name}[{len(data)}] = {{")

    for page in range(PAGES):
        print(f"    // Page {page}")
        start = page * WIDTH
        end   = start + WIDTH

        for i in range(start, end, 12):
            row = ", ".join(f"0x{b:02X}" for b in data[i:min(i+12, end)])
            print(f"    {row},")

    print("};\n")

# ---- Generate all positions ----
bitmap_names = []

for frame in range(6):
    img = empty_bitmap()

    draw_shaft(img)
    draw_keyway(img, frame)

    data = to_ssd1306_bytes(img)
    name = f"bmpRotary_{frame}"
    bitmap_names.append(name)

    emit_c_array(name, data)

print("uint8_t __code * __code bmpImageList[6] = {")
for name in bitmap_names:
    print(f"    {name},")
print("};")
