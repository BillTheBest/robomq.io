# One to One (Direct)

For one to one messaging, a producer sends messages to specified queue.  A consumer receives messages from that queue.  To ensure message is not lost, message acknowledgments can be sent back to producer to confirm a particular message has been received.

![Diagram of One to One messaging](./images/one-one.png)

> Browse the chapter of AMQP Introduction first if you're new to AMQP.  

----------

## Python

###Prerequisites

**Python client AMQP library**

The Python library we use for this example can be found at <a href="https://github.com/pika/pika" target="_blank">https://github.com/pika/pika</a>.  

You can install it through `sudo pip install pika`.  

Finally, import this library in your program.

```python
import pika
```

The full documentation of this library is at <a href="https://pika.readthedocs.org/en/0.9.14/" target="_blank">https://pika.readthedocs.org/en/0.9.14/</a>.

> pika library is not thread safe. Do not use a connection or channel across threads.

###Producer
The first thing we need to do is to establish a connection with <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  
Set heartbeat to 60 seconds, so that client will confirm the connectivity with broker.  

```python
credentials = pika.PlainCredentials(username, password)
connection = pika.BlockingConnection(pika.ConnectionParameters(host = server, port = port, virtual_host = vhost, credentials = credentials, heartbeat_interval = 60))
channel = connection.channel()
```

Then producer can publish messages to the default exchange where queue name itself is the routing key.  
It will assign a blank string to exchange parameter in publish function to use the default exchange.  
Delivery mode = 1 means it's a non-persistent message.

```python 
properties = pika.spec.BasicProperties(content_type = "text/plain", delivery_mode = 1)
channel.basic_publish(exchange = "", routing_key = routingKey, body = "Hello World!", properties = properties)
```

At last, producer will disconnect with the <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

```python
connection.close()
```

###Consumer
The same as producer, consumer needs to first connect to <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

Then consumer will declare a queue. By default, the queue will be bound to the default exchange with the same binding key as its name.  
Auto-delete means after all consumers have finished consuming it, the queue will be deleted by broker.  

```python
channel.queue_declare(queue = queueName, auto_delete = True)
```

Finally, consumer can consume messages from the queue.  
The `no_ack` parameter indicates if consumer needs to explicitly send acknowledgment back to broker when it has received the message. In this example, `no_ack` equals to true, so producer does not explicitly acknowledge received messages.  
The `start_consuming()` function will be blocking the process until `stop_consuming()` is invoked or exception happens.  

```python
channel.basic_consume(consumer_callback = onMessage, queue = queueName, no_ack = True)
channel.start_consuming()
```

When messages are received, a callback function `onMessage()` will be invoked to print the message content.  

```python
def onMessage(channel, method, properties, body):
	print body
```

###Putting it all together

**producer.py**

```python 
import pika
	
server = "hostname"
port = 5672
vhost = "yourvhost"
username = "username"
password = "password"
routingKey = "testQ"
	
try:
	#connect
	credentials = pika.PlainCredentials(username, password)
	connection = pika.BlockingConnection(pika.ConnectionParameters(host = server, port = port, virtual_host = vhost, credentials = credentials, heartbeat_interval = 60))
	channel = connection.channel()
	
	#send message
	#assigning blank string to exchange is to use the default exchange, where queue name is the routing key
	properties = pika.spec.BasicProperties(content_type = "text/plain", delivery_mode = 1)
	channel.basic_publish(exchange = "", routing_key = routingKey, body = "Hello World!", properties = properties)
	
	#disconnect
	connection.close()
except Exception, e:
	print e
```

**consumer.py**

```python
import pika
import time
	
server = "hostname"
port = 5672
vhost = "yourvhost"
username = "username"
password = "password"
queueName = "testQ"
	
#callback funtion on receiving messages
def onMessage(channel, method, properties, body):
	print body
	
while True:
	try:
		#connect
		credentials = pika.PlainCredentials(username, password)
		connection = pika.BlockingConnection(pika.ConnectionParameters(host = server, port = port, virtual_host = vhost, credentials = credentials, heartbeat_interval = 60))
		channel = connection.channel()
	
		#declare queue and consume messages
		#one-to-one messaging uses the default exchange, where queue name is the routing key
		channel.queue_declare(queue = queueName, auto_delete = True)
		channel.basic_consume(consumer_callback = onMessage, queue = queueName, no_ack = True)
		channel.start_consuming()
	except Exception, e:
		#reconnect on exception
		print "Exception handled, reconnecting...\nDetail:\n%s" % e
		try:
			connection.close()
		except:
			pass
		time.sleep(5)
```
## Node.js

###Prerequisites

**Node.js client AMQP library**

The Node.js library we use for this example can be found at <a href="https://github.com/squaremo/amqp.node" target="_blank">https://github.com/squaremo/amqp.node</a>.    

You can install the library through `sudo npm install amqplib`.  

Finally, require this library in your program.

```javascript
var amqp = require("amqplib");
```
The full documentation of this library is at <a href="https://www.squaremobius.net/amqp.node/doc/channel_api.html" target="_blank">https://www.squaremobius.net/amqp.node/doc/channel_api.html</a>.

###Producer
The first thing we need to do is to establish a connection with <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  
Set heartbeat to 60 seconds, so that client will confirm the connectivity with broker.  
As shown in the code, this library provides chainable callback API in the form of `.then(callback)`.  
> For the default vhost "/", you will need to insert "%2f" (its hexadecimal ASCII code) to the AMQP URI, instead of "/" itself.  

