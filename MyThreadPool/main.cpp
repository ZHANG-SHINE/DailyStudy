#include "MyThreadPool.h"
#include <iostream>

using namespace std;
using namespace MySpace;

int main(int argv, char** argc) {
    MyThreadPool::GetInstance().Init(2);
    thread th1([] {
        auto printStr = [] (string a, string b) {
            cout << "thread: " << this_thread::get_id() << " is running printStr" << endl;
            this_thread::sleep_for(1000ms);
            cout << a + b << endl;
        };
        string a = "hello world";
        string b = "!";
        auto res = MyThreadPool::GetInstance().AddTask(printStr, a, b);

        // 有个坑，如果没有等待运行结束就推出线程了的话，a 的值被抹除了
        res->wait();
    });

    thread th2([] {
        auto swap = [] (int& a, int& b) {
            cout << "thread: " << this_thread::get_id() << " is running swap" << endl;
            this_thread::sleep_for(1000ms);
            int c = a;
            a = b;
            b = c;
        };
        int x = 3;
        int y = 4;

        // 不能直接把常量放进这个方法里面，必须要是变量
        auto res = MyThreadPool::GetInstance().AddTask(swap, x, y);
        if (res != nullptr && res->valid()) {
            res->wait();
            cout << "x = " << x << ", y = " << y << endl;
        }
    });
    
    thread th3([] {
        auto add = [] (int a, int b) -> int {
            cout << "thread: " << this_thread::get_id() << " is running add" << endl;
            this_thread::sleep_for(1000ms);
            return a + b;
        };
        int i = 3;
        int j = 4;
        auto res1 = MyThreadPool::GetInstance().AddTask(add, i, j);
        if (res1 != nullptr && res1->valid()) {
            res1->wait();
            cout << "i + j = " << res1->get() << endl;
        }
    });

    th1.join();
    th2.join();
    th3.join();
    return 0;
}