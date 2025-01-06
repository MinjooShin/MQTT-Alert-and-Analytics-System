# 컴파일러
CXX = g++

# 컴파일러 옵션
CXXFLAGS = -I./include -lpaho-mqttpp3 -lpaho-mqtt3as -pthread

# 소스 및 출력 파일
SRC = src/main.cpp src/publisher.cpp src/subscriber.cpp
TARGET = mqtt_monitor

# 빌드
all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# 정리
clean:
	rm -f $(TARGET)