```javascript
producer = amqp.connect("amqp://" + username + ":" + password + "@" + server + ":" + port + "/" + vhost + "?heartbeat=60");
producer.then(function(conn) {
	return conn.createConfirmChannel().then(successCallback);
}).then(null, failureCallback);
```

Then producer can publish messages to the default exchange where queue name itself is the routing key.  
It will assign a blank string to exchange parameter in publish function to use the default exchange.  
Delivery mode = 1 means it's a non-persistent message.

```javascript
ch.publish("", routingKey, content = new Buffer("Hello World!"), options = {contentType: "text/plain", deliveryMode: 1},  callback);
```

At last, producer will disconnect with the <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

```javascript
conn.close();
```

###Consumer
The same as producer, consumer needs to first connect to <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  
The difference is that consumer uses `conn.createChannel()` function, while producer uses `conn.createConfirmChannel()` because the latter one is only useful for publish confirm.  

Then consumer will declare a queue. By default, the queue will be bound to the default exchange with the same binding key as its name.  
Durable means the queue will survive possible broker failover. It's false in this example.  
Auto-delete means after all consumers have finished consuming it, the queue will be deleted by broker.  
Exclusive means no other consumer can consume the queue when this one is consuming it. It's false in this example.  

```javascript
ch.assertQueue(queueName, {durable: false, autoDelete: true, exclusive: false});
```

Finally, consumer can consume messages from the queue.  
The `noAck` option indicates if consumer needs to explicitly send acknowledgment back to broker when it has received the message. In this example, `noAck` is true, so producer does not explicitly acknowledge received messages.  
The second parameter of `consume()` function is the callback on receiving messages. In this example, when messages are received, the callback function will be invoked to print the message content.  

```javascript
ch.consume(queueName, function(message) {
	console.log(message.content.toString());
}, {noAck: true});  
```

###Putting it all together

**producer.js**

```javascript
var amqp = require("amqplib");
	
var server = "hostname";
var port = "5672";
var vhost = "yourvhost"; //for "/" vhost, use "%2f" instead
var username = "username";
var password = "password";
var routingKey = "testQ";
	
producer = amqp.connect("amqp://" + username + ":" + password + "@" + server + ":" + port + "/" + vhost + "?heartbeat=60");
producer.then(function(conn) {
	return conn.createConfirmChannel().then(function(ch) {
		//assigning blank string to exchange is to use the default exchange, where queue name is the routing key
		ch.publish("", routingKey, content = new Buffer("Hello World!"), options = {contentType: "text/plain", deliveryMode: 1}, function(err, ok) {
			if (err != null) {
				console.error("Error: failed to send message\n" + err);
			}
			conn.close();
		});
	});
}).then(null, function(err) {
	console.error(err);
});
```

**consumer.js**

```javascript
var amqp = require("amqplib");
var domain = require("domain");
	
var server = "hostname";
var port = "5672";
var vhost = "yourvhost"; //for "/" vhost, use "%2f" instead
var username = "username";
var password = "password";
var queueName = "testQ";
	
//use domain module to handle reconnecting
var consumer = null;
var dom = domain.create();
dom.on("error", relisten);
dom.run(listen);
	
function listen() {
	consumer = amqp.connect("amqp://" + username + ":" + password + "@" + server + ":" + port + "/" + vhost + "?heartbeat=60");
	consumer.then(function(conn) {
		return conn.createChannel().then(function(ch) {
			//one-to-one messaging uses the default exchange, where queue name is the routing key
			ch.assertQueue(queueName, {durable: false, autoDelete: true, exclusive: false});
			ch.consume(queueName, function(message) {
				//callback funtion on receiving messages
				console.log(message.content.toString());
			}, {noAck: true});
		});
	}).then(null, function(err) {
		console.error("Exception handled, reconnecting...\nDetail:\n" + err);
		setTimeout(listen, 5000);
	});
}
	
function relisten() {
	consumer.then(function(conn) {
		conn.close();
	});	
	setTimeout(listen, 5000);
}
```

## PHP

### Prerequisite

**PHP client AMQP library**

The PHP library we use for this example can be found at <a href="https://github.com/videlalvaro/php-amqplib" target="_blank">https://github.com/videlalvaro/php-amqplib</a>.  

It uses composer to install in a few steps.  

1. Add a `composer.json` file to your project:

```json
{
	"require": {
		"videlalvaro/php-amqplib": "2.2.*"
	}
}
```

2. Download the latest composer in the same path:

```bash
curl -sS https://getcomposer.org/installer | php
```

3. Install the library through composer:

```bash
./composer.phar install
```

Finally, require this library in your program and use the classes.

```php
require_once __DIR__ . '/../vendor/autoload.php'; //directory of library folder
use PhpAmqpLib\Connection\AMQPConnection;
use PhpAmqpLib\Message\AMQPMessage;
```

###Producer
The first thing we need to do is to establish a connection with <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  
Set heartbeat to 60 seconds, so that client will confirm the connectivity with broker.  

```php
$connection = new AMQPConnection($server, $port, $username, $password, $vhost, $heartbeat = 60);
$channel =  $connection->channel();	
```

Then producer can publish messages to the default exchange where queue name itself is the routing key.  
It will assign a blank string to exchange parameter in publish function to use the default exchange.  
Delivery mode = 1 means it's a non-persistent message.

```php
$message = new AMQPMessage("Hello World!", array("content_type" => "text/plain", "delivery_mode" => 1));
$channel->basic_publish($message, $exchange = "", $routingKey);
```

At last, producer will disconnect with the <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

```php
$connection->close();
```

###Consumer
The same as producer, consumer needs to first connect to <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

