import pyb, sensor, image, time, math
 
# ******************** SPI SEND INTERUPT ********************
spi = pyb.SPI(2, pyb.SPI.SLAVE, polarity=0, phase=0)
led_red = pyb.LED(1)
led_green = pyb.LED(2)
spi_list = [85, 0, 0, 0, 0, 0, 0, 0]
spi_data = bytearray(spi_list)
def nss_callback(line):
    global spi, spi_data
    try:
        spi.send(spi_data, timeout=1000)
        led_green.on()
        led_red.off()
        print(spi_data)
    except OSError as err:
        led_green.off()
        led_red.on()
        pass
 
pyb.ExtInt(pyb.Pin("P3"), pyb.ExtInt.IRQ_FALLING, pyb.Pin.PULL_UP, nss_callback)
 
# ******************** IMAGE DETECTION ********************
 
GRAYSCALE_THRESHOLD = [(0, 64, -28, 7, -9, 16)]
 
ROIS = [
        (0, 100, 160, 20, 0.7),
        (0,  50, 160, 20, 0.3),
        (0,   0, 160, 20, 0.1)
       ]
       
RED_THRESHOLD = [(24, 65, 23, 60, 0, 52)] 

RROIS = [
         (0, 100, 160, 20)
        ]
 
weight_sum = 0
 
for r in ROIS: weight_sum += r[4]
 
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()
 
while(True):
    # ***** IMAGE DETECTION CODE *****
    clock.tick()
    img = sensor.snapshot()
    centroid_sum = 0
    for r in ROIS:
        blobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi=r[0:4], merge=True)
        if blobs:
            largest_blob = max(blobs, key=lambda b: b.pixels())
            img.draw_rectangle(largest_blob.rect())
            img.draw_cross(largest_blob.cx(), largest_blob.cy())
            centroid_sum += largest_blob.cx() * r[4]
    center_pos = (centroid_sum / weight_sum) # Determine center of line.
    deflection_angle = 0
    deflection_angle = -math.atan((center_pos-80)/60)
    deflection_angle = math.degrees(deflection_angle)
    
    # Find red line
    for r in RROIS:
        redline = 0
        blobs = img.find_blobs(RED_THRESHOLD, roi=r[0:4], merge=True)
        if blobs:
            largest_blob = max(blobs, key=lambda b: b.pixels())
            img.draw_rectangle(largest_blob.rect(), color=(255, 0, 0))
            img.draw_cross(largest_blob.cx(), largest_blob.cy(), color=(255, 0, 0))
            redline = 1
            
            
    # ***** SET and SEND VALUES over SPI to ARDUINO *****
    spi_list[1]=int(deflection_angle)+100
    spi_list[2]=int(redline)
    spi_data = bytearray(spi_list)
    # print("Turn Angle: %f" % deflection_angle)
    # print("FPS: " + str(clock.fps()))
    print(spi_list)