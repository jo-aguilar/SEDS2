#include "Estado.h"

std::string Estado::retorna_nome() {return this->nome;}
int Estado::retorna_assinatura() {return this->assinatura;}
std::unordered_map<std::string, std::string> Estado::retorna_vizinhos (){ return this->vizinhos;}
std::vector<std::string> Estado::retorna_chaves() const{
//retorna um vetor com todas as chaves da rede
	std::vector<std::string> chaves;
	chaves.reserve(this->vizinhos.size());
	for (const auto & par: this->vizinhos){
		chaves.push_back(par.first);
	}
	return chaves;
}

void Estado::adiciona_vizinho(std::string chave, std::string valor){
//Adiciona um novo vizinho a um certo estado
	this->vizinhos.emplace(chave, valor);
}

