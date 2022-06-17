/*
 * Copyright 2022 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <thread>
#include <mutex>
using namespace std;

/**
 * @todo write docs
 */
class ThreadManager
{
public:
    ThreadManager(){
        numThreads = 0;
        targetThreads = 1;
    }
    ThreadManager(const ThreadManager& toCpy){
        numThreads = 0;
        targetThreads = toCpy.targetThreads;
    }
    ThreadManager(int th){
        targetThreads = th;
    }

    inline void requestThread(){
        for(;;){
            threadLock.lock();
            if(numThreads < targetThreads) {
                numThreads++;
                threadLock.unlock();
    //             cerr << "thread created\n";
                return;
            }else{
                threadLock.unlock();
                this_thread::sleep_for (std::chrono::milliseconds(10));
            }
        }
    }

    inline void deleteThread(){
        //cerr << "thread done\n";
        threadLock.lock();
        numThreads--;
        threadLock.unlock();
    }

    inline void waitThreads(){
        for(;;){
            threadLock.lock();
            if(numThreads <= 0){
                threadLock.unlock();
                break;
            }else{
                threadLock.unlock();
                this_thread::sleep_for (std::chrono::milliseconds(10));
            }
        }
    }
    int targetThreads = 1;
private:
    int numThreads = 0;
    mutex threadLock;
};

#endif // THREADMANAGER_H