Then consumer will declare a queue. By default, the queue will be bound to the default exchange with the same binding key as its name.  
Auto-delete means after all consumers have finished consuming it, the queue will be deleted by broker.  

```php
$channel->queue_declare($queueName, false, false, false, $auto_delete = true);
```

Finally, consumer can consume messages from the queue.  
The `no_ack` parameter indicates if consumer needs to explicitly send acknowledgment back to broker when it has received the message. In this example, `no_ack` equals to true, so producer does not explicitly acknowledge received messages.  
The while loop will be blocking the process and listening for messages until exception happens.  

```php
$channel->basic_consume($queueName, "", false, $no_ack = true, false, false, $callback = $onMessage);

while(count($channel->callbacks)) {
	$channel->wait();
}

When messages are received, a callback function will be invoked to print the message content.  

$onMessage = function ($message) {
	echo $message->body.PHP_EOL;
};
```

### Putting it together

**producer.php**

```php
<?php
require_once __DIR__ . '/../vendor/autoload.php'; //directory of library folder
use PhpAmqpLib\Connection\AMQPConnection;
use PhpAmqpLib\Message\AMQPMessage;
	
$server = "hostname";
$port = 5672;
$vhost = "yourvhost";
$username = "username";
$password = "password";
$routingKey = "testQ";
	
try {
	//connect
	$connection = new AMQPConnection($server, $port, $username, $password, $vhost, $heartbeat = 60);
	$channel =  $connection->channel();	
	
	//send message
	//assigning blank string to exchange is to use the default exchange, where queue name is the routing key
	$message = new AMQPMessage("Hello World!", array("content_type" => "text/plain", "delivery_mode" => 1));
	$channel->basic_publish($message, $exchange = "", $routingKey);
	
	//disconnect
	$connection->close();
} catch(Exception $e) {
	echo $e.PHP_EOL;
}
?>
```

**consumer.php**

```php
<?php	
require_once __DIR__."/../vendor/autoload.php"; //directory of library folder
use PhpAmqpLib\Connection\AMQPConnection;
	
$server = "hostname";
$port = 5672;
$vhost = "yourvhost";
$username = "username";
$password = "password";
$queueName = "testQ";
	
//callback funtion on receiving messages
$onMessage = function ($message) {
	echo $message->body.PHP_EOL;
};
	
while (true) {
	try {
		//connect
		$connection = new AMQPConnection($server, $port, $username, $password, $vhost, $heartbeat = 60);
		$channel = $connection->channel();
	
		//declare queue and consume messages
		//one-to-one messaging uses the default exchange, where queue name is the routing key
		$channel->queue_declare($queueName, false, false, false, $auto_delete = true);
		$channel->basic_consume($queueName, "", false, $no_ack = true, false, false, $callback = $onMessage);
	
		//start consuming
		while(count($channel->callbacks)) {
			$channel->wait();
		}
	} catch(Exception $e) {
		//reconnect on exception
		echo "Exception handled, reconnecting...\nDetail:\n".$e.PHP_EOL;
		if ($connection != null) {
			try {
				$connection->close();
			} catch (Exception $e1) {}
		}
		sleep(5);
	}
}
?>
```

## Ruby

###Prerequisites

**Ruby client AMQP library**

The Ruby library we use for this example can be found at <a href="http://rubybunny.info/" target="_blank">http://rubybunny.info/</a>.  

With Ruby version >= 2.0, you can install it through `sudo gem install bunny`.  

Finally, import this library in your program.  

```ruby
require "bunny"
```

The full documentation of this library is at <a href="http://rubybunny.info/articles/guides.html" target="_blank">http://rubybunny.info/articles/guides.html</a>.

> We recommend combining the documentation with the source code of this library when you use it because some of the documentation out there is not being updated timely from our observation.  

###Producer
The first thing we need to do is to establish a connection with <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  
Set heartbeat to 60 seconds, so that client will confirm the connectivity with broker.  
Although the library provides a connection property named `recover_from_connection_close`, we discourage you to use it. The reason will be explained in the Consumer section.  

```ruby
connection = Bunny.new(:host => server, :port => port, :vhost => vhost, :user => username, :pass => password, :heartbeat => 60, :recover_from_connection_close => false)
connection.start
channel = connection.create_channel
```

Then producer can publish messages to the default exchange where queue name itself is the routing key.  
Delivery mode = 1 means it's a non-persistent message.

```ruby 
exchange = channel.default_exchange
exchange.publish("Hello World!", :routing_key => routingKey, :content_type => "text/plain", :delivery_mode => 1)
```

At last, producer will disconnect with the <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

```ruby
connection.close
```

###Consumer
The same as producer, consumer needs to first connect to <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

Then consumer will declare a queue. By default, the queue will be bound to the default exchange with the same binding key as its name.  
Auto-delete means after all consumers have finished consuming it, the queue will be deleted by broker.  

```ruby
queue = channel.queue(queueName, :auto_delete => true)
```

After that, consumer can consume messages from the queue.  
The `manual_ack` parameter indicates if consumer needs to manually send acknowledgment back to broker when it has received the message. In this example, `manual_ack` equals to false, so producer does not manually acknowledge received messages.  
The `subscribe()` function is followed by a callback which will be invoked to print the message payload on receiving a message.  

```ruby
queue.subscribe(:block => false, :manual_ack => false) do |delivery_info, metadata, payload|
  puts payload
end
```

As we mentioned in the Producer section, `recover_from_connection_close` is set to false when connecting to <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker. It matters for consumers because `recover_from_connection_close` will only recover the connection, it won't recreate exchange and queue in case they are gone. Therefore, a more robust approach is  letting your code handle reconnecting on its own and keep checking the existence of the subscribed queue.  

