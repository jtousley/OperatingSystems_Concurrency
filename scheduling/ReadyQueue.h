#ifndef READYQUEUE_H
#define READYQUEUE_H

#include "Process.h"

template <class TYPE>
class RQ{
public:
	RQ(bool init){ FIFO = init; }
	void pop(){
		if(FIFO){
			ReadyQueue2.pop();
		}
		else{
			ReadyQueue1.pop();
		}
	};
	TYPE front(){
		if(FIFO){
			return ReadyQueue2.front();
		}
		else{
			return ReadyQueue1.top();
		}
	};
	bool empty(){
		if(FIFO){
			return ReadyQueue2.empty();
		}
		else{
			return ReadyQueue1.empty();
		}
	};
	void push(TYPE ele){
		if(FIFO){
			ReadyQueue2.push(ele);
		}
		else{
			ReadyQueue1.push(ele);
		}
	};

private:
	std::priority_queue<Process, std::vector<Process>, ProcessCompare> ReadyQueue1;
	std::queue<Process> ReadyQueue2;
	bool FIFO;
};

#endif