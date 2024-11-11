//
// Created by ato-markin on 11/11/24.
//


#include <iostream>
#include <ff/dff.hpp>

using namespace ff;

// Define a simple task structure that holds an integer.
struct myTask_t {
    int value;

    // Serialization to pass the task through the pipeline.
    template<class Archive>
    void serialize(Archive & archive) {
        archive(value);
    }
};

// Define the first node that will perform a simple computation (adding 1 to the value).
struct Node1 : ff_monode_t<myTask_t> {
    myTask_t* svc(myTask_t* task) {
        task->value += 1;  // Add 1 to the task value.
        std::cout << "Node1: value = " << task->value << std::endl;
        return task;
    }
};

// Define the second node that will perform another computation (adding 2 to the value).
struct Node2 : ff_minode_t<myTask_t> {
    myTask_t* svc(myTask_t* task) {
        task->value += 2;  // Add 2 to the task value.
        std::cout << "Node2: value = " << task->value << std::endl;
        delete task;  // After processing, delete the task.
        return GO_ON;  // Continue to process any remaining tasks.
    }
};

int main(int argc, char*argv[]) {
    // Initialize FastFlow (DFF) system
    if (DFF_Init(argc, argv) < 0) {
        std::cerr << "DFF_Init failed!" << std::endl;
        return -1;
    }

    // Create a pipeline with one group (G1)
    ff_pipeline pipe;

    // Create instances of Node1 and Node2
    Node1 n1;
    Node2 n2;

    // Add the nodes to the pipeline
    pipe.add_stage(&n1);  // First stage: Node1
    pipe.add_stage(&n2);  // Second stage: Node2

    // Create a group for the pipeline (this allows running the nodes in parallel if needed)
    auto G1 = pipe.createGroup("G1");

    // Create a task with an initial value
    myTask_t* task = new myTask_t;
    task->value = 0;  // Initial value is 0.

    // Send the task to the pipeline
    ff_send_out(task);

    // Run the pipeline and wait for it to finish processing
    if (pipe.run_and_wait_end() < 0) {
        std::cerr << "Error running the pipeline!" << std::endl;
        return -1;
    }

    return 0;
}
