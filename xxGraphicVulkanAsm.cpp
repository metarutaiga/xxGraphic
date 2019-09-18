#include "xxGraphicInternal.h"
#include "xxGraphicVulkanAsm.h"

#include "vulkan/spirv.h"
#include "vulkan/GLSL.std.450.h"

//==============================================================================
//  Vulkan SPIR-V Assembly Shader
//==============================================================================
#define _glsl_std_450                   0x4c534c47, 0x6474732e, 0x3035342e
#define _main                           0x6e69616d
#define _in                             0x00006e69
#define _out                            0x0074756f
#define _uniformBuffer                  0x66696e75, 0x426d726f, 0x65666675, 0x00000072
#define _position                       0x69736f70, 0x6e6f6974
#define _color                          0x6f6c6f63, 0x00000072
#define _uv                             0x00007675
#define _gl_PerVertex                   0x505f6c67, 0x65567265, 0x78657472
#define _gl_Position                    0x505f6c67, 0x7469736f, 0x006e6f69
#define _tex                            0x00786574
#define _sam                            0x006d6173
#define Op(op, length)                  (op | (length << SpvWordCountShift))
//------------------------------------------------------------------------------
const uint32_t vertexShaderCodeSPIRV[] =
{
    SpvMagicNumber, SpvVersion, 0, 53, 0,
    Op(SpvOpCapability, 2),             SpvCapabilityShader,
    Op(SpvOpExtInstImport, 6),          1, _glsl_std_450, 0,
    Op(SpvOpMemoryModel, 3),            SpvAddressingModelLogical, SpvMemoryModelGLSL450,
    Op(SpvOpEntryPoint, 10),            SpvExecutionModelVertex, 4, _main, 0, 11, 15, 21, 27, 37,
    Op(SpvOpSource, 3),                 SpvSourceLanguageGLSL, 450,
    Op(SpvOpName, 4),                   4, _main, 0,
    Op(SpvOpName, 3),                   9, 0,
    Op(SpvOpMemberName, 6),             9, 0, _color, 0,
    Op(SpvOpMemberName, 5),             9, 1, _uv, 0,
    Op(SpvOpName, 4),                   11, _out, 0,
    Op(SpvOpName, 5),                   15, _color, 0,
    Op(SpvOpName, 4),                   21, _uv, 0,
    Op(SpvOpName, 6),                   25, _gl_PerVertex, 0,
    Op(SpvOpMemberName, 7),             25, 0, _gl_Position, 0,
    Op(SpvOpName, 3),                   27, 0,
    Op(SpvOpName, 7),                   29, _uniformBuffer, 0,
    Op(SpvOpMemberName, 3),             29, 0,
    Op(SpvOpName, 3),                   31, 0,
    Op(SpvOpName, 5),                   37, _position, 0,
    Op(SpvOpDecorate, 4),               11, SpvDecorationLocation, 0,
    Op(SpvOpDecorate, 4),               15, SpvDecorationLocation, 1,
    Op(SpvOpDecorate, 4),               21, SpvDecorationLocation, 2,
    Op(SpvOpMemberDecorate, 5),         25, 0, SpvDecorationBuiltIn, SpvBuiltInPosition,
    Op(SpvOpDecorate, 3),               25, SpvDecorationBlock,
    Op(SpvOpMemberDecorate, 4),         29, 0, SpvDecorationColMajor,
    Op(SpvOpMemberDecorate, 5),         29, 0, SpvDecorationOffset, 0,
    Op(SpvOpMemberDecorate, 5),         29, 0, SpvDecorationMatrixStride, 16,
    Op(SpvOpDecorate, 3),               29, SpvDecorationBlock,
    Op(SpvOpDecorate, 4),               31, SpvDecorationDescriptorSet, 0,
    Op(SpvOpDecorate, 4),               31, SpvDecorationBinding, 0,
    Op(SpvOpDecorate, 4),               37, SpvDecorationLocation, 0,
    Op(SpvOpTypeVoid, 2),               2,
    Op(SpvOpTypeFunction, 3),           3, 2,
    Op(SpvOpTypeFloat, 3),              6, 32,
    Op(SpvOpTypeVector, 4),             7, 6, 4,
    Op(SpvOpTypeVector, 4),             8, 6, 2,
    Op(SpvOpTypeStruct, 4),             9, 7, 8,
    Op(SpvOpTypePointer, 4),            10, SpvStorageClassOutput, 9,
    Op(SpvOpVariable, 4),               10, 11, SpvStorageClassOutput,
    Op(SpvOpTypeInt, 4),                12, 32, 1,
    Op(SpvOpConstant, 4),               12, 13, 0,
    Op(SpvOpTypePointer, 4),            14, SpvStorageClassInput, 7,
    Op(SpvOpVariable, 4),               14, 15, SpvStorageClassInput,
    Op(SpvOpTypePointer, 4),            17, SpvStorageClassOutput, 7,
    Op(SpvOpConstant, 4),               12, 19, 1,
    Op(SpvOpTypePointer, 4),            20, SpvStorageClassInput, 8,
    Op(SpvOpVariable, 4),               20, 21, SpvStorageClassInput,
    Op(SpvOpTypePointer, 4),            23, SpvStorageClassOutput, 8,
    Op(SpvOpTypeStruct, 3),             25, 7,
    Op(SpvOpTypePointer, 4),            26, SpvStorageClassOutput, 25,
    Op(SpvOpVariable, 4),               26, 27, SpvStorageClassOutput,
    Op(SpvOpTypeMatrix, 4),             28, 7, 4,
    Op(SpvOpTypeStruct, 3),             29, 28,
    Op(SpvOpTypePointer, 4),            30, SpvStorageClassUniform, 29,
    Op(SpvOpVariable, 4),               30, 31, SpvStorageClassUniform,
    Op(SpvOpTypePointer, 4),            32, SpvStorageClassUniform, 28,
    Op(SpvOpTypeVector, 4),             35, 6, 3,
    Op(SpvOpTypePointer, 4),            36, SpvStorageClassInput, 35,
    Op(SpvOpVariable, 4),               36, 37, SpvStorageClassInput,
    Op(SpvOpConstant, 4),               6, 39, 0x3f800000,
    Op(SpvOpTypeInt, 4),                46, 32, 0,
    Op(SpvOpConstant, 4),               46, 47, 1,
    Op(SpvOpTypePointer, 4),            48, SpvStorageClassOutput, 6,
    Op(SpvOpFunction, 5),               2, 4, SpvFunctionControlMaskNone, 3,
    Op(SpvOpLabel, 2),                  5,
    Op(SpvOpLoad, 4),                   7, 16, 15,
    Op(SpvOpAccessChain, 5),            17, 18, 11, 13,
    Op(SpvOpStore, 3),                  18, 16,
    Op(SpvOpLoad, 4),                   8, 22, 21,
    Op(SpvOpAccessChain, 5),            23, 24, 11, 19,
    Op(SpvOpStore, 3),                  24, 22,
    Op(SpvOpAccessChain, 5),            32, 33, 31, 13,
    Op(SpvOpLoad, 4),                   28, 34, 33,
    Op(SpvOpLoad, 4),                   35, 38, 37,
    Op(SpvOpCompositeExtract, 5),       6, 40, 38, 0,
    Op(SpvOpCompositeExtract, 5),       6, 41, 38, 1,
    Op(SpvOpCompositeExtract, 5),       6, 42, 38, 2,
    Op(SpvOpCompositeConstruct, 7),     7, 43, 40, 41, 42, 39,
    Op(SpvOpMatrixTimesVector, 5),      7, 44, 34, 43,
    Op(SpvOpAccessChain, 5),            17, 45, 27, 13,
    Op(SpvOpStore, 3),                  45, 44,
    Op(SpvOpAccessChain, 6),            48, 49, 27, 13, 47,
    Op(SpvOpLoad, 4),                   6, 50, 49,
    Op(SpvOpFNegate, 4),                6, 51, 50,
    Op(SpvOpAccessChain, 6),            48, 52, 27, 13, 47,
    Op(SpvOpStore, 3),                  52, 51,
    Op(SpvOpReturn, 1),
    Op(SpvOpFunctionEnd, 1)
};
const uint32_t vertexShaderCodeSPIRVSize = sizeof(vertexShaderCodeSPIRV);
//------------------------------------------------------------------------------
const uint32_t fragmentShaderCodeSPIRV[] =
{
    SpvMagicNumber, SpvVersion, 0, 35, 0,
    Op(SpvOpCapability, 2),             SpvCapabilityShader,
    Op(SpvOpExtInstImport, 6),          1, _glsl_std_450, 0,
    Op(SpvOpMemoryModel, 3),            SpvAddressingModelLogical, SpvMemoryModelGLSL450,
    Op(SpvOpEntryPoint, 7),             SpvExecutionModelFragment, 4, _main, 0, 9, 13,
    Op(SpvOpExecutionMode, 3),          4, SpvExecutionModeOriginUpperLeft,
    Op(SpvOpSource, 3),                 SpvSourceLanguageGLSL, 450,
    Op(SpvOpName, 4),                   4, _main, 0,
    Op(SpvOpName, 5),                   9, _color, 0,
    Op(SpvOpName, 3),                   11, 0,
    Op(SpvOpMemberName, 6),             11, 0, _color, 0,
    Op(SpvOpMemberName, 5),             11, 1, _uv, 0,
    Op(SpvOpName, 4),                   13, _in, 0,
    Op(SpvOpName, 4),                   21, _tex, 0,
    Op(SpvOpName, 4),                   25, _sam, 0,
    Op(SpvOpDecorate, 4),               9, SpvDecorationLocation, 0,
    Op(SpvOpDecorate, 4),               13, SpvDecorationLocation, 0,
    Op(SpvOpDecorate, 4),               21, SpvDecorationDescriptorSet, 0,
    Op(SpvOpDecorate, 4),               21, SpvDecorationBinding, xxGraphicDescriptor::FRAGMENT_TEXTURE,
    Op(SpvOpDecorate, 4),               25, SpvDecorationDescriptorSet, 0,
    Op(SpvOpDecorate, 4),               25, SpvDecorationBinding, xxGraphicDescriptor::FRAGMENT_SAMPLER,
    Op(SpvOpTypeVoid, 2),               2,
    Op(SpvOpTypeFunction, 3),           3, 2,
    Op(SpvOpTypeFloat, 3),              6, 32,
    Op(SpvOpTypeVector, 4),             7, 6, 4,
    Op(SpvOpTypePointer, 4),            8, SpvStorageClassOutput, 7,
    Op(SpvOpVariable, 4),               8, 9, SpvStorageClassOutput,
    Op(SpvOpTypeVector, 4),             10, 6, 2,
    Op(SpvOpTypeStruct, 4),             11, 7, 10,
    Op(SpvOpTypePointer, 4),            12, SpvStorageClassInput, 11,
    Op(SpvOpVariable, 4),               12, 13, SpvStorageClassInput,
    Op(SpvOpTypeInt, 4),                14, 32, 1,
    Op(SpvOpConstant, 4),               14, 15, 0,
    Op(SpvOpTypePointer, 4),            16, SpvStorageClassInput, 7,
    Op(SpvOpTypeImage, 9),              19, 6, SpvDim2D, 0, 0, 0, 1, SpvImageFormatUnknown,
    Op(SpvOpTypePointer, 4),            20, SpvStorageClassUniformConstant, 19,
    Op(SpvOpVariable, 4),               20, 21, SpvStorageClassUniformConstant,
    Op(SpvOpTypeSampler, 2),            23,
    Op(SpvOpTypePointer, 4),            24, SpvStorageClassUniformConstant, 23,
    Op(SpvOpVariable, 4),               24, 25, SpvStorageClassUniformConstant,
    Op(SpvOpTypeSampledImage, 3),       27, 19,
    Op(SpvOpConstant, 4),               14, 29, 1,
    Op(SpvOpTypePointer, 4),            30, SpvStorageClassInput, 10,
    Op(SpvOpFunction, 5),               2, 4, SpvFunctionControlMaskNone, 3,
    Op(SpvOpLabel, 2),                  5,
    Op(SpvOpAccessChain, 5),            16, 17, 13, 15,
    Op(SpvOpLoad, 4),                   7, 18, 17,
    Op(SpvOpLoad, 4),                   19, 22, 21,
    Op(SpvOpLoad, 4),                   23, 26, 25,
    Op(SpvOpSampledImage, 5),           27, 28, 22, 26,
    Op(SpvOpAccessChain, 5),            30, 31, 13, 29,
    Op(SpvOpLoad, 4),                   10, 32, 31,
    Op(SpvOpImageSampleImplicitLod, 5), 7, 33, 28, 32,
    Op(SpvOpFMul, 5),                   7, 34, 18, 33,
    Op(SpvOpStore, 3),                  9, 34,
    Op(SpvOpReturn, 1),
    Op(SpvOpFunctionEnd, 1)
};
const uint32_t fragmentShaderCodeSPIRVSize = sizeof(fragmentShaderCodeSPIRV);
//------------------------------------------------------------------------------
