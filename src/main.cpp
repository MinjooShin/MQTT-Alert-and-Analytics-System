#include <iostream>
#include <thread>
#include "../include/publisher.h"
#include "../include/subscriber.h"

using namespace std;

// MQTT Broker 정보
const string SERVER_ADDRESS("tcp://localhost:1883");
const string PUBLISHER_TOPIC("server/VM1");
const string SUBSCRIBER_TOPIC("server/VM1");

int main() {
    // Publisher 실행
    thread publisher_thread([] {
        Publisher publisher(SERVER_ADDRESS, PUBLISHER_TOPIC);
        publisher.startPublishing();
    });

    // Subscriber 실행
    thread subscriber_thread([] {
        Subscriber subscriber(SERVER_ADDRESS, SUBSCRIBER_TOPIC);
        subscriber.startSubscribing();
        this_thread::sleep_for(chrono::seconds(60)); // 1분간 실행
    });

    publisher_thread.join();
    subscriber_thread.join();

    return 0;
}
