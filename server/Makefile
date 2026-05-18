CXX = c++
CXXFLAGS = -Wall -O2
LIBS = -lhiredis
TARGET = server
SOURCE = server.cpp

.PHONY: build run clean watch help

.PHONY: dev

build:
	@echo "🔨 Compilando..."
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LIBS)
	@echo "✅ Compilação concluída!"

run: build
	@echo "🚀 Iniciando servidor..."
	./$(TARGET)

clean:
	@echo "🧹 Limpando..."
	rm -f $(TARGET) *.o
	@echo "✅ Limpeza concluída!"

watch:
	@echo "👀 Monitorando mudanças..."
	python3 watch.py

dev: watch
	@echo "💡 Alias para modo desenvolvimento (watch)"

help:
	@echo "Comandos disponíveis:"
	@echo "  make build  - Compila o servidor"
	@echo "  make run    - Compila e roda o servidor"
	@echo "  make watch  - Monitora mudanças e recompila automaticamente"
	@echo "  make clean  - Remove arquivos compilados"
	@echo "  make help   - Mostra esta mensagem"
