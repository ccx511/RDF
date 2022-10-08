#include "utils.h"
#include <iostream>
#include <dirent.h>
#include <errno.h>
#include <mpi.h>
#include <map>
#include "SPARQLLexer.hpp"
#include "SPARQLParser.hpp"
#include "QueryLoad.cpp"
#include "query.cpp"
#include<chrono>

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}


int main(int argc, char** argv) {
	if (argc < 4) {
		throwException("Usage: QueryLoadEncoder <predicate_map_file> <subj_map_file> <query_files_directory>");
	}
	string line, directory = string(argv[3]);
	vector<string> files = vector<string>();
	boost::unordered_map<string, long long> verts_map, preds_map;
	long long id, max_predicate = numeric_limits<long long>::min(), max_verts = numeric_limits<long long>::min();
	char value[1000000];
	FILE * pFile;

	//MPI
	int myid, numprocs,dest;
	MPI_Status status;
	char message[100];
	char message1[100];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
       	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);


	if(myid == 0)
	{
			

		//create consistent_hash_ring
	        consistent_hash_t consistent_hash_;
        	hash_ring(consistent_hash_);
        	cout << "hash_ring create succeed" << endl;

		//Reading predicates dictionary
	        pFile = fopen(argv[1], "r");
        	while(fscanf(pFile, "%lld %[^\r\n] ", &id, value)==2){
                	if(id>max_predicate)
                        	max_predicate = id+1;
               		preds_map[value] = id;
        	}
        	fclose(pFile);


        	id = -1;
        	value[0] = '\0';

        	//Reading vertices dictionary
        	pFile = fopen (argv[2],"r");
        	while(fscanf(pFile, "%lld %[^\r\n] ", &id, value)==2){
                	if(id>max_verts){
                        	max_verts = id+1;
                	}
                	verts_map[value] = id;
        	}
        	fclose(pFile);

		auto t1 = std::chrono::system_clock::now();	
		getdir(directory,files);
        cout<<"Will encode "<<(files.size())<<" query files."<<endl;

		//process query
		cout<<"Number of vertices: "<<max_verts<<", Number of Predicates: "<<max_predicate<<endl;
		for(unsigned q = 0 ; q < files.size(); q++){
			cout << "reading parsing and encoding queries from file: " << files[q] << endl;
			load_encode_queries(directory+files[q], preds_map, verts_map, max_predicate, max_verts, consistent_hash_);

			cout << "done" << endl;
			cout.flush();

		}
		int flag = 0;
		vector<vector<vector<int>>>ans;
		while(true){
			int B[2][10000];
			MPI_Recv(&(B[0][0]),2*10000,MPI_INT,MPI_ANY_SOURCE,99,MPI_COMM_WORLD,&status);
			vector<vector<int>> result(2);
			if(B[0][0] == -2 && B[1][0] ==-2){
                                flag++;
                        }
			if(flag == 4) {
				break;
			}
			for(int j = 0;j < 10000;j++){
				if(B[0][j]==-2&&B[1][j]==-2){
					break;
				}
				result[0].push_back(B[0][j]);
				result[1].push_back(B[1][j]);
			}
	
			for(int i = 0;i < 2;i++){
				for(int j = 0;j < result[0].size();j++){
					//cout << "---------"<< result[i][j] << "\t";
				}
				//cout << "************************" << endl;
			}
			ans.push_back(result);
		}
		/*for(int i = 0;i < ans.size();i++){
			for(int j = 0;j < 2;j++){
				for(int k = 0;k < ans[i][0].size();k++){
					cout << ans[i][j][k] << "\t";
				}
				cout << endl;
			}
		}*/
		
		auto t2 = std::chrono::system_clock::now();
   		auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
		std::cout << " elapsed time: " << dt << "ms" << std::endl;

	}
	else{
		while(true){
			MPI_Recv(message1, 100, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
			cout << "process" << myid << message1 << endl;
			string str = message1;
			if(str == "exit"){
				int **A;
                        	A = alloc_2d_int(2,10000);
				A[0][0] = -2;
				A[1][0] = -2;
				MPI_Send(&(A[0][0]),2*10000,MPI_INT,0,99,MPI_COMM_WORLD);
				break;
			}
			int flag1 = str.find_last_of("<");
			int flag2 = str.find_last_of(">");
			int len = str.size();
		

			Elm Sub, Pred, Obj;
			Sub.value = "";
			Pred.value = "";
			Obj.value = "";
			for(int i = 0;i < flag1;i++){
				Sub.value += str[i];
			}
			Sub.isVar = str[len-2]-'0';
			for(int i = flag1;i < flag2+1;i++){
                	        Pred.value += str[i];
                	}
			Pred.isVar = 0;
			for(int i = flag2+1;i < len-2;i++){
                        	Obj.value += str[i];
                	}
			Obj.isVar = str[len-1]-'0';
			Triple triple;
			triple.sub = Sub;
			triple.obj = Obj;
			triple.pred = Pred;

			cout << "process" << myid << "\t" << triple.sub.value << "\t" << triple.sub.isVar << endl;
			cout << "process" << myid << "\t" << triple.pred.value << "\t" << triple.pred.isVar << endl;
			cout << "process" << myid << "\t" << triple.obj.value << "\t" << triple.obj.isVar << endl;
			cout << myid << "**************" << message1 << "\t" << str   <<  endl;
			string filename = "node" + std::to_string(myid);
			vector<vector<int>> result(2);
			Query(triple,result,filename);

	 		/*for(int i = 0;i < 2;i++){
    				for(int j = 0;j < result[0].size();j++){
    					cout << result[i][j] << "\t" ;
    				}
    				cout << endl;
    			}*/
			int m = result[0].size();
			int **A;
			A = alloc_2d_int(2,10000);
			for(int i = 0;i < 2;i++){
				for(int j = 0;j <= m;j++){
					if(j == m){
						A[i][j] = -2;
					}
					else{
						A[i][j] = result[i][j];
					}
				}
			}
			
			for(int i = 0;i < 2;i++){
                for(int j = 0;j < 10;j++){
                        cout << A[i][j] << "\t";
                }
            cout << endl;
            }
			MPI_Send(&(A[0][0]),2*10000,MPI_INT,0,99,MPI_COMM_WORLD);
			free(A[0]);
			free(A);
		}
	}

	MPI_Finalize();
	return 0;
}
