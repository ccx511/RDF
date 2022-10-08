//============================================================================
// Name        : QueryLoadEncoder.cpp
// Version     :
// Copyright   : KAUST-Infocloud
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <boost/crc.hpp>
#include "utils.h"
#include <iostream>
#include <dirent.h>
#include <errno.h>
#include "SPARQLLexer.hpp"
#include "SPARQLParser.hpp"
#include "hash.cpp"
using namespace std;

int getdir (string dir, vector<string> &files)
{
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(dir.c_str())) == NULL) {
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}

	while ((dirp = readdir(dp)) != NULL) {
		if(((int)dirp->d_type) == 8)
			files.push_back(string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}
struct Elm {
	std::string value;
	bool isVar;
	//Elm(std::string value,bool isVar);
};

struct Triple {
	Elm sub,pred,obj;
	//Triple(Elm sub,Elm pred,Elm obj);
};


void  encode_query(SPARQLParser & parser, ofstream &stream, boost::unordered_map<string, long long>& predicate_map, boost::unordered_map<string, long long>& subj_map, long long max_predicate, long long max_verts, consistent_hash_t& consistent_hash_) {
	boost::unordered_map<string, long long>::iterator it;
	
	int varSub = 0;
	int varObj = 0;
	std::string sub;
	std::string pred;
	std::string obj;
	std::vector<Triple>Qry;
	int knownSub = 0;	
	int knownObj = 0;
	std::vector<Triple>QuerySub;
	std::vector<Triple>QueryObj;
	std::vector<Triple>Query;
	std::vector<Triple>QuerySorted;
	
	for(unsigned i = 0 ; i < parser.projection.size() ;i++){
		stream<<"?"<<parser.getVariableName(parser.projection[i])<<" ";
	}
	stream << endl;
	
	
	//************************************************************* 
	for(unsigned i= 0; i < parser.patterns.patterns.size(); i++){
		if(parser.patterns.patterns[i].subject.type == SPARQLParser::Element::Variable){
			sub = "?" + parser.getVariableName(parser.patterns.patterns[i].subject.id);
			varSub = 1;
		}
		else if(parser.patterns.patterns[i].subject.type == SPARQLParser::Element::IRI){
			varSub = 0;
			stringstream ss;
			it = subj_map.find(parser.patterns.patterns[i].subject.value);
			if (it == subj_map.end()) {
				ss << "<" << max_verts << ">"; 
			}
			else
				ss << "<" << it->second << ">"; 
			sub  = ss.str();
		}
		else{
			varSub = 0;
			stringstream ss;
			it = subj_map.find(parser.patterns.patterns[i].subject.value);
			if (it == subj_map.end()) {
				ss <<  "\"" << max_verts << "\""; 
			}
			else
				ss << "\"" << it->second << "\""; 
			sub = ss.str();
		}
		
		stringstream sp;
		it = predicate_map.find(parser.patterns.patterns[i].predicate.value);
		if (it == predicate_map.end()) {
			sp << "<" << max_verts << ">";
			pred = sp.str();
		}
		else	
			sp << "<" << it->second << ">";
			pred = sp.str();
		
		stringstream so;
		if(parser.patterns.patterns[i].object.type == SPARQLParser::Element::Variable){
			obj = "?" + parser.getVariableName(parser.patterns.patterns[i].object.id);
			varObj = 1;
		}
		else if(parser.patterns.patterns[i].object.type == SPARQLParser::Element::IRI){
			varObj = 0;
			it = subj_map.find(parser.patterns.patterns[i].object.value);
			if (it == subj_map.end()) {
				so <<  "<" << max_verts <<  ">";
			}
			else
				so << "<" << it->second <<  ">";
			obj = so.str();
		}
		else{
			varObj = 0;
			it = subj_map.find(parser.patterns.patterns[i].object.value);
			if (it == subj_map.end()) {
				so <<  "\"" << max_verts <<  "\"";
			}
			else
				so <<  "\"" << max_verts <<  "\"";
			obj = so.str();
		}
		
		cout << sub << "   " << pred << "   " << obj << endl;
		Elm Sub; Sub.value = sub; Sub.isVar = varSub;
		Elm Pred; Pred.value = pred; Pred.isVar = 0;
		Elm Obj; Obj.value = obj; Obj.isVar = varObj;
		Triple triple;
		triple.sub = Sub;
		triple.obj = Obj;
		triple.pred = Pred;
		Qry.push_back(triple);
		if(varSub == 0){
			QuerySub.push_back(triple);
		}
		else if(varObj == 0){
			QueryObj.push_back(triple);
		}
		else{
			Query.push_back(triple);
		}
	}
	for(unsigned i = 0;i < Qry.size();i++){
		cout << Qry[i].sub.value << "\t " << Qry[i].sub.isVar << endl;
		cout << Qry[i].pred.value << "\t " << Qry[i].pred.isVar << endl;
		cout << Qry[i].obj.value << "\t " << Qry[i].obj.isVar << endl;
	}

	cout << "******************************************" << endl;

	for(unsigned i = 0;i < QuerySub.size();i++){

		//calculate hash code of subject
		string str = QuerySub[i].sub.value;
		boost::crc_32_type ret;
		ret.process_bytes(str.data(),str.length());
		int hashnum = ret.checksum();
		std::cout<<"========================================================="<<std::endl;
		std::cout << str<< "'s hashcode:" ;
		std::cout << ret.checksum() << std::endl;

		//send query to node
		consistent_hash_t::iterator ite;
	   	ite = consistent_hash_.find(hashnum);
		cout << ite->second.node_id << "-------------------------" << endl;
		//std::cout<<boost::format("node:%1%,vnode:%2%,hash:%3%") % nodes[ite->second.node_id] % ite->second.vnode_id % ite->first << std::endl;
	        /*char number = nodes[ite->second.node_id][12];
		cout << "-------------------------" << number << endl;
	    	int num = number - '0';*/
		int num = ite->second.node_id;

    		char message[100];
		string mess = QuerySub[i].sub.value + QuerySub[i].pred.value + QuerySub[i].obj.value + std::to_string(QuerySub[i].sub.isVar) + std::to_string(QuerySub[i].obj.isVar);
		strcpy(message, mess.data());	
    		MPI_Send(message, strlen(message)+1, MPI_CHAR, num, 99, MPI_COMM_WORLD);

                cout << "knownsub  " << QuerySub[i].sub.value << "\t " << QuerySub[i].pred.value << "\t " << QuerySub[i].obj.value << endl;
        }

	for(unsigned i = 0;i < QueryObj.size();i++){
		char message[100];
                string mess = QueryObj[i].sub.value + QueryObj[i].pred.value + QueryObj[i].obj.value + std::to_string(QueryObj[i].sub.isVar) + std::to_string(QueryObj[i].obj.isVar);
                strcpy(message, mess.data());
		MPI_Send(message, strlen(message)+1, MPI_CHAR, 1, 99, MPI_COMM_WORLD);
                MPI_Send(message, strlen(message)+1, MPI_CHAR, 2, 99, MPI_COMM_WORLD);
                MPI_Send(message, strlen(message)+1, MPI_CHAR, 3, 99, MPI_COMM_WORLD);
                MPI_Send(message, strlen(message)+1, MPI_CHAR, 4, 99, MPI_COMM_WORLD);
                cout << "knownobj  " << QueryObj[i].sub.value << "\t " << QueryObj[i].pred.value << "\t " << QueryObj[i].obj.value << endl;
        }
	for(unsigned i = 0;i < Query.size();i++){

		char message[100];
                string mess = Query[i].sub.value + Query[i].pred.value + Query[i].obj.value + std::to_string(Query[i].sub.isVar) + std::to_string(Query[i].obj.isVar);
                strcpy(message, mess.data());
		MPI_Send(message, strlen(message)+1, MPI_CHAR, 1, 99, MPI_COMM_WORLD);
                MPI_Send(message, strlen(message)+1, MPI_CHAR, 2, 99, MPI_COMM_WORLD);
                MPI_Send(message, strlen(message)+1, MPI_CHAR, 3, 99, MPI_COMM_WORLD);
                MPI_Send(message, strlen(message)+1, MPI_CHAR, 4, 99, MPI_COMM_WORLD);

                cout <<"else  " << Query[i].sub.value << "\t " << Query[i].pred.value << "\t " << Query[i].obj.value << endl;
        }

	
	//************************************************************* 
	
	for(unsigned i= 0; i < parser.patterns.patterns.size(); i++){
		if(parser.patterns.patterns[i].subject.type == SPARQLParser::Element::Variable){
			stream<<"\t?"<<parser.getVariableName(parser.patterns.patterns[i].subject.id)<<" ";
		}
		else if(parser.patterns.patterns[i].subject.type == SPARQLParser::Element::IRI){
			it = subj_map.find(parser.patterns.patterns[i].subject.value);
			if (it == subj_map.end()) {
				stream<<"\t<"<<max_verts<<"> ";
			}
			else
				stream<<"\t<"<<it->second<<"> ";
		}
		else{
			it = subj_map.find(parser.patterns.patterns[i].subject.value);
			if (it == subj_map.end()) {
				stream<<"\t\""<<max_verts<<"\" ";
			}
			else
				stream<<"\t\""<<it->second<<"\" ";
		}

		it = predicate_map.find(parser.patterns.patterns[i].predicate.value);
		if (it == predicate_map.end()) {
			stream<<"<"<<max_predicate<<"> ";
		}
		else
			stream<<"<"<<it->second<<"> ";

		if(parser.patterns.patterns[i].object.type == SPARQLParser::Element::Variable){
			stream<<"?"<<parser.getVariableName(parser.patterns.patterns[i].object.id)<<" ."<<endl;
		}
		else if(parser.patterns.patterns[i].object.type == SPARQLParser::Element::IRI){
			it = subj_map.find(parser.patterns.patterns[i].object.value);
			if (it == subj_map.end()) {
				stream<<"<"<<max_verts<<"> ."<<endl;
			}
			else
				stream<<"<"<<it->second<<"> ."<<endl;
		}
		else{
			it = subj_map.find(parser.patterns.patterns[i].object.value);
			if (it == subj_map.end()) {
				stream<<"\""<<max_verts<<"\" ."<<endl;
			}
			else
				stream<<"\""<<it->second<<"\" ."<<endl;
		}
	}


}

void load_encode_queries(string queryFile, boost::unordered_map<string, long long>& predicate_map, boost::unordered_map<string, long long>& subj_map, long long max_predicate, long long max_verts, consistent_hash_t& consistent_hash_) {
	ifstream queryIn(queryFile.c_str());
	string outputFileName = queryFile+"_encoded";
	ofstream queryOut(outputFileName.c_str());
	string querystring, str;

	if (!queryIn) {
		throwException("Query file '" + queryFile
				+ "' was not found. Please try again!\n");
	}
	// read the query string
	querystring = "";

	int counter = 0;
	while (true) {
		getline(queryIn,str);
		cout << str << endl;
		cout << strcmp("#EOQ",str.c_str()) << endl;
		if(strcmp("#EOQ",str.c_str())!=-13){
			querystring+=str;
			if (!queryIn.good())
				break;
			querystring+='\n';
		}
		else {
			cout<<counter<<"\n"<<querystring<<endl;
			counter++;
			SPARQLLexer lexer(querystring);
			//*****************************************************************
			SPARQLParser parser(lexer);
			//*****************************************************************
			try {
				parser.parse();
			} catch (const SPARQLParser::ParserException& e) {
				cerr << "parse error: " << e.message << endl;
				return;
			}

			encode_query(parser, queryOut, predicate_map, subj_map, max_predicate, max_verts, consistent_hash_);
			querystring = "";
		}
	}
	MPI_Send("exit", strlen("exit")+1, MPI_CHAR, 1, 99, MPI_COMM_WORLD);
	MPI_Send("exit", strlen("exit")+1, MPI_CHAR, 2, 99, MPI_COMM_WORLD);
	MPI_Send("exit", strlen("exit")+1, MPI_CHAR, 3, 99, MPI_COMM_WORLD);
	MPI_Send("exit", strlen("exit")+1, MPI_CHAR, 4, 99, MPI_COMM_WORLD);
	queryIn.close();
	queryOut.close();
}
