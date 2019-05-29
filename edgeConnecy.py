import paho.mqtt.client as mqtt
import requests
import json
import serial

ser = serial.Serial("/dev/ttyACM0", 9600)

password = ''

def on_message(client, user, msg):
	ser.flush()
	global employeeId, password
	topic = msg.topic
	m_decode = str(msg.payload.decode("utf-8", "ignore"))
	if topic == "/edge":
		print("ID:", m_decode)
		result = requests.get('https://i1i35qkgci.execute-api.us-east-2.amazonaws.com/Dev/employee/'+m_decode)
		employee = json.loads(result.text)
		print(employee)
		if str(employee['password']) == str(password):
			print('Password Confirm!')
			requests.get('https://i1i35qkgci.execute-api.us-east-2.amazonaws.com/Dev/employee/'+m_decode+'/status')
			requests.get('https://i1i35qkgci.execute-api.us-east-2.amazonaws.com/Dev/employee/'+m_decode+'/settime')
			ser.write('CONFIRM')
	if topic == "/edge/password":
		print("Password Arduino:", m_decode)
		password = m_decode

def readSerialInput():
    # TODO: Implement read serial input from arduino and return a number 
    Str = ser.readline() 
    if Str != "":
        valueStr = Str.split(';') # get end of each command
        value = valueStr[0].split(':') # separate arribute and value
        print(value)
        if len(value) == 2:
			if value[0] == 'Password':
				print(value[1])
				password = value[1]
				client.publish('/edge/password', password)
				client.publish('camera/checkID', "")

broker = "postman.cloudmqtt.com"

print("Connect to broker", broker)

client = mqtt.Client("Edge")

client.on_message = on_message

client.username_pw_set("din", "test123")
client.connect(broker, 17079)

client.loop_start()

client.subscribe("/edge")
client.subscribe("/edge/password")

while True:
    while ser.in_waiting:
        readSerialInput()

client.loop_stop()

client.disconnect()

