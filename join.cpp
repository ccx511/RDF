#include<cstdio>
#include<string>
#include<iostream>
#include<vector>
#include<fstream>
#include<cmath>
using namespace std;

void interVector(vector<vector<int>>& ans1,vector<vector<int>>& ans2,int tag){
	int len1 = ans1[0].size();
	int len2 = ans2[0].size();
	int flag = 0;
	for(int i = 1;i < len1;i++){
		for(int j = 1;j < len2;j++){
			if(ans1[tag][i]==ans2[0][j]){
				if(ans1[1-tag][i]!=ans2[1][j]){
					ans1[tag][i] = -1;
					ans1[1-tag][i] = -1;
					ans2[0][j] = -1;
					ans2[1][j] = -1;
				}
				flag = 1;
			}
			
		}
		if(flag == 0){
			ans1[tag][i] = -1;
			ans1[1-tag][i] = -1;
		}
		flag = 0;
	}
	for(int i = 1;i < len2;i++){
		for(int j = 1;j < len1;j++){
			if(ans2[tag][i]==ans1[0][j]){
				if(ans2[1-tag][i]!=ans1[1][j]){
					ans2[tag][i] = -1;
					ans2[1-tag][i] = -1;
					ans1[0][j] = -1;
					ans1[1][j] = -1;
				}
				flag = 1;
			}
		}
		if(flag == 0){
			ans2[tag][i] = -1;
			ans2[1-tag][i] = -1;
		}
		flag = 0;
	}
}


void intersection(vector<int>& ans1,vector<int>& ans2){
	if(ans1[0]!=ans2[0]){
		return; 
	}
	int len1 = ans1.size();
	int len2 = ans2.size();
	int flag = 0;
	for(int i = 1;i < len1;i++){
		for(int j = 1;j < len2;j++){
			if(ans1[i] == ans2[j]){
				flag = 1;
			}
		}
		if(flag == 0){
			ans1[i] = -1;
		}
		flag = 0;
	}
	for(int i = 1;i < len2;i++){
		for(int j = 1;j < len1;j++){
			if(ans2[i] == ans1[j]){
				flag = 1;
			}
		}
		if(flag == 0){
			ans2[i] = -1;
		}
		flag = 0;
	}
	
	cout << "----------------------------------------" << endl;
	for(int i = 0; i < len1;i++){
		cout << ans1[i] << "\t"; 
	}
	cout << endl; 
	
	for(int i = 0; i < len2;i++){
		cout << ans2[i] << "\t"; 
	}
	cout << endl; 
	cout << "----------------------------------------" << endl;
}

void inter(vector<vector<int>>& ans1,vector<int>& ans2){
	int var1 = ans1[0][0];
	int var2 = ans1[1][0];
	int var3 = ans2[0];
	if(var1!=var3 && var2 != var3){
		return;
	}
	if(var1 == var3){
		intersection(ans1[0],ans2);
	}else{
		intersection(ans1[1],ans2);
	}
	for(int i = 1;i < ans1[0].size();i++){
		if(ans1[0][i] == -1 || ans1[1][i] == -1){
			ans1[0][i] = -1;
			ans1[1][i] = -1;
		}
	}
}

void Join(vector<vector<int>>& ans1,vector<vector<int>>& ans2){
	
								 
	for(int i = 0;i < ans1.size();i++){
		for(int j = 0;j < ans1[0].size();j++){
			cout << ans1[i][j] << "\t";
		}
		cout << endl;
	}
	for(int i = 0;i < ans2.size();i++){
		for(int j = 0;j < ans2[0].size();j++){
			cout << ans2[i][j] << "\t";
		}
		cout << endl;
	}
	
	int var1,var2,var3,var4;
	var1 = ans1[0][0];
	var2 = ans1[1][0];
	var3 = ans2[0][0];
	var4 = ans2[1][0];
	
	cout << "变量为:" << endl; 
	cout << var1 << "\t" << var2 << "\t" << var3 << "\t" << var4 << endl;
	cout << endl;
	
	int len1 = ans1[0].size();
	int len2 = ans2[0].size();
	if(var1!=-1 && var2!=-1 && var3!=-1 && var4!=-1){
		cout << "四个都是变量" << endl; 
		if(var1 == var3 || var1 == var4){
			if(var2 == var3){
				interVector(ans1,ans2,1);
			}else if(var2 == var4){
				interVector(ans1,ans2,0);
			}else{
				inter(ans2,ans1[0]); 
				for(int i = 0;i < ans1[0].size();i++){
					if(ans1[0][i] == -1){
						ans1[1][i] = -1;
					}
				}
			}
		}else if(var2 == var3 || var2 == var4){
			inter(ans2,ans1[1]); 
			for(int i = 0;i < ans1[0].size();i++){
				if(ans1[1][i] == -1){
					ans1[0][i] = -1;
				}
			}
		}
		
	}
	
	else if(var1+var2+var3+var4+2 == abs(var1)+abs(var2)+abs(var3)+abs(var4)){
		
		cout << "有一个不是变量" << endl; 
		
		if(var1 == -1){
			inter(ans2,ans1[1]);
		}else if(var2 == -1){
			inter(ans2,ans1[0]);
		}else if(var3 == -1){
			inter(ans1,ans2[1]);
		}else{
			inter(ans1,ans2[0]);
		}
		
		
	}
	
	
	else{
		cout << "有两个不是变量" << endl; 
		if(var1 == -1){
			if(var3 == -1){
				intersection(ans1[1],ans2[1]);
			}else{
				intersection(ans1[1],ans2[0]);
			}
		} 
		if(var2 == -1){
			if(var3 == -1){
				intersection(ans1[0],ans2[1]);
			}else{
				intersection(ans1[0],ans2[0]);
			}
		} 
		
	}	
}
 
