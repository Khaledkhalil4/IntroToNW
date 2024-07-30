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
    double processTime;

    Packet(double arrivalTime, double startProcessingTime, double waitTime = 0, double finishTime = 0, int serverId = -1): //maybe we don't need serverID
    arrivalTime(arrivalTime), startProcessingTime(startProcessingTime), waitTime(waitTime), finishTime(finishTime),
    serverID(serverId),processTime(0){};
};


class Server {
public:
    double serverProb;
    int queueSize;
    double miu;
    vector<Packet> packets;
    vector<Packet> queue;
    int totalWaitTime;
    double curTime;


    Server(double serverProb_, int queueSize_, double miu_) : serverProb(serverProb_), queueSize(queueSize_),
                    miu(miu_),packets() {};

    bool isServerFull() {
        return queue.size() >= queueSize; // idk if > or >= mostly >=
    }

    bool addPacketToQueue( Packet& packet) {
        if (isServerFull()) {
            return false;
        }
        if(!queue.empty()) { //there is more than one packet in the queue
            packet.waitTime += (queue.back().finishTime - packet.arrivalTime);
            packet.finishTime = packet.arrivalTime + packet.waitTime + packet.processTime;
        }else{
            packet.startProcessingTime = curTime;
            packet.finishTime = packet.startProcessingTime + packet.processTime;
        }
        queue.push_back(packet);
        return true;

    }
};

class Simulator{
private:
    double simTime;
    int serversNum;
    vector<double> serversProb; // Probabilty of each req going to each server
    double lambda;
    vector<double> queueSizes; // [i] is the queue size of server i
    vector<double> muis; //muis[i] is the mui of server i

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
            lambda(lambda), queueSizes(queueSizes), muis(muis),  totalRejected(0),
              totalAccepted(0), totalServiceTime(0), totalWaitTime(0), tEnd(0)
    {
        for(int i = 0; i < serversNum; i++){
            servers.emplace_back(serversProb[i],queueSizes[i],muis[i]);
        }
    }

    double poissonTimeDist(double lamda)
    {
        exponential_distribution<double> dist(lamda);
        random_device rd;
        mt19937 gen(rd());
        return dist(gen);
    }
    void updateQueue(Server& server){
        vector<Packet>& curQueue = server.queue;
        vector<Packet> updatedQueue;
        for(int i = 0; i < server.queueSize; i++){
            if(server.curTime > curQueue[i].finishTime) //the job is finished so we don't add it to the queue.
                continue;
            updatedQueue.push_back(curQueue[i]);
        }
        server.queue = updatedQueue; // we have the queue without the finished jobs :D
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

    void printOutput(){
        double avgWaitingTime = totalAccepted > 0 ? totalWaitTime/totalAccepted : 0;
        double avgServiceTime = totalAccepted > 0 ? totalServiceTime/totalAccepted : 0;

        cout << totalAccepted << " "
             << totalRejected << " "
             << tEnd << " "
             << avgWaitingTime << " "
             << avgServiceTime << endl;
    }

    void run() {
        double time = 0;

        while(time < simTime) {
            int serverIndex = findOutServer();
            Packet toAdd(time, 0, 0, 0, serverIndex);
            toAdd.processTime = poissonTimeDist(muis[serverIndex]);
            servers[serverIndex].packets.push_back(toAdd);
            time += poissonTimeDist(lambda);
        }

        for(int i = 0; i < serversNum; i++){
            Server& curServer = servers[i];
            for(int j = 0; j < curServer.packets.size(); j++){
                vector<Packet>& packets = curServer.packets;
                curServer.curTime = max(curServer.curTime, packets[j].arrivalTime);
                updateQueue(curServer);
                if(curServer.addPacketToQueue(packets[j])){
                    totalAccepted++;
                    totalServiceTime += packets[j].processTime;
                    totalWaitTime += packets[j].waitTime;
                    tEnd = max(tEnd,packets[j].finishTime);
                }else
                    totalRejected++;
            }
        }
        printOutput();
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
    sim.run();

    return 0;

}