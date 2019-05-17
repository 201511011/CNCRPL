#include <stdio.h>
#include <string>
#include <iostream>
#include <queue>
#include <deque>
#include <ctime>
#include <cstdio>
#include <map>
#include <Windows.h>
#include <math.h>
#include <ctime>
#include "RPL.h"
#include "packet.h"
#include "init.h"
#include "dodaginit.h"
using namespace std;

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
	int rate ;
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

void dodaginit(struct node n[20],int slot[3][10], int re1[10],int re2[10],int re3[10]) {
	int n0 = 0, n1 = 1, n2 = 2;
	int v1 = 0, v2 = 0, v3 = 0;
	n[0].etx = 0;//root 초기화 
	

	n[0].node_number = 0;
	for (int i = 1; i < 20; i++) {
	
		int position = rand() % 300+100;
		n[i].etx = position / 100;
		if (n[i].etx == 1 && v1 > 3) {
			position = rand() % 200 + 200;
			n[i].etx = position / 100;
		}
		n[i].node_number = i;
	
		n[i].send.v[0] = n[i].node_number;
		n[i].existparent = -10;
		n[i].send.v[1] = n[i].etx;
		n[i].send.v[2] = position;//position
		n[i].send.v[3] = 0;//cnc 
		Sleep(100);
		printf("%d :ETX : %d  Rank : %d \n", i, position, n[i].etx);
		n[i].mypacekt.nodenumber = i;

		n[i].time = n[i].time + i+10;

		if (n[i].etx == 1) {
	
			re1[v1] = i;
			v1++;
			
			slot[0][n0] = i;
			if (n0 + 2 >= 10)	n0++;
			n0 = (n0 + 2) % 10;
			
			n[i].rate = 1;
		}
		else if (n[i].etx == 2) {
	
			slot[1][n1] = i;
			re2[v2] = i;
			v2++;
			if (n1 + 2 >= 10)	n1++;
			n1 = (n1 + 2) % 10;
			n[i].rate = 1;
		}
		else if (n[i].etx == 3) {
	
			
			slot[2][n2] = i;
			re3[v3] = i;
			v3++;
			if (n2 + 2 >= 10)	n2++;
			n2 = (n2 + 2) % 10;
			n[i].rate = 1;
		}

	}
}

//parent선택 
void start(struct node n[20], int sen[10], int res[10],int resu[10]){	
	
	//보낼 node setting 
	deque <int> que1;
	//root parent인 경우  
	if (sen[0] == 0) {
		que1.push_back(0);
	}
	//다른 node가 parent 인 경우 
	else {
		for (int i = 0; i < 10; i++) {
			if (sen[i] != 0) {
				que1.push_back(sen[i]);
			}
		}	
	}
	//받을 node setting 
	deque <int> re1;
	deque <int> par1;
	int i = 0;
	//받을 node number의 array 정리하기 
	for (int i = 0; i < 10; i++) {
		//parent의 child 
		if (res[i] != 0) {
			re1.push_back(res[i]);
		}
	
	}
	
	while (que1.empty() != 1) {//보낼 node 존재 
		//parent DIO multicast
		int number = 0;

		int i = que1.at(0);//i==parent
		int parent = que1.at(0);
		que1.pop_front();//que1 = parent 
		cout << endl;
		printf("---------- parent : %d---------- \n\n", i);
		deque <int> re(re1);
		cout << "child 수 : "<<re.size() << endl << endl;

		while (re.empty() != 1) {//받을 node 존재 
			
			int k = re.at(0);//k==child
			int child = re.at(0);
			re.pop_front();
			//parent set에 추가 
			n[k].parent_set.insert(make_pair(n[i].node_number, n[i].etx));
			cout << "child "<<n[k].node_number << " : ";
			//받은 DIO 바탕으로 my_etx계산 
			srand((unsigned int)time(0));
			Sleep(500);
			int my_position = n[i].send.v[2] + (rand() % 100) + 10;
			int my_etx = my_position / 10;
			int pp = n[k].preferrednumber;
			// 기존 parent보다 좋거나 preferred parent가 NULL인 경우	

			if ( my_position < n[k].send.v[2] && n[i].cnc<4){ //&&n[i].cnc<n[pp].preferrednumber) { //i=parent k =child
			
				//기존 parent 보다 좋은 경우 기존 parent에게 parent를 바꾼다는 것을 알려준다. 
				//int val = parentchange(&n[k], &n[i]);
				//DIO 재구성 
				DAO da;
				da.dao[0] = n[i].node_number;//parent number
				da.dao[1] = n[k].node_number;//child number 
				//parent에게 DAO 전달 
				
				int p = DAOs(&n[k] ,&n[i]);//p=0 accept p=1 deny 

				//기존 parent에도 parent가 바뀐 것을 DAO를 보내서 알려준다.	
		
				int val = DAOACKs(&n[i], &n[k], p);
				
				//DIO 재구성 
				if (val != -1) {//accept	
					resu[number] = val;
					number++;
					CDIO(&n[k], &n[i], my_position);
					n[child].existparent = 100;
				}
			}
			else if (n[child].existparent == -10) {
				cout << "no parnet" << endl;
				n[child].existparent = 100;
				DAO da;
				da.dao[0] = n[parent].node_number;//parent number
				da.dao[1] = n[child].node_number;//child number 
				//parent에게 DAO 전달 
				int p = DAOs(&n[child], &n[parent]);//p=0 accept p=1 deny 

				//기존 parent에도 parent가 바뀐 것을 DAO를 보내서 알려준다.	
				int val = DAOACKs(&n[parent], &n[child], p);

				//Parent로 부터 Accept된 경우 DIO 재구성 
				if (val != -1) {
					//change DIO 
					CDIO(&n[child], &n[parent], my_position);
				}
			}
			else
			{
				cout << "Non happen" << endl;
			}
		}
	}
}