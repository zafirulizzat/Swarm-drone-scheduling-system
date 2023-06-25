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
    int deadline;
    int battery;
    int requestTime;
    int startSwapping;
    int taskDurationTIme;
    int laxity;
    int priority;
    int arrivalTime;
    int ready;
    int waiting;
    int finishTime;
    int station_id;

    Drone()
    {

    }
    Drone(int id, int arrivalTime, int taskDurationTIme, int distance, int deadline, int battery) {
        this->id = id;
        this->arrivalTime = arrivalTime;
        this->taskDurationTIme = taskDurationTIme;
        this->distance = distance;
        this->deadline = deadline;
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

    // ...
    drone.station_id = obj.id;
    drone.startSwapping = swapTime;
    while (drone.battery < 60) {
        swapTime++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Station Id" << obj.id << " is charging  drone id: " << drone.id << " battery " << drone.battery << std::endl;
        drone.battery++;
    }
    drone.finishTime = swapTime + drone.requestTime + drone.distance; //////
    drone.ready = drone.requestTime + drone.distance;
    drone.waiting = drone.startSwapping - drone.requestTime; //drone.requestTime; //check balik 
    // Use the extra value argument
    obj.status = 1;
    // Unlock the mutex when done
}

bool priorityByLaxity(const Drone& a, const Drone& b) {
    return a.laxity > b.laxity;
}


int main() {

    std::vector<Drone> drones = {
            Drone(1,14,147, 35, 373, 36), //id,arrivalTime,taskDurationTime,distance,deadline,battery
            Drone(2,23,89, 70, 384, 27),
            Drone(3,6, 198,79, 441, 26),
            Drone(4,31, 204,27, 357, 34),
            Drone(5,45,181, 81, 430, 24),
            Drone(6,9,119, 48, 376, 33),
            Drone(7,38,123, 39, 395, 30),
            Drone(8,18,92, 39, 424, 32),
            Drone(9,12,176, 41, 351, 28),
            Drone(10,29,81, 20, 401, 23),
            Drone(11,8,171, 65, 413, 35),
            Drone(12,40,59, 67, 429, 41),
            Drone(13,8,101, 152, 408, 24),
            Drone(14,2, 174,16, 372, 23),
            Drone(15,33, 153,92, 427, 21),
            Drone(16,20,86, 192, 369, 24),
            Drone(17,47,227, 70, 435, 37),
            Drone(18,10, 112,73, 359, 23),
            Drone(19,27, 211,56, 406, 21),
            Drone(20,22,193, 11, 369, 36),
            Drone(21,43,237, 26, 410, 26),
            Drone(22,9,71, 221, 438, 33),
            Drone(23,35,60, 156, 405, 25),
            Drone(24,16,81, 122, 409, 27),
            Drone(25,36,155, 65, 374, 23),
            Drone(26,16,215, 41, 428, 39),
            Drone(27,42,95, 70, 439, 21),
            Drone(28,13, 187,108, 411, 37),
            Drone(29,21, 225,27, 367, 21),
            Drone(30,46,138, 59, 376, 39),
            Drone(31, 17, 134, 57, 196, 25),
            Drone(32, 23, 121, 63, 252, 23),
            Drone(33, 4, 168, 122, 315, 21),
            Drone(34, 42, 167, 28, 199, 33),
            Drone(35, 15, 98, 49, 152, 30),
            Drone(36, 29, 125, 109, 303, 27),
            Drone(37, 8, 90, 165, 342, 38),
            Drone(38, 35, 87, 176, 316, 27),
            Drone(39, 20, 124, 76, 225, 34),
            Drone(40, 46, 94, 68, 183, 23),
    };

    for (const Drone& drone : drones) {
        std::cout << "Drone " << drone.id << " - Deadline: " << drone.deadline << std::endl;
    }
    std::vector<Drone> scheduler;
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
                drones[j].laxity = drones[j].deadline - drones[j].taskDurationTIme;
                scheduler.push_back(drones[j]);
                drones.erase(drones.begin() + j);
                j--;
            }
            j++;
            requestTime++;
        }
        if (!drones.empty())
        {
            i = 0;
        }
    }
    std::sort(scheduler.begin(), scheduler.end(), priorityByLaxity);
    cout << "After sorting accourding to laxity" << endl;
    for (const Drone& drone : scheduler) {
        std::cout << "Drone " << drone.id << " - laxity: " << drone.laxity << std::endl;
    }
    int swapTime = scheduler[0].requestTime;
    ChargeStation obj1, obj2;
    obj1.id = 1;
    obj2.id = 2;
    //obj3.id = 3; //
    // Initialize the threads without arguments
    std::thread t1;
    std::thread t2;
    //std::thread t3;

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
        file << "Drone: " << abs(drone.id) << " Station: " << drone.station_id << " ready time: " << drone.ready << " waiting time: " << drone.waiting << " finish time: " << drone.finishTime << std::endl;
    }
    cout << endl << "Data Saved";
    file.close();

    return 0;
}