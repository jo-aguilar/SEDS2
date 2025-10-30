#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <thread>
#include <chrono>
using namespace std;

class Estado {
	private:
		string nome;
		int assinatura;
		unordered_map<string,string>vizinhos;	
	public:
		Estado(const string nn, const int as, unordered_map<string,string> vv) 
			: nome(nn), assinatura(as), vizinhos(vv) {}
		~Estado() {}
		string retorna_nome(){ return nome;}
		int retorna_assinatura() {return assinatura;}
		unordered_map<string,string>retorna_vizinhos () {return vizinhos;}
		vector<string> retorna_chaves() const;
};
vector<string> Estado::retorna_chaves() const {
	std::vector<string> chaves;
	chaves.reserve(vizinhos.size());               
	for (const auto& par : vizinhos) { 
		chaves.push_back(par.first);
	}
	return chaves;
}

int gera_aleatorio(const int n);
string n_transic(const string tn, const unordered_map<string, Estado> es);
void espera(const int tempo) { this_thread::sleep_for(chrono::milliseconds(tempo)); }

int main(){
	unordered_map <string, Estado> estados;
	estados.emplace("inicial", Estado("inicial", 0,{{"t1","inicial"}, {"t2","a"}}));
	estados.emplace("a", Estado("a", 1, {{"t2","a"}, {"t3","b"}, {"t1","c"}}));
	estados.emplace("b", Estado("b", 2, {{"t2","b"}, {"t1","d"}}));
	estados.emplace("c", Estado("c", 3, {{"t1","c"}, {"t2","d"}, {"t3","e"}}));
	estados.emplace("d", Estado("d", 4, {{"t2","d"}, {"t3","e"}}));
	estados.emplace("e", Estado("e", 5, {{"t3","e"}, {"t1","inicial"}}));
	vector<string>chaves = estados.at("a").retorna_chaves();
	
	//início do funcionamento
	string estado_atual = n_transic("inicial", estados);
	for(int i = 0; i < 20; i++){
		espera(500);
		estado_atual = n_transic(estado_atual, estados);
	}
}

int gera_aleatorio(const int n){
//Devolve um número aleatório inteiro de 0 a n
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dist(0, n);
	int numero = dist(gen);
	return numero;
}

string n_transic(const string tn, const unordered_map<string, Estado> es) {
	vector<string>transics = es.at(tn).retorna_chaves();
	cout << "Estado atual é : " << tn << endl;
	//escolha do próximo estado
	const int tamanho_transics = transics.size();
	string proxima_transic = transics[gera_aleatorio(tamanho_transics)];
	Estado estado = es.at(tn);
	unordered_map<string,string>vizinhos = estado.retorna_vizinhos();
	const string novo_estado = vizinhos.at(proxima_transic);
	return novo_estado;
}
