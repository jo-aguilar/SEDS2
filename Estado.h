#ifndef ESTADO_H
#define ESTADO_H

#include <unordered_map>
#include <string>
#include <vector>

class Estado {
	private:
		std::string nome;
		int assinatura;
		std::unordered_map<std::string,std::string>vizinhos;
	public:
		Estado(const std::string nn, const int as, std::unordered_map<std::string,std::string> vv)
			:nome(nn), assinatura(as), vizinhos(vv) {}
		Estado(const std::string nn, const int as)
			:nome(nn), assinatura(as){}
		~Estado(){}
		std::string retorna_nome();
		int retorna_assinatura();
		std::unordered_map<std::string,std::string>retorna_vizinhos();
		std::vector<std::string> retorna_chaves() const;
		void adiciona_vizinho(std::string chave, std::string valor);
};

#endif //ESTADO_H
