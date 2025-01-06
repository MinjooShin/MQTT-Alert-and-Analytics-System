#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <mqtt/async_client.h>
#include <string>

using namespace std;

const int QOS = 1;

class Publisher {
public:
    Publisher(const string& server_address, const string& topic);
    void startPublishing(); // MQTT 메시지 전송 루프 시작

private:
    mqtt::async_client client; // MQTT 비동기 클라이언트 객체
    string topic;              // Publish할 토픽 이름

    // 시스템 상태 정보를 가져오는 유틸리티 함수들
    float getCpuUsage();                     // CPU 사용률 계산
    float getRamUsage();                     // RAM 사용률 계산
    float getDiskUsage(const string& path);  // 디스크 사용률 계산 (기본 경로: "/")
};

#endif
