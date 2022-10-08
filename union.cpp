#include<cstdio>
#include<string>
#include<iostream>
#include<vector>
#include<fstream>
#include<cmath>
using namespace std;

void Union(vector<vector<int>>& ans1,vector<vector<int>>& ans2){
								 
	int var1,var2,var3,var4;
	var1 = ans1[0][0];
	var2 = ans1[1][0];
	var3 = ans2[0][0];
	var4 = ans2[1][0];
	
	if(var1 == var3 && var2 == var4 ) {
		ans1[0].insert(ans1[0].end(),ans2[0].begin(),ans2[0].end());
		ans1[1].insert(ans1[1].end(),ans2[1].begin(),ans2[1].end());
		ans2[0].insert(ans2[0].end(),ans1[0].begin(),ans1[0].end());
		ans2[1].insert(ans2[1].end(),ans1[1].begin(),ans1[1].end());
	}
	
	for(int i = 0;i < 2;i++){
		for(int j = 0;j < ans1[0].size();j++){
			cout << ans2[i][j] << "\t";
		}	
		cout << endl;
	}
} 
