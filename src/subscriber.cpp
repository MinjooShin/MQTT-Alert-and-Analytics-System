#include "../include/subscriber.h"
#include <iostream>

using namespace std;

Subscriber::Subscriber(const string& server_address, const string& topic)
    : client(server_address, ""), topic(topic) {
    client.set_callback(*this);
}

void Subscriber::startSubscribing() {
    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        client.connect(connOpts)->wait();
        cout << "Subscriber connected to broker." << endl;

        client.subscribe(topic, QOS)->wait();
        cout << "Subscribed to topic: " << topic << endl;
    } catch (const mqtt::exception& exc) {
        cerr << "Subscriber error: " << exc.what() << endl;
    }
}

void Subscriber::message_arrived(mqtt::const_message_ptr msg) {
    cout << "Message received: " << msg->to_string() << endl;

    regex number_regex(R"(\d+)");
    smatch matches;

    string message = msg->to_string();
    string::const_iterator searchStart(message.cbegin());
    
    cout << "Extracted numbers: ";
    while (regex_search(searchStart, message.cend(), matches, number_regex)) {
        cout << matches[0] << " ";  // 숫자만 출력
        searchStart = matches.suffix().first;
    }
    cout << endl;

    // 데이터 분석 및 threshold 초과 여부 확인
    if (message.find("CPU: 90") != string::npos) {  // 예: CPU 90% 초과
        cout << "[ALERT] CPU usage exceeded threshold!" << endl;
        // 능동부저 알림 추가 (예: 소리 출력)
    }

    if (message.find("RAM: 90") != string::npos) {  // RAM이 90% 초과한 경우
        cout << "[ALERT] RAM usage exceeded threshold!" << endl;
    }

    if (message.find("Disk: 90") != string::npos) {  // Disk 사용량이 90% 초과한 경우
        cout << "[ALERT] Disk usage exceeded threshold!" << endl;
    }
}

