///CSCE 4600.1 
///Project 1 
///March 4, 2020
///Written by Joshua Bednarz, Alex Triampol, and Ryan Vanek

#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 

/*
Semaphore explanation:

When LINKING: Add one (sem_post) to that list's semaphore - if positive, this inicates that there are resources to consume
When UNLINKING: Subtract one (sem_wait) to that list's semaphore - if < 1, this indicates that there are no resources to consume and must waut

When LINKING AND UNLINKING: Lock that list's mutex so the list cannot be linked or unlinked while it is currently already being linkred or unlinked.
*/


using namespace std;

sem_t mutex_list1, mutex_list2, mutex_freelist_produce, mutex_freelist_transfer, mutex_list1_linker, mutex_list2_linker, mutex_freelist_linker;
vector<int>list1, list2, freelist;


void link(int num, vector<int> &list) {

	list.push_back(num); // add b to end of list

}

int unlink(vector<int> &list){
	
	int num = list[list.size() - 1]; // unlink the last element in list
	list.pop_back();// remove from list
	
	return num;
		
}
int produce_information_in_block(int num){
	
	srand( time( NULL ) );
	// num should equal 0 if it's a free memory
	return rand() % 10 + 1;  
}

int consume_information_in_block(int num){
	return 0;
}

void use_block_x_to_produce_into_in_y(int* x, int* y){
	*y = *x;
	return;
}


void *produce(void *args){
	int b;
	
	while(1){

		cout << "PRODUCE\n";

		sem_wait(&mutex_freelist_produce);
		sem_wait(&mutex_freelist_linker);
		b = unlink(freelist);	// lock for accessing memory
		sem_post(&mutex_freelist_linker);

		b = produce_information_in_block(b);	// realized produce_information_in_block() probably means give it a number
		
		sem_wait(&mutex_list1_linker);
		link(b, list1);
		sem_post(&mutex_list1_linker);
		sem_post(&mutex_list1);
	}
}

void *transfer(void *args){
	int x, y;

	while(1){

	cout << "TRANSFER\n";

	// Remove one from list1's full count, unlink, then add one to empty count.
	sem_wait(&mutex_list1);
	sem_wait(&mutex_list1_linker);
	x = unlink(list1);
	sem_post(&mutex_list1_linker);

	sem_wait(&mutex_freelist_transfer);
	sem_wait(&mutex_freelist_linker);
	y = unlink(freelist);
	sem_post(&mutex_freelist_linker);

	use_block_x_to_produce_into_in_y(&x, &y);

	// Link used block to freelist, add one to freelist count.
	sem_wait(&mutex_freelist_linker);
	link(x, freelist);
	sem_post(&mutex_freelist_linker);
	sem_post(&mutex_freelist_transfer);

	// Remove one from list2's empty count, link y, then add one to its full count.
	sem_wait(&mutex_list2_linker);
	link(y, list2);
	sem_post(&mutex_list2_linker);
	sem_post(&mutex_list2);
	}
}

void* consume(void* args){
	int c;


	while(1){		
	
		cout << "CONSUME\n";

		//wait until there is data to consume in list 2, then consume it
        sem_wait(&mutex_list2);
		sem_wait(&mutex_list2_linker);
        c = unlink(list2);
		sem_post(&mutex_list2_linker);

        c = consume_information_in_block(c);
            
		//add it back to freelist
		sem_wait(&mutex_freelist_linker);
        link(c, freelist);
		sem_post(&mutex_freelist_linker);
        sem_post(&mutex_freelist_produce);


	}
}


int main() {

	pthread_t threads[3];
		
	for (int i = 0; i < 12; i++)
		freelist.push_back(0); // freelist = {0, 0, 0, 0, 0}

	sem_init(&mutex_list1, 0, 0);
	sem_init(&mutex_list2, 0, 0);
	sem_init(&mutex_freelist_produce, 0, 5);
	sem_init(&mutex_freelist_transfer, 0, 5);
	sem_init(&mutex_list1_linker, 0, 1);
    sem_init(&mutex_list2_linker, 0, 1);
    sem_init(&mutex_freelist_linker, 0, 1);

	pthread_create(&threads[0], NULL, &produce, NULL);
	pthread_create(&threads[1], NULL, &transfer, NULL);
	pthread_create(&threads[2], NULL, &consume, NULL);

	sem_destroy(&mutex_list1);
	sem_destroy(&mutex_list2);
	sem_destroy(&mutex_freelist_produce);
    sem_destroy(&mutex_freelist_transfer);
	sem_destroy(&mutex_list1_linker);
	sem_destroy(&mutex_list2_linker);
	sem_destroy(&mutex_freelist_linker);

	pthread_exit(NULL);

}