```ruby
while true
  raise "Lost the subscribed queue %s" % queueName unless connection.queue_exists?(queueName)
  sleep 1
end
```

###Putting it all together

**producer.rb**

```ruby 
require "bunny"

server = "hostname"
port = 5672
vhost = "yourvhost"
username = "username"
password = "password"
routingKey = "testQ"

begin
  #connect
  connection = Bunny.new(:host => server, :port => port, :vhost => vhost, :user => username, :pass => password, :heartbeat => 60, :recover_from_connection_close => false)
  connection.start
  channel = connection.create_channel

  #send message
  #assigning blank string to exchange is to use the default exchange, where queue name is the routing key
  exchange = channel.default_exchange
  exchange.publish("Hello World!", :routing_key => routingKey, :content_type => "text/plain", :delivery_mode => 1)

  #disconnect
  connection.close
rescue Exception => e
  puts e
end
```

**consumer.rb**

```ruby
require "bunny"

server = "hostname"
port = 5672
vhost = "yourvhost"
username = "username"
password = "password"
queueName = "testQ"

while true
  begin
    #connect, disable auto-reconnect so as to manually reconnect
    connection = Bunny.new(:host => server, :port => port, :vhost => vhost, :user => username, :pass => password, :heartbeat => 60, :recover_from_connection_close => false)
    connection.start
    channel = connection.create_channel

    #declare queue and consume messages
    #one-to-one messaging uses the default exchange, where queue name is the routing key
    queue = channel.queue(queueName, :auto_delete => true)
    queue.subscribe(:block => false, :manual_ack => false) do |delivery_info, metadata, payload|
      puts payload
    end
    #keep checking the existence of the subscribed queue
    while true
      raise "Lost the subscribed queue %s" % queueName unless connection.queue_exists?(queueName)
      sleep 1
    end
  rescue Exception => e
    #reconnect on exception
    puts "Exception handled, reconnecting...\nDetail:\n%s" % e
    #blindly clean old connection
    begin
      connection.close
    end
    sleep 5
  end
end
```

## Java

###Prerequisites

**Java client AMQP library**

The Java library we use for this example can be found at <a href="https://www.rabbitmq.com/java-client.html" target="_blank">https://www.rabbitmq.com/java-client.html</a>.  

Download the library jar file, then import this library in your program `import com.rabbitmq.client.*;` and compile your source code with the jar file. For example,  

```bash
javac -cp ".:./rabbitmq-client.jar" Producer.java Consumer.java 
```

Run the producer and consumer classes. For example,  

```bash
java -cp ".:./rabbitmq-client.jar" Consumer
java -cp ".:./rabbitmq-client.jar" Producer
```

Of course, you can eventually compress your producer and consumer classes into jar files.

###Producer
The first thing we need to do is to establish a connection with <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  
Set heartbeat to 60 seconds, so that client will confirm the connectivity with broker.  

```java
ConnectionFactory factory = new ConnectionFactory();
factory.setHost(server);
factory.setPort(port);
factory.setVirtualHost(vhost);
factory.setUsername(username);
factory.setPassword(password);
factory.setRequestedHeartbeat(60);
connection = factory.newConnection();
channel = connection.createChannel();
```

Then producer can publish messages to the default exchange where queue name itself is the routing key.  
It will assign a blank string to exchange parameter in publish function to use the default exchange.  

```java 
String message = "Hello World!";
channel.basicPublish("", routingKey, MessageProperties.TEXT_PLAIN, message.getBytes());
```

At last, producer will disconnect with the <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

```java
connection.close();
```

###Consumer
The same as producer, consumer needs to first connect to <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

Then consumer will declare a queue. By default, the queue will be bound to the default exchange with the same binding key as its name.  
The fourth parameter auto-delete is true. That means after all consumers have finished consuming it, the queue will be deleted by broker.  

```java
channel.queueDeclare(queueName, false, false, true, null);
```

Finally, consumer can consume messages from the queue.  
The second parameter of `basicConsume()` function no-ack indicates if consumer needs to explicitly send acknowledgment back to broker when it has received the message. In this example, no-ack equals to true, so producer does not explicitly acknowledge received messages.  
The while loop will be blocking the process and listening for messages until exception happens. When messages are received, it will print the message content.  

```java
QueueingConsumer qc = new QueueingConsumer(channel);
channel.basicConsume(queueName, true, qc);
while (true) {
	QueueingConsumer.Delivery delivery = qc.nextDelivery();
	String message = new String(delivery.getBody());
	System.out.println(message);
}
```

###Putting it all together

**Producer.java**

```java
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.MessageProperties;
	
public class Producer {
	
	private Connection connection;
	private Channel channel;
	private static String server = "hostname";
	private static int port = 5672;
	private static String vhost = "yourvhost";
	private static String username = "username";
	private static String password = "password";
	private static String routingKey = "testQ";
	
	private void produce() {
		try {
			//connect
			ConnectionFactory factory = new ConnectionFactory();
			factory.setHost(server);
			factory.setPort(port);
			factory.setVirtualHost(vhost);
			factory.setUsername(username);
			factory.setPassword(password);
			factory.setRequestedHeartbeat(60);
			connection = factory.newConnection();
			channel = connection.createChannel();
	
			//send message
			String message = "Hello World!";
			//assigning blank string to exchange is to use the default exchange, where queue name is the routing key
			channel.basicPublish("", routingKey, MessageProperties.TEXT_PLAIN, message.getBytes());
	
			//disconnect
			connection.close();
		} catch(Exception e) {
			System.out.println(e);
			System.exit(-1);			
		}	
	}
	
	public static void main(String[] args) {
		Producer p = new Producer();
		p.produce();
	}
}
```

