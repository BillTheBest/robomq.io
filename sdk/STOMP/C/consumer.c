/**
 * File: consumer.c
 * Description: This is the STOMP consumer handles incoming messages
 *     published by producers from a particular queue.
 *     Consumer prints the message body as it receives messages.
 *
 * Author: Eamin Zhang
 * robomq.io (http://www.robomq.io)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./stomp/stomp.h" //depends on where you place the library

struct ctx {
	const char *destination;
};

/**
 * This is the method to print headers.
 */
static void dump_hdrs(int hdrc, const struct stomp_hdr *hdrs)
{
	int i;
	for (i=0; i < hdrc; i++) {
		fprintf(stdout, "%s:%s\n", hdrs[i].key, hdrs[i].val);
	}
}

/**
 * This is the callback method on receiving message.
 * @It prints the message body.
 */
static void _message(stomp_session_t *s, void *ctx, void *session_ctx)
{
	struct stomp_ctx_message *e = ctx;
	//dump_hdrs(e->hdrc, e->hdrs); //print the headers
	fprintf(stdout, "%s\n", (const char *)e->body);
}

/**
 * This is the callback method on error.
 * @It prints the error information.
 */
static void _error(stomp_session_t *session, void *ctx, void *session_ctx)
{
	struct stomp_ctx_error *e = ctx;
	dump_hdrs(e->hdrc, e->hdrs);
	fprintf(stderr, "err: %s\n", (const char *)e->body);
}

/**
 * This is the main method which creates and sets consumer instance.
 * @Exceptions on connection and subscription error.
 */
int main(int argc, char *argv[]) 
{
	char* server = "localhost";
	char* port = "61613";
	char* login = "guest";
	char* passcode = "guest";
	char* vhost = "/";
	//There're more options other than /queue/...
	char* destination = "/queue/test";
	int err;
	int subscription;
	struct ctx client;
	stomp_session_t *session;

	struct stomp_hdr conn_hdrs[] = {
		{"login", login},
		{"passcode", passcode},
		{"vhost", vhost},
		{"accept-version", "1.2"},
		{"heart-beat", "1000,1000"},
	};

	session = stomp_session_new(&client);
	if (!session) {
		perror("stomp");
		exit(EXIT_FAILURE);
	}

	stomp_callback_set(session, SCB_ERROR, _error);
	stomp_callback_set(session, SCB_MESSAGE, _message);

	err = stomp_connect(session, server, port, sizeof(conn_hdrs)/sizeof(struct stomp_hdr), conn_hdrs);
	if (err) {
		perror("stomp");
		stomp_session_free(session);
		exit(EXIT_FAILURE);
	}

	struct stomp_hdr sub_hdrs[] = {
		{"destination", destination},
		{"ack", "auto"},	//could set "ack" header to "client" and manually stomp_ack() / stomp_nack()
		{"id", "0"},
	};

	err = stomp_subscribe(session, sizeof(sub_hdrs)/sizeof(struct stomp_hdr), sub_hdrs);
	if (err<0) {
		perror("stomp");
		stomp_session_free(session);
		exit(EXIT_FAILURE);
	}
	else {
		subscription = err;	//if success, return sub token
	}

	err = stomp_run(session);	//necessary to really run the process, stop when stomp_disconnect() called
	if (err) {
		perror("stomp");
		stomp_session_free(session);
		exit(EXIT_FAILURE);
	}

	//the subsequent code is just to show you how to close the consumer
	//it will not be actually executed because of the stomp_run() above is blocking
 /*
	struct stomp_hdr unsub_hdrs[] = {
		{"id", "0"},	//same id as when you subscribed
	};
	err = stomp_unsubscribe(session, subscription, sizeof(unsub_hdrs)/sizeof(struct stomp_hdr), unsub_hdrs);
	if (err) {
		perror("stomp");
		stomp_session_free(session);
		exit(EXIT_FAILURE);
	}

	struct stomp_hdr disconn_hdrs[] = {
	};	//could use receipt to gracefully disconnect
	err = stomp_disconnect(session, sizeof(disconn_hdrs)/sizeof(struct stomp_hdr), disconn_hdrs);
	if (err) {
		perror("stomp");
		stomp_session_free(session);
		exit(EXIT_FAILURE);
	}

	stomp_session_free(session);
	exit(EXIT_SUCCESS);
*/

	return 0;
}