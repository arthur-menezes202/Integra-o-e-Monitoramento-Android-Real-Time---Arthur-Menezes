#include <iostream>
#include <hiredis/hiredis.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

void enviarSinalParaAndroid(const char* ipAndroid, const char* nome, const char* telefone) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8081); // Porta que o Android está ouvindo
    servaddr.sin_addr.s_addr = inet_addr(ipAndroid);

    std::string mensagem = "SALVAR_CONTATO:" + std::string(nome) + "," + std::string(telefone);
    
    sendto(sockfd, mensagem.c_str(), mensagem.length(), 0, 
           (const struct sockaddr *)&servaddr, sizeof(servaddr));
    
    close(sockfd);
    std::cout << "Sinal enviado para o Android!" << std::endl;
}

int main() {
    // 1. Conecta ao Redis
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        std::cerr << "Erro ao conectar no Redis!" << std::endl;
        return 1;
    }

    // 2. Configura o Socket UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080); // Porta que o servidor vai ouvir

    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    std::cout << "Servidor C++ aguardando eventos do Android na porta 8080..." << std::endl;

    char buffer[1024];
    while (true) {
        int n = recvfrom(sockfd, buffer, 1024, 0, NULL, NULL);
        buffer[n] = '\0';
        
        std::string info(buffer);
        std::cout << "Evento recebido: " << info << std::endl;

        // 3. Salva no Redis (Chave: "ultimo_evento")
        redisReply *reply = (redisReply *)redisCommand(c, "SET ultimo_evento %s", info.c_str());
        freeReplyObject(reply);
        
        std::cout << "Informação salva no Redis com sucesso!" << std::endl;
    }

    redisFree(c);
    return 0;
}