**Consumer.java**

```java
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.QueueingConsumer;
	
public class Consumer {
	
	private Connection connection;
	private Channel channel;
	private static String server = "hostname";
	private static int port = 5672;
	private static String vhost = "yourvhost";
	private static String username = "username";
	private static String password = "password";
	private static String queueName = "testQ";
	
	private void consume() {
		while (true) {
			try {
				//connect
				ConnectionFactory factory = new ConnectionFactory();
				factory.setHost(server);
				factory.setPort(port);
				factory.setVirtualHost(vhost);
				factory.setUsername(username);
				factory.setPassword(password);
				factory.setRequestedHeartbeat(60);
				connection = factory.newConnection();
				channel = connection.createChannel();
			
				//declare queue and consume messages
				//one-to-one messaging uses the default exchange, where queue name is the routing key
				channel.queueDeclare(queueName, false, false, true, null);
				QueueingConsumer qc = new QueueingConsumer(channel);
				channel.basicConsume(queueName, true, qc);
				while (true) {
					QueueingConsumer.Delivery delivery = qc.nextDelivery();
					String message = new String(delivery.getBody());
					System.out.println(message);
				}
			} catch(Exception e) {
				//reconnect on exception
				System.out.printf("Exception handled, reconnecting...\nDetail:\n%s\n", e);
				try {
					connection.close();
				} catch (Exception e1) {}
				try {
					Thread.sleep(5000); 
				} catch(Exception e2) {}
			}
		}
	}
	
	public static void main(String[] args) {
		Consumer c = new Consumer();
		c.consume();
	}
}
```

## Go

###Prerequisites

**Go client AMQP library**

The Go library we use for this example can be found at <a href="https://github.com/streadway/amqp" target="_blank">https://github.com/streadway/amqp</a>.  

You can install it through `go get github.com/streadway/amqp`.  

Finally, import this library in your program.  

```go
import "github.com/streadway/amqp"
```

The full documentation of this library is at <a href="https://godoc.org/github.com/streadway/amqp" target="_blank">https://godoc.org/github.com/streadway/amqp</a>.  

###Producer
The first thing we need to do is to establish a connection with <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  
Set heartbeat to 60 seconds, so that client will confirm the connectivity with broker.  

```go
connection, err := amqp.DialConfig(fmt.Sprintf("amqp://%s:%s@%s:%d/%s", username, password, server, port, vhost), amqp.Config{Heartbeat: 60 * time.Second})
channel, err := connection.Channel()
```

Then producer can publish messages to the default exchange where queue name itself is the routing key.  
It will assign a blank string to exchange parameter in publish function to use the default exchange.  
Delivery mode = 1 means it's a non-persistent message.  

```go 
err = channel.Publish("", routingKey, false, false, amqp.Publishing{ContentType:  "text/plain", DeliveryMode: 1, Body: []byte("Hello World!")})
```

At last, producer will disconnect with the <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

```go
connection.Close()
```

###Consumer
The same as producer, consumer needs to first connect to <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> broker.  

Then consumer will declare a queue. By default, the queue will be bound to the default exchange with the same binding key as its name.  
Durable means the exchange or queue will survive possible broker failover. It's false in this example.  
Auto-delete means after all consumers have finished consuming it, the exchange or queue will be deleted by broker.  
Exclusive means no other consumer can consume the queue when this one is consuming it.  

```go
// durable = false; auto-delete = true; exclusive = false
queue, err := channel.QueueDeclare(queueName, false, true, false, false, nil)
```

Finally, consumer can consume messages from the queue.  
Consumer-tag can be later used to `Cancel()` this consumer when it's no longer needed.  
Auto-ack parameter indicates if consumer needs to explicitly send acknowledgment back to broker when it has received the message. In this example, auto-ack equals to true, so producer does not explicitly acknowledge received messages.  

```go
// consumer-tag = "consumer"; auto-ack = true
messageChan, err := channel.Consume(queue.Name, "consumer", true, false, false, false, nil)
```

Note a message channel is returned by the `Consume()` function. Incoming messages will be received through that channel.  

> Channel in Golang is a typed conduit through which you can send and receive values. Sends and receives block until the other side is ready.  

```go
for message := range messageChan {
	fmt.Println(string(message.Body))
}
```

###Putting it all together

**producer.go**

```go
package main

import (
	"fmt"
	"github.com/streadway/amqp"
	"os"
	"time"
)

var server = "hostname"
var port = 5672
var vhost = "yourvhost"
var username = "username"
var password = "password"
var routingKey = "testQ"

func main() {
	connection, err := amqp.DialConfig(fmt.Sprintf("amqp://%s:%s@%s:%d/%s", username, password, server, port, vhost),
		amqp.Config{Heartbeat: 60 * time.Second})
	if err != nil {
		fmt.Printf("Failed to connect, err: %v\n", err)
		os.Exit(1)
	}
	defer connection.Close()

	channel, err := connection.Channel()
	if err != nil {
		fmt.Printf("Failed to create channel, err: %v\n", err)
		os.Exit(1)
	}
	defer channel.Close()

	err = channel.Publish(
		// assigning blank string to exchange is to use the default exchange, where queue name is the routing key
		"",         // exchange
		routingKey, // routing key
		false,      // mandatory
		false,      // immediate
		amqp.Publishing{
			ContentType:  "text/plain",
			DeliveryMode: 1,
			Body:         []byte("Hello World!"),
		})
	if err != nil {
		fmt.Printf("Failed to publish message, err: %v\n", err)
		os.Exit(1)
	}
}
```

