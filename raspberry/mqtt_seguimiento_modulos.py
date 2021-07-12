#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import socket
import fcntl
import struct

import os
import base64
import threading
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

import mariadb

import datetime

raspberryCam = True
try:
	import picamera
except ImportError:
	raspberryCam = False

from lxml import html
import requests

modulos = []
mod_tip = []
clients = {}
city = None
picx = None

mqtt_ip = "192.168.1.79"
mqtt_port = 1883

def database(obtener, sql):
	try:
		with mariadb.connect(host="localhost",port=3306,user="phpmyadmin",password="pass",database="Domotica") as db: # login credentials
			cursor = db.cursor()
			cursor.execute(sql)
			
			if obtener == True:
				return cursor.fetchall()
			else:
				db.commit()
	except (mariadb.Error, mariadb.Warning) as e:
		print(f"DB FAIL: {e}")
		
def tiempo(t):
	print(f"Tiempo limite, actualizando... [{t}]")
	try:
		results = database(True, "SELECT ind,Val FROM Valor WHERE Tiempo=\'"+t+"\'")
		if results == None:
			return

		total = {}
		num = {}
		for row in results:
			ind = int(row[0])
			if not ind in total:
				total[ind] = 0
				num[ind] = 0
			total[ind] += row[1]
			num[ind] += 1
		database(False, "DELETE FROM Valor WHERE Tiempo=\'"+t+"\'")
		for indicador in total.keys():
			tmp = ''
			hora = datetime.datetime.now().minute
			total[indicador] /= num[indicador]
			if t == 's':
				tmp = 'm'
			elif t == 'm':
				tmp = 'h'
				hora = datetime.datetime.now().hour
			elif t == 'h':
				tmp = 'd'
				hora = datetime.datetime.now().day
			elif t == 'd':
				return
			ind = total[indicador]
			print(f"{str(indicador)}: {str(ind)}")
			
			database(False, "INSERT INTO Valor(ind,Tiempo,Time,Val) VALUES ("+str(indicador)+",\'"+str(tmp)+"\',"+str(hora)+","+str(ind)+")")
	except (mariadb.Error, mariadb.Warning) as e:
		print(f"DB load error: {e}")
	
def enviar(ID, tip, valu):
	try:
		database(False, "INSERT INTO Valor(ind,Tiempo,Time,Val) SELECT T.ind,'s',"+str(datetime.datetime.now().second)+","+str(valu)+" FROM Tipos as T WHERE T.ID='"+ID+"' AND T.Tipo='"+tip+"' AND T.RoA='r';")
	except (mariadb.Error, mariadb.Warning) as e:
		print(f"DB load error: {e}")

def cam():
	global client
	global clients
	global picx
	
	while True:		
		b64 = foto("/home/pi/Pictures/img.jpg")
		
		for cliente,n in clients.items():
			print(f"Enviando foto a {cliente}...")
			client.publish(cliente, "refresh."+b64)#"refresh."+f)
			n+=1
			if n>=10:
				clients.pop(cliente)
				print(f"Sesion de {cliente} expirada.")
				if len(clients)==0:
					picx.close()
			else:
				clients.update( {cliente : n} )

def regar(cliente):
	global client
	for x in range(4):
		client.publish(cliente,"me"+str(x+1));
	time.sleep(2*60)
	for x in range(4):
		client.publish(cliente,"ma"+str(x+1));

def on_connect(client, userdata, flags, rc):
	#print("Conectado. " + str(rc))
	client.subscribe("central")

