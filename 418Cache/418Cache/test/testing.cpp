#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>

int main()
{	
	std::vector<std::string*> strings;

	for (int i = 0; i < 10; ++i)
	{
		std::string* temp = new std::string(i, 'c');
		strings.push_back(tempn);
	}
	for (int i = 0; i < 10; ++i)
	{
		std::cout<<*strings[i]<<"\n";
	}
}	

for(int i = 0; i < constants.getNumProcessors(); i++){
		printf("number of jobs cache %d SHOULD have is %d \n", i, outstandingRequests.at(i).size());
		printf("temp pid is %d \n", (*temp).getProcessorId());
		caches[i] = new Cache(i, constants, &outstandingRequests.at(i));
		printf("caches[%d] pId is %d address is %x \n", i, (*caches[i]).getProcessorId(), temp);
		for(int y = 0; y < i; y++){
			printf("inside the for loop, up to this point cache %d pId is %d at addr %x \n", y, (*caches[y]).getProcessorId(), caches[y]);
		}
	}
}