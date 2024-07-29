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
private:
    double simTime;
    int serversNum;
    vector<double> serversProb; // Probabilty of each req going to each server
    double lambda;
    vector<double> queueSizes; // [i] is the queue size of server i
    vector<double> muis; //muis[i] is the mui of server i
    double curTime; //maybe needs more ?

    int totalRejected; // to return
    int totalAccepted; // to return
    int totalServiceTime; // for avg service time
    int totalWaitTime; // for avg wait time
    vector<vector<Packet>> servers; // where each vec is a server, maybe can make it a class too whatever :P
                                    // used vector instead of queue if we want to access specific index

public:
    Simulator(double simTime, int serversNum, vector<double> serversProb,
    double lambda, vector<double> queueSizes, vector<double> muis, double curTime = 0)
            : simTime(simTime), serversNum(serversNum), serversProb(serversProb),
            lambda(lambda), queueSizes(queueSizes), muis(muis), curTime(curTime), totalRejected(0),
              totalAccepted(0), totalServiceTime(0), totalWaitTime(0)
    {
        servers.resize(serversNum); // Initialize the servers vector with the number of servers
    }

    bool isServerFull(int serverID){
        return servers[serverID].size() >= queueSizes[serverID];
    }
    int findOutServer(){

        uniform_real_distribution<double> uni_d(0.0, 1.0);
        random_device rd;
        mt19937 gen(rd());
        double probVal = uni_d(gen);
        double cumulativeProb = 0.0;
        int serverIndex = 0;

        for (size_t i = 0; i < serversProb.size(); ++i) {
            cumulativeProb += serversProb[i];
            if (probVal <= cumulativeProb) {
                serverIndex = i;
                break;
            }
        }
        return serverIndex;
    }


};

int main(int argc, char* argv[]){
    //~~~~~~~~~~Taking the inputs ~~~~~~~~~~~~//
    double simTime;
    int serversNum;
    vector<double> serversProb;
    double lambda;
    vector<double> queueSizes;
    vector<double> muis;
    int i = 1;
    simTime = stod(string(argv[i++]));
    serversNum = stoi(string(argv[i++]));
    for(int j = 0 ; j < serversNum; j++){
        serversProb.push_back(stod(string(argv[i++])));
    }
    lambda = stod(string(argv[i++]));
    for(int j = 0 ; j < serversNum; j++){
        queueSizes.push_back(stod(string(argv[i++])));
    }
    for(int j = 0 ; j < serversNum; j++){
        muis.push_back(stod(string(argv[i++])));
    }

    //~~~~~~~~~~ initializing and running the Sim ~~~~~~~~~~~//
    Simulator sim(simTime,serversNum,serversProb,lambda,queueSizes,muis);
    //something like sum.run;

    return 0;

}