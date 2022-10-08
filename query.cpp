#include<cstdio>
#include<string>
#include<iostream>
#include<vector>
#include<fstream>
using namespace std;



int strToInt(string str,int first,int last){
	int val = 0;
	for(int i = first+1;i < last; i++){
		val = str[i]-'0' + val*10;
	}
	return val;
}

int Query(Triple triple,vector<vector<int>>& result,string filename){
	
	std::fstream  f(filename);
    string  line;
    int sub1, sub2, pre1, pre2, obj1, obj2;
    int sub,pre,obj;
    int pred;
    int var1,var2;
    
    if(triple.sub.isVar == 0){
	int val = strToInt(triple.sub.value,0,triple.sub.value.size()-1);
    	var2 = triple.obj.value[2] - '0';
    	result[0].push_back(val*-1-1);
    	result[1].push_back(var2);
    }
    else if(triple.obj.isVar == 0){
    	var1 = triple.sub.value[2] - '0';
	int val = strToInt(triple.obj.value,0,triple.obj.value.size()-1);
    	result[0].push_back(var1);
    	result[1].push_back(val*-1-1);
    }
    else{
    	var1 = triple.sub.value[2] - '0';
    	var2 = triple.obj.value[2] - '0';
    	result[0].push_back(var1);
    	result[1].push_back(var2);
    }
    
    cout << "variable: "<< result[0][0] << "\t" << result[1][0] << endl;
    pred = strToInt(triple.pred.value,0,triple.pred.value.size()-1);
    cout << pred << endl;
    
    while(getline(f,line))
    {	
    	sub1 = line.find_first_of("<");
    	sub2 = line.find_first_of(">");
    	pre1 = line.find_last_of("<");
    	pre2 = line.find_last_of(">");
    	obj1 = line.find_first_of("\"");
    	obj2 = line.find_last_of("\"");
    	
    	sub = strToInt(line,sub1,sub2);
    	pre = strToInt(line,pre1,pre2);
    	obj = strToInt(line,obj1,obj2);
    	
    	//cout << sub << "\t" << pre << "\t" << obj << endl;
    	
    	if(pre == pred){
    		if(triple.sub.isVar == 0){
    			if(sub == strToInt(triple.sub.value,0,triple.sub.value.size()-1)){
    				result[1].push_back(obj);
    				result[0].push_back(-1);
    				cout << line << endl;
    			}
    		}
    		else if(triple.obj.isVar == 0){
    			if(obj == strToInt(triple.obj.value,0,triple.obj.value.size()-1)){
    				result[1].push_back(-1);
    				result[0].push_back(sub);
    				cout << line << endl;
    			}
    		}
    		else {
    			result[0].push_back(sub);
    			result[1].push_back(obj);
    			cout << line << endl; 
    		}
    	}
    }
	return 0;
}
 
