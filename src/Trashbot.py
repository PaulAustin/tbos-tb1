from microbit import *
import ustruct

#GPIO Pins
O4 = 0
O5 = 1
IO6 = 2
IO7 = 3
IO8 = 4
IO9 = 5

#GPIO Values
ModeValue = 0
WriteValue = 0
PullupValue = 0

#GPIO Pin Config
OUTPUT = 0
INPUT = 1
INPUT_PULLUP = 2


class tbos:
	
    def gyroReadReg(r):
        buf1 = bytearray([0, 0])
        buf1[0] = r
        i2c.write(0x6B, buf1)
        b = i2c.read(0x6B, 1)
        return b[0]

    def gyroReadReg16(r):
        buf2 = bytearray([0, 0])
        l = tbos.gyroReadReg(r)
        h = tbos.gyroReadReg(r + 1)
        buf2[0] = l
        buf2[1] = h
        t = ustruct.unpack_from('<h', buf2, 0)
        return t[0]
 
    def gyroWriteReg(r, v):
        buf2 = bytearray([0, 0])
        buf2[0] = r
        buf2[1] = v
        i2c.write(0x6B, buf2)

    def gyroGetX():
        return int(tbos.gyroReadReg16(0x28) / 114)

    def gyroGetY():
        return int(tbos.gyroReadReg16(0x2A) / 114)

    def gyroGetZ():
        return int(tbos.gyroReadReg16(0x2C) / 114)
        
    def gyroInit():
        # Initialize control registers 
        # on the L3GD20H
        tbos.gyroWriteReg(0x20, 0x0F)    
        tbos.gyroWriteReg(0x21, 0x00)    
        tbos.gyroWriteReg(0x22, 0x00)    
        tbos.gyroWriteReg(0x23, 0x00)    
        tbos.gyroWriteReg(0x24, 0x00)
        return tbos.gyroReadReg(0x0f)
		
		
	def setMotorPower(m, p):
        buf2 = bytearray([0, 0])
        if m == 1:
            buf2[0] = 10
        elif m == 2:
            buf2[0] = 20
        else:
            return  # no such motor
        if p > 100:
            p = 100
        if p < -100:
            p = -100
        buf2[1] = int(p)
        pin16.write_digital(0)
        spi.write(buf2)
        pin16.write_digital(1)
        return
	
    def MotorBrake(m):
        buf2 = bytearray([0, 0])
        if m == 1:
            buf2[0] = 12
        elif m == 2:
            buf2[0] = 22
        else:
            return  # no such motor
		
        buf2[1] = int(1)
        pin16.write_digital(0)
        spi.write(buf2)
        pin16.write_digital(1)
        return

    def playNote(k):
        buf2 = bytearray([0, 0])
		#kRM_NoteSolfege
        buf2[0] = 62
        if k < 0:
            k = 0
        if k > 88:
            k = 88
        buf2[1] = int(k)
        pin16.write_digital(0)
        spi.write(buf2)
        pin16.write_digital(1)
        return
        
	#Didn't seem to work
    def playFrequency(k):
        buf4 = bytearray([63, 116, 0, 0])
		#kRM_NoteHertz
        if k < 0:
            k = 0
        if k > 10000:
            k = 10000
            
        buf4[2] = (int(k) & 0x000000ff)    #low bit first now
        buf4[3] = (int(k) & 0x0000ff00) >> 8


        pin16.write_digital(0)
        spi.write(buf4)
        pin16.write_digital(1)
        return
        
    def setNoteLength(k):
        buf = bytearray([61,0])
        
        buf[1] = int(k)
        
        pin16.write_digital(0)
        spi.write(buf)
        pin16.write_digital(1)
        return
        
       ##Encoder1 Register: 30
       ##Encoder2 Register: 31
    def get8BitRegister(reg):
        bufRegWr = bytearray([0, 0, 0])
        bufRegRd = bytearray([0, 0, 0])
        bufRegWr[0] = -reg

        pin16.write_digital(0)
        spi.write_readinto(bufRegWr, bufRegRd)
        pin16.write_digital(1)
        
        r = bufRegRd[2];
        return r
               
    def get16BitRegister(reg):
        bufRegWr = bytearray([0, 116, 0, 0])
        bufRegRd = bytearray([0, 0, 0, 0])
        bufRegWr[0] = -reg

        pin16.write_digital(0)
        spi.write_readinto(bufRegWr, bufRegRd)
        pin16.write_digital(1)
        r = bufRegRd[2];
        r += bufRegRd[3]<<8;
        return r
        
    def get32BitRegister(reg):
        bufRegWr = bytearray([0, 117, 0, 0, 0, 0])
        bufRegRd = bytearray([0, 0, 0, 0, 0, 0])
        bufRegWr[0] = -reg

        pin16.write_digital(0)
        spi.write_readinto(bufRegWr, bufRegRd)
        pin16.write_digital(1)
        
        #r = bufRegRd[2];
        #r += bufRegRd[3]<<8;
        #r += bufRegRd[4]<<16;
        #r += bufRegRd[5]<<24;
        #return r
        t = ustruct.unpack_from('<i', bufRegRd, 2)
        return t[0]

    def setServoPosition(ServoNum, pos):
        buf = bytearray([50,116,0,0])
        if ServoNum == 1:
            buf[0] = 50
        elif ServoNum == 2:
            buf[0] = 51
        elif ServoNum == 3:
            buf[0] = 52
        else:
            return 0 #No such Servo
        
        buf[2] = (int(pos) & 0x000000ff)
        buf[3] = ((int(pos) & 0x0000ff00) >> 8)
        
        pin16.write_digital(0)
        spi.write(buf)
        pin16.write_digital(1)
        return

    def pinMode(pin, mode):
        global ModeValue
        global PullupValue
        buf = bytearray([73, 0])
        buf1 = bytearray([74, 0])
        
        #OUTPUT
        if(mode == 0):
            ModeValue |= int(bin(1 << pin),2)
        #INPUT
        elif mode == 1:
            ModeValue &= int(bin(~(1 << pin)),2)
            PullupValue &= int(bin(~(1 << pin)),2)
        #INPUT_PULLUP
        elif mode == 2:
            ModeValue &= int(bin(~(1 << pin)),2)
            PullupValue |= int(bin(1 << pin),2)
        else:
            return 0 #Invalid mode
                    
        buf[1] = ModeValue
        buf1[1] = PullupValue
        
		#Don't know if sending 2 buffers works
		#like this or not
        pin16.write_digital(0)
        spi.write(buf)
		spi.write(buf1)
        pin16.write_digital(1)
        return
        
    def digitalWrite(pin, value):
        global WriteValue
        buf = bytearray([72, 0])
        if value == 0:
            WriteValue  &= int(bin(~(1 << pin)), 2)
        elif value == 1:
            WriteValue  |= int(bin((1 << pin)), 2)
        else:
            return 0 #Invalid value
            
        buf[1] = WriteValue
        
        pin16.write_digital(0)
        spi.write(buf)
        pin16.write_digital(1)
        return
        
    def digitalRead(pin):
        byte = tbos.get8BitRegister(71)
        bit = 1 & (byte >> pin)
        return bit


############ Add your code here.##############
spi.init()
while True:
	
