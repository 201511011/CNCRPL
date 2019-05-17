#pragma once
#include <stdio.h>
#include <iostream>
#include <queue>
#include <deque>
#include <ctime>
#include <cstdio>
#include <map>
#include <Windows.h>
// 구조체 

struct DIO;
struct DAO;
struct DAO_ACK;
struct node;
struct DODAG;
struct packet;

//함수 
int DAOs(node *from, node *to);
int DAOACKs(node *par, node *chil, int ack);
void DIO_MUL(node *par, node *chil);
void CDIO(node *chil, node *par, int my_etx);
int parentchange(node *parent, node *child);
void DIO_multicast(struct node node[20], int nodenumber, int parent[10], int child[10]);
void trickletimer(struct node node[20]);
void printdodag(struct node node[20]);