def on_message(client, userdata, msg):
	global last
	global picx
	message = str(msg.payload)
	#print(msg.topic + ": " + message)
	msg = message.split(' ')
	print(msg)
	
	prefix = msg[0]
	tipo = msg[1]
	mensaje = ""
	if len(msg) >= 3:
		mensaje = msg[2]
	RoA = 'r'
	if tipo == "movimiento":
		RoA = 'a'
		if mensaje == "MOV":
			alarma()
	elif tipo == "alarma":
		RoA = 'a'
		if mensaje == "deact" or mensaje=="enr":
			results = database(True, "SELECT code FROM Alarm;")
			for row in results:
				if(msg[0]=="" or (msg[0]==str(row[0]))):
					if mensaje == "deact":
						client.publish(prefix, "deact")
						database(False, "UPDATE Alarm SET status=0 WHERE 1;")
					else:
						client.publish(prefix, "enr")
		elif mensaje == "act":
			database(False, "UPDATE Alarm SET status=1 WHERE 1;")
	elif tipo == "gas":
		results = database(True, "SELECT ind FROM Tipos WHERE ID=\""+prefix+"\";")
		for row in results:
			if mensaje=="1":
				RoA = 'a'
				gas(row2[0])
			else:
				results2 = database(True, "SELECT maxVal FROM Alarma WHERE ind="+str(row[0])+";")
				for row2 in results2:
					if mensaje>=row2[0]:
						gas(row2[0])
	elif tipo == "fuego":
		RoA = 'a'
		fuego()
	elif tipo == "btn":
		results = database(True, "SELECT Ejecutar FROM Botones WHERE Nombre=\""+mensaje+"\";")
		for row in results:
			print(f"Publicando '{row[0]}' a central...")
			client.publish("central", "btn " + str(row[0]))
		return
	elif tipo == "mag":
		RoA = 'a'
		if mensaje == "OPEN":
			alarma()
	elif tipo == "led":
		RoA = 'a'
		results = database(True, "SELECT Tiempo FROM Nombres WHERE ID=\""+prefix+"\";")
		for row in results:
			client.publish(prefix, "delay " + str(row[0]))
			print(f"Delay LED: {row[0]}")
	elif tipo == "enchufe":
		RoA = 'a'
		client.publish("central", "btn int "+prefix)
		client.publish("central", "btn int "+prefix)
	elif tipo == "int":
		try:
			results = database(True, "SELECT ind FROM Tipos WHERE ID='"+mensaje+"';")
			for row in results:
				results2 = database(True, "SELECT Status FROM Enchufes WHERE ind='"+str(row[0])+"';")
				status = True
				for row2 in results2:
					status = bool(row2[0])
					
				status = not status
				
				client.publish(mensaje, int(status))
				database(False, "UPDATE Enchufes SET Status="+str(int(status))+" WHERE ind="+str(row[0])+";")
		except:
			print("DB load error")
		return
	elif tipo == "reg":
		print(f"Regando {mensaje}...")
		threading.Thread(target=regar, args=(mensaje,)).start()
		return
	elif tipo == "humedad1" or tipo == "humedad2" or tipo == "humedad3" or tipo == "humedad4":
		results = database(True, "SELECT checkHumidity,humedad,checkRain,rangoCheck,probCheck FROM Riego WHERE ID='"+mensaje+"';")
		for row in results:
			if row[0] == True and (row[2] == False or clima(row[3], row[4]) == True):
				if row[1]>mensaje:
					client.publish(prefix, 'me'+tipo[7])
				else:
					client.publish(prefix, 'ma'+tipo[7])
		if not prefix in modulos:
			database(False, "INSERT INTO Riego(ID,checkRain,rangoCheck,probCheck,checkHumidity,humedad,forzarRiego,vecesPorDia,duracionRiego) VALUES (\""+prefix+"\",1,24,60,1,40,0,0,5);")
		tipo = "humedadPlanta"+tipo[7]
						
	elif tipo == "WEB":
		if mensaje == "foto" and picx != None:
			if len(clients)==0:
				picx = picamera.PiCamera()
			clients.update( {prefix : 0} )
			#print("Enviando foto a WEB...")
			#client.publish(prefix, "refresh."+last)
			#print("Listo!")
		return
	elif tipo == "?":
		retraso = 1
		try:
			results = database(True, "SELECT Tiempo FROM Nombres WHERE ID='"+prefix+"'")
			for row in results:
				retraso = row[0]
		except:
			print(f"DB load error")
		print(f"Retraso de {prefix}: {str(retraso)}s")
		client.publish(prefix, retraso)
		return
		
	if prefix == "btn":
		return
	
	if not prefix in modulos:
		print(f"Nuevo modulo ({prefix})")
		modulos.append(prefix)
		database(False, "INSERT INTO Nombres(ID,Nombre,Tiempo) VALUES (\""+prefix+"\",\""+prefix+"\",5);")
	
	if not (prefix+" "+tipo) in mod_tip:
		print(f"Nuevo tipo ({tipo}, de {prefix})")
		mod_tip.append(prefix+" "+tipo)
		try:
			results = database(True, "SELECT MAX(ind) FROM Tipos")
			val = results[0][0]
			if val is None:
				val = -1
			val += 1
			#print val
			database(False, "INSERT INTO Tipos(ind,ID,Tipo,RoA) VALUES ("+str(val)+",\""+prefix+"\",\""+tipo+"\",\""+RoA+"\");")
			if(tipo == "enchufe"):
				database(False, "INSERT INTO Enchufes(ind,Status) VALUES ("+str(val)+",1);")
			elif(tipo == "led"):
				database(False, "INSERT INTO LED(ind,Status,R,G,B,W) VALUES ("+str(val)+",\"simp\",0,0,0,0);")
			elif(tipo == "alarma"):
				database(False, "INSERT INTO Alarma(ind,maxVal,status) VALUES ("+str(val)+",600,0);")
		except (mariadb.Error, mariadb.Warning) as e:
			print(f"DB load error: {e}")
			
	if RoA == 'r':
		enviar(prefix, tipo, mensaje)

