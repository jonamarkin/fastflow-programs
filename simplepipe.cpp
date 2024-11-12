//
// Created by ato-markin on 11/11/24.
//


#include <iostream>
#include <ff/dff.hpp>

using namespace ff;


struct myTask_t {
    std::string str;
    struct S_t {
        long t;
        float f;
    }S;

    template<class Archive>
    void serialize(Archive & archive) {
        archive(str, S.t, S.f);
    }
};


struct Node1: ff_monode_t<myTask_t> {
    Node1(long ntasks):ntasks(ntasks){}
    myTask_t* svc(myTask_t*) {
        for (long i=0; i<ntasks; i++) {
            myTask_t* task = new myTask_t;
            task->str="Hello";
            task->S.t=i;
            task->S.f=i*1.0;
            ff_send_out(task);
        }
        return EOS;
    }

    const long ntasks;

};

struct Node2: ff_monode_t<myTask_t> {
    Node2(long ntasks):ntasks(ntasks){}

    myTask_t* svc(myTask_t* t) {
        t->str += std::string(" World");

        ++processed;
        delete t;
        return GO_ON;
    }

    void svc_end() {
        if (processed != ntasks) {
            abort();
        }

        ff::cout << "RESULT OK\n";
    }

    long ntasks;
    long processed = 0;
};

int main(int argc, char*argv[]) {

    if(DFF_Init(argc, argv)<0) {
        error("DFF_Init\n");
        return -1;
    }

    long ntasks = 1000;
    if(argc>1) {
        ntasks = std::stol(argv[1]);
    }

    ff_pipeline pipe;
    Node1 node1(ntasks);
    Node2 node2(ntasks);

    pipe.add_stage(&node1);
    pipe.add_stage(&node2);

    auto G1 = pipe.createGroup("G1");

    if(pipe.run_and_wait_end()<0) {
        error("running the main pipe\n");
        return -1;
    }

    return 0;
}

