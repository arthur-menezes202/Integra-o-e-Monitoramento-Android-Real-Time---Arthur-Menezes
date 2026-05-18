#!/bin/bash

# Script para monitorar mudanças e recompilar/reiniciar o servidor automaticamente

SERVER_PID=""
EXECUTABLE="server"
SOURCE="server.cpp"

# Função para compilar
compile() {
    echo "🔨 Compilando..."
    if c++ $SOURCE -o $EXECUTABLE -lhiredis; then
        echo "✅ Compilação concluída com sucesso!"
        return 0
    else
        echo "❌ Erro na compilação!"
        return 1
    fi
}

# Função para rodar o servidor
run_server() {
    if [ ! -z "$SERVER_PID" ]; then
        echo "🛑 Parando servidor anterior (PID: $SERVER_PID)..."
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
    fi
    
    echo "🚀 Iniciando servidor..."
    ./$EXECUTABLE &
    SERVER_PID=$!
    echo "✨ Servidor rodando (PID: $SERVER_PID)"
}

# Compilar e rodar pela primeira vez
if compile; then
    run_server
fi

# Monitorar mudanças
echo "👀 Monitorando mudanças em $SOURCE..."
inotifywait -m -e modify $SOURCE |
while read path action file; do
    echo ""
    echo "📝 Arquivo modificado: $file"
    
    if compile; then
        run_server
    fi
done
