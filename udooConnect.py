import httplib, urllib
import time
import serial
import sys
import commands
import struct
import subprocess
import pickle
from ubidots import ApiClient
sleep = 10 #seconds between posts
DEBUG = False
#testVariabs
lightsOn = False
lightsOff = False
motorOn = False
startTime = time.time()
refTime = startTime
refTimeWat = startTime - 40
refTimePh = startTime - 28800
refTimeLight = startTime + 40
#endTestVariabs 
#Create serial device for reading serial
arduino = serial.Serial('/dev/ttymxc3',115200,timeout=0)
arduino.flushOutput()
arduino.flushInput()
serial_buffer = ""
line = ""
arduino_data = []
temp_str = '0'
temp = 0
attempts = 0
mark1 = 0
mark2 = 0
variabs = {"firstTime": True}
success = False
while success == False:

	try:
	    api = ApiClient("A1E-d5c762feef9d1508abb365575350c9e19c72")
	    print "connected api"
	    timeTest = time.time()
	    #testVar
	    lightHours = api.get_variable("59e81448c03f9716b74ccf34")
	    #print str(int(lightHours.get_values(1)[0]['value']))
	    errorLed = api.get_variable("59e81439c03f97177f01b200")
	    baseLevel = api.get_variable("59e81422c03f97163455e153")
	    acidLevel = api.get_variable("59e8142dc03f9715f9352616")
	    #time.sleep(0.5)
	    macroLevel = api.get_variable("59e8140bc03f9715f9352615")
	    microLevel = api.get_variable("59e81418c03f9716b74ccf31")
	    waterLevel = api.get_variable("59e81401c03f97163455e149")
	    ledHrs = api.get_variable("59e8145ac03f97177f01b207")
	    sunHrs = api.get_variable("59e81451c03f97177f01b206")
	    pH = api.get_variable("59e813ecc03f9716b74ccf2b")
	    timeTest1 = time.time()
	    print "time spent: "
	    print timeTest1 - timeTest
	    success = True
	
	except:
	    time.sleep(0.5)
	    pass
	    #print "Couldn't connect to the API, check your Internet connection"
	    #exit(0)

#recover variabs in case of shutdown
try:
	variabs = pickle.load( open( "save.p", "rb" ) )
	print variabs['firstTime']
except:
	print "no pickle file"


#init for arduino so Arduino knows it can send data.
#arduino.write("I\n")

def WriteArduino():
    global api, lightHours
    lightHours = api.get_variable("59e81448c03f9716b74ccf34")
    data = str(int(lightHours.get_values(1)[0]['value']))
    arduino.write(data)
    time.sleep(0.1)    

def ReadArduino(arduino_out):
#Sends Lighthours value then reads sensor values in the following order:
    global serial_buffer, line, api
    arduino_out = ""
    serial_data = arduino.read()                 # Read 1 character
    #print serial_data
    while serial_data:                             # If there is a character 
        serial_buffer += serial_data            # Add character to buffer
        if DEBUG: print "ReadArduino:Serial buffer:",(repr(serial_buffer))
        # if there is a new line in it, then create Line string
        if "\r\n" in serial_buffer:
            if DEBUG: print "ReadArduino:New Line:",(repr(serial_buffer))
            line = serial_buffer
            serial_buffer = "" # empty Serial buffer
        serial_data = arduino.read()
        # if there is a Line string then fin	ish reading the Line
        if line:
                # strip string for unwanted characters
                line = line.replace("\r\n", "")
                arduino_out = line
                if DEBUG: print "ReadArduino:arduino_out:", arduino_out
                line = ""
                if arduino_out == "": arduino_out = None
                # add the last character to the new buffer
                serial_buffer += serial_data
                return arduino_out
    

