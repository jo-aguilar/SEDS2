#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Estado.h"
#include <fstream>
#include <random>
#include <thread>
#include <chrono>
#include <cstdlib>
//comunicação
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <filesystem>
#define SOCKET_PATH "/tmp/comunica_redes"

using namespace std;


int gera_aleatorio(const int n);
string n_transic(const string& tn, const unordered_map<string, Estado>& es);
void espera(const int tempo) { this_thread::sleep_for(chrono::milliseconds(tempo)); }
void transcreve_rede(unordered_map <string, Estado> es);
void uds_send(const std::string& msg);

int main(){
	if(filesystem::exists("redes.txt")){
		cout << "Arquivo existe" << endl;
	} else {
		cout << "Arquivo não existe" << endl;
	}
			
	unordered_map <string, Estado> estados;
	estados.emplace("inicial", Estado("inicial", 0,{{"t1","inicial"}, {"t2","a"}}));
	estados.emplace("a", Estado("a", 1, {{"t2","a"}, {"t3","b"}, {"t1","c"}}));
	estados.emplace("b", Estado("b", 2, {{"t2","b"}, {"t1","d"}}));
	estados.emplace("c", Estado("c", 3, {{"t1","c"}, {"t2","d"}, {"t3","e"}}));
	estados.emplace("d", Estado("d", 4, {{"t2","d"}, {"t3","e"}}));
	estados.emplace("e", Estado("e", 5, {{"t3","e"}, {"t1","inicial"}}));
	vector<string>chaves = estados.at("a").retorna_chaves();
	transcreve_rede(estados);
	system("x-terminal-emulator -e './observador' &");
	
	//início do funcionamento
	string estado_atual = n_transic("inicial", estados);
	//for(int i = 0; i < 20; i++){
	//	espera(500);
	//	estado_atual = n_transic(estado_atual, estados);
	//}
	//cout << "Transição tomada: " << estado_atual << endl;
	
	//system("x-terminal-emulator &");



}

int gera_aleatorio(const int n){
//Devolve um número aleatório inteiro de 0 a n que servem como 
//índices para transições dos estados
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dist(0, n);
	int numero = dist(gen);
	return numero;
}

string n_transic(const string& tn, const unordered_map<string, Estado>& es) {
//Executa preliminares para a transição de um estado para outro de forma aleatória,
//mas seguindo como regra as transições intrínsecas a estado 
	vector<string>transics = es.at(tn).retorna_chaves();
	cout << "Estado atual é : " << tn << endl;
	//escolha do próximo estado
	int tamanho_transics = transics.size();
	string proxima_transic = transics[gera_aleatorio(tamanho_transics)];

	//envia transição para documento a ser lido 
	uds_send(proxima_transic);
	
	Estado estado = es.at(tn);
	unordered_map<string,string>vizinhos = estado.retorna_vizinhos();
	string novo_estado = vizinhos.at(proxima_transic);
	return novo_estado;
}

void transcreve_rede(unordered_map <string, Estado> es){
	ofstream arquivo("rede.txt");
	
	//obtenção de todos os estados da rede
	vector<string>estados;
	estados.reserve(es.size());
	for (const auto& par : es) { estados.push_back(par.first); }
	
	for(int j = 0; j < estados.size(); j++){
		Estado atual = es.at(estados[j]);
		//obtenção do map de vizinhos e transições, respectivamente
		unordered_map<string, string>vizinhos = atual.retorna_vizinhos();
		vector<string>chaves = atual.retorna_chaves();
	
		//escrita do estado, assinatura, transições e vizinhos
		arquivo << atual.retorna_nome() << " " << atual.retorna_assinatura();
		for(auto c : chaves) 
			arquivo << " " << c << " " << vizinhos.at(c);
		arquivo << "\n";
	}

	arquivo.close();
}


void uds_send(const std::string& msg) {
//Comunicação UDP para envio de transições para o cliente 
//a partir desse servidor
	unlink(SOCKET_PATH);

	int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return;
	}

	sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);
	
	if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(server_fd);
		return;
	}
	if (listen(server_fd, 1) < 0) {
		perror("listen");
		close(server_fd);
		return;
	}

	int client_fd = accept(server_fd, nullptr, nullptr);
	if (client_fd < 0) {
		perror("accept");
		close(server_fd);
		return;
	}

	send(client_fd, msg.c_str(), msg.size(), MSG_DONTWAIT);

	close(client_fd);
	close(server_fd);
	unlink(SOCKET_PATH);
}

