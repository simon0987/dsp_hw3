//
//  main.cpp
//  DSP_hw3
//
//  Created by 柯哲邦 on 2018/12/8.
//  Copyright © 2018年 柯哲邦. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include "Vocab.h"
#include "Ngram.h"

using namespace std;
//global variables
map<string,int> myMap;
vector<vector<string> > myVec;

int buildMap(const char* filename){
    fstream file;
    file.open(filename);
    string rawline;
    int counter = 0;
    while(getline(file,rawline)){
        stringstream ss(rawline);
        if(ss.good()){
            string key;
            int value = counter;
            ss >> key;
            myMap.insert(pair<string, int>(key,value));
            myVec.push_back(vector<string>());
            while(ss.good()){
                string ch;
                ss >> ch;
                myVec[counter].push_back(ch);
            }
            counter++;
        }
    }
}

//get log P(w2|w1)
double calclogProb(string w1,string w2,Ngram &lm,Vocab &voc){
    //do bigram
    VocabIndex unk =voc.getIndex(Vocab_Unknown);
    VocabIndex w1_index = voc.getIndex(w1.c_str());
    VocabIndex w2_index = voc.getIndex(w2.c_str());
    if(w1_index == Vocab_None) //didn't find
        w1_index = unk;
    if(w2_index == Vocab_None)
        w2_index = unk;
    VocabIndex context[] = {w1_index,Vocab_None};
    LogP bigramP = lm.wordProb(w2_index, context);
    if(bigramP == LogP_Zero){
        VocabIndex context[] = {Vocab_None};
        LogP unigramP = lm.wordProb(w2_index, context);
        if(bigramP == LogP_Zero){
            return -1e9;
        }
        else
            return unigramP;
    }
    else
        return bigramP;
}

string searchViterbi(string rawline, Ngram &lm,Vocab &voc){
    vector<string> original;
    vector<vector<string> > grid;
    vector<vector<double> > prob;
    vector<vector<int> > psi;
    
    //rawline to vocab index
    stringstream ss(rawline);
    int counter = 0;
    while(ss.good()){
        string ch;
        ss >> ch;
        original.push_back(ch);
        grid.push_back(vector<string>());
        prob.push_back(vector<double>());
        psi.push_back(vector<int>());
        int index = myMap[ch];
        for(int i = 0; i < myVec[index].size();++i){
            grid[counter].push_back(myVec[index][i]);
            prob[counter].push_back(-1e9);
            psi[counter].push_back(0);
        }
        counter++;
    }
    
    //do log viterbi
    for(int i = 0 ; i < grid[0].size();++i){
        prob[0][i] = calclogProb("<s>",grid[0][i],lm,voc);
    }
    
    for(int i = 1;i < counter;i++){
        for(int j = 0; j < grid[i].size();j++){
            for(int k = 0;k < grid[i-1].size();k++){
                double w1w2P = calclogProb(grid[i-1][k], grid[i][j], lm, voc);
                if(prob[i-1][k] + w1w2P > prob[i][j]){
                    prob[i][j] = prob[i-1][k] + w1w2P;
                    psi[i][j] = k;
                }
            }
        }
    }
    
    //do backtrack
    double max = -1e9;
    int arg_max = 0;
    for(int i = 0 ; i < grid[counter-2].size();i++){
        if(prob[counter-2][i] >= max){
            max  = prob[counter-2][i];
            arg_max = i;
        }
    }
    vector<string> backtrack;
    for(int i = counter-2;i >= 0; i--){
        backtrack.push_back(grid[i][arg_max]);
        arg_max = psi[i][arg_max];
    }
    string ans = "<s>";
    for(int i = backtrack.size()-1;i >= 0; i--){
        ans += " " + backtrack[i];
    }
    ans+=" </s>";
    return ans;
    return "debug";
}
int main(int argc, const char * argv[]) {
    int ngram_order = 2;
    Vocab voc;
    Ngram lm(voc, ngram_order);
    
    File lmfile(argv[3],"r");
    lm.read(lmfile);
    buildMap(argv[2]);
    
    fstream test;
    test.open(argv[1]);
    string rawline;
    ofstream outfile(argv[4]);
    while(getline(test,rawline)){
        string result = searchViterbi(rawline, lm, voc);
        outfile << result << endl;
    }
    return 0;
}
