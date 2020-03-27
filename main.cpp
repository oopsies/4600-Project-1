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

sem_t mutex_list1, mutex_list2, mutex_freelist_produce, mutex_freelist_transfer, mutex;
sem_t counting, list1_full_count, list2_full_count, list1_empty_count, list2_empty_count;
vector<int>list1, list2, freelist;


void link(int num, vector<int> &list) {
	
	sem_wait(&mutex);
	list.push_back(num); // add b to end of list
	sem_post(&mutex);

}

int unlink(vector<int> &list){
	
	sem_wait(&mutex);
	int num = list[list.size() - 1]; // unlink the last element in list
	list.pop_back();// remove from list
	sem_post(&mutex);
	
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

		//if (freelist.size() == 0)
        //cout << "\n\nERROR: FREELIST SIZE IS 0\n\n";
		sem_wait(&mutex_freelist_produce);
		b = unlink(freelist);	// lock for accessing memory


		b = produce_information_in_block(b);	// realized produce_information_in_block() probably means give it a number
		
		
		link(b, list1);
		sem_post(&mutex_list1);
	}
}

void *transfer(void *args){
	int x, y;

	while(1){

	cout << "TRANSFER\n";



	// Remove one from list1's full count, unlink, then add one to empty count.
	sem_wait(&mutex_list1);
	x = unlink(list1);

	//if (freelist.size() == 0)
    //    cout << "\n\nERROR: FREELIST SIZE IS 0\n\n";
	sem_wait(&mutex_freelist_transfer);
	y = unlink(freelist);


	use_block_x_to_produce_into_in_y(&x, &y);

	// Link used block to freelist, add one to freelist count.
	link(x, freelist);
	sem_post(&mutex_freelist_transfer);

	// Remove one from list2's empty count, link y, then add one to its full count.
	link(y, list2);
	sem_post(&mutex_list2);
	}
}

void* consume(void* args){
	int c;


	while(1){		
	
		cout << "CONSUME\n";

		//wait until there is data to consume in list 2, then consume it
        sem_wait(&mutex_list2);
        c = unlink(list2);

        c = consume_information_in_block(c);
            
		//add it back to freelist
        link(c, freelist);
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
	sem_init(&mutex, 0, 1);
	//sem_init(&counting, 0, 5);
	//sem_init(&list1_empty_count, 0, 5);
	//sem_init(&list2_empty_count, 0, 5);
	//sem_init(&list1_full_count, 0, 0);
	//sem_init(&list2_full_count, 0, 0);

	pthread_create(&threads[0], NULL, &produce, NULL);
	pthread_create(&threads[1], NULL, &transfer, NULL);
	pthread_create(&threads[2], NULL, &consume, NULL);

	sem_destroy(&mutex_list1);
	sem_destroy(&mutex_list2);
	sem_destroy(&mutex_freelist_produce);
    sem_destroy(&mutex_freelist_transfer);
	sem_destroy(&mutex);
	/*sem_destroy(&counting);
	sem_destroy(&list1_empty_count);
	sem_destroy(&list2_empty_count);
	sem_destroy(&list1_full_count);
	sem_destroy(&list2_full_count);*/

	pthread_exit(NULL);

}
