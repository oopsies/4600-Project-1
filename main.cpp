///CSCE 4600.1 
///Project 1 
///March 4, 2020
///Written by team names

#include <iostream>
#include <vector>
#include <pthread.h>

using namespace std;

void produce(){
	while(1){



	}
}

void transfer(){
	while(1){




	}
}

void consume(){
	while(1){
		


	}
}


int main() {

	pthread_t threads[3];

	pthread_create(&threads[0], NULL, produce);
	pthread_create(&threads[1], NULL, transfer);
	pthread_create(&threads[2], NULL, consume);

	


	pthread_exit(NULL);



}
