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

class ConsoleDisplay():
    
    def __init__(self, characterset):
        self.chars = symbols[characterset]
        
    def start(self):
        pass
    
    def end(self):
        pass

    def move_cursor (self, y, x):
        print("\033[%d;%dH" % (y, x))
    
    def display_image(self, columns, rows, bitsperpixel, data):
        self.move_cursor(0,0)
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
                rstr+=self.chars[pv]
    
            print(rstr)