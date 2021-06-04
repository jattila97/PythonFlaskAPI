from flask import Flask, redirect, url_for, render_template, jsonify, request, json
from flask_restful import Resource, Api
import mysql.connector

app = Flask(__name__)
api=Api(app)
@app.route("/")
def home():
    return render_template("home.html")

@app.route("/datas")
def user():
    return render_template("datas.html")

@app.route("/datas/Temperature")
def temp():
    mydb = mysql.connector.connect(
        host="mysql",
        user="root",
        password="hyperpassword",
        database="datas",
        port=3306
        )     
    mycursor = mydb.cursor()
    mycursor.execute("SELECT * FROM Temperature")
    TempTable = mycursor.fetchall()
    mycursor.execute("SELECT * FROM Humidity")
    HumTable = mycursor.fetchall()
    for x in HumTable:
        TempTable.append(x)
    reply = []
    for dataItem in TempTable:
        if dataItem[0] == "Temperature":
            reply.append(dataItem)

    values = []
    time = []
    for data in reply:
        values.append(float(data[1]))
        time.append(data[2])
    return render_template("temp.html", values=values, time = time)

@app.route("/datas/Humidity")
def hum():
    mydb = mysql.connector.connect(
        host="mysql",
        user="root",
        password="hyperpassword",
        database="datas",
        port=3306
        )     
    mycursor = mydb.cursor()
    mycursor.execute("SELECT * FROM Temperature")
    TempTable = mycursor.fetchall()
    mycursor.execute("SELECT * FROM Humidity")
    HumTable = mycursor.fetchall()
    for x in HumTable:
        TempTable.append(x)
    reply = []
    for dataItem in TempTable:
        if dataItem[0] == "Humidity":
            reply.append(dataItem)
    values = []
    time = []
    for data in reply:
        values.append(float(data[1]))
        time.append(data[2])
    return render_template("hum.html", values = values, time = time)

@app.route("/sendData", methods=['POST'])
def post():
    newData = request.get_json(silent=True)
    newDataToDB = {'type':newData['type'],'value':newData['value'], 'time':newData['time']}
    mydb = mysql.connector.connect(
        host="mysql",
        user="root",
        password="hyperpassword",
        database="datas",
        port=3306
        )
    mycursor = mydb.cursor()
    tableType=newData['type']
    sql = f"INSERT INTO {tableType} (type, value, time) VALUES (%s, %s, %s)"
    val=[(newData['type'],newData['value'],newData['time'])]
    mycursor.executemany(sql, val)
    mydb.commit()
    return newDataToDB, 201
    
class data(Resource):
    def get(self, type1):
        mydb = mysql.connector.connect(
            host="mysql",
            user="root",
            password="hyperpassword",
            database="datas",
            port=3306
            )     
        mycursor = mydb.cursor()
        mycursor.execute("SELECT * FROM Temperature")
        TempTable = mycursor.fetchall()
        mycursor.execute("SELECT * FROM Humidity")
        HumTable = mycursor.fetchall()
        for x in HumTable:
            TempTable.append(x)
        reply = []
        for dataItem in TempTable:
            if type1 == dataItem[0]:
                reply.append(dataItem)
        return reply, 200

api.add_resource(data, '/getData/<string:type1>')



app.run(host= '0.0.0.0',port=8091, debug=True)