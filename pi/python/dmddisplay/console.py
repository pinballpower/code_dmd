#
# Display DMD frames on console
#

# Translate bits to characters for different bits/pixel
# 4 bit is really only 3 bit
symbols = {
 "wpc": [" ","*"],
 "whitestar": [" ",".","o","O","*","?","?","?","?","?","?","?","?","?","?","?"],
 "spike": [" "," ",".",".",".","o","o","o","O","O","O","*","*","*","#","#"]
}

s=symbols["spike"];
def move_cursor (y, x):
    print("\033[%d;%dH" % (y, x))

def display_image(columns, rows, bitsperpixel, data):
    move_cursor(0,0)
    pixelmask=0xff>>(8-bitsperpixel)
    pixelindex=0
    pixelbit=8
    for _r in range(rows):
        rstr=""
        for _c in range(columns):
            pixelbit -= bitsperpixel
            if pixelbit < 0:
                pixelbit += 8
                pixelindex += 1
            d=data[pixelindex]
            pv = ((d >> pixelbit) & pixelmask)
            rstr+=s[pv]

        print(rstr)