**consumer.go**

```go
package main

import (
	"fmt"
	"github.com/streadway/amqp"
	"time"
)

var server = "hostname"
var port = 5672
var vhost = "yourvhost"
var username = "username"
var password = "password"
var queueName = "testQ"

func main() {
	// Infinite loop to auto-reconnect on failure
Loop:
	for {
		fmt.Println("Starting in 5 seconds...")
		time.Sleep(5 * time.Second)

		connection, err := amqp.DialConfig(fmt.Sprintf("amqp://%s:%s@%s:%d/%s", username, password, server, port, vhost),
			amqp.Config{Heartbeat: 60 * time.Second})
		if err != nil {
			fmt.Printf("Failed to connect, err: %v\n", err)
			continue Loop
		}
		defer connection.Close()

		channel, err := connection.Channel()
		if err != nil {
			fmt.Printf("Failed to create channel, err: %v\n", err)
			continue Loop
		}
		defer channel.Close()

		// one-to-one messaging uses the default exchange, where queue name is the routing key
		queue, err := channel.QueueDeclare(
			queueName, // name
			false,     // durable
			true,      // auto-delete
			false,     // exclusive
			false,     // no-wait
			nil,       // args
		)
		if err != nil {
			fmt.Printf("Failed to declare queue, err: %v\n", err)
			continue Loop
		}

		messageChan, err := channel.Consume(
			queue.Name, // queue
			"consumer", // consumer tag
			true,       // auto-ack
			false,      // exclusive
			false,      // no-local
			false,      // no-wait
			nil,        // args
		)
		if err != nil {
			fmt.Printf("Failed to consume messages, err: %v\n", err)
			continue Loop
		}

		fmt.Println("Started consuming messages.")
		for message := range messageChan {
			fmt.Println(string(message.Body))
		}
	}
}
```

## C
### Prerequisites

**C client AMQP library**

robomq.io is built on AMQP, an open, general-purpose protocol for messaging. There are a number of clients for AMQP in many different languages.  However, we'll choose a simple C-language AMQP client library written for use with v2.0+ of the RabbitMQ broker.

<a href="https://github.com/alanxz/rabbitmq-c/tree/master/librabbitmq" target="_blank">https://github.com/alanxz/rabbitmq-c/tree/master/librabbitmq</a>

You can copy librabbitmq subfolder from latest release located here on GitHub:

<a href="https://github.com/alanxz/rabbitmq-c" target="_blank">https://github.com/alanxz/rabbitmq-c</a>

Alternatively, thanks to Subversion support in GitHub, you can use svn export directly:

```bash
svn export https://github.com/alanxz/rabbitmq-c/trunk/librabbitmq
```

Copy the librabbitmq package into your working directory:

```bash
cp librabbitmq ./
```

Also copy all source files and Makefile from <a href="https://www.robomq.io" target="_blank">RoboMQ.io</a> SDK at <https://github.com/robomq/robomq.io/tree/master/sdk/AMQP/C> into the same directory.  

Now your working directory should have the content as bellow:  
*broadcast*  config.h  *librabbitmq*  Makefile  *one-to-one*  *request-reply*  *routing-key* *topic*

Use the Makefile to compile under a Linux terminal.  

* Run `make type={sub-directory}` to compile the producer and consumer under the sub-directory.  
* Before compiling the next sub-directory, run `make clean` to clean up the compiled files.	 

Note that these examples provide a simple client implementation to get started but does not go into detailed description of all flags passed into the AMQP methods. 
A complete reference to RabbitMQ's implementaton of version 0-9-1 of the AMQP specification can be found in this guide.
<a href="https://www.rabbitmq.com/amqp-0-9-1-reference.html" target="_blank">https://www.rabbitmq.com/amqp-0-9-1-reference.html</a>

### Producer
First, producer should initialize a connection to the robomq.io server. 

```c
amqp_connection_state_t conn = amqp_new_connection();
amqp_socket_t *socket = NULL;
char hostname[] = "hostname"; // robomq.io hostname
int port = 5672; //default
char user[] = "username"; // robomq.io username
char password[] = "password"; // robomq.io password
char vhost[] = "vhost"; // robomq.io account vhost
amqp_channel_t channel = 1;
int channel_max = 0;
int frame_max = 131072;
int heartbeat = 60;
int status = 0;
	
// Opening socket
socket = amqp_tcp_socket_new(conn);
	
status = amqp_socket_open(socket, hostname, port);
if (status) {
	printf("Error opening TCP socket, status = %d, exiting.", status);
}
	
amqp_login(conn, vhost, channel_max, frame_max, heartbeat, AMQP_SASL_METHOD_PLAIN, user, password);
amqp_channel_open(conn, channel);
```

Then, producer should publish messages to the specified exchange attached with routing key. If not specified, that routing key is the queue name. Based on that routing key, messages will be sent through the exchange and distributed to the right queue. 

```c
amqp_basic_properties_t props;
props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
props.content_type = amqp_cstring_bytes("text/plain");
props.delivery_mode = 1; /* non-persistent delivery mode */
amqp_boolean_t mandatory = 0;
amqp_boolean_t immediate = 0;
char exchange_name[] = "hello-exchange";
char routing_key[] = "hola";
int result;
	
// Sending message
result = amqp_basic_publish(conn,
		channel,
		amqp_cstring_bytes(exchange_name),
		amqp_cstring_bytes(routing_key),
		mandatory,
		immediate,
		&props,
		amqp_cstring_bytes("Hello"));
```

