#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// Função para enviar o comando para o Android
void disparar_contato(const std::string& ip_android, const std::string& nome, const std::string& fone) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8081); // Porta que o Android ouve
    servaddr.sin_addr.s_addr = inet_addr(ip_android.c_str());

    std::string mensagem = "SALVAR_CONTATO:" + nome + "," + fone;

    sendto(sockfd, mensagem.c_str(), mensagem.length(), 0, 
           (const struct sockaddr *)&servaddr, sizeof(servaddr));

    std::cout << "Comando enviado para " << ip_android << ": " << mensagem << std::endl;
    close(sockfd);
}

int main() {
    std::string ip;
    std::cout << "--- Disparador de Contatos C++ ---" << std::endl;
    std::cout << "Digite o IP do celular Android: ";
    std::cin >> ip;

    while (true) {
        std::string nome, telefone;
        std::cout << "\n--- Novo Disparo ---" << std::endl;
        std::cout << "Nome do contato: ";
        std::cin.ignore(); // Limpar buffer
        std::getline(std::cin, nome);
        std::cout << "Telefone: ";
        std::cin >> telefone;

        disparar_contato(ip, nome, telefone);
        
        std::cout << "Deseja enviar outro? (s/n): ";
        char cont;
        std::cin >> cont;
        if (cont == 'n') break;
    }

    return 0;
}