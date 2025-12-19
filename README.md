# JetsonWindowsSSH - Control Robótico con DQN

## Descripción

Este proyecto implementa un sistema de control robótico utilizando Redes Neuronales Profundas Q (DQN - Deep Q-Networks) para aprendizaje por refuerzo. El sistema permite la comunicación en tiempo real entre un dispositivo NVIDIA Jetson (cliente) y un servidor en Windows, facilitando el envío de acciones de control generadas por el modelo DQN.

El proyecto combina aprendizaje profundo con comunicación de red TCP/IP, permitiendo que un agente de IA tome decisiones en un entorno distribuido entre plataformas heterogéneas.

## Arquitectura

El sistema consta de los siguientes componentes principales:

### Cliente Jetson (`client_jetson.cpp`)
- Ejecuta en NVIDIA Jetson
- Implementa el controlador DQN en C++ con PyTorch
- Genera acciones basadas en estados del entorno
- Envía acciones al servidor Windows vía TCP

### Servidor Windows (`server_jetson.cpp`)
- Ejecuta en Windows
- Recibe acciones del cliente Jetson
- Procesa y muestra las acciones recibidas

### Controlador DQN
- Implementado en dos versiones: C++ (`DQ_Network_CUDA.h`) y Python (`DQ_Network_CUDA.py`)
- Utiliza PyTorch con soporte CUDA para aceleración GPU
- Incluye buffer de replay, exploración ε-greedy y actualización de red objetivo

### Otros archivos
- `main.py`: Servidor simple en Python para pruebas
- `client_windows.cpp`, `server_windows.cpp`: Variantes adicionales para Windows
- `client_windows_0.cpp`, `server_windows_0.cpp`: Versiones alternativas

## Requisitos del Sistema

### Para Jetson (Cliente)
- NVIDIA Jetson con CUDA
- C++17 o superior
- PyTorch C++ con CUDA
- Compilador g++

### Para Windows (Servidor)
- Windows 10/11
- Visual Studio con soporte C++
- PyTorch (para versiones Python)
- Python 3.8+ (opcional)

## Dependencias

### C++
- PyTorch C++ (libtorch)
- CUDA Toolkit
- Winsock2 (para Windows)

### Python
- PyTorch
- CUDA (opcional, para aceleración GPU)

## Instalación

### 1. Configuración del Entorno Jetson
```bash
# Instalar PyTorch C++ en Jetson
# Descargar libtorch desde https://pytorch.org/cppdist/
# Configurar variables de entorno para CUDA
```

### 2. Configuración del Entorno Windows
```bash
# Instalar PyTorch
pip install torch torchvision torchaudio

# Para CUDA (si tienes GPU NVIDIA)
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118
```

### 3. Compilación
```bash
# Compilar cliente Jetson
g++ client_jetson.cpp -o client_jetson -std=c++17 -I/path/to/libtorch/include -L/path/to/libtorch/lib -ltorch -lc10 -lcuda

# Compilar servidor Windows
cl server_jetson.cpp /EHsc /I"path\to\libtorch\include" /link /LIBPATH:"path\to\libtorch\lib" torch.lib c10.lib Ws2_32.lib
```

## Uso

### Configuración de Red
1. Conectar Jetson y Windows a la misma red
2. Configurar IP estática en Windows: `192.168.200.1`
3. Asegurar que el puerto 9999 esté abierto

### Ejecución
1. **Iniciar el servidor en Windows:**
   ```bash
   ./server_jetson.exe
   ```
   O para Python:
   ```bash
   python main.py
   ```

2. **Iniciar el cliente en Jetson:**
   ```bash
   ./client_jetson
   ```

3. El cliente comenzará a enviar acciones generadas por el DQN cada segundo.

### Parámetros del DQN
- **Estados de entrada:** 3 (configurable)
- **Acciones de salida:** 3 (configurable)
- **Epsilon inicial:** 1.0 (exploración máxima)
- **Epsilon mínimo:** 0.05
- **Decaimiento epsilon:** 0.9999
- **Gamma:** 0.99 (factor de descuento)
- **Tamaño batch:** 64
- **Buffer máximo:** 5000 experiencias

## Funcionamiento Técnico

### Algoritmo DQN
1. **Exploración vs Explotación:** El agente elige acciones aleatorias con probabilidad ε, decreciendo con el tiempo
2. **Aprendizaje:** Utiliza Q-learning con función de aproximación neuronal
3. **Buffer de Replay:** Almacena experiencias para entrenamiento offline
4. **Red Objetivo:** Copia periódica de la red de política para estabilidad

### Comunicación
- **Protocolo:** TCP/IP
- **Puerto:** 9999
- **Formato de datos:** Enteros de 4 bytes (acciones)
- **Frecuencia:** 1 Hz (configurable)

## Archivos del Proyecto

| Archivo | Descripción |
|---------|-------------|
| `client_jetson.cpp` | Cliente DQN para Jetson |
| `server_jetson.cpp` | Servidor para Windows |
| `DQ_Network_CUDA.h` | Implementación DQN en C++ |
| `DQ_Network_CUDA.py` | Implementación DQN en Python |
| `main.py` | Servidor de prueba en Python |
| `client_windows.cpp` | Cliente alternativo para Windows |
| `server_windows.cpp` | Servidor alternativo para Windows |
| `client_windows_0.cpp` | Versión 0 del cliente Windows |
| `server_windows_0.cpp` | Versión 0 del servidor Windows |

## Desarrollo y Pruebas

### Entrenamiento del DQN
Para entrenar el modelo DQN, implementar un bucle de entrenamiento que:
1. Recopile experiencias del entorno
2. Muestree batches del buffer de replay
3. Calcule pérdida y actualice pesos
4. Actualice red objetivo periódicamente

### Depuración
- Verificar conectividad de red entre dispositivos
- Monitorear valores de epsilon y pérdida durante entrenamiento
- Usar herramientas de profiling CUDA para optimización

## Limitaciones y Mejoras Futuras

### Limitaciones Actuales
- Comunicación unidireccional (solo acciones del cliente al servidor)
- Sin feedback del entorno al agente
- Implementación básica sin optimizaciones avanzadas

### Mejoras Posibles
- Implementar comunicación bidireccional
- Agregar procesamiento de sensores en tiempo real
- Integrar con ROS (Robot Operating System)
- Optimizar para baja latencia
- Agregar persistencia de modelos entrenados

## Licencia

Este proyecto está bajo la licencia MIT. Ver archivo LICENSE para más detalles.

