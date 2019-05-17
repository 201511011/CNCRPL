#include <stdio.h>
#include <iostream>
#include <queue>
#include <deque>
#include <ctime>
#include <cstdio>
#include <map>
#include <Windows.h>
#include "RPL.h"
#include "packet.h"

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
	int etx=0;
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



void printdodag(struct node node[20]) {
	cout << endl << endl;
	for (int i = 0; i < 20; i++) {
		if (node[i].cnc != 0) {
			printf("%d's child / cnc: %d  ---", i, node[i].cnc);
			for (int q = 0; q < node[i].child.size(); q++) {
				cout << node[i].child[q] << " ";
			}
			cout << endl;
		}
	}
	cout << endl << endl;
}

struct DODAG {
	int array[10][10]; //routing table 저장 
};

int DAOs(node *chil, node *parent) {

	//parent가 child 받아줌 
	if ((parent->cnc) <= 3) {
		//	to->child.push_back(from->node_number);
		parent->dao_ack.val = 0;//accept 
		return 0;
	}
	//parent가 child거절함 
	else if ((parent->cnc) > 3) {
		parent->dao_ack.val = 1;//deny 
		return 1;
	}
}

int DAOACKs(node *par, node *chil, int ack) {
	//accept
	if (ack == 0) {

	//	printf("%d's child : %d\n", par->node_number, chil->node_number);
		cout << "Parent(" << par->node_number << ") --> Child(" << chil->node_number << ")" << endl;
		par->cnc++;
		par->child.push_back(chil->node_number);
		chil->preferred_parent = par;
		chil->parent_set.insert(make_pair(par->node_number, par->etx));
		chil->preferrednumber = par->node_number;
		cout << chil->node_number << "'s parent : " << chil->preferred_parent->node_number << endl;
	//	int number = par->node_number;
		return chil->node_number;
	}
	//deny
	else
		return -1;
}

void DIO_MUL(node *par, node *chil) {

	//child의 receive_DIO에 parent의 DIO를 넣어준다. 
	chil->receive.push_back(par->send);
	//parent set에 넣어준다. 
	chil->parent_set.insert(make_pair(par->node_number, par->etx));
	//값 count해준다. 
}

//change DIO 
void CDIO(node *chil, node *par, int my_position) {

	chil->etx = par->etx+1;
	chil->send.v[0] = chil->node_number;
	chil->send.v[1] = par->etx + 1;
	chil->send.v[2] = my_position;
	chil->send.v[3] = par->cnc;
}

int parentchange(node *parent, node *child) {

	DAO da;
	da.dao[0] = parent->node_number;//parent number
	da.dao[1] = child->node_number;//child number 
	//parent에게 DAO 전달 
	int p = DAOs(child, parent);//p=0 accept p=1 deny 

	//기존 parent에도 parent가 바뀐 것을 DAO를 보내서 알려준다.	
	int val = DAOACKs(child, parent, p);
	return val;
	//DIO 재구성 

}

void DIO_multicast(struct node node[20], int nodenumber, int parent[10], int child[10]) {

	int i = 0;
	int k = 0;
	int num = 0;
	int par = 0;
	//DIO_Multicast가 root인 경우 
	if (nodenumber == 0) {
		return;
	}
	//parent가 root인 경우 
	
	//parent에게 DIO 전송--> parent가 변경 된 것 인지 해서 조취 
	if (parent[0] != 0) {
		for (int i = 0; i < 10; i++) {
			par = parent[i];
			//parent가 남아있음 
			//parent가 root가 아닌 경우 
		
			if (par != 0) {	
				for (int i = 0; i < node[par].child.size(); i++) {
					num = node[par].child.at(0);
					
					//찾은 child는 해당 child가 아니다. 
					if (nodenumber != num || node[nodenumber].preferrednumber == par) {
						node[par].child.pop_front();
						node[par].child.push_back(num);
					}
					//해당 child 찾음 
					else if(nodenumber==num&&node[nodenumber].preferrednumber!=par) {
						
						node[par].child.pop_front();
						
						node[par].cnc--;
						cout << par << " emliminate " << nodenumber << " as a child" << endl;
						
					}
				}
			}
			//parent가 root인 경우 	
		}
	}

	//child에게 DIO 전송 
	if (child[0] != 0) {
		for (int i = 0; i < 10; i++) {
			//child에게 DIO 전송 --> 자신의 etx를 전송한다. 
			int chil = child[i];
			if (child[i]!=0&&chil != 0) {
				int my_position = node[nodenumber].send.v[2] + rand() % 100 + 10;
				int pp = node[chil].preferrednumber;
				//parent change 

				if (my_position < node[chil].send.v[2] && node[nodenumber].cnc < 4&&nodenumber!=node[chil].preferrednumber) {

					DAO da;
					//parent number
					da.dao[0] = nodenumber;
					//child number 
					da.dao[1] = chil;
					//parent에게 DAO 전달 
				
					int p = DAOs(&node[chil], &node[nodenumber]);
					//기존 parent에도 parent가 바뀐 것을 DAO를 보내서 알려준다.	
			
					int val = DAOACKs(&node[nodenumber], &node[chil], p);
					//Parent로 부터 Accept된 경우 DIO 재구성 
					if (val != -1) {
						//change DIO 
						CDIO(&node[chil], &node[nodenumber], my_position);
					}
			
				}
			}
		}
	}
	return;
}

void trickletimer(struct node node[20]) {

	for (int i = 1; i < 20; i++) {
		//매 slot frame마다 time을 감소 시켜 준다. 
		if (node[i].time != 0) {
			node[i].time = (node[i].time) - 1;
		}
		else {
			//5번이상 전송오류 발생하면 trickle timer reset 
			if (node[i].fail > 5) {
				node[i].period = 8;
			}
			//fail이 발생하지 않으면 trickle timer *2 진행 <62 
			else if (node[i].period < 64) {
				node[i].period * 2;
				node[i].time = node[i].period;
			}
		}
	}
	return;
}