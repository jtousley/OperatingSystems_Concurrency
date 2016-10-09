#ifndef CACHE_H
#define CACHE_H

#include <vector>

class Cache{
public:
	Cache(unsigned int SIZE){
		for(uint i = 0; i < SIZE; i++){
			Container.push_back(-1);
		}
		index = 0;
	};
	bool Find(int procID){
		for(uint i = 0; i < Container.size(); i++){
			if(Container[i] == procID){
				return true;
			}
		}
		Add(procID);

		return false;
	};
private:
	std::vector<int> Container;
	unsigned int index;

	void Add(int procID){
		Container[index] = procID;
		index++;
		if(index == Container.size()){
			index = 0;
		}
	};

};

#endif