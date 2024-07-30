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
    double startProcessingTime;
    double waitTime;
    double finishTime;
    double serverID;

    Packet(double arrivalTime, double startProcessingTime, double waitTime = 0, double finishTime = 0, int serverId = -1): //maybe we don't need serverID
    arrivalTime(arrivalTime), startProcessingTime(startProcessingTime), waitTime(waitTime), finishTime(finishTime),
    serverID(serverId){};
};


class Server {
public:
    Server(double serverProb_, int queueSize_, double miu_) : serverProb(serverProb_), queueSize(queueSize_), miu(miu_),packets() {}
    bool isServerFull() {
        return packets.size() >= queueSize; // idk if > or >=
    }
    bool acceptRequest(Packet packet ,int t){
        packet.arrivalTime = t;
        if (isServerFull()){
            return false;
        }
        packets.push_back(packet);
    }

    void handleRequests(int t){
        for(int i=0 ;i<miu ; i++){
            totalWaitTime += t - packets.front().arrivalTime;
          //  packets.pop_front();
        }
    }

    double serverProb;
    int queueSize;
    double miu;
    vector<Packet> packets;
    int totalWaitTime;


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
    double totalServiceTime; // for avg service time
    double totalWaitTime; // for avg wait time
    double tEnd;
    vector<Server> servers; // where each vec is a server, maybe can make it a class too whatever :P
                                    // used vector instead of queue if we want to access specific index

public:
    Simulator(double simTime, int serversNum, vector<double> serversProb,
    double lambda, vector<double> queueSizes, vector<double> muis, double curTime = 0)
            : simTime(simTime), serversNum(serversNum), serversProb(serversProb),
            lambda(lambda), queueSizes(queueSizes), muis(muis), curTime(curTime), totalRejected(0),
              totalAccepted(0), totalServiceTime(0), totalWaitTime(0), tEnd(0)
    {
        for(int i = 0; i < serversNum; i++){
            servers.push_back(Server(serversProb[i],queueSizes[i],muis[i]));
        }
    }

    double poissonTimeDist(double lamda)
    {
        exponential_distribution<double> dist(lamda);
        random_device rd;
        mt19937 gen(rd());
        return dist(gen);
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

    void run() {
        double curTime = 0;
        while(curTime < simTime) {
            int serverIndex = findOutServer();
            Packet toAdd(curTime,0,0,0,serverIndex);
            servers[serverIndex].packets.push_back(toAdd);
            curTime += poissonTimeDist(lambda);
        }
        for(int i = 0; i < serversNum; i++){
            Server& curServer = servers[i];
            for(int j = 0; j < curServer.packets.size(); j++){
                vector<Packet>& packets = curServer.packets;
                double proccessTime = poissonTimeDist(curServer.miu);
            }


        }

        int totalWaitTime = 0;
        for (int serverIndex = 0; serverIndex < serversNum; serverIndex++) {
            totalWaitTime += servers[serverIndex].totalWaitTime;
        }
        double avgWaitTime = totalWaitTime / totalAccepted;
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