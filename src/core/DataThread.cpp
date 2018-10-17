//
// Created by Raffaele Montella on 14/10/2018.
//

#include "DataThread.hpp"

DataThread::DataThread() : t() {

}

void DataThread::start() {
    if (status==DataThread::Status::READY) {
        t = std::thread(&DataThread::run, this);
        threadStop = false;
    }
}

void DataThread::stop() {
    threadStop=true;
}

void DataThread::join() {
    t.join();
}

void DataThread::onStart() {}
void DataThread::onRun() {}
void DataThread::onStop() {}

void DataThread::run(){
    status=Status::RUNNING;
    onStart();
    onRun();
    onStop();
    status=Status::STOPPED;
}