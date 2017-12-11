#include "RCSwitch.h"
#include <iostream>
#include <unistd.h>
#include <mosquitto.h>
#include <string.h>

// mqtt topic pattern: matrix/433/set/<group>/<device>, example: matrix/433/set/11111/00001
const char *topicPattern = "matrix/433/set/+/+";

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
	if(message->payloadlen){
                printf("%s %s\n", (char *)message->topic, (char *)message->payload);
        } else {
                printf("%s (null)\n", (char *)message->topic);
        }

	bool matchesPattern;
	if (!mosquitto_topic_matches_sub(topicPattern, (char *)message->topic, &matchesPattern)) {
	    printf("matrix 433 message received");
	    if (matchesPattern) {
		char **hierarchy;
		int i;
		mosquitto_sub_topic_tokenise((char *)message->topic, &hierarchy, &i);
		printf("433 group: %s, switch: %s", hierarchy[3], hierarchy[4]);
		if (!strcmp((char *)message->payload, "ON")) {
			printf("switch on");
			((RCSwitch*)userdata)->switchOn(hierarchy[3], hierarchy[4]);
	  	} else {
			printf("switch off");
			((RCSwitch*)userdata)->switchOff(hierarchy[3], hierarchy[4]);
		}
	    }
	}

        fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	fprintf(stdout, "Connected");
	int i;
	if(!result){
		/* Subscribe to topic pattern on successful connect. */
		mosquitto_subscribe(mosq, NULL, topicPattern, 0);
	} else {
		fprintf(stderr, "Connect failed\n");
	}
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;
	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
	printf("%s\n", str);
}

int main() {
	RCSwitch mySwitch = RCSwitch();
	mySwitch.enableTransmit(0);
 	mySwitch.setPulseLength(300);

	int i;
	const char *host = "192.168.178.81";
	int port = 1883;
	int keepalive = 60;
	bool clean_session = true;
	struct mosquitto *mosq = NULL;
	int mqtt_version = MQTT_PROTOCOL_V311;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, clean_session, &mySwitch);
	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

	mosquitto_log_callback_set(mosq, my_log_callback);
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_message_callback_set(mosq, my_message_callback);
	mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
	mosquitto_opts_set(mosq, MOSQ_OPT_PROTOCOL_VERSION, &mqtt_version);
//        mosquitto_username_pw_set(mosq, "UsernameIfPasswordSet", "PasswordIfSet");

	int connection = mosquitto_connect(mosq, host, port, keepalive);
        if (connection){
                fprintf(stderr, strerror(connection));
		fprintf(stderr, "Unable to connect.\n");
		return 1;
	}

	mosquitto_loop_forever(mosq, -1, 1);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}
