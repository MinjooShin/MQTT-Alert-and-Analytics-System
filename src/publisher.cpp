#include "../include/publisher.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <sys/sysinfo.h> // Linux 시스템 상태 확인
#include <fstream>
#include <sstream>
#include <sys/statvfs.h> // 디스크 사용량 계산용

using namespace std;

Publisher::Publisher(const string& server_address, const string& topic)
    : client(server_address, ""), topic(topic) {}

// CPU 사용률 계산
float getCpuUsage() {
    ifstream file("/proc/stat");
    string line;
    getline(file, line); // 첫 번째 줄 읽기 (cpu 정보)
    file.close();

    istringstream iss(line);
    string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;
    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    // 전체 시간과 idle 시간 계산
    long total_time = user + nice + system + idle + iowait + irq + softirq + steal;
    long idle_time = idle + iowait;

    // 100ms 후 다시 읽어서 CPU 사용률 계산
    this_thread::sleep_for(chrono::milliseconds(100));
    ifstream file2("/proc/stat");
    getline(file2, line);
    file2.close();

    iss.clear();
    iss.str(line);
    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    long total_time2 = user + nice + system + idle + iowait + irq + softirq + steal;
    long idle_time2 = idle + iowait;

    // CPU 사용률 계산 (차이값으로 계산)
    float cpu_usage = 100.0 * (1.0 - (float)(idle_time2 - idle_time) / (total_time2 - total_time));
    return cpu_usage;
}

// RAM 사용량 계산
float getRamUsage() {
    ifstream file("/proc/meminfo");
    string key;
    long value, total_ram = 0, free_ram = 0;

    while (file >> key >> value) {
        if (key == "MemTotal:") {
            total_ram = value; // 총 RAM (kB 단위)
        } else if (key == "MemAvailable:") {
            free_ram = value; // 사용 가능한 RAM (kB 단위)
            break;
        }
    }
    file.close();

    float used_ram = ((float)(total_ram - free_ram) / total_ram) * 100; // 사용률 계산
    return used_ram;
}

// 디스크 사용량 계산
float getDiskUsage(const string& path = "/") {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        return -1; // 오류 발생 시 -1 반환
    }

    // 디스크 사용률 계산
    long total = stat.f_blocks * stat.f_frsize; // 총 디스크 크기
    long free = stat.f_bfree * stat.f_frsize;   // 사용 가능한 디스크 크기
    float used = ((float)(total - free) / total) * 100;
    return used;
}

void Publisher::startPublishing() {
    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        client.connect(connOpts)->wait();
        cout << "Publisher connected to broker." << endl;

        while (true) {
            // 시스템 정보 수집
            struct sysinfo sys_info;
            if (sysinfo(&sys_info) != 0) {
                cerr << "Error getting system info!" << endl;
                continue;
            }

            // CPU, RAM, Disk Usage 데이터 생성
            float cpu_usage = getCpuUsage();
            float ram_usage = getRamUsage();
            float disk_usage = getDiskUsage();

            string payload = "CPU: " + to_string(cpu_usage) + "%, " +
                             "RAM: " + to_string(ram_usage) + "%, " +
                             "Disk: " + to_string(disk_usage) + "%, " +
                             "Uptime: " + to_string(sys_info.uptime) + " sec";

            // 데이터 Publish
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            pubmsg->set_qos(QOS);
            client.publish(pubmsg)->wait();
            cout << "Published: " << payload << endl;

            this_thread::sleep_for(chrono::seconds(5)); // 5초 주기로 Publish
        }
    } catch (const mqtt::exception& exc) {
        cerr << "Publisher error: " << exc.what() << endl;
    }
}