Finally, after all messaged are produced, producer should terminate this connection.

```c
amqp_channel_close(conn, channel, AMQP_REPLY_SUCCESS);
amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
amqp_destroy_connection(conn);
```

### Consumer
First, consumer should initialize connection to the robomq.io server.

```c
amqp_connection_state_t conn = amqp_new_connection();
amqp_socket_t *socket = NULL;
char hostname[] = "hostname"; // robomq.io hostname
int port = 5672; //default
char user[] = "username"; // robomq.io username
char password[] = "password"; // robomq.io password
char vhost[] = "vhost"; // robomq.io account vhost
amqp_channel_t channel = 1;
int channel_max = 0;
int frame_max = 131072;
int heartbeat = 60;
int status = 0;
	
// Opening socket
socket = amqp_tcp_socket_new(conn);
	
status = amqp_socket_open(socket, hostname, port);
if (status) {
	printf("Error opening TCP socket, status = %d, exiting.", status);
}
	
amqp_login(conn, vhost, channel_max, frame_max, heartbeat, AMQP_SASL_METHOD_PLAIN, user, password);
amqp_channel_open(conn, channel);
```

Then consumer should create a queue and subscribe to a queue. This queue will work as a mailbox where all messages published to it will be stored until they are consumed.
The **direct** exchange type is specified below in **exchange_type** definition.

```c
amqp_bytes_t queue;
amqp_channel_t channel = 1;
amqp_boolean_t passive = 0;
amqp_boolean_t durable = 0;
amqp_boolean_t exclusive = 0;
amqp_boolean_t auto_delete = 1;
amqp_boolean_t internal = 0;
char exchange_name[] = "hello-exchange";
char exchange_type[] = "direct";
char queue_name[] = "hello-queue";
char binding_key[] = "hola";
	
// Declaring exchange
amqp_exchange_declare(conn, channel, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(exchange_type),
		passive, durable, auto_delete, internal, amqp_empty_table);
	
if(amqp_get_rpc_reply(conn).reply_type != AMQP_RESPONSE_NORMAL) {
	printf("Error declaring exchange: %d\n", amqp_get_rpc_reply(conn));
	exit(1);
}
	
// Declaring queue
amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, channel, amqp_cstring_bytes(queue_name),
		passive, durable, exclusive, auto_delete, amqp_empty_table);
	
if(amqp_get_rpc_reply(conn).reply_type != AMQP_RESPONSE_NORMAL) {
	printf("Error declaring queue: %d\n", amqp_get_rpc_reply(conn));
	exit(1);
}
queue = amqp_bytes_malloc_dup(r->queue);
	
// Binding to queue
amqp_queue_bind(conn, channel, queue, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(binding_key),
		amqp_empty_table);
```

Note that all the queues declared without specific binding key use the queue name as the default binding key.

At this point, consumer should start consuming messages.
The **no_ack** parameter indicates whether consumer will automatically send acknowledgment back to broker. For this example, producer does not explicitly acknowledge received messages.  Therefore, we set **no_ack** attribute value as true.

Then, consumer should receive messages and implement any desired processing on message contents.

```c
amqp_boolean_t no_local = 0;
amqp_boolean_t no_ack = 1;
amqp_boolean_t exclusive = 0;
amqp_frame_t frame;
	
// Consuming the message
amqp_basic_consume(conn, channel, queue, amqp_empty_bytes, no_local, no_ack, exclusive, amqp_empty_table);
	
amqp_rpc_reply_t result;
amqp_envelope_t envelope;
	
amqp_maybe_release_buffers(conn);
result = amqp_consume_message(conn, &envelope, NULL, 0);
	
if (AMQP_RESPONSE_NORMAL == result.reply_type) {
	
	printf("Received message size: %d\nbody: %s\n", (int)envelope.message.body.len, (char *)envelope.message.body.bytes);
	
	amqp_destroy_envelope(&envelope);
}
```

### Putting it all together
The full code below includes some basic AMQP error handling for consumer that is useful when declaring exchanges and queues.  In addition, main receiver loop attempts to reconnect upon network connection failure.

**producer.c**

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
	
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
	
amqp_connection_state_t mqconnect() {
	
	amqp_connection_state_t conn = amqp_new_connection();
	amqp_socket_t *socket = NULL;
	char hostname[] = "localhost"; // robomq.io hostname
	int port = 5672; //default
	char user[] = "guest"; // robomq.io username
	char password[] = "guest"; // robomq.io password
	char vhost[] = "/"; // robomq.io account vhost
	amqp_channel_t channel = 1;
	int channel_max = 0;
	int frame_max = 131072;
	int heartbeat = 60;
	int status = 0;
	
	// Opening socket
	socket = amqp_tcp_socket_new(conn);
	
	status = amqp_socket_open(socket, hostname, port);
	if (status) {
		printf("Error opening TCP socket, status = %d, exiting.", status);
	}
	
	amqp_login(conn, vhost, channel_max, frame_max, heartbeat, AMQP_SASL_METHOD_PLAIN, user, password);
	amqp_channel_open(conn, channel);
	
	return conn;
}
	
