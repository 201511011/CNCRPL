#include <stdio.h>
#include <iostream>
#include <deque>
#include <ctime>
#include <cstdio>
#include <map>
#include <Windows.h>
#include <math.h>
#include <ctime>
#include "init.h"
#include "RPL.h"
#include "packet.h"



using namespace std;
int coverflow=0, cslot = 0;
int slot[3][10] = { {0,},{0,},{0,} };

//rank에 있는 node_numver들 
//val0 = rank0의 node number ->root 
//val1 = rank1의 node number...
int val0[10] = { 0,0,0,0,0,0,0,0,0,0 };
int val1[10] = { 0,0,0,0,0,0,0,0,0,0 };
int val2[10] = { 0,0,0,0,0,0,0,0,0,0 };
int val3[10] = { 0,0,0,0,0,0,0,0,0,0 };
//int val4[10] = { 0, };

//start로 연결할 때 사용할 sender들 
//se1 : rank1의 sender들 ... 
int se1[10] = { 0, };
int se2[10] = { 0, };
int se3[10] = { 0, };

//저장할 node들의 유효값을 저장할 node 
int resu[10] = { 0, };

int routing[10][3];

struct DIO {
	// 1. node_number 2.rank 3.position 4.cnc 
	int v[4] = { 0 };
};
struct DAO {
	int dao[2] = { 0 };
};

struct DAO_ACK {
	int val;// ACCEPT =0 DENY =1
};

struct packet {
	int forward = 0;
	int generate = 0;
	int fail = 0;
	int nodenumber = 0;
};

struct node {
	int node_number;
	int cnc = 0;
	int etx;
	int rate;
	int time = 2;
	int period = 8;
	int fail = 0;
	deque <DIO> receive;// 받은 DIO 
	deque <packet> packet;
	DIO send; // 보낼 DIO 
	map<int, int> parent_set;// key=etx , value = DIO -> 자동정렬 
	deque <int> child;
	DAO_ACK dao_ack;
	DAO dao;
	node *preferred_parent;
	int preferrednumber = 0;
	struct packet mypacekt;
	int existparent = -10;
};


int main(void) {
	//node 생성 
	struct node n[20];
	

	//root 초기화 
	n[0].node_number = 0;
	n[0].etx = 0;
	//DODAG 초기화 
	dodaginit(n,slot,val1, val2, val3);

	//slotframe 할당 확인 
	cout << endl << "Slotframe" << endl;
	for (int q = 0; q < 3; q++) {
		for (int k = 0; k < 10; k++) {
			cout << slot[q][k] << " ";
		}
		cout << endl;
	}


	//void start(struct node n[20], int sen[10], int res[10], int resu[10])
	cout << endl << "**********0->1**********" << endl;
	start(n, val0, val1, resu);//node sender receiver parent 유효값 
	
	for (int i = 0; i < 10; i++) {
		if (resu[i] != 0) {
			se2[i] = resu[i];
		//	cout << se2[i]<<" ";
		}
		resu[i] = 0;
	}
	cout << endl;
	
	cout << endl << "**********1->2**********" << endl;
	start(n, se2, val2, resu);
	for (int i = 0; i < 10; i++) {
		if (resu[i] != 0) {
			se3[i] = resu[i];
		//	cout << se3[i]<<" ";
		}
		resu[i] = 0;
	}


	cout << endl << "**********2->3**********" << endl;
	start(n, se3, val3,resu);


//relation 확인 
	cout << endl << endl;
	for (int i = 0; i < 20; i++) {
		if (n[i].cnc != 0) {
			printf("%d's child / cnc: %d  ---", i, n[i].cnc);
			for (int q = 0; q < n[i].child.size(); q++) {
				cout << n[i].child[q] << " ";
			}
			cout << endl;
		}
	}
	cout << endl << endl;

//packet 주고 받기 - delay 계산 

	//DODAG 초기화 
	
	int thetime = 0;
	for (int i = 0; i < 1000; i++) {
		cslot++;
		cout << "----------slotframe" << cslot <<"----------" <<endl;
		coverflow=coverflow+slotframe(n, slot);
		trickletimer(n);

	
		for (int i = 1; i < 20; i++) {
			if (n[i].time == 0) {
				thetime = i;
				cout << "DIO_Multicast" << thetime << endl;
				
				//rank 1
			//	printdodag(n);
				if (n[thetime].send.v[1] == 1) {
					DIO_multicast(n, thetime, val0, val2);	
				}
				//rank2
				else if (n[thetime].send.v[1] == 2) {
					DIO_multicast(n, thetime, val1, val3);
				}
				//rank3
				else if (n[thetime].send.v[1] == 3) {
					DIO_multicast(n, thetime, val2, val0);				
				}  
			//	printdodag(n);
			}
		}
	}
		
	int num = 0;
	for (int i = 1; i < 20; i++) {
		cout << i << " generate : " << n[i].mypacekt.generate ;
		num = num + n[i].mypacekt.generate;
		cout << " fail : " << n[i].mypacekt.fail;
		cout << " forward : " << n[i].mypacekt.forward<<endl;
	}

	cout << "RPL";
	cout << "Count overflow "<<coverflow << endl;
	cout <<"Total:" <<num<<" Root:"<< n[0].mypacekt.forward << endl;
	cout << "----------종료----------";
	system("pause");
	return 0;	
}