//
//  main.cpp
//  Course_Work_Project
//
//  Created by Дар'я Угнівенко
//  Copyright © 2022 dddd. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <cmath>


struct linkedListItem{
    linkedListItem *nextElement = NULL;
    
    std::string word;
    std::vector<int> files;
    
    public:
    linkedListItem(std::string word_): word(word_),files(){};
    
    void addNewFile(int file);
    void showAllFiles();
};



void linkedListItem::addNewFile(int file){
    files.push_back(file);
}
void linkedListItem::showAllFiles(){
    for(int i = 0; i < files.size();i++){
        std::cout<< files[i] <<"\n";
    }
}

class linkedList{
    linkedListItem *head = NULL;
    
    public:
    linkedListItem* addElement(std::string data, int file);
    linkedListItem* findElement(std::string data);
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
    //auto isExist = linkedList::findElement(data);
    //if(isExist == nullptr){
    linkedListItem* new_item = new linkedListItem(data);
    //std::cout<<"wow:"<<new_item->word<<"\n";
    if(head != NULL){
        auto isExist = linkedList::findElement(data);
        if(isExist != nullptr){
            isExist->addNewFile(file);
            return isExist;
        }else{
            new_item->nextElement = head;
        }
    }
        head = new_item;
        head->addNewFile(file);
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
    std::vector<linkedList> buckets;
    int size;
    public:
    hashTable(int size_):size(size_){
        std::vector<linkedList> buckets_(size_);
        buckets = buckets_;
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
    buckets[position].addElement(word,file);
}

void hashTable::printElements(){
    for(int i = 0; i< buckets.size(); i++){
        buckets[i].printList();
    }
}

int main(int argc, const char * argv[]) {
    

    hashTable* hash = new hashTable(6);
    hash->addElement("a",1);
    hash->addElement("b",2);
    hash->addElement("a",5);
    hash->addElement("c",3);
    hash->printElements();
    
    return 0;
}
