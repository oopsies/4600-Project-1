///CSCE 4600.1 
///Project 1 
///March 4, 2020
///Written by team names

#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 

using namespace std;

sem_t mutex_list1;
sem_t counting;
vector<int>list1, freelist;


void link(int num, vector<int> &list) {
	
	list.push_back(num); // add b to end of list

}

int unlink(vector<int> &list){

	int num = list[list.size()]; // unlink the last element in list
	list.pop_back(); // remove from list
	
	return num; 
		
}
int produce_information_in_block(int num){
	
	srand( time( NULL ) );

	// num should equal 0 if it's a free memory
	return rand() % 10 + 1;  
}

void *produce(void *args){
	int b;
	/*
		while (1)
		Temporarily removed my while loop to test.
		If it's included, I get a seg fault.
	*/
		
		
			sem_wait(&counting);
			b = unlink(freelist);	// lock for accessing memory				
			sem_post(&counting);
			
		
			b = produce_information_in_block(b);
			// realized produce_information_in_block() probably means give it a number
			
		
		sem_wait(&mutex_list1);
		link(b, list1);
		sem_post(&mutex_list1);
}

void *transfer(void *args){
	while(1){




	}
}

void *consume(void *args){
	while(1){
		


	}
}


int main() {

	pthread_t threads[3];
	
	list1.push_back(2);
	list1.push_back(4);
	
	freelist.push_back(0);
	freelist.push_back(0);

	sem_init(&mutex_list1, 0, 1);
	sem_init(&counting, 0, 5);

	pthread_create(&threads[0], NULL, &produce, NULL);
	pthread_create(&threads[1], NULL, &transfer, NULL);
	pthread_create(&threads[2], NULL, &consume, NULL);

	sem_destroy(&mutex_list1);
	sem_destroy(&counting);

	pthread_exit(NULL);

}
