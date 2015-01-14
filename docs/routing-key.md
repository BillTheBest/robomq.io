# Routing - Key Based

Routing - Key based messaging is an extension of direct exchange allowing filtering of messages based on a producer’s routing key.  Messages published to the exchange will be routed to queues bound to that exchange with matching binding key.  All other messages will be filtered.  A consumer will define callback functions to process messages that are selectively received.

![Diagram of Routing - Key based messaging](./images/routing-key.png)

----------

## Python

###Producer
First producer should initialize a connection to [robomq.io](http://www.robomq.io). 

Then producer should initialize a direct-type exchange. This exchange will deliver the message to the queue based on the routing keys. 

	channel.exchange_declare(exchange='exchangeName', type='direct')

Then producer should publish message to the exchange with specific routing key. 

	channel.basic_publish(exchange='exchangeName', routing_key=severity, body=message)

For this example, the routing key is 'routingKey'.  Only the queue binding to this exchange with same routing key, as 'routingKey', can received messages.

After finishing sending messages, producer should terminate the connection. 

###Consumer

For consumer, it should initializes connection to [robomq.io](http://www.robomq.io) first.

Then consumer will initializes the same exchange as producer did.

	channel.exchange_declare(exchange ='exchangeName', type='direct')

Then consumer should declare a queue to listen and consume messages from.. 

	channel.queue_declare(queue=queue_name,exclusive=True)

Then bind the queue to the exchange with a specific routing key. This key will be the identifier for this queue to receive messages from. 


	severity = 'routingKey'	
	channel.queue_bind(exchange='exchangeName', queue=queue_name, routing_key=severity)


After that, we can define own callback function for processing messages like what we did in previous chapter. 
Then consumer is ready to work. 

	channel.basic_consume(callback, queue=queue_name, no_ack=True)
	channel.start_consuming()

Now all the messages with the same routing key as this queue's will get consumed by this consumer.  

###Putting it all together

**producer.py**


	import pika
	import sys

	connection = pika.BlockingConnection(pika.ConnectionParameters( host='your host'))
	channel = connection.channel()

	channel.exchange_declare(exchange='exchangeName', type='direct')

	severity = 'routingKey'
	message = 'Hello World!'
	channel.basic_publish(exchange='exchangeName', routing_key=severity, body=message)
	print 'Sent %r:%r' % (severity, message)
	connection.close()

**Consumer.py**

	import pika
	import sys

	connection = pika.BlockingConnection(pika.ConnectionParameters( host='your host'))
	channel = connection.channel()

	channel.exchange_declare(exchange='exchangeName', type='direct')

	queue_name = 'queueName'
	channel.queue_declare(queue=queue_name,exclusive=True)
	severity = 'routingKey'
	channel.queue_bind(exchange='exchangeName',
					queue=queue_name,
					routing_key=severity)
	print 'Waiting for logs. To exit press CTRL+C'

	def callback(ch, method, properties, body):
		print '%r:%r' % (method.routing_key, body,)

	channel.basic_consume(callback, queue=queue_name, no_ack=True)

	channel.start_consuming()
	
## Java
###Producer
First producer should initializes a connection to [robomq.io](http://www.robomq.io) like tutorial in before section. 

Then producer should initialize a direct-type exchange. This exchange will deliver the message to the queue based on the queues' routing keys. 

	channel.exchangeDeclare(EXCHANGE_NAME, 'direct');
	
Then producer should publish message to the exchange with specific routing key. 

	channel.basicPublish(EXCHANGE_NAME, severity, null, message.getBytes());
        
For this example, the routing key is **'routingKey'**. Only the queue binding to this exchange with matching name, can received this message. 

After finishing sending messages, producer should terminate the connection. 

###Consumer

For consumer, it should initializes connection to [robomq.io](http://www.robomq.io) first.

Then consumer will initializes the same exchange as producer did.

	channel.exchangeDeclare(EXCHANGE_NAME, 'direct');

Then consumer should declare a queue to listen. 

        String queueName = channel.queueDeclare().getQueue();


Then binding the queue to the exchange with a specific routing key.  Messages published to this exchange with routing key matching **"routingKey"** will be received by consumer.  All other messages will be filtered.


	String severity = 'routingKey';
	channel.queueBind(queueName, EXCHANGE_NAME, severity);

Now all the messages with the same routing key as this queue's will get consumed by this consumer.  
	
	QueueingConsumer consumer = new QueueingConsumer(channel);
	channel.basicConsume(queueName, true, consumer);

	while (true) {
		QueueingConsumer.Delivery delivery = consumer.nextDelivery();
		String message = new String(delivery.getBody());
		String routingKey = delivery.getEnvelope().getRoutingKey();

            System.out.println("Received '" + routingKey + "':'" + message + "'");
        }


###Putting it all together

**producer.java**

	import java.io.IOException;
	import com.rabbitmq.client.ConnectionFactory;
	import com.rabbitmq.client.Connection;
	import com.rabbitmq.client.Channel;

	public class Producer {
		private static final String EXCHANGE_NAME = 'exchangeName';
		public static void main(String[] argv)
			throws java.io.IOException {
			ConnectionFactory factory = new ConnectionFactory();
			factory.setHost('your host');
			Connection connection = factory.newConnection();
			Channel channel = connection.createChannel();
		
			channel.exchangeDeclare(EXCHANGE_NAME, 'direct');
			String severity = 'routingKey';
			String message = 'hello world';
			channel.basicPublish(EXCHANGE_NAME, severity, null, message.getBytes());
			System.out.println("Sent '" + severity + "':'" + message + "'");
			channel.close();
			connection.close();
		}
	}
**consumer.java**

	import java.io.IOException;
	import com.rabbitmq.client.ConnectionFactory;
	import com.rabbitmq.client.Connection;
	import com.rabbitmq.client.Channel;
	import com.rabbitmq.client.QueueingConsumer;


	public class Consumer {
		private static final String EXCHANGE_NAME = 'exchangeName';
		public static void main(String[] argv)
                  throws java.io.IOException,
                  java.lang.InterruptedException {
                  ConnectionFactory factory = new ConnectionFactory();
                  factory.setHost('your host');
                  Connection connection = factory.newConnection();
                  Channel channel = connection.createChannel();
			channel.exchangeDeclare(EXCHANGE_NAME, 'direct');
			String queueName = channel.queueDeclare().getQueue();

			String severity = 'routingKey';
			channel.queueBind(queueName, EXCHANGE_NAME, severity);

			System.out.println('Waiting for messages. To exit press CTRL+C');

			QueueingConsumer consumer = new QueueingConsumer(channel);
			channel.basicConsume(queueName, true, consumer);

			while (true) {
				QueueingConsumer.Delivery delivery = consumer.nextDelivery();
				String message = new String(delivery.getBody());
				String routingKey = delivery.getEnvelope().getRoutingKey();
				System.out.println("Received '" + routingKey + "':'" + message + "'");
			}
		}
	}

## Node.js
###Producer
First producer should initializes a connection to [robomq.io](http://www.robomq.io) like tutorial in before section. 

Then producer should initialize a direct-type exchange. This exchange will deliver the message to the queue based on the queues' routing keys. 

	connection.exchange('exchangeName', options={type:'direct', autoDelete:false}, function(exchange)

Then producer should publish message to the exchange with specific routing key. 

	exchange.publish('routingKey','hello world');
For this example, the routing key is 'routingKey'. Only the queue binding to this exchange and also has same routing key, as 'routingKey', can received this message. 

After finishing sending messages, producer should terminate the connection. 

###Consumer

For consumer, it should initializes connection to [robomq.io](http://www.robomq.io) first.

Then consumer will initializes the same exchange as producer did.

	connection.exchange('exchangeName', options={type:'direct', autoDelete:false}, function(exchange)


Then consumer should declare a queue to listen. 

Then binding the queue to the exchange with a specific routing key. This key will be the identifier for this queue getting messages form this exchange. 

After that, we can define own callback function for processing messages like what we did in previous chapter. 
Then consumer is ready to work. 

Now all the messages with the same routing key as this queue's will get consumed by this consumer.  

	var queue = connection.queue('queueName', options={},function(queue){
		console.log('Declare one queue, name is ' + queue.name);
		queue.bind('exchangeName', 'routingKey');
		queue.subscribe(function (msg){
			console.log('consumer received the message'+msg.data);
		});	
	});


###Putting it all together

**producer.js**

	var amqp = require('amqp');
	var connection = amqp.createConnection({ host: 'your host', port: 'port' });

	connection.on('ready',function(){
		connection.exchange('exchangeName', options={type:'direct', autoDelete:false}, function(exchange){
			console.log('start send message');
			exchange.publish('routingKey','hello world');
		});
	});
	
**consumer.js**

	var amqp = require('amqp');
	var connection = amqp.createConnection({ host: 'your host', port: 'port' });

	connection.on('ready', function(){
		connection.exchange('exchangeName', options={type:'direct', autoDelete:false}, function(exchange){
			var queue = connection.queue('queueName', options={},function(queue){
				console.log('Declare one queue, name is ' + queue.name);
				queue.bind('exchangeName', 'routingKey');
				queue.subscribe(function (msg){
				console.log('consumer received the message'+msg.data);
				});	
			});
		});
	});
	
## C
### Prerequisites

**C client AMQP library**

RoboMQ is built on AMQP, an open, general-purpose protocol for messaging. There are a number of clients for AMQP in many different languages.  However, we'll choose a simple C-language AMQP client library written for use with v2.0+ of the RabbitMQ broker.

[https://github.com/alanxz/rabbitmq-c/tree/master/librabbitmq](https://github.com/alanxz/rabbitmq-c/tree/master/librabbitmq)

Download the client library package, and copy it into your working directory:

	cp librabbitmq ./


Note that these examples provide a simple client implementation to get started but does not go into detailed description of all flags passed into the AMQP methods. 
A complete reference to RabbitMQ's implementaton of version 0-9-1 of the AMQP specification can be found in this guide.
[https://www.rabbitmq.com/amqp-0-9-1-reference.html](https://www.rabbitmq.com/amqp-0-9-1-reference.html)


### Producer
For routing-Key based messaging, the producer should publish messages to the specified exchange allowing filtering of messages based on a producer’s routing key.  Based on that routing key, messages will be sent through the exchange and distributed to the right queue.  If not specified, that routing key is the queue name.

	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 1; /* non-persistent delivery mode */
	amqp_boolean_t mandatory = 0;
	amqp_boolean_t immediate = 0;
	char exchange_name[] = "hello-exchange";
	char routing_key[] = "routingKey";
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


### Consumer
Then the consumer should create a queue and subscribe to a queue. This queue will work similarly to the one-to-one example using the **direct** exchange type, however, only messages published to this exchange with routing key matching **"routingKey"** will be received by consumer.  All other messages will be filtered.

	amqp_bytes_t queue;
	amqp_channel_t channel = 1;
	amqp_boolean_t passive = 0;
	amqp_boolean_t durable = 0;
	amqp_boolean_t exclusive = 0;
	amqp_boolean_t auto_delete = 1;
	char exchange_name[] = "hello-exchange";
	char exchange_type[] = "direct";
	char queue_name[] = "hello-queue";
	char binding_key[] = "routingKey";
	
	// Declaring exchange
	amqp_exchange_declare(conn, channel, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(exchange_type),
			passive, durable, amqp_empty_table);
	
	// Declaring queue
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, channel, amqp_cstring_bytes(queue_name),
			passive, durable, exclusive, auto_delete, amqp_empty_table);

	queue = amqp_bytes_malloc_dup(r->queue);
	
	// Binding to queue
	amqp_queue_bind(conn, channel, queue, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(binding_key),
			amqp_empty_table);


Note that all the queues declared without specific binding key use the queue name as the default binding key.

At this point, consumer should start consuming messages.


### How to build/run client
Now we have two c files, one is producer.c, another is consumer.c. 
(1) Use the Makefile in sdk repository to compile under a Linux terminal. 
(2) cd to the directory which containing client source code 
(3) Run make all


### Putting it all together
The full code below includes some basic AMQP error handling for consumer that is useful when declaring exchanges and queues.

**producer.c**

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	
	#include <amqp_tcp_socket.h>
	#include <amqp.h>
	#include <amqp_framing.h>
	
	amqp_connection_state_t mqconnect() {
	
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
		int heartbeat = 0;
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
		char routing_key[] = "routingKey";
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
				amqp_cstring_bytes("Hello"));
	
		// Closing connection
		amqp_channel_close(conn, channel, AMQP_REPLY_SUCCESS);
		amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
		amqp_destroy_connection(conn);
	
		return 0;
	}


**consumer.c**

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	
	#include <amqp_tcp_socket.h>
	#include <amqp.h>
	#include <amqp_framing.h>
	
	amqp_connection_state_t mqconnect() {
	
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
		int heartbeat = 0;
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
	
	amqp_bytes_t mqdeclare(amqp_connection_state_t conn) {
		amqp_bytes_t queue;
		amqp_channel_t channel = 1;
		amqp_boolean_t passive = 0;
		amqp_boolean_t durable = 0;
		amqp_boolean_t exclusive = 0;
		amqp_boolean_t auto_delete = 1;
		char exchange_name[] = "hello-exchange";
		char exchange_type[] = "direct";
		char queue_name[] = "hello-queue";
		char binding_key[] = "routingKey";
		amqp_rpc_reply_t reply;
	
		// Declaring exchange
		amqp_exchange_declare(conn, channel, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(exchange_type),
				passive, durable, amqp_empty_table);
	
		reply = amqp_get_rpc_reply(conn);
		if(reply.reply_type != AMQP_RESPONSE_NORMAL) {
			amqp_connection_close_t *m = (amqp_connection_close_t *) reply.reply.decoded;
			fprintf(stderr, "%s: server connection error %d, message: %.*s\n",
					"Error declaring exchange",
					m->reply_code,
					(int) m->reply_text.len, (char *) m->reply_text.bytes);
			exit(1);
		}
	
		// Declaring queue
		amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, channel, amqp_cstring_bytes(queue_name),
				passive, durable, exclusive, auto_delete, amqp_empty_table);
	
		reply = amqp_get_rpc_reply(conn);
		if(reply.reply_type != AMQP_RESPONSE_NORMAL) {
			amqp_connection_close_t *m = (amqp_connection_close_t *) reply.reply.decoded;
					fprintf(stderr, "%s: server connection error %d, message: %.*s\n",
							"Error declaring queue",
							m->reply_code,
							(int) m->reply_text.len, (char *) m->reply_text.bytes);
			exit(1);
		}
		queue = amqp_bytes_malloc_dup(r->queue);
	
		// Binding to queue
		amqp_queue_bind(conn, channel, queue, amqp_cstring_bytes(exchange_name), amqp_cstring_bytes(binding_key),
				amqp_empty_table);
	
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
		amqp_frame_t frame;
	
		conn = mqconnect();
		queue = mqdeclare(conn);
	
		// Consuming the message
		amqp_basic_consume(conn, channel, queue, amqp_empty_bytes, no_local, no_ack, exclusive, amqp_empty_table);
	
		while (1) {
			amqp_rpc_reply_t result;
			amqp_envelope_t envelope;
	
			amqp_maybe_release_buffers(conn);
			result = amqp_consume_message(conn, &envelope, NULL, 0);
	
			if (AMQP_RESPONSE_NORMAL == result.reply_type) {
	
				printf("Received message size: %d\nbody: %s\n", envelope.message.body.len, envelope.message.body.bytes);
	
				amqp_destroy_envelope(&envelope);
			}
		}
	
		// Closing connection
		amqp_channel_close(conn, channel, AMQP_REPLY_SUCCESS);
		amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
		amqp_destroy_connection(conn);
	
		return 0;
	}