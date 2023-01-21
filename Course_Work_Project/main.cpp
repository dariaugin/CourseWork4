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
#include <dirent.h>
using namespace std::chrono;
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <thread>

#define PORT 8080



#define AMOUNTF 200
#define AMOUNTTH 2
#define HASHSIZE 8


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
    void getFiles(nodeTree *node,std::vector<int>& f);
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
        //std::cout<<"left \n";
            BST::showTree(node->left);
        }if(node->right != NULL){
            //std::cout<<"right \n";
            BST::showTree(node->right);
        }
        
    }else{
        std::cout<<"error- "<<node->key;
    }
}

void BST::getFiles(nodeTree *node, std::vector<int>& f){
    if(node-> key != NULL ){
        f.push_back(node->key);
        if(node->left != NULL){
             BST::getFiles(node->left, f);
        }
        if(node->right != NULL){
            BST::getFiles(node->right, f);
        }
    }
    
}



struct linkedListItem{
    linkedListItem *nextElement = NULL;
    std::string word;
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
    linkedListItem *head = NULL;
    int id;
    public:
    linkedList(int id_):id(id_){};
    linkedListItem* findElement(std::string data);
    linkedListItem* addElement(std::string data, int file, int position);
    std::vector<int> findElement_getFiles(std::string data);
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

std::vector<int> linkedList::findElement_getFiles(std::string data){
    linkedListItem* current = head;
    //std::vector<int>* files;
    while(current != NULL){
        if(current->word == data){
            std::vector<int> files;
            current->files.getFiles(current->files.root, files);
            return files;
            
        }
        current = current->nextElement;
    }
    return {};
};

linkedListItem* linkedList::addElement(std::string data, int file,int position){
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
    int size;
    public:
    hashTable(int size_ ):size(size_){
        for(int i = 0; i<size_; i++){
            linkedList* l1 = new linkedList(i);
            buckets.push_back(l1);
        }
    }
    int hashFunction(std::string &word);
    void addElement(std::string word, int file);
    std::vector<int> findElement(std::string word);
    void printElements();
};

int hashTable::hashFunction(std::string &word){
    int prime = 31;
    int hashCode = 0;
    for (int i = 0; i < word.length(); i++) {
        hashCode += word[i] * pow(prime, i);
    }
    if( hashCode % size < 0){
        return -1 * (hashCode % size);
    }
    return hashCode % size;
    
};

void hashTable::addElement(std::string word, int file){
    int position = hashTable::hashFunction(word);
    buckets[position]->addElement(word,file,position);
    
}

void hashTable::printElements(){
    for(int i = 0; i< buckets.size(); i++){
        buckets[i]->printList();
    }
}

std::vector<int> hashTable::findElement(std::string word){
    int position = hashTable::hashFunction(word);
    return buckets[position]->findElement_getFiles(word);
}

std::string erase_notalpha(std::string word){
    
    int i = 0;
    int size = word.length();
    while(i < size){
        if (!isalnum(word[i]) || word[i] == ' '){
        word.erase(i,1);
        size--;
        }else
        i++;
    }
    return word;
}
std::string process_word(std::string word){
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    return erase_notalpha(word);
}

void processFile(std::string name, int id, hashTable* hash)
{
    
    std::ifstream file ("/Users/daraugnivenko/Documents/my_files_test/" + name);
    if (!file.is_open()) return;
    
    std::string word;
    while (file >> word)
    {
        
        hash->addElement(process_word(word),id);
    }
}

std::string getTheFile(int id){
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;
    id += 2;
    dp = opendir("/Users/daraugnivenko/Documents/my_files_test");
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
    
    int start = id_of_thread*(AMOUNTF/AMOUNTTH) + 1;
    int end = start +  AMOUNTF/AMOUNTTH;
    int current = start;

    while(start < end && current <= AMOUNTF){
        
        std::string name = getTheFile(current);
        
        processFile(name, current, hash);
        current++;
    }
}


struct Server{
    int master_socket , addrlen , new_socket , client_socket[30] ,
    max_clients = 50 , activity, valread , sd, maxsd;
    struct sockaddr_in address;
    hashTable *hashh;
    fd_set readfds;
    std::vector<std::thread> threads;
    void start(){
        startHash();
        bind_and_llisten();
        handleConnection();
    }
    
    void bind_and_llisten(){
        if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
            {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
        
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons( PORT );
            
            
            if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
            {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
            printf("PORT %d \n", PORT);
            
            if (listen(master_socket, 3) < 0)
            {
                perror("listen failed");
                exit(EXIT_FAILURE);
            }

            addrlen = sizeof(address);
            printf("SERVER waiting for connections ...");
    }
    void handleConnection(){

        while(true)
        {
            FD_ZERO(&readfds);
            FD_SET(master_socket, &readfds);
            maxsd = master_socket;

            activity = select( maxsd + 1 , &readfds , NULL , NULL , NULL);
            
            if ((activity < 0) && (errno!=EINTR)){
                    printf("select failed");
            }
            if (FD_ISSET(master_socket, &readfds))
                {
                    handleNewConnection();
                }
            }
    }
    
    
    std::vector<int> findWordUser(int ns, std::string word, hashTable *hash){
        return hash->findElement(word);
    }
    
    
    
    void startNewUserSession(int ns){
        char word[5000];
        const char* mes = "Server waiting for your data...\nWhat word do you want to find?\n";
        write( ns, mes , strlen(mes));
        
        read( ns, word, sizeof(word));
        std::string str_(word);
        std::cout<<"Searching for word ::: "<< word<<"\n";
        std::vector<int> result = findWordUser(ns, word, hashh);
        
        std::string result_str = "Files: ";
        for(int i = 0; i < result.size(); i++){
            result_str += std::to_string(result[i]) + " ";
        }
        
        char const *result_char = result_str.c_str();
        write(ns, result_char, strlen(result_char));
        std::cout<<result_char<<"\n";
        

    }
    
    void startNewThreadUser(int ns){
        std::thread th = std::thread(&Server::startNewUserSession,this,ns);
        th.detach();
    }

    void handleNewConnection(){
        if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            if (new_socket > maxsd) {
                maxsd = new_socket;
                
            }
        
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                (address.sin_port));
                
            int status[2];
        
                if(read( new_socket, status, sizeof(status))){
                    
                    startNewThreadUser(new_socket);

                }else{
                    perror("unknown connection");
                    exit(EXIT_FAILURE);
                }
        }
    
    void startHash(){
        std::vector<std::thread> threads;
        hashTable* hash = new hashTable(HASHSIZE);
        
        auto start = high_resolution_clock::now();
        for(int i = 0; i<AMOUNTTH; i++){
        
            threads.push_back(std::thread(startThread, i, hash));
        }
        
        for(int i = 0; i<AMOUNTTH; i++){
            threads[i].join();
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        std::cout <<"time: "<<duration.count() << "\n";
        hash->printElements();
        hashh = hash;
    }
};


int main(int argc, const char * argv[]) {
    Server server;
    server.start();
    return 0;
}
