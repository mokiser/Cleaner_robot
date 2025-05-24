#include <iostream>
#include <string>
#include <mosquitto.h>
#include <vector>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

const char* BROKER_IP = "localhost";
const char* TOPIC = "laptop/commands";

// Класс для безопасной работы с Mosquitto
class MqttWrapper {
public:
    MqttWrapper() : mosq(nullptr) {
        mosquitto_lib_init();
        mosq = mosquitto_new(nullptr, true, nullptr);
    }

    ~MqttWrapper() {
        if(mosq) {
            mosquitto_destroy(mosq);
        }
        mosquitto_lib_cleanup();
    }

    bool connect() {
        return mosquitto_connect(mosq, BROKER_IP, 1883, 60) == MOSQ_ERR_SUCCESS;
    }

    bool send(const std::string& message) {
        return mosquitto_publish(
            mosq, nullptr, TOPIC,
            message.size(), message.c_str(), 0, false
        ) == MOSQ_ERR_SUCCESS;
    }

private:
    mosquitto* mosq;
};

class VisionProcessor {
public:
    VisionProcessor() : cap(0) {
        if(!cap.isOpened()) throw std::runtime_error("Camera error");
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    }

    struct DetectionResult {
        cv::Point front;
        cv::Point back;
        cv::Point target;
        double angle = 0.0;
        double distance = 0.0;
    };

    DetectionResult process() {
        DetectionResult result;
        cap >> frame;
        if(frame.empty()) return result;

        // Цветовая сегментация
        cv::Mat hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        
        result.front = findCenter(hsv, cv::Scalar(0, 120, 70), cv::Scalar(10, 255, 255));
        result.back = findCenter(hsv, cv::Scalar(90, 120, 70), cv::Scalar(130, 255, 255));
        result.target = findCenter(hsv, cv::Scalar(40, 120, 70), cv::Scalar(80, 255, 255));

        if(result.front.x > 0 && result.back.x > 0 && result.target.x > 0) {
            cv::Point direction = result.front - result.back;
            cv::Point to_target = result.target - result.front;
            
            // Расчет угла и расстояния
            result.angle = atan2(
                direction.x * to_target.y - direction.y * to_target.x,
                direction.x * to_target.x + direction.y * to_target.y
            ) * 180 / CV_PI;

            result.distance = cv::norm(to_target);
        }

        drawDebugInfo(result);
        return result;
    }

private:
    cv::VideoCapture cap;
    cv::Mat frame;

    cv::Point findCenter(const cv::Mat& hsv, const cv::Scalar& low, const cv::Scalar& high) {
        cv::Mat mask;
        cv::inRange(hsv, low, high, mask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        if(contours.empty()) return cv::Point(-1, -1);

        auto largest = *std::max_element(contours.begin(), contours.end(),
            [](const auto& a, const auto& b) {
                return cv::contourArea(a) < cv::contourArea(b);
            });

        cv::Rect rect = cv::boundingRect(largest);
        return cv::Point(rect.x + rect.width/2, rect.y + rect.height/2);
    }

    void drawDebugInfo(const DetectionResult& res) {
        if(res.front.x > 0 && res.back.x > 0) {
            cv::circle(frame, res.front, 10, cv::Scalar(0, 0, 255), -1);
            cv::circle(frame, res.back, 10, cv::Scalar(255, 0, 0), -1);
            cv::line(frame, res.back, res.front, cv::Scalar(200, 200, 200), 2);
        }

        if(res.target.x > 0) {
            cv::circle(frame, res.target, 15, cv::Scalar(0, 255, 0), 2);
            if(res.front.x > 0) {
                cv::line(frame, res.front, res.target, cv::Scalar(0, 255, 0), 1);
            }
        }

        cv::imshow("Robot View", frame);
        cv::waitKey(1);
    }
};

class ControlSystem {
public:
    void run() {
        MqttWrapper mqtt;
        VisionProcessor vision;

        if(!mqtt.connect()) {
            std::cerr << "MQTT connection failed!" << std::endl;
            return;
        }

        while(true) {
            auto res = vision.process();
            
            if(res.target.x < 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            if(res.distance < 30.0) {
                mqtt.send("shutdown:0");
                break;
            }

            std::string command = (abs(res.angle) > 5.0) ? 
                (res.angle > 0 ? "left:100" : "right:100") : 
                "forward:300";

            if(!mqtt.send(command)) {
                std::cerr << "Failed to send command!" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

int main() {
    ControlSystem controller;
    controller.run();
    return 0;
}

