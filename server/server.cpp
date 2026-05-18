#include <iostream>
#include <hiredis/hiredis.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Para inet_addr
#include <cstring>
#include <unistd.h>

// Função para enviar o comando de salvar contato para o Android
void enviarContatoParaAndroid(const char* androidIp, const std::string& nome, const std::string& telefone) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr;
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8081); // Porta que o Android está ouvindo
    servaddr.sin_addr.s_addr = inet_addr(androidIp);

    std::string mensagem = "SALVAR_CONTATO:" + nome + "," + telefone;
    
    sendto(sockfd, mensagem.c_str(), mensagem.length(), 0, 
           (const struct sockaddr *)&servaddr, sizeof(servaddr));
    
    std::cout << "Comando enviado para o Android: " << mensagem << std::endl;
    close(sockfd);
}

int main() {
    // 1. Conecta ao Redis
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        std::cerr << "Erro ao conectar no Redis!" << std::endl;
        return 1;
    }

    // 2. Configura o Socket UDP para receber eventos do Android
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080); // Porta que o servidor ouve o Android

    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    std::cout << "Servidor C++ aguardando eventos na porta 8080..." << std::endl;

    char buffer[1024];
    socklen_t len = sizeof(cliaddr);

    while (true) {
        int n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        std::string info(buffer);
        // Pega o IP do Android que enviou a mensagem
        char *androidIp = inet_ntoa(cliaddr.sin_addr);
        
        std::cout << "Evento recebido de " << androidIp << ": " << info << std::endl;

        // 3. Salva no Redis
        redisReply *reply = (redisReply *)redisCommand(c, "SET ultimo_evento %s", info.c_str());
        freeReplyObject(reply);
        
        // 4. EXEMPLO: Ao receber o clique do botão, o servidor "responde" 
        // mandando o Android salvar um contato de teste
        std::cout << "Respondendo ao Android para salvar  um contato..." << std::endl;
        enviarContatoParaAndroid(androidIp, "Intelitr Suporte", "11999999999");
    }

    redisFree(c);
    close(sockfd);
    return 0;
}