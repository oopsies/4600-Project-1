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

sem_t mutex_list1, mutex_list2, mutex_freelist;
sem_t counting, list1_full_count, list2_full_count, list1_empty_count, list2_empty_count;
vector<int>list1, list2, freelist;


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
		sem_wait(&counting);
		sem_wait(&mutex_freelist);
		b = unlink(freelist);	// lock for accessing memory
		b = produce_information_in_block(b);	// realized produce_information_in_block() probably means give it a number
		sem_post(&mutex_freelist);
		sem_post(&counting);
		
		sem_wait(&list1_empty_count);
		sem_wait(&mutex_list1);
		link(b, list1);
		sem_post(&mutex_list1);
		sem_post(&list1_full_count);
	}
}

void *transfer(void *args){
	int x, y;

	while(1){

	// Remove one from list1's full count, unlink, then add one to empty count.
	sem_wait(&list1_full_count);
	sem_wait(&mutex_list1);
	x = unlink(list1);
	sem_post(&mutex_list1);
	sem_post(&list1_empty_count);

	sem_wait(&counting);
	y = unlink(freelist);

	use_block_x_to_produce_into_in_y(&x, &y);

	// Link used block to freelist, add one to freelist count.
	link(x, freelist);
	sem_post(&counting);

	// Remove one from list2's empty count, link y, then add one to its full count.
	sem_wait(&list2_empty_count);
	sem_wait(&mutex_list2);
	link(y, list2);
	sem_post(&mutex_list2);
	sem_post(&list2_full_count);
	}
}

void* consume(void* args){
	while(1){
		int c;		

			//wait until there is data to consume in list 2, then consume it
			sem_wait(&list2_full_count);
            sem_wait(&mutex_list2);
            c = unlink(list2);
            sem_post(&mutex_list2);
			sem_post(&list2_empty_count);


            c = consume_information_in_block(c);
            
		//add it back to freelist
        sem_wait(&counting);
        link(c, freelist);
        sem_post(&counting);


	}
}


int main() {

	pthread_t threads[3];
	
	for (int i = 2; i < 12 ; i+=2)
		list1.push_back(i); // list1 = {2, 4, 6, 8, 10}
	
	for (int i = 0; i < 5; i++)
		freelist.push_back(0); // freelist = {0, 0, 0, 0, 0}

	sem_init(&mutex_list1, 0, 1);
	sem_init(&mutex_list2, 0, 1);
	sem_init(&mutex_freelist, 0, 1);
	sem_init(&counting, 0, 5);
	sem_init(&list1_empty_count, 0, 5);
	sem_init(&list2_empty_count, 0, 5);
	sem_init(&list1_full_count, 0, 0);
	sem_init(&list2_full_count, 0, 0);

	pthread_create(&threads[0], NULL, &produce, NULL);
	pthread_create(&threads[1], NULL, &transfer, NULL);
	pthread_create(&threads[2], NULL, &consume, NULL);

	sem_destroy(&mutex_list1);
	sem_destroy(&mutex_list2);
	sem_destroy(&mutex_freelist);
	sem_destroy(&counting);
	sem_destroy(&list1_empty_count);
	sem_destroy(&list2_empty_count);
	sem_destroy(&list1_full_count);
	sem_destroy(&list2_full_count);

	pthread_exit(NULL);

}
