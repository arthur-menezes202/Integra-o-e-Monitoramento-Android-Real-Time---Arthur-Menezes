#!/usr/bin/env python3
"""
Script para monitorar mudanças no server.cpp e recompilar/reiniciar automaticamente
"""

import os
import subprocess
import time
import signal
import sys
from pathlib import Path

SOURCE = "server.cpp"
EXECUTABLE = "server"
server_process = None

def signal_handler(sig, frame):
    global server_process
    print("\n🛑 Encerrando...")
    if server_process:
        server_process.terminate()
        try:
            server_process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            server_process.kill()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def compile_server():
    """Compila o servidor"""
    print("🔨 Compilando...")
    try:
        result = subprocess.run(
            ["c++", SOURCE, "-o", EXECUTABLE, "-lhiredis"],
            capture_output=True,
            text=True,
            timeout=10
        )
        if result.returncode == 0:
            print("✅ Compilação concluída com sucesso!")
            return True
        else:
            print("❌ Erro na compilação:")
            print(result.stderr)
            return False
    except subprocess.TimeoutExpired:
        print("❌ Timeout na compilação!")
        return False

def run_server():
    """Roda o servidor"""
    global server_process
    
    if server_process and server_process.poll() is None:
        print("🛑 Parando servidor anterior...")
        server_process.terminate()
        try:
            server_process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            server_process.kill()
    
    print("🚀 Iniciando servidor...")
    try:
        # Executa o binário diretamente e herda stdout/stderr para ver logs ao vivo
        server_process = subprocess.Popen([f"./{EXECUTABLE}"])
        print(f"✨ Servidor rodando (PID: {server_process.pid})")
    except Exception as e:
        print(f"❌ Erro ao iniciar servidor: {e}")

def get_file_mtime():
    """Retorna o tempo de modificação do arquivo"""
    try:
        return os.path.getmtime(SOURCE)
    except OSError:
        return None

# Compilar e rodar pela primeira vez
if compile_server():
    run_server()

# Monitorar mudanças
print(f"👀 Monitorando mudanças em {SOURCE}...")
last_mtime = get_file_mtime()

try:
    while True:
        time.sleep(1)
        current_mtime = get_file_mtime()
        
        if current_mtime and current_mtime != last_mtime:
            print(f"\n📝 Arquivo modificado!")
            last_mtime = current_mtime
            
            if compile_server():
                run_server()
except KeyboardInterrupt:
    signal_handler(None, None)
