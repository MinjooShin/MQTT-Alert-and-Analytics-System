#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <mqtt/async_client.h>

using namespace std;

const int QOS = 1;

class Subscriber : public virtual mqtt::callback {
public:
    Subscriber(const string& server_address, const string& topic);
    void startSubscribing();

    // MQTT 콜백 함수
    void message_arrived(mqtt::const_message_ptr msg) override;

private:
    mqtt::async_client client;
    string topic;
};

#endif
