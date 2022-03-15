from raylib import *

px_x=10
px_y=10
c_x=0
c_y=px_y/2

class RaylibDisplay():
    
    def __init__(self, width, height, pix_size, pix_spacing, palettesize, color=RED):
        self.pix_size=pix_size
            
        self.pix_rad=int(pix_size/2)
        self.pix_spacing=pix_spacing
        self.width=width
        self.height=height
        
        self.palette=[None]*palettesize
        for i in range(palettesize):
            alpha=(1/palettesize)*i
            self.palette[i]=Fade(color, alpha )
            
        
    def start(self):
        InitWindow(self.width, self.height, b"DMD display")
    
    def end(self):
        CloseWindow()


    def display_image(self, columns, rows, bitsperpixel, data):
        pixelmask=0xff>>(8-bitsperpixel)
        pixelindex=0
        pixelbit=8
        
        BeginDrawing()
        ClearBackground(RAYWHITE)
        
        c_y = self.pix_size/2+self.pix_spacing
         
        for _r in range(rows):
            c_x = self.pix_size/2+self.pix_spacing
             
            for _c in range(columns):
                pixelbit -= bitsperpixel
                if pixelbit < 0:
                    pixelbit += 8
                    pixelindex += 1
                d=data[pixelindex]
                pv = ((d >> pixelbit) & pixelmask)
                 
                DrawCircle(int(c_x), int(c_y), self.pix_rad, self.palette[pv])
                 
                c_x += self.pix_size+self.pix_spacing
                 
            c_y += self.pix_size+self.pix_spacing
                    
            
        EndDrawing()
        
