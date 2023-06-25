#include <iostream>
#include <thread>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <chrono>
#include <queue>
#include <atomic>
using namespace std;

class Drone {
public:
    int id;
    int distance;
    //int deadline;
    int battery;
    int requestTime;
    int startSwapping;
    int ready;
    int waiting;
    int finishTime;
    int stationId;

    Drone()
    {

    }
    Drone(int id, int distance, int deadline, int battery) {
        this->id = id;
        this->distance = distance;
        //this->deadline = deadline;
        this->battery = battery;
        this->requestTime = 0;
        this->ready = 0;
        this->waiting = 0;
    }

    void tick() {
        battery--;
    }
};

class ChargeStation {
public:
    int status = 1;
    int id;

    // ...

    std::mutex mutex;
};

void myThreadFunction(ChargeStation& obj, Drone& drone, int& swapTime) {
    // Lock the mutex before accessing the status attribute
    std::lock_guard<std::mutex> lock(obj.mutex);

    // Access the status attribute
    obj.status = 0;
    drone.stationId = obj.id;
    // ...
    drone.startSwapping = swapTime;
    while (drone.battery < 80) {
        swapTime++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Station Id" << obj.id << " is charging  drone id: " << drone.id << " battery " << drone.battery << std::endl;
        drone.battery++;
    }
    drone.finishTime = swapTime + drone.requestTime + drone.distance; //////////
    drone.ready = drone.requestTime + drone.distance;
    drone.waiting = drone.startSwapping - drone.requestTime;
    // Use the extra value argument
    obj.status = 1;
    // Unlock the mutex when done
}



int main() {

    std::vector<Drone> drones = {

            Drone(1, 35, 373, 36), //id,arrivalTime,taskDurationTime,distance,deadline,battery
            Drone(2, 70, 384, 27),
            Drone(3,79, 441, 26),
            Drone(4,27, 357, 34),
            Drone(5, 81, 430, 24),
            Drone(6, 48, 376, 33),
            Drone(7, 39, 395, 30),
            Drone(8, 39, 424, 32),
            Drone(9, 41, 351, 28),
            Drone(10, 20, 401, 23),
            Drone(11, 65, 413, 35),
            Drone(12, 67, 429, 41),
            Drone(13, 152, 408, 24),
            Drone(14,16, 372, 23),
            Drone(15,92, 427, 21),
            Drone(16, 192, 369, 24),
            Drone(17, 70, 435, 37),
            Drone(18,73, 359, 23),
            Drone(19,56, 406, 21),
            Drone(20, 11, 369, 36),

    };
    vector<Drone> scheduler;
    int requestTime = 0;
    int i = 0;
    while (i < drones.size())
    {
        int j = i;
        while (j < drones.size())
        {
            drones[j].tick();
            if (drones[j].battery <= 20)
            {
                drones[j].requestTime = requestTime;
                scheduler.push_back(drones[j]);
                drones.erase(drones.begin() + j);
            }
            j++;
            requestTime++;
        }
        if (!drones.empty())
        {
            i = 0;
        }
    }
    int swapTime = scheduler[0].requestTime;
    ChargeStation obj1, obj2;
    obj1.id = 1;
    obj2.id = 2;

    // Initialize the threads without arguments
    std::thread t1;
    std::thread t2;

    i = 0;
    while (i < scheduler.size()) {

        if (obj1.status == 1) {
            // Assign arguments to t1
            t1 = std::thread(myThreadFunction, ref(obj1), ref(scheduler[i]), ref(swapTime));
            i++;
        }
        if (obj2.status == 1) {
            // Assign arguments to t2
            t2 = std::thread(myThreadFunction, ref(obj2), ref(scheduler[i]), ref(swapTime));
            i++;
        }

        // ...

        // Wait for the threads to finish
        if (t1.joinable()) {
            t1.join();
        }
        if (t2.joinable()) {
            t2.join();
        }

    }
    cout << "Saving Data" << endl;
    std::ofstream file("data.csv");
    for (const Drone& drone : scheduler) {
        file << "Drone: " << abs(drone.id) << " Station: " << drone.stationId << " ready time: " << drone.ready << " waiting time: " << drone.waiting << " finish time: " << drone.finishTime << std::endl;
    }
    cout << endl << "Data Saved";
    file.close();

    return 0;
}