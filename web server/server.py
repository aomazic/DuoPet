import signal
import tornado.ioloop
import tornado.web
import tornado.websocket
from tornado.ioloop import PeriodicCallback
import serial
import paho.mqtt.client as mqtt

# Define global variables for MQTT
mqtt_broker_address = "localhost"
mqtt_topic = "feeder_data"
db_host = "localhost"
db_user = "your_username"
db_password = "your_password"
db_database = "feeder_db"

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")  # Replace with the actual HTML filename

class FeederHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print("WebSocket opened")
        self.schedule_read_data()

    def on_close(self):
        print("WebSocket closed")
        self.stop_read_data()

    def schedule_read_data(self):
        # Schedule the periodic callback to read data from MQTT every 2 seconds
        self.callback = PeriodicCallback(self.read_data, 2000)
        self.callback.start()

    def read_data(self):
        try:
            # Connect to MQTT broker and subscribe to the topic
            client = mqtt.Client()
            client.connect(mqtt_broker_address, 1883, 60)
            client.subscribe(mqtt_topic)

            # Define callback for receiving messages from MQTT
            def on_message(client, userdata, msg):
                # Process the received message (update database, send to web page, etc.)
                save_to_database(data)
                data = msg.payload.decode("utf-8")
                self.write_message(f"Received data: {data}")

            client.on_message = on_message
            client.loop_start()  # Start the MQTT loop

        except Exception as e:
            print(f"Error reading data: {e}")

    def stop_read_data(self):
        if hasattr(self, 'callback') and self.callback:
            self.callback.stop()
            del self.callback

def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/ws", FeederHandler),
    ])

def stop_tornado(signum, frame):
    print("Stopping Tornado app...")
    tornado.ioloop.IOLoop.current().stop()

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print("Server is running on http://localhost:8888")

    # Register a signal handler to stop the Tornado app on SIGINT (Ctrl+C)
    signal.signal(signal.SIGINT, stop_tornado)

    tornado.ioloop.IOLoop.current().start()

def save_to_database(data):
    try:
        # Connect to MySQL database
        connection = pymysql.connect(host=db_host, user=db_user, password=db_password, database=db_database)
        with connection.cursor() as cursor:
            # Insert data into database (replace with your actual table structure)
            sql = "INSERT INTO feeder_data_table (data) VALUES (%s)"
            cursor.execute(sql, (data,))
        connection.commit()
    except Exception as e:
        print(f"Error saving to database: {e}")
    finally:
        connection.close()