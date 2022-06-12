#include "MyLog.h"
#include <iostream>
#include <thread>

using namespace MySpace;
using namespace std;

void func () {
    LOG_INFO("THIS IS FUNC");
}


int main() {
    LOG_INFO("this is main");
    func();
    thread th([] {
        for (int i = 0; i < 10; i++) {
            LOG_DEBUG("this is thread-1");
        }
    });

    thread th1([] {
        for (int i = 0; i < 10; i++) {
            LOG_ERROR("this is thread-2");
        }
    });

    th.join();
    th1.join();
    return 0;
}