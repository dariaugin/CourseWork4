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

std::mutex m;
struct linkedListItem{
    linkedListItem *nextElement = NULL;
    
    std::string word;
    std::vector<int> files;
    
    public:
    linkedListItem(std::string word_): word(word_),files(){};
    
    void addNewFile(int file);
    void showAllFiles();
};


//parallelizm
void linkedListItem::addNewFile(int file){
    files.push_back(file);
}
void linkedListItem::showAllFiles(){
    for(int i = 0; i < files.size();i++){
        std::cout<< files[i] <<"\n";
    }
}

class linkedList{
    std::mutex segmentMutex;
    linkedListItem *head = NULL;
    int id;
    public:
    linkedList(int id_):id(id_),segmentMutex(){};
    linkedListItem* findElement(std::string data);
    linkedListItem* addElement(std::string data, int file);
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

linkedListItem* linkedList::addElement(std::string data, int file){
    sleep(id);
    m.lock();
    std::cout<<"Start...?"<<id<<"\n";
    m.unlock();
    //std::lock_guard<std::mutex> lock (segmentMutex);
    segmentMutex.lock();
    m.lock();
    std::cout<<"Inside"<<id<<"\n";
    m.unlock();
    linkedListItem* new_item = new linkedListItem(data);
    if(head != NULL){
        auto isExist = linkedList::findElement(data);
        if(isExist != nullptr){
            isExist->addNewFile(file);
            segmentMutex.unlock();
            m.lock();
            std::cout<<"Outside1 "<<id<<"\n";
            m.unlock();
            return isExist;
        }else{
            new_item->nextElement = head;
        }
    }
        head = new_item;
        head->addNewFile(file);
        segmentMutex.unlock();
    m.lock();
        std::cout<<"Outside2 "<<id<<"\n";
    m.unlock();
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
    int size;
    public:
    hashTable(int size_):size(size_){
        for(int i = 0; i<size_; i++){
            linkedList* l1 = new linkedList(i);
            buckets.push_back(l1);
            
        }
        //std::vector<linkedList> buckets_(size_);
        //buckets.resize(5, 0);
        //buckets = buckets_;
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
    return hashCode % size;
    
};

void hashTable::addElement(std::string word, int file){
    int position = hashTable::hashFunction(word);
    buckets[position]->addElement(word,file);
}

void hashTable::printElements(){
    for(int i = 0; i< buckets.size(); i++){
        buckets[i]->printList();
    }
}

int main(int argc, const char * argv[]) {
    

    hashTable* hash = new hashTable(6);
    
    //hash->addElement("b",2);
    //hash->addElement("a",5);
    //hash->addElement("c",3);
    //hash->printElements();
    
    std::thread t1(&hashTable::addElement,hash,"a",1);
    std::thread t2(&hashTable::addElement,hash,"a",2);
    std::thread t3(&hashTable::addElement,hash,"b",2);
    std::thread t4(&hashTable::addElement,hash,"c",2);
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    hash->printElements();
    return 0;
}