def gas(ind):
	database(False, "UPDATE Alarma SET gas=1 WHERE ind="+str(ind)+";")

def fuego():
	results = database(True, "SELECT ind FROM Tipos WHERE ID=\""+prefix+"\";")
	for row in results:
		database(False, "UPDATE Alarma SET fuego=1 WHERE ind="+str(row[0])+";")

def clima(rango, prob):
	page = requests.get(city.read(), timeout = 15.0)
	if page.status_code == 200:
		tree = html.fromstring(page.content)
		num = []
		
		bucle("0")
		bucle("1")
		
		for x in range(rango-1):
			num[x] = num[x][:-1]
			if num[x][0]!='<' and int(num[x]) >= prob:
				return True
		
		return False
		
def bucle(ind):
	new = ""
	x = 1
	no = False
	
	while no == False:
		new = insertar(ind, x)
		if new != "":
			num.append(new)
		else:
			no = True
		x+=1
		
def on_publish(mosq, obj, mid):
	print(f"mid: {str(mid)}")

def alarma():
	print("Alarma")
	
def foto(path):
	global picx
	
	if picx == None:
		return
	
	#picx.start_preview()
	picx.capture(path)
	#picx.stop_preview()
	#picx.close()
	with open(path, "rb") as image_file:
		return base64.b64encode(image_file.read())



if __name__ == '__main__':
	city = open("lluvia.txt","r")

	if raspberryCam:
		picx = picamera.PiCamera()

	print("Modulos encontrados:")
	try:
		results = database(True, "SELECT * FROM Nombres")
		for row in results:
			pre = row[0]
			print(f"{pre}: {row[1]}")
			modulos.append(pre)
	except:
		print("DB load error")
		
	print("Tipos:")
	try:
		results = database(True, "SELECT ID,Tipo FROM Tipos")
		for row in results:
			mod_tip.append(row[0]+" "+row[1])
			print(f"{row[0]}: {row[1]}")
	except:
		print("DB load error")

	client = mqtt.Client()
	client.on_connect = on_connect
	client.on_message = on_message
	client.connect(mqtt_ip, mqtt_port, 60)
	client.loop_start()

	thr = threading.Thread(target=cam)
	thr.daemon = True
	thr.start()

	while True:
		# TODO update with timer
		tiemp = datetime.datetime.now()
		if tiemp.second == 0:
			tiempo('s')
			if tiemp.minute == 0:
				tiempo('m')
				if tiemp.hour == 0:
					tiempo('h')
					if tiemp.day == 1:
						tiempo('d')
		
		time.sleep(1)