#Report Udoo Neo Arduino Sensor data to Thingspeak Channel
def writesensordata():
    global temp, errorLed, baseLevel, api, acidLevel, temp_str, lights
    global macroLevel, microLevel, waterLevel, ledHrs, sunHrs, ph
    
    while True:
        #Escribir a arduino para despertarlo y recibir datos
        #WriteArduino()
	lightHours = api.get_variable("59e81448c03f9716b74ccf34")
    	print "light Hours:"
	print str(int(lightHours.get_values(1)[0]['value']))
	 
        arduino.write('0')
	time.sleep(0.1)
        #Led Error
	temp_str = ReadArduino(temp_str)
	if temp_str is not None:
            temp_str = int(temp_str)
	    print "Error: "
	    print temp_str
            errorLed.save_value({'value':temp_str})	        	        
	#nivel Base
	temp_str = ReadArduino(temp_str)
        if temp_str is not None:
            temp_str = int(temp_str)
	    print "Base level:"
	    print temp_str
	    baseLevel.save_value({'value':temp_str})
	#nivel Acido
	temp_str = ReadArduino(temp_str)
        if temp_str is not None:
            temp_str = int(temp_str)
	    print "Acid level:"
	    print temp_str
	    acidLevel.save_value({'value':temp_str})	        	        
	#nivel Macro
	temp_str = ReadArduino(temp_str)
        if temp_str is not None:
            temp_str = int(temp_str)
	    print "Macro Nutrients level:"
	    print temp_str
	    macroLevel.save_value({'value':temp_str})	        	        
	#nivel Micro
	temp_str = ReadArduino(temp_str)
        if temp_str is not None:
            temp_str = int(temp_str)
	    print "Micro Nutrients level:"
	    print temp_str
	    microLevel.save_value({'value':temp_str})	        	        
	#nivel Agua
	temp_str = ReadArduino(temp_str)
        if temp_str is not None:
            temp_str = int(temp_str)
	    print "Water level:"
	    print temp_str
	    waterLevel.save_value({'value':temp_str})	        	        	        	        
	break
     
def lightTest():
	global api, lights, startTime, refTime, lightsOn
	#test LED

	print "Lights time Difference: "
	print time.time() - refTime
	print int(lights.get_values(1)[0]['value'])
        if time.time() - refTime > 20 and lightsOn == True :
			print "lights Off"
			refTime = time.time()
			lightsOn = False
			arduino.write('2')
			time.sleep(0.1)
        elif time.time() - refTime > 20 and lightsOn == False :
			print "lights On"
			refTime = time.time()
			lightsOn = True
			arduino.write('1')
			time.sleep(0.1)

def waterCycle():
	global api, startTime, refTimeWat, motorOn
	#test LED
	
	#print "Motor time Difference: "
	#print time.time() - refTimeWat
        if time.time() - refTimeWat > 20 and motorOn == True :
			print "motor Off"
			refTimeWat = time.time()
			motorOn = False
			arduino.write('4')
			time.sleep(0.1)
        elif time.time() - refTimeWat > 40 and motorOn == False :
			print "motor On"
			refTimeWat = time.time()
			motorOn = True
			arduino.write('3')
			time.sleep(0.1)
def checkPh():
	global api, refTimePh, startTime
	
	if time.time() - refTimePh > 28800:
		print "Checking pH"
		#Tell arduino to check ph
		#arduino.write('5')
		refTimePh = time.time()

def lightCycle():
	global api, startTime, refTimeLight, lightHours, lightsOn, lightsOff, mark1, mark2
	hours = 0
	attempts = 0
	while attempts < 5:
	    try:
		print "getting hours"
		hours = int(lightHours.get_values(1)[0]['value'])
		attempts = 5
	    except:
		print "cant get var"
		time.sleep(0.5)
		attempts += 1
	print hours
	lightHoursInSecs =  hours * 3600
	darkHoursInSecs = (24*3600)-lightHoursInSecs
	lightHoursInSecs = 40
	darkHoursInSecs = 20

	if lightsOn == False and lightsOff == False:
		refTimeLight = time.time()

	if time.time() - refTimeLight < lightHoursInSecs and lightsOn == False and lightsOff == False:
		#tell arduino to turn lights on
		arduino.write('1')
		print "lights turned on"
		refTimeLight = time.time()
		
		lightsOn = True 
	if time.time() - refTimeLight > lightHoursInSecs and lightsOn == True:
		print "lights turned off"
		#tell arduino to turn light off
		arduino.write('2')
		refTimeLight = time.time()
		mark2 = time.time()
		
		lightsOn = False
		lightsOff = True
	if time.time() - refTimeLight > darkHoursInSecs and lightsOn == False and lightsOff == True:
		#permite que el primer if se active luego de completar tiempo de descanso
		mark1 = time.time()
		print ("lights off for: ", mark1 - mark2)
		lightsOff = False
	
#sleep for desired amount of time
if __name__ == "__main__":
        while True:
                #writesensordata()
                #time.sleep(0.2)
		if variabs['firstTime'] == True:
			#revisa todos los niveles
			arduino.write('7')
			time.sleep(0.2)
			temp_str = ReadArduino(temp_str)
			        if temp_str is not None:
					
			#llama funciones de primera vez (inyectar macro y micro)
			#arduino.write('5')
			print "First Time"
			variabs['firstTime'] = False
			pickle.dump( variabs, open( "save.p", "wb" ) )
			time.sleep(1)
		checkPh()
		waterCycle()
		lightCycle()
                time.sleep(sleep)