#Thanks ChatGPT

import math

WIDTH  = 32
HEIGHT = 32
PAGES  = HEIGHT // 8

CENTER_X = WIDTH // 2
CENTER_Y = HEIGHT // 2

RADIUS        = 13        # circle radius
THICKNESS     = 2         # circle line thickness
NOTCH_LENGTH  = 5         # length of notch
NOTCH_WIDTH   = 2         # angular width (pixels)

def empty_bitmap():
    return [[0 for _ in range(WIDTH)] for _ in range(HEIGHT)]

def draw_circle(img):
    for y in range(HEIGHT):
        for x in range(WIDTH):
            dx = x - CENTER_X + 0.5
            dy = y - CENTER_Y + 0.5
            d = math.sqrt(dx*dx + dy*dy)
            if RADIUS - THICKNESS <= d <= RADIUS + THICKNESS:
                img[y][x] = 1

def draw_notch(img, hour):
    angle = -math.pi / 2 + hour * (2 * math.pi / 12)
    for r in range(RADIUS, RADIUS + NOTCH_LENGTH):
        for w in range(-NOTCH_WIDTH, NOTCH_WIDTH + 1):
            x = int(CENTER_X + (r * math.cos(angle)) - w * math.sin(angle))
            y = int(CENTER_Y + (r * math.sin(angle)) + w * math.cos(angle))
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

# ---- Generate all 12 positions ----
bitmap_names = []

for hour in range(12):
    img = empty_bitmap()
    draw_circle(img)
    draw_notch(img, hour)

    data = to_ssd1306_bytes(img)
    name = f"bmpImageHour_{hour}"
    bitmap_names.append(name)

    emit_c_array(name, data)

# ---- Generate index array ----
print(f"const __code uint8_t *bmpImageList[12] = {{")
for name in bitmap_names:
    print(f"    {name},")
print("};")
