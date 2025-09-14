# SDL Shader Compiler

A domain-specific language (DSL) shader compiler that translates high-level C++-like shader code to both GLSL and CUDA.

## Features

- **Multi-target compilation**: Generate both GLSL and CUDA code from a single DSL
- **C++-like syntax**: Familiar syntax for shader development
- **Type-safe**: Strong type checking and semantic analysis
- **Optimized output**: Generates efficient target code
- **Extensible**: Modular architecture for adding new target languages

## Architecture

```
┌─────────────┐    ┌──────────┐    ┌─────────────┐    ┌──────────────┐
│   Lexer     │───▶│  Parser  │───▶│  Semantic   │───▶│ Code         │
│             │    │          │    │  Analyzer   │    │ Generators   │
└─────────────┘    └──────────┘    └─────────────┘    └──────────────┘
       │                  │               │                    │
       ▼                  ▼               ▼                    ▼
   Tokenize          Build AST      Type Check           GLSL / CUDA
```

### Components

- **Lexer**: Tokenizes the input DSL code into meaningful tokens
- **Parser**: Builds an Abstract Syntax Tree (AST) from tokens
- **Semantic Analyzer**: Performs type checking and semantic validation
- **Code Generators**: Generate target language code (GLSL/CUDA)

## Building

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Or use make on Unix systems
make
```

### Build Configurations

```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release build (default)
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Usage

### Basic Usage

```bash
# Compile to GLSL
./sdl_compiler -t glsl input.sdl -o output.glsl

# Compile to CUDA
./sdl_compiler -t cuda input.sdl -o output.cu

# Compile to both
./sdl_compiler -t glsl,cuda input.sdl -o output
```

### Command Line Options

- `-t, --target <target>`: Target language (glsl, cuda, or both)
- `-o, --output <file>`: Output file name
- `-I, --include <dir>`: Add include directory
- `-D, --define <macro>`: Define preprocessor macro
- `-v, --verbose`: Enable verbose output
- `-h, --help`: Show help message

## DSL Syntax Example

```cpp
// SDL Shader example
shader vertex_main : vertex {
    in vec3 position;
    in vec2 texCoord;
    
    out vec2 vTexCoord;
    
    uniform mat4 mvpMatrix;
    
    void main() {
        gl_Position = mvpMatrix * vec4(position, 1.0);
        vTexCoord = texCoord;
    }
}

shader fragment_main : fragment {
    in vec2 vTexCoord;
    out vec4 fragColor;
    
    uniform sampler2D mainTexture;
    
    void main() {
        fragColor = texture(mainTexture, vTexCoord);
    }
}
```

## Testing

```bash
# Run all tests
make test

# Or using ctest
ctest --verbose
```

## Project Structure

```
├── CMakeLists.txt          # Build configuration
├── README.md              # This file
├── include/               # Header files
│   ├── lexer/            # Lexer headers
│   ├── parser/           # Parser headers
│   ├── semantic/         # Semantic analyzer headers
│   ├── codegen/          # Code generator headers
│   ├── compiler/         # Main compiler interface
│   └── utils/            # Utility headers
├── src/                  # Source files
│   ├── lexer/           # Lexical analysis
│   ├── parser/          # Syntax analysis
│   ├── semantic/        # Semantic analysis
│   ├── codegen/         # Code generation
│   ├── compiler/        # Main compiler logic
│   ├── cli/             # Command line interface
│   ├── utils/           # Utilities
│   └── main.cpp         # Entry point
├── tests/               # Test files
├── examples/            # Example SDL files
└── docs/                # Documentation
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## License

MIT License - see LICENSE file for details.
