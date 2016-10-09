#ifndef EVENT_H
#define EVENT_H
#include "Process.h"

const int CPUTYPE = 0,
		IOTYPE = 1,
		PROCESSTYPE = 2;

class Event{
public:
	// Event(){}
	Event(double t, bool interrupt){ timeInstant = t; interrupted = interrupt; }
	double getTime()const{ return timeInstant; }
	bool getInterrupt()const { return interrupted; }
	virtual int getDevice() = 0;
	virtual int getType() = 0;
	virtual Process& getProcess() = 0;
	bool operator<(const Event& rhs) const
	{
		return timeInstant > rhs.timeInstant;		//backwards to create min heap
	}
protected:
	double timeInstant;
	bool interrupted;
};

// Ex: Pointer = std::shared_ptr<Event>
template <typename Pointer>
struct PointerLess{
public:
	bool operator()(const Pointer & lhs, const Pointer & rhs) const
	{
		return *lhs < *rhs;
	}
};

class CPUFinished : public Event{
public:
	CPUFinished(double t, int dev, Process& p, bool interrupt) : Event(t, interrupt), device(dev), task(p){
		type = CPUTYPE;
	}
	int getDevice(){ return device; }
	int getType(){ return type; }
	Process& getProcess(){ return task; }
private:
	int device;
	int type;
	Process task;
};

class IOFinished : public Event{
public:
	IOFinished(double t, int dev, Process& p) : Event(t, false), device(dev), task(p){
		type = IOTYPE;
	}
	int getDevice(){ return device; }
	int getType(){ return type; }
	Process& getProcess(){ return task; }
private:
	int device;
	int type;
	Process task;
};

class startProcess : public Event{
public:
	startProcess(double t, int IODevices, bool cpuheavyprocess, int id, bool approx) : 
		Event(t, false), task(Process(t, IODevices, cpuheavyprocess, id, approx))
{
		type = PROCESSTYPE;
	}
	int getDevice(){ return device; }
	int getType(){ return type; }
	Process& getProcess(){ return task; }
private:
	int device;
	int type;
	Process task;
};

#endif