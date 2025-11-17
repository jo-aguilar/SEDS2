//g++ observador.cpp Estado.cpp -o observador
#include "Estado.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
//comunicação entre cliente e servidor
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <filesystem>
//manipulação de tempo
#include <thread>
#include <chrono>
#include <algorithm>

#define SOCKET_PATH "/tmp/comunica_redes"
using namespace std;

vector<string>tokenizar_linha(string entrada);
unordered_map<string, Estado> retorna_rede();
string uds_recv();
void espera(const int tempo) { this_thread::sleep_for(chrono::milliseconds(tempo)); }

int main(int argc, char* argv[]){
	//recebe PID de ./seds2 para poder manipular o programa
	string pid = uds_recv();
	cout << "PID atual : " << pid << endl;
	
	unordered_map<string, Estado>estados = retorna_rede();
	unordered_map<string,string>vizinhos_atuais;
	vector<string>chaves;
	Estado atual("", 0);
	
	if(argc==1 or argc==2){
		atual = estados.at("inicial");
		cout << "Primeiro estado é o inicial" << endl;
		vizinhos_atuais = atual.retorna_vizinhos();
		chaves = estados.at("inicial").retorna_chaves();
	}
	int a = 0;

	while(true){
		if(filesystem::exists("/tmp/comunica_redes")){
			string resposta = uds_recv();

			if(resposta != ""){
				if(a==10) resposta = "aa";
				if(find(chaves.begin(), chaves.end(), resposta) != chaves.end()){
					cout << ++a <<  ". Transição tomada: " << resposta << endl;
				}else{
				//[!!!] FALHA. Transição não está presente entre chaves
					cout << "[!!!] Erro" << endl;
					system(("kill " + pid).c_str());
					string comando = "./estados";
						
					system((comando).c_str());
					//espera(1000);
					return 0;
				}
					atual = estados.at(vizinhos_atuais.at(resposta));
					vizinhos_atuais = atual.retorna_vizinhos();
					chaves = atual.retorna_chaves();
				
			} else { continue; }
		} else { continue; }
	
	}
}

vector<string>tokenizar_linha(string entrada){
	//transforma as linhas capturas em strings tratadas para uso da
	//construção da rede
	vector<string>saida;
	stringstream ss(entrada);
	string token;
	while(ss>>token){ saida.push_back(token); };
	return saida;
}

unordered_map<string, Estado> retorna_rede(){
	ifstream arquivo("rede.txt");
	if(!arquivo){
		//Arquivo inexistente
		cout << "Rede não encontrada.\nTerminando...\n";
	}
	//importação da rede
	unordered_map <string, Estado> estados;	
	for(string captura; getline(arquivo, captura);){
		//cria um novo estado para ser inserido dentro da rede
		//a partir dos dados já conseguidos no documento 
		vector<string> saida = tokenizar_linha(captura);
		Estado novo_estado(saida[0], stoi(saida[1]));

		for(int i = 2; i < saida.size()-1; i+=2){
			//cria as leis de transição e passa cada uma das 
			//condições para o novo estado
			novo_estado.adiciona_vizinho(saida[i], saida[i+1]);
		}
		estados.emplace(novo_estado.retorna_nome(), novo_estado);
	}	
	arquivo.close();
	return estados;	
}

string uds_recv() {
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		cout << "[ERRO]" << endl;
		//espera(10000);
		//perror("socket");
		return "";
	}

	sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);
	if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
		//perror("connect");
		close(sock);
		return "";
	}

	char buffer[256]{};
	int n = read(sock, buffer, sizeof(buffer));
	close(sock);

	return (n > 0 ? std::string(buffer, n) : "");
}
