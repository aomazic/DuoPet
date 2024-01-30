import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import MySQLdb as mdb
import paho.mqtt.client as mqtt
from datetime import datetime

wss = []

db = mdb.connect(host="localhost", user="user", password="password", database="duoPet")
cursor = db.cursor()

mqtt_broker = "192.168.23.67"
mqtt_topic_detections = "duoPet/detections"
mqtt_topic_status = "duoPet/status"
mqtt_topic_controler = "duoPet/controler"

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")

class WSHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        # Allow any origin to connect (you might want to make this more restrictive in production)
        return True

    def open(self):
        print('New user is connected.\n')
        if self not in wss:
            wss.append(self)

    def on_close(self):
        print('Connection closed\n')
        if self in wss:
            wss.remove(self)

    def on_message(self, message):
        try:
            publish_to_mqtt(mqtt_topic_controler, message)
        except Exception as e:
            print(f"Error processing WebSocket message: {e}")


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
        detection_time = datetime.now()
        sql = "INSERT INTO petDetections (detection_time, pet_type, is_bowl_full) VALUES (%s, %s, %s)"
        values = (detection_time, pet_type, is_bowl_full)
        cursor.execute(sql, values)
        db.commit()

        # Broadcast the data to all connected WebSocket clients
        for client in wss:
            client.write_message({
                "detection_time": detection_time.strftime("%Y-%m-%d %H:%M:%S"),
                "pet_type": pet_type,
                "is_bowl_full": is_bowl_full
            })

        print("Data saved to database and broadcasted to WebSocket clients.")
    except Exception as e:
        db.rollback()
        print(f"Error saving data to database: {e}")

def publish_to_mqtt(topic, message):
    client.publish(topic, message)

client = mqtt.Client()
client.on_message = on_message
client.connect(mqtt_broker, 1883, 60)
client.subscribe(mqtt_topic_detections)
client.subscribe(mqtt_topic_status)
client.loop_start()

application = tornado.web.Application([
    (r'/', MainHandler),
    (r'/ws', WSHandler),
])

if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(8888)

    main_loop = tornado.ioloop.IOLoop.instance()
    main_loop.start()
