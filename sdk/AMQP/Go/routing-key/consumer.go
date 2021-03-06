/**
* File: consumer.go
* Description: This is the AMQP consumer handles incoming
*     communication from clients publishing messages to a broker server.
*     Messages can be received over AMQP exchange types including one-to-one,
*     from broadcast pattern, or selectively using specified binding key.
*
* Author: Eamin Zhang
* robomq.io (http://www.robomq.io)
*/

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
var exchangeName = "testEx"
var queueName = "testQ1"
var routingKey = "test"

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

		err = channel.ExchangeDeclare(
			exchangeName, // name
			"direct",     // type
			false,        // durable
			true,         // audo-delete
			false,        // internal
			false,        // no-wait
			nil,          // args
		)
		if err != nil {
			fmt.Printf("Failed to declare exchange, err: %v\n", err)
			continue Loop
		}

		queue, err := channel.QueueDeclare(
			queueName, // name
			false,     // durable
			true,      // auto-delete
			true,      // exclusive
			false,     // no-wait
			nil,       // args
		)
		if err != nil {
			fmt.Printf("Failed to declare queue, err: %v\n", err)
			continue Loop
		}

		err = channel.QueueBind(
			queueName,    // queue
			routingKey,   // key
			exchangeName, // exchange
			false,        // no-wait
			nil,          // args
		)
		if err != nil {
			fmt.Printf("Failed to bind queue with exchange, err: %v\n", err)
			continue Loop
		}

		messageChan, err := channel.Consume(
			queue.Name, // queue
			"consumer", // consumer tag
			true,       // auto-ack
			true,       // exclusive
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