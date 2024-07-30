//
// Created by Khaled on 7/28/2024.
//
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <queue>

using namespace std;

class Packet {
public:
    int arrivalTime;
    int processingTime;
    int waitTime;
    int finishTime;

    Packet(int arrivalTime, int processingTime, int waitTime = 0, int finishTime = 0):
            arrivalTime(arrivalTime), processingTime(processingTime), waitTime(waitTime),finishTime(finishTime) {};
};

class Server {
public:
    Server(double serverProb_, int queueSize_, double miu_) : serverProb(serverProb_), queueSize(queueSize_), miu(miu_),packets() {}
    bool isServerFull() {
        return packets.size() > queueSize;
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
            packets.pop_front();
        }
    }

    //the curr packet
    double serverProb;
    int queueSize;
    double miu;
    std::queue<Packet> packets;
    int totalWaitTime;

};



class Simulator {
private:
    int simTime;
    int serversNum;
    double lambda;
    vector<Server> servers ;
    double curTime; //maybe needs more ?
    int totalRejected; // to return
    int totalAccepted; // to return
    int totalServiceTime; // for avg service time
    int totalWaitTime; // for avg wait time
    int tEnd;
    // used vector instead of queue if we want to access specific index

public:
    Simulator(int simTime, int serversNum, double lambda, vector<Server> servers, double curTime = 0)
            : simTime(simTime), serversNum(serversNum),
              lambda(lambda),servers(servers) curTime(curTime), totalRejected(0),
    totalAccepted(0), totalServiceTime(0), totalWaitTime(0) ,tEnd(0) {
    }



    int findOutServer() {

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
        int t = 0;
        for(t =0 ; t < simTime ; t++)
        {
            //we get lambda packets in each t
            for(int requestCounter =0 ; requestCounter <lambda ; requestCounter++ )
            {
                Packet packet(t,0);
                int serverIndex = findOutServer();
                Server &server =  servers[serverIndex];
                bool accepted = server.acceptRequest(packet,t);
                if(accepted){
                    totalAccepted++;
                }
                else totalRejected++;
                //server.handleRequests(t);
            }
        }
        for (;;t++) {
            bool finish = true;
            for (int serverIndex = 0; serverIndex < serversNum; serverIndex++) {
                if (!servers[serverIndex].packets.empty()) {
                    servers[serverIndex].handleRequests(t);
                    finish = false;
                }
            }
            if (finish) break;//t-1
        }
        tEnd = t-1;

        int totalWaitTime =0 ;
        for (int serverIndex = 0; serverIndex < serversNum; serverIndex++) {
            totalWaitTime += servers[serverIndex].totalWaitTime;
        }
        double avgWaitTime = totalWaitTime / totalAccepted;

    };

    int main(int argc, char* argv[]){
        //~~Taking the inputs ~~~~~~~~~~~~//
        int simTime;
        int serversNum;
        vector<double> serversProb;
        double lambda;
        vector<double> queueSizes;
        vector<double> muis;
        int i = 1;
        simTime = stoi(string(argv[i++]));
        serversNum = stoi(string(argv[i++]));
        vector<Server> servers(serversNum);

        for(int j = 0 ; j < serversNum; j++){
            servers[j].serverProb = stod(string(argv[i++]));
        }
        lambda = stod(string(argv[i++]));
        for(int j = 0 ; j < serversNum; j++){
            servers[j].queueSize = stod(string(argv[i++]));
        }
        for(int j = 0 ; j < serversNum; j++){
            servers[j].miu = stod(string(argv[i++]));
        }

        //~~~~~~~~~~ initializing and running the Sim ~~~~~~~~~~~//
        Simulator sim(simTime,serversNum,lambda,servers);
        sim.run();
        //something like sum.run;

        return 0;

    }