import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import MySQLdb as mdb
import paho.mqtt.client as mqtt

wss = []

db = mdb.connect(host="localhost", user="user", password="password", database="duoPet")
cursor = db.cursor()

mqtt_broker = "192.168.23.67"
mqtt_topic_detections = "duoPet/detections"
mqtt_topic_status= "duoPet/status"

class WSHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self):
        print('New user is connected.\n')
        if self not in wss:
            wss.append(self)

    def on_close(self):
        print('Connection closed\n')
        if self in wss:
            wss.remove(self)

def on_message(client, userdata, msg):
    data = msg.payload.decode('utf-8')
    
    # Check the MQTT topic and handle messages accordingly
    if msg.topic == mqtt_topic_detections:
        is_bowl_full, pet_type = data.split(',')
        save_to_database(is_bowl_full, pet_type)
    elif msg.topic == mqtt_topic_status:
        numbers = data.split(',')
        for client in wss:
            client.write_message(numbers)
      
    else:
        print(f"Unknown topic: {msg.topic}")

def save_to_database(is_bowl_full, pet_type):
    try:
        sql = "INSERT INTO petDetections (detection_time, pet_type, is_bowl_full) VALUES (NOW(), %s, %s)"
        values = (pet_type, is_bowl_full)
        cursor.execute(sql, values)
        db.commit()
        print("Data saved to database.")
    except Exception as e:
        db.rollback()
        print(f"Error saving data to database: {e}")

client = mqtt.Client()
client.on_message = on_message
client.connect(mqtt_broker, 1883, 60)
client.subscribe(mqtt_topic_detections)
client.subscribe(mqtt_topic_status)
client.loop_start()

application = tornado.web.Application([(r'/ws', WSHandler),])

if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(8888)

    main_loop = tornado.ioloop.IOLoop.instance()
    main_loop.start()
