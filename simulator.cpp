//
// Created by Khaled on 7/28/2024.
//
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

using namespace std;

class Packet {
public:
    double arrivalTime;
    double processingTime;
    double waitTime;
    double finishTime;
    double serverID;

    Packet(double arrivalTime, double processingTime, double waitTime = 0, double finishTime = 0, int serverId = -1): //maybe we don't need serverID
    arrivalTime(arrivalTime), processingTime(processingTime), waitTime(waitTime),finishTime(finishTime),
        serverID(serverId){};
};


class Simulator{

};

int main(int argc, char* argv[]){

}