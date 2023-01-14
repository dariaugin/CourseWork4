//
//  main.cpp
//  lab3par
//
//  Created by Дарья Угнивенко on 07.12.2022.
//  Copyright © 2022 dddd. All rights reserved.
//
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <fstream>
#include "server.hpp"
#include <dirent.h>


std::mutex m;
std::mutex p;
#define AMOUNTF 3
#define AMOUNTTH 3



struct nodeTree{
    int key = NULL;
    nodeTree *left = NULL;
    nodeTree *right = NULL;
    
    nodeTree(int key_):key(key_){};
};



struct BST{
    nodeTree *root = NULL;
    
    void addNode(int key);
    void checkNode(nodeTree *node, nodeTree *nodeToAdd);
    nodeTree * searchNode(nodeTree *node,int key);
    void showTree(nodeTree *node);
};

nodeTree * BST::searchNode(nodeTree *node, int key){

    if(node == NULL || node->key == key){
        return node;
    }
    if(node->key > key){
        return this->searchNode(node->left, key);
    }
        return this->searchNode(node->right, key);
    
    
}


void BST::checkNode(nodeTree *node, nodeTree *nodeToAdd){
    if(nodeToAdd->key < node->key){
        if(node->left != NULL){
            checkNode(node->left, nodeToAdd);
        }else{
            node->left =nodeToAdd;
        }
    }else if(nodeToAdd->key > node->key ){
        if(node->right != NULL){
            checkNode(node->right, nodeToAdd);
        }else{
            node->right =nodeToAdd;
        }
    }
    
    
}

void BST::addNode(int key){
    nodeTree* newNode = new nodeTree(key);
    if(root == NULL){
        root = newNode;
    }else{
        checkNode(root, newNode);
    }
}

void BST::showTree(nodeTree *node){
    if(node-> key != NULL ){
        std::cout<< node->key<<"\n";
        if(node->left != NULL){
        std::cout<<"left \n";
            BST::showTree(node->left);
        }if(node->right != NULL){
            std::cout<<"right \n";
            BST::showTree(node->right);
        }
        std::cout<<"-----\n";
    }else{
        std::cout<<"error- "<<node->key;
    }
}


struct linkedListItem{
    linkedListItem *nextElement = NULL;
    
    std::string word;
    //std::vector<int> files;
    BST files;
    public:
    linkedListItem(std::string word_): word(word_),files(){};
    
    void addNewFile(int file);
    void showAllFiles();
};

void linkedListItem::addNewFile(int file){
    files.addNode(file);
}
void linkedListItem::showAllFiles(){
    files.showTree(files.root);
}

class linkedList{
    std::mutex segmentMutex;
    std::mutex &accessMutex;
    linkedListItem *head = NULL;
    int id;
    public:
    linkedList(int id_, std::mutex &accessMutex_):id(id_), accessMutex(accessMutex_){};
    linkedListItem* findElement(std::string data);
    linkedListItem* addElement(std::string data, int file, int position);
    void printList();
};



linkedListItem *linkedList::findElement(std::string data){
    linkedListItem* current = head;
    while(current != NULL){
        if(current->word == data){
            return current;
        }
        current = current->nextElement;
    }
    return nullptr;
};

linkedListItem* linkedList::addElement(std::string data, int file,int position){
    accessMutex.unlock();
    segmentMutex.lock();
    linkedListItem* new_item = new linkedListItem(data);
    if(head != NULL){
        auto isExist = linkedList::findElement(data);
        if(isExist != nullptr){
            isExist->addNewFile(file);
            segmentMutex.unlock();
            return isExist;
        }else{
            new_item->nextElement = head;
        }
    }
        head = new_item;
        head->addNewFile(file);
    segmentMutex.unlock();
        return new_item;

};

void linkedList::printList(){
    linkedListItem* current = head;
    
    while(current != NULL){
        std::cout<<"word:"<< current->word<<"\n";
        current->showAllFiles();
        current = current->nextElement;
    }
}


class hashTable{
    std::vector<linkedList*> buckets;
    std::mutex accessMutex;
    int size;
    public:
    hashTable(int size_ ):size(size_){
        for(int i = 0; i<size_; i++){

            linkedList* l1 = new linkedList(i, std::ref(accessMutex));
            buckets.push_back(l1);
            
        }
        
    }
    int hashFunction(std::string &word);
    void addElement(std::string word, int file);
    void findElement();
    void printElements();
    
    
};

int hashTable::hashFunction(std::string &word){

    const int PRIME_CONST = 31;
    int hashCode = 0;
    for (int i = 0; i < word.length(); i++) {
        hashCode += word[i] * pow(PRIME_CONST, i);
    }
    if( hashCode % size < 0){
        return -1 * (hashCode % size);
    }
    return hashCode % size;
    
};

void hashTable::addElement(std::string word, int file){
    int position = hashTable::hashFunction(word);
    
    accessMutex.lock();
    buckets[position]->addElement(word,file,position);
    
}

void hashTable::printElements(){
    for(int i = 0; i< buckets.size(); i++){
        buckets[i]->printList();
    }
}


void processFile(std::string name, int id, hashTable* hash)
{
    
    std::ifstream file ("/Users/daraugnivenko/Documents/courseFiles/" + name);
    std::cout<<name;
    if (!file.is_open()) return;
    
    std::string word;
    while (file >> word)
    {
        
        hash->addElement(word,id);
    }
}

std::string getTheFile(int id){
    //p.lock();
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;
    id += 2;
    dp = opendir("/Users/daraugnivenko/Documents/courseFiles");
    if (dp != nullptr) {
        int i = 1;
        while ((entry = readdir(dp))){
            if(id == i){
            std::string res = entry->d_name;
            closedir(dp);
            //p.unlock();
            return res;
                
            }
            i++;
        }
    }
    return NULL;
}



void startThread(int id_of_thread, hashTable* hash){
    
    int start = id_of_thread + 1;
    int end = id_of_thread + AMOUNTF/AMOUNTTH;
    int current = start;
    while(start <= end && current <= AMOUNTTH){
        
        std::string name = getTheFile(current);
        processFile(name, current, hash);
        current++;
    }
}

int main(int argc, const char * argv[]) {
    
    std::vector<std::thread> threads;
    hashTable* hash = new hashTable(6);
    

    
    //hash->printElements();
   
    /*std::thread t1(&hashTable::addElement,hash,"a",1);
    std::thread t2(&hashTable::addElement,hash,"a",2);
    std::thread t3(&hashTable::addElement,hash,"b",2);
    std::thread t4(&hashTable::addElement,hash,"c",2);
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    hash->printElements();*/
    
    for(int i = 0; i<AMOUNTTH; i++){
    
        threads.push_back(std::thread(startThread, i, hash));
    }
    
    for(int i = 0; i<AMOUNTTH; i++){
        threads[i].join();
    }
    
    hash->printElements();
    
    return 0;
}