int main(int argc, char const *const *argv)
{
	amqp_connection_state_t conn;
	amqp_channel_t channel = 1;
	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 1; /* non-persistent delivery mode */
	amqp_boolean_t mandatory = 0;
	amqp_boolean_t immediate = 0;
	char exchange_name[] = "hello-exchange";
	char routing_key[] = "hola";
	char *msg_body = "Hello\n";
	int result;
	
	conn = mqconnect();
	
	// Sending message
	result = amqp_basic_publish(conn,
			channel,
			amqp_cstring_bytes(exchange_name),
			amqp_cstring_bytes(routing_key),
			mandatory,
			immediate,
			&props,
			amqp_cstring_bytes(msg_body));
	
	if (AMQP_RESPONSE_NONE != result) {
		printf("Producer AMQP failure occurred, response code = %d\n",
				result);
	}
	
	// Closing connection
	amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
	amqp_destroy_connection(conn);
	
	return 0;
}
```

**consumer.c**

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
	
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
	
amqp_connection_state_t mqconnect() {
	
	amqp_connection_state_t conn = amqp_new_connection();
	amqp_socket_t *socket = NULL;
	char hostname[] = "localhost"; // robomq.io hostname
	int port = 5672; //default
	char user[] = "guest"; // robomq.io username
	char password[] = "guest"; // robomq.io password
	char vhost[] = "/"; // robomq.io account vhost
	amqp_channel_t channel = 1;
	amqp_rpc_reply_t reply;
	int channel_max = 0;
	int frame_max = 131072;
	int heartbeat = 60;
	int status = 0;
	
	// Opening socket
	socket = amqp_tcp_socket_new(conn);
	
	status = amqp_socket_open(socket, hostname, port);
	if (status) {
		printf("Error opening TCP socket, status = %d\n", status);
	}
	
	reply = amqp_login(conn, vhost, channel_max, frame_max, heartbeat, AMQP_SASL_METHOD_PLAIN, user, password);
	if(reply.reply_type != AMQP_RESPONSE_NORMAL) {
		fprintf(stderr, "%s: server connection reply code: %d\n",
				"Error logging in", reply.reply_type);
	}
	
	amqp_channel_open(conn, channel);
	
	return conn;
}
	
amqp_bytes_t mqdeclare(amqp_connection_state_t conn, const char *exchange_name, const char *queue_name) {
	amqp_bytes_t queue;
	amqp_channel_t channel = 1;
	amqp_boolean_t passive = 0;
	amqp_boolean_t durable = 0;
	amqp_boolean_t exclusive = 0;
	amqp_boolean_t auto_delete = 1;
	amqp_boolean_t internal = 0;
	char exchange_type[] = "direct";
	char binding_key[] = "hola";
	amqp_rpc_reply_t reply;
	
	// Declaring exchange
	amqp_exchange_declare(conn, channel, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(exchange_type),
			passive, durable, auto_delete, internal, amqp_empty_table);
	
	reply = amqp_get_rpc_reply(conn);
	if(reply.reply_type != AMQP_RESPONSE_NORMAL) {
		amqp_connection_close_t *m = (amqp_connection_close_t *) reply.reply.decoded;
		if(NULL != m) {
			fprintf(stderr, "%s: server connection error %d, message: %.*s\n",
					"Error declaring exchange",
					m->reply_code,
					(int) m->reply_text.len, (char *) m->reply_text.bytes);
		}
	}
	
	// Declaring queue
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, channel, amqp_cstring_bytes(queue_name),
			passive, durable, exclusive, auto_delete, amqp_empty_table);
	
	reply = amqp_get_rpc_reply(conn);
	if(reply.reply_type != AMQP_RESPONSE_NORMAL) {
		fprintf(stderr, "%s: server connection reply code: %d\n",
				"Error declaring queue", reply.reply_type);
	}
	else {
		queue = amqp_bytes_malloc_dup(r->queue);
	
		// Binding to queue
		amqp_queue_bind(conn, channel, queue, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(binding_key),
				amqp_empty_table);
	}
	
	return queue;
}
	
int main(int argc, char const *const *argv)
{
	amqp_connection_state_t conn;
	amqp_bytes_t queue;
	amqp_channel_t channel = 1;
	amqp_boolean_t no_local = 0;
	amqp_boolean_t no_ack = 1;
	amqp_boolean_t exclusive = 0;
	char exchange_name[] = "hello-exchange";
	char queue_name[] = "hello-queue";
	int retry_time = 5; // retry time in seconds
	
	conn = mqconnect();
	queue = mqdeclare(conn, &exchange_name[0], &queue_name[0]);
	
	// Consuming the message
	amqp_basic_consume(conn, channel, queue, amqp_empty_bytes, no_local, no_ack, exclusive, amqp_empty_table);
	
	while (1) {
		amqp_rpc_reply_t result;
		amqp_envelope_t envelope;
	
		amqp_maybe_release_buffers(conn);
		result = amqp_consume_message(conn, &envelope, NULL, 0);
	
		if (AMQP_RESPONSE_NORMAL != result.reply_type) {
			printf("Consumer AMQP failure occurred, response code = %d, retrying in %d seconds...\n",
					result.reply_type, retry_time);
	
			// Closing current connection before reconnecting
			amqp_connection_close(conn, AMQP_CONNECTION_FORCED);
			amqp_destroy_connection(conn);
	
			// Reconnecting on exception
			conn = mqconnect();
			queue = mqdeclare(conn, &exchange_name[0], &queue_name[0]);
			amqp_basic_consume(conn, channel, queue, amqp_empty_bytes, no_local, no_ack, exclusive, amqp_empty_table);
			sleep(retry_time);
		}
		else {
			printf("Received message size: %d\nbody: %s\n", (int)envelope.message.body.len, (char *)envelope.message.body.bytes);
	
			amqp_destroy_envelope(&envelope);
		}
	}
	
	return 0;
}
```

