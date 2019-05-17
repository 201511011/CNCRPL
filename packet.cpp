#include <ctime>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <deque>
#include <ctime>
#include <cstdio>
#include <map>
#include <Windows.h>
#include "RPL.h"
#include "init.h"
#include <string>


using namespace std;
//packet은 upward만 전송한다 가정 
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

//node마다 packet 생성 하기 
void makepacket(struct node node[20]) {
	//root는 packet 생성 x 	
	Sleep(100);
	srand((unsigned int)time(0));
	
	int ps = 0;
	cout <<  "Packet Generate : " ;
	for (int i = 1; i < 20; i++) {
		
		ps = rand() % 10;
		if (ps < node[i].rate) {
			cout << node[i].node_number<<" ";
			node[i].mypacekt.generate++;
			node[i].packet.push_back(node[i].mypacekt);
		}	
	}
	cout << endl;
}


int slotframe(struct node node[20],int slot[3][10]) {
	//DIO 전송 시간 c++
	int i = 0;
	int child = 0,parent=0,overflow=0;
	
	// slotframe 전송 
	makepacket(node);
	for (int i = 0; i < 10; i++) {
		for (int k = 0; k < 3; k++) {
			child = slot[k][i];
		

			if (child != 0&&node[child].packet.size()!=0) {//node가 root가 아님 && 보낼 packet이 존재 
		
				parent = node[child].preferrednumber;
				cout << "child(" << child << ")" << "-->";
				cout << "parent(" << parent << ")";
			
				//Buffer overflow 
				if (node[parent].packet.size() >= 20) {
					cout << "  Buffer oveflow" << endl;
					node[child].packet.pop_front();
					node[parent].mypacekt.fail++;
					node[child].mypacekt.forward++;
					node[parent].fail + 1;
					overflow++;
				}
				//SUCCESS
				else
				{
					
					node[parent].packet.push_back(node[child].packet.at(0));
					node[child].packet.pop_front();
					node[child].mypacekt.forward++;
					cout << " Success" << endl;
					
				}
				
			}
		}

		if (node[0].packet.size() != 0) {
			node[0].packet.pop_front();
			node[0].mypacekt.forward++;
		}
	//각 node의 주기마다  slotframe 생성 
	}
	Sleep(1000);
	
	return overflow;
}

