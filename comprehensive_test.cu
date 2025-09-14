#include <cuda_runtime.h>
#include <device_launch_parameters.h>

// CUDA Kernel: testVertex
__global__ void testVertex_kernel(float* vertices, float* output, int numVertices) {
float3 position    ;
float2 texCoord    ;
float2 vTexCoord    ;
__device__ void main(    ) {
        vTexCoord = texCoord;
        gl_Position = vec4(position, 1.0);
    }
    
}

// CUDA Kernel: testFragment
__global__ void testFragment_kernel(float* pixels, int width, int height) {
float2 vTexCoord    ;
float4 fragColor    ;
__constant__ cudaTextureObject_t diffuseTexture    ;
__device__ void main(    ) {
float3 color = texture(diffuseTexture, vTexCoord).rgb        ;
        fragColor = vec4(color, 1.0);
    }
    
}

