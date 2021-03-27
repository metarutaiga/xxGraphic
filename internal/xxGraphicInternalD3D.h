//==============================================================================
// xxGraphic : Direct3D Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#define COM_NO_WINDOWS_H
#include <objbase.h>

// DirectDraw
interface DECLSPEC_UUID("6C14DB80-A733-11CE-A521-0020AF0BE560") IDirectDraw;
interface DECLSPEC_UUID("B3A6F3E0-2B43-11CF-A2DE-00AA00B93356") IDirectDraw2;
interface DECLSPEC_UUID("9c59509a-39bd-11d1-8c4a-00c04fd930c5") IDirectDraw4;
interface DECLSPEC_UUID("15e65ec0-3b9c-11d2-b92f-00609797ea5b") IDirectDraw7;
interface DECLSPEC_UUID("6C14DB81-A733-11CE-A521-0020AF0BE560") IDirectDrawSurface;
interface DECLSPEC_UUID("57805885-6eec-11cf-9441-a82303c10e27") IDirectDrawSurface2;
interface DECLSPEC_UUID("DA044E00-69B2-11D0-A1D5-00AA00B8DFBB") IDirectDrawSurface3;
interface DECLSPEC_UUID("0B2B8630-AD35-11D0-8EA6-00609797EA5B") IDirectDrawSurface4;
interface DECLSPEC_UUID("06675a80-3b9b-11d2-b92f-00609797ea5b") IDirectDrawSurface7;
interface DECLSPEC_UUID("6C14DB84-A733-11CE-A521-0020AF0BE560") IDirectDrawPalette;
interface DECLSPEC_UUID("6C14DB85-A733-11CE-A521-0020AF0BE560") IDirectDrawClipper;
interface DECLSPEC_UUID("4B9F0EE0-0D7E-11D0-9B06-00A0C903A3B8") IDirectDrawColorControl;
interface DECLSPEC_UUID("69C11C3E-B46B-11D1-AD7A-00C04FC29B4E") IDirectDrawGammaControl;

// DirectX Graphics Infrastructure
interface DECLSPEC_UUID("310d36a0-d2e7-4c0a-aa04-6a9d23b8886a") IDXGISwapChain;
interface DECLSPEC_UUID("790a45f7-0d42-4876-983a-0a55cfe6f4aa") IDXGISwapChain1;
interface DECLSPEC_UUID("a8be2ac4-199f-4946-b331-79599fb98de7") IDXGISwapChain2;
interface DECLSPEC_UUID("94d99bdb-f1f8-4ab0-b236-7da0170edab1") IDXGISwapChain3;
interface DECLSPEC_UUID("3D585D5A-BD4A-489E-B1F4-3DBCB6452FFB") IDXGISwapChain4;

// Direct3D 3.0
interface DECLSPEC_UUID("3BBA0080-2421-11CF-A31A-00AA00B93356") IDirect3D;
interface DECLSPEC_UUID("64108800-957d-11d0-89ab-00a0c9054129") IDirect3DDevice;
interface DECLSPEC_UUID("2CDCD9E0-25A0-11CF-A31A-00AA00B93356") IDirect3DTexture;
interface DECLSPEC_UUID("4417C142-33AD-11CF-816F-0000C020156E") IDirect3DLight;
interface DECLSPEC_UUID("4417C144-33AD-11CF-816F-0000C020156E") IDirect3DMaterial;
interface DECLSPEC_UUID("4417C145-33AD-11CF-816F-0000C020156E") IDirect3DExecuteBuffer;
interface DECLSPEC_UUID("4417C146-33AD-11CF-816F-0000C020156E") IDirect3DViewport;

// Direct3D 5.0
interface DECLSPEC_UUID("6aae1ec1-662a-11d0-889d-00aa00bbb76a") IDirect3D2;
interface DECLSPEC_UUID("F2086B20-259F-11CF-A31A-00AA00B93356") IDirect3DRampDevice;
interface DECLSPEC_UUID("A4665C60-2673-11CF-A31A-00AA00B93356") IDirect3DRGBDevice;
interface DECLSPEC_UUID("84E63dE0-46AA-11CF-816F-0000C020156E") IDirect3DHALDevice;
interface DECLSPEC_UUID("881949a1-d6f3-11d0-89ab-00a0c9054129") IDirect3DMMXDevice;
interface DECLSPEC_UUID("93281501-8cf8-11d0-89ab-00a0c9054129") IDirect3DDevice2;
interface DECLSPEC_UUID("93281502-8cf8-11d0-89ab-00a0c9054129") IDirect3DTexture2;
interface DECLSPEC_UUID("93281503-8cf8-11d0-89ab-00a0c9054129") IDirect3DMaterial2;
interface DECLSPEC_UUID("93281500-8cf8-11d0-89ab-00a0c9054129") IDirect3DViewport2;

// Direct3D 6.0
interface DECLSPEC_UUID("bb223240-e72b-11d0-a9b4-00aa00c0993e") IDirect3D3;
interface DECLSPEC_UUID("50936643-13e9-11d1-89aa-00a0c9054129") IDirect3DRefDevice;
interface DECLSPEC_UUID("8767df22-bacc-11d1-8969-00a0c90629a8") IDirect3DNullDevice;
interface DECLSPEC_UUID("b0ab3b60-33d7-11d1-a981-00c04fd7b174") IDirect3DDevice3;
interface DECLSPEC_UUID("ca9c46f4-d3c5-11d1-b75a-00600852b312") IDirect3DMaterial3;
interface DECLSPEC_UUID("b0ab3b61-33d7-11d1-a981-00c04fd7b174") IDirect3DViewport3;
interface DECLSPEC_UUID("7a503555-4a83-11d1-a5db-00a0c90367f8") IDirect3DVertexBuffer;

// Direct3D 7.0
interface DECLSPEC_UUID("f5049e77-4861-11d2-a407-00a0c90629a8") IDirect3D7;
interface DECLSPEC_UUID("f5049e78-4861-11d2-a407-00a0c90629a8") IDirect3DTnLHalDevice;
interface DECLSPEC_UUID("f5049e79-4861-11d2-a407-00a0c90629a8") IDirect3DDevice7;
interface DECLSPEC_UUID("f5049e7d-4861-11d2-a407-00a0c90629a8") IDirect3DVertexBuffer7;

// Direct3D 8.0
interface DECLSPEC_UUID("1dd9e8da-1c77-4d40-b0cf-98fefdff9512") IDirect3D8;
interface DECLSPEC_UUID("7385e5df-8fe8-41d5-86b6-d7b48547b6cf") IDirect3DDevice8;
interface DECLSPEC_UUID("1b36bb7b-09b7-410a-b445-7d1430d7b33f") IDirect3DResource8;
interface DECLSPEC_UUID("b4211cfa-51b9-4a9f-ab78-db99b2bb678e") IDirect3DBaseTexture8;
interface DECLSPEC_UUID("e4cdd575-2866-4f01-b12e-7eece1ec9358") IDirect3DTexture8;
interface DECLSPEC_UUID("4b8aaafa-140f-42ba-9131-597eafaa2ead") IDirect3DVolumeTexture8;
interface DECLSPEC_UUID("3ee5b968-2aca-4c34-8bb5-7e0c3d19b750") IDirect3DCubeTexture8;
interface DECLSPEC_UUID("8aeeeac7-05f9-44d4-b591-000b0df1cb95") IDirect3DVertexBuffer8;
interface DECLSPEC_UUID("0e689c9a-053d-44a0-9d92-db0e3d750f86") IDirect3DIndexBuffer8;
interface DECLSPEC_UUID("b96eebca-b326-4ea5-882f-2ff5bae021dd") IDirect3DSurface8;
interface DECLSPEC_UUID("bd7349f5-14f1-42e4-9c79-972380db40c0") IDirect3DVolume8;
interface DECLSPEC_UUID("928c088b-76b9-4c6b-a536-a590853876cd") IDirect3DSwapChain8;

// Direct3D 9.0
interface DECLSPEC_UUID("81bdcbca-64d4-426d-ae8d-ad0147f4275c") IDirect3D9;
interface DECLSPEC_UUID("d0223b96-bf7a-43fd-92bd-a43b0d82b9eb") IDirect3DDevice9;
interface DECLSPEC_UUID("b07c4fe5-310d-4ba8-a23c-4f0f206f218b") IDirect3DStateBlock9;
interface DECLSPEC_UUID("05eec05d-8f7d-4362-b999-d1baf357c704") IDirect3DResource9;
interface DECLSPEC_UUID("dd13c59c-36fa-4098-a8fb-c7ed39dc8546") IDirect3DVertexDeclaration9;
interface DECLSPEC_UUID("efc5557e-6265-4613-8a94-43857889eb36") IDirect3DVertexShader9;
interface DECLSPEC_UUID("6d3bdbdc-5b02-4415-b852-ce5e8bccb289") IDirect3DPixelShader9;
interface DECLSPEC_UUID("580ca87e-1d3c-4d54-991d-b7d3e3c298ce") IDirect3DBaseTexture9;
interface DECLSPEC_UUID("85c31227-3de5-4f00-9b3a-f11ac38c18b5") IDirect3DTexture9;
interface DECLSPEC_UUID("2518526c-e789-4111-a7b9-47ef328d13e6") IDirect3DVolumeTexture9;
interface DECLSPEC_UUID("fff32f81-d953-473a-9223-93d652aba93f") IDirect3DCubeTexture9;
interface DECLSPEC_UUID("b64bb1b5-fd70-4df6-bf91-19d0a12455e3") IDirect3DVertexBuffer9;
interface DECLSPEC_UUID("7c9dd65e-d3f7-4529-acee-785830acde35") IDirect3DIndexBuffer9;
interface DECLSPEC_UUID("0cfbaf3a-9ff6-429a-99b3-a2796af8b89b") IDirect3DSurface9;
interface DECLSPEC_UUID("24f416e6-1f67-4aa7-b88e-d33f6f3128a1") IDirect3DVolume9;
interface DECLSPEC_UUID("794950f2-adfc-458a-905e-10a10b0b503b") IDirect3DSwapChain9;
interface DECLSPEC_UUID("d9771460-a695-4f26-bbd3-27b840b541cc") IDirect3DQuery9;

// Direct3D 9Ex
interface DECLSPEC_UUID("02177241-69FC-400C-8FF1-93A44DF6861D") IDirect3D9Ex;
interface DECLSPEC_UUID("b18b10ce-2649-405a-870f-95f777d4313a") IDirect3DDevice9Ex;
interface DECLSPEC_UUID("91886caf-1c3d-4d2e-a0ab-3e4c7d8d3303") IDirect3DSwapChain9Ex;
interface DECLSPEC_UUID("187aeb13-aaf5-4c59-876d-e059088c0df8") IDirect3D9ExOverlayExtension;
interface DECLSPEC_UUID("26dc4561-a1ee-4ae7-96da-118a36c0ec95") IDirect3DDevice9Video;
interface DECLSPEC_UUID("ff24beee-da21-4beb-98b5-d2f899f98af9") IDirect3DAuthenticatedChannel9;
interface DECLSPEC_UUID("fa0ab799-7a9c-48ca-8c5b-237e71a54434") IDirect3DCryptoSession9;

// Direct3D 9On12
interface DECLSPEC_UUID("e7fda234-b589-4049-940d-8878977531c8") IDirect3DDevice9On12;

// Direct3D 10.0
interface DECLSPEC_UUID("9B7E4C00-342C-4106-A19F-4F2704F689F0") ID3D10DeviceChild;
interface DECLSPEC_UUID("2B4B1CC8-A4AD-41f8-8322-CA86FC3EC675") ID3D10DepthStencilState;
interface DECLSPEC_UUID("EDAD8D19-8A35-4d6d-8566-2EA276CDE161") ID3D10BlendState;
interface DECLSPEC_UUID("A2A07292-89AF-4345-BE2E-C53D9FBB6E9F") ID3D10RasterizerState;
interface DECLSPEC_UUID("9B7E4C01-342C-4106-A19F-4F2704F689F0") ID3D10Resource;
interface DECLSPEC_UUID("9B7E4C02-342C-4106-A19F-4F2704F689F0") ID3D10Buffer;
interface DECLSPEC_UUID("9B7E4C03-342C-4106-A19F-4F2704F689F0") ID3D10Texture1D;
interface DECLSPEC_UUID("9B7E4C04-342C-4106-A19F-4F2704F689F0") ID3D10Texture2D;
interface DECLSPEC_UUID("9B7E4C05-342C-4106-A19F-4F2704F689F0") ID3D10Texture3D;
interface DECLSPEC_UUID("C902B03F-60A7-49BA-9936-2A3AB37A7E33") ID3D10View;
interface DECLSPEC_UUID("9B7E4C07-342C-4106-A19F-4F2704F689F0") ID3D10ShaderResourceView;
interface DECLSPEC_UUID("9B7E4C08-342C-4106-A19F-4F2704F689F0") ID3D10RenderTargetView;
interface DECLSPEC_UUID("9B7E4C09-342C-4106-A19F-4F2704F689F0") ID3D10DepthStencilView;
interface DECLSPEC_UUID("9B7E4C0A-342C-4106-A19F-4F2704F689F0") ID3D10VertexShader;
interface DECLSPEC_UUID("6316BE88-54CD-4040-AB44-20461BC81F68") ID3D10GeometryShader;
interface DECLSPEC_UUID("4968B601-9D00-4cde-8346-8E7F675819B6") ID3D10PixelShader;
interface DECLSPEC_UUID("9B7E4C0B-342C-4106-A19F-4F2704F689F0") ID3D10InputLayout;
interface DECLSPEC_UUID("9B7E4C0C-342C-4106-A19F-4F2704F689F0") ID3D10SamplerState;
interface DECLSPEC_UUID("9B7E4C0D-342C-4106-A19F-4F2704F689F0") ID3D10Asynchronous;
interface DECLSPEC_UUID("9B7E4C0E-342C-4106-A19F-4F2704F689F0") ID3D10Query;
interface DECLSPEC_UUID("9B7E4C10-342C-4106-A19F-4F2704F689F0") ID3D10Predicate;
interface DECLSPEC_UUID("9B7E4C11-342C-4106-A19F-4F2704F689F0") ID3D10Counter;
interface DECLSPEC_UUID("9B7E4C0F-342C-4106-A19F-4F2704F689F0") ID3D10Device;
interface DECLSPEC_UUID("9B7E4E00-342C-4106-A19F-4F2704F689F0") ID3D10Multithread;

// Direct3D 10.1
interface DECLSPEC_UUID("EDAD8D99-8A35-4d6d-8566-2EA276CDE161") ID3D10BlendState1;
interface DECLSPEC_UUID("9B7E4C87-342C-4106-A19F-4F2704F689F0") ID3D10ShaderResourceView1;
interface DECLSPEC_UUID("9B7E4C8F-342C-4106-A19F-4F2704F689F0") ID3D10Device1;

// Direct3D 11.0
interface DECLSPEC_UUID("1841e5c8-16b0-489b-bcc8-44cfb0d5deae") ID3D11DeviceChild;
interface DECLSPEC_UUID("03823efb-8d8f-4e1c-9aa2-f64bb2cbfdf1") ID3D11DepthStencilState;
interface DECLSPEC_UUID("75b68faa-347d-4159-8f45-a0640f01cd9a") ID3D11BlendState;
interface DECLSPEC_UUID("9bb4ab81-ab1a-4d8f-b506-fc04200b6ee7") ID3D11RasterizerState;
interface DECLSPEC_UUID("dc8e63f3-d12b-4952-b47b-5e45026a862d") ID3D11Resource;
interface DECLSPEC_UUID("48570b85-d1ee-4fcd-a250-eb350722b037") ID3D11Buffer;
interface DECLSPEC_UUID("f8fb5c27-c6b3-4f75-a4c8-439af2ef564c") ID3D11Texture1D;
interface DECLSPEC_UUID("6f15aaf2-d208-4e89-9ab4-489535d34f9c") ID3D11Texture2D;
interface DECLSPEC_UUID("037e866e-f56d-4357-a8af-9dabbe6e250e") ID3D11Texture3D;
interface DECLSPEC_UUID("839d1216-bb2e-412b-b7f4-a9dbebe08ed1") ID3D11View;
interface DECLSPEC_UUID("b0e06fe0-8192-4e1a-b1ca-36d7414710b2") ID3D11ShaderResourceView;
interface DECLSPEC_UUID("dfdba067-0b8d-4865-875b-d7b4516cc164") ID3D11RenderTargetView;
interface DECLSPEC_UUID("9fdac92a-1876-48c3-afad-25b94f84a9b6") ID3D11DepthStencilView;
interface DECLSPEC_UUID("28acf509-7f5c-48f6-8611-f316010a6380") ID3D11UnorderedAccessView;
interface DECLSPEC_UUID("3b301d64-d678-4289-8897-22f8928b72f3") ID3D11VertexShader;
interface DECLSPEC_UUID("8e5c6061-628a-4c8e-8264-bbe45cb3d5dd") ID3D11HullShader;
interface DECLSPEC_UUID("f582c508-0f36-490c-9977-31eece268cfa") ID3D11DomainShader;
interface DECLSPEC_UUID("38325b96-effb-4022-ba02-2e795b70275c") ID3D11GeometryShader;
interface DECLSPEC_UUID("ea82e40d-51dc-4f33-93d4-db7c9125ae8c") ID3D11PixelShader;
interface DECLSPEC_UUID("4f5b196e-c2bd-495e-bd01-1fded38e4969") ID3D11ComputeShader;
interface DECLSPEC_UUID("e4819ddc-4cf0-4025-bd26-5de82a3e07b7") ID3D11InputLayout;
interface DECLSPEC_UUID("da6fea51-564c-4487-9810-f0d0f9b4e3a5") ID3D11SamplerState;
interface DECLSPEC_UUID("4b35d0cd-1e15-4258-9c98-1b1333f6dd3b") ID3D11Asynchronous;
interface DECLSPEC_UUID("d6c00747-87b7-425e-b84d-44d108560afd") ID3D11Query;
interface DECLSPEC_UUID("9eb576dd-9f77-4d86-81aa-8bab5fe490e2") ID3D11Predicate;
interface DECLSPEC_UUID("6e8c49fb-a371-4770-b440-29086022b741") ID3D11Counter;
interface DECLSPEC_UUID("a6cd7faa-b0b7-4a2f-9436-8662a65797cb") ID3D11ClassInstance;
interface DECLSPEC_UUID("ddf57cba-9543-46e4-a12b-f207a0fe7fed") ID3D11ClassLinkage;
interface DECLSPEC_UUID("a24bc4d1-769e-43f7-8013-98ff566c18e2") ID3D11CommandList;
interface DECLSPEC_UUID("c0bfa96c-e089-44fb-8eaf-26f8796190da") ID3D11DeviceContext;
interface DECLSPEC_UUID("3C9C5B51-995D-48d1-9B8D-FA5CAEDED65C") ID3D11VideoDecoder;
interface DECLSPEC_UUID("31627037-53AB-4200-9061-05FAA9AB45F9") ID3D11VideoProcessorEnumerator;
interface DECLSPEC_UUID("1D7B0652-185F-41c6-85CE-0C5BE3D4AE6C") ID3D11VideoProcessor;
interface DECLSPEC_UUID("3015A308-DCBD-47aa-A747-192486D14D4A") ID3D11AuthenticatedChannel;
interface DECLSPEC_UUID("9B32F9AD-BDCC-40a6-A39D-D5C865845720") ID3D11CryptoSession;
interface DECLSPEC_UUID("C2931AEA-2A85-4f20-860F-FBA1FD256E18") ID3D11VideoDecoderOutputView;
interface DECLSPEC_UUID("11EC5A5F-51DC-4945-AB34-6E8C21300EA5") ID3D11VideoProcessorInputView;
interface DECLSPEC_UUID("A048285E-25A9-4527-BD93-D68B68C44254") ID3D11VideoProcessorOutputView;
interface DECLSPEC_UUID("61F21C45-3C0E-4a74-9CEA-67100D9AD5E4") ID3D11VideoContext;
interface DECLSPEC_UUID("10EC4D5B-975A-4689-B9E4-D0AAC30FE333") ID3D11VideoDevice;
interface DECLSPEC_UUID("db6f6ddb-ac77-4e88-8253-819df9bbf140") ID3D11Device;

// Direct3D 11.1
interface DECLSPEC_UUID("cc86fabe-da55-401d-85e7-e3c9de2877e9") ID3D11BlendState1;
interface DECLSPEC_UUID("1217d7a6-5039-418c-b042-9cbe256afd6e") ID3D11RasterizerState1;
interface DECLSPEC_UUID("5c1e0d8a-7c23-48f9-8c59-a92958ceff11") ID3DDeviceContextState;
interface DECLSPEC_UUID("bb2c6faa-b5fb-4082-8e6b-388b8cfa90e1") ID3D11DeviceContext1;
interface DECLSPEC_UUID("A7F026DA-A5F8-4487-A564-15E34357651E") ID3D11VideoContext1;
interface DECLSPEC_UUID("29DA1D51-1321-4454-804B-F5FC9F861F0F") ID3D11VideoDevice1;
interface DECLSPEC_UUID("465217F2-5568-43CF-B5B9-F61D54531CA1") ID3D11VideoProcessorEnumerator1;
interface DECLSPEC_UUID("a04bfb29-08ef-43d6-a49c-a9bdbdcbe686") ID3D11Device1;
interface DECLSPEC_UUID("b2daad8b-03d4-4dbf-95eb-32ab4b63d0ab") ID3DUserDefinedAnnotation;

// Direct3D 11.2
interface DECLSPEC_UUID("420d5b32-b90c-4da4-bef0-359f6a24a83a") ID3D11DeviceContext2;
interface DECLSPEC_UUID("9d06dffa-d1e5-4d07-83a8-1bb123f2f841") ID3D11Device2;

// Direct3D 11.3
interface DECLSPEC_UUID("51218251-1E33-4617-9CCB-4D3A4367E7BB") ID3D11Texture2D1;
interface DECLSPEC_UUID("0C711683-2853-4846-9BB0-F3E60639E46A") ID3D11Texture3D1;
interface DECLSPEC_UUID("6fbd02fb-209f-46c4-b059-2ed15586a6ac") ID3D11RasterizerState2;
interface DECLSPEC_UUID("91308b87-9040-411d-8c67-c39253ce3802") ID3D11ShaderResourceView1;
interface DECLSPEC_UUID("ffbe2e23-f011-418a-ac56-5ceed7c5b94b") ID3D11RenderTargetView1;
interface DECLSPEC_UUID("7b3b6153-a886-4544-ab37-6537c8500403") ID3D11UnorderedAccessView1;
interface DECLSPEC_UUID("631b4766-36dc-461d-8db6-c47e13e60916") ID3D11Query1;
interface DECLSPEC_UUID("b4e3c01d-e79e-4637-91b2-510e9f4c9b8f") ID3D11DeviceContext3;
interface DECLSPEC_UUID("affde9d1-1df7-4bb7-8a34-0f46251dab80") ID3D11Fence;
interface DECLSPEC_UUID("917600da-f58c-4c33-98d8-3e15b390fa24") ID3D11DeviceContext4;
interface DECLSPEC_UUID("A05C8C37-D2C6-4732-B3A0-9CE0B0DC9AE6") ID3D11Device3;

// Direct3D 11.4 - 11.5
interface DECLSPEC_UUID("8992ab71-02e6-4b8d-ba48-b056dcda42c4") ID3D11Device4;
interface DECLSPEC_UUID("8ffde202-a0e7-45df-9e01-e837801b5ea0") ID3D11Device5;
interface DECLSPEC_UUID("9B7E4E00-342C-4106-A19F-4F2704F689F0") ID3D11Multithread;
interface DECLSPEC_UUID("C4E7374C-6243-4D1B-AE87-52B4F740E261") ID3D11VideoContext2;
interface DECLSPEC_UUID("59C0CB01-35F0-4A70-8F67-87905C906A53") ID3D11VideoDevice2;
interface DECLSPEC_UUID("A9E2FAA0-CB39-418F-A0B7-D8AAD4DE672E") ID3D11VideoContext3;

// Direct3D 11On12
interface DECLSPEC_UUID("85611e73-70a9-490e-9614-a9e302777904") ID3D11On12Device;
interface DECLSPEC_UUID("bdb64df4-ea2f-4c70-b861-aaab1258bb5d") ID3D11On12Device1;
interface DECLSPEC_UUID("dc90f331-4740-43fa-866e-67f12cb58223") ID3D11On12Device2;

// Direct3D 12.0
interface DECLSPEC_UUID("c4fec28f-7966-4e95-9f94-f431cb56c3b8") ID3D12Object;
interface DECLSPEC_UUID("905db94b-a00c-4140-9df5-2b64ca9ea357") ID3D12DeviceChild;
interface DECLSPEC_UUID("c54a6b66-72df-4ee8-8be5-a946a1429214") ID3D12RootSignature;
interface DECLSPEC_UUID("34AB647B-3CC8-46AC-841B-C0965645C046") ID3D12RootSignatureDeserializer;
interface DECLSPEC_UUID("7F91CE67-090C-4BB7-B78E-ED8FF2E31DA0") ID3D12VersionedRootSignatureDeserializer;
interface DECLSPEC_UUID("63ee58fb-1268-4835-86da-f008ce62f0d6") ID3D12Pageable;
interface DECLSPEC_UUID("6b3b2502-6e51-45b3-90ee-9884265e8df3") ID3D12Heap;
interface DECLSPEC_UUID("696442be-a72e-4059-bc79-5b5c98040fad") ID3D12Resource;
interface DECLSPEC_UUID("6102dee4-af59-4b09-b999-b44d73f09b24") ID3D12CommandAllocator;
interface DECLSPEC_UUID("0a753dcf-c4d8-4b91-adf6-be5a60d95a76") ID3D12Fence;
interface DECLSPEC_UUID("433685fe-e22b-4ca0-a8db-b5b4f4dd0e4a") ID3D12Fence1;
interface DECLSPEC_UUID("765a30f3-f624-4c6f-a828-ace948622445") ID3D12PipelineState;
interface DECLSPEC_UUID("8efb471d-616c-4f49-90f7-127bb763fa51") ID3D12DescriptorHeap;
interface DECLSPEC_UUID("0d9658ae-ed45-469e-a61d-970ec583cab4") ID3D12QueryHeap;
interface DECLSPEC_UUID("c36a797c-ec80-4f0a-8985-a7b2475082d1") ID3D12CommandSignature;
interface DECLSPEC_UUID("7116d91c-e7e4-47ce-b8c6-ec8168f437e5") ID3D12CommandList;
interface DECLSPEC_UUID("5b160d0f-ac1b-4185-8ba8-b3ae42a5a455") ID3D12GraphicsCommandList;
interface DECLSPEC_UUID("553103fb-1fe7-4557-bb38-946d7d0e7ca7") ID3D12GraphicsCommandList1;
interface DECLSPEC_UUID("38C3E585-FF17-412C-9150-4FC6F9D72A28") ID3D12GraphicsCommandList2;
interface DECLSPEC_UUID("0ec870a6-5d7e-4c22-8cfc-5baae07616ed") ID3D12CommandQueue;
interface DECLSPEC_UUID("189819f1-1db6-4b57-be54-1821339b85f7") ID3D12Device;
interface DECLSPEC_UUID("c64226a8-9201-46af-b4cc-53fb9ff7414f") ID3D12PipelineLibrary;
interface DECLSPEC_UUID("80eabf42-2568-4e5e-bd82-c37f86961dc3") ID3D12PipelineLibrary1;
interface DECLSPEC_UUID("77acce80-638e-4e65-8895-c1f23386863e") ID3D12Device1;
interface DECLSPEC_UUID("30baa41e-b15b-475c-a0bb-1af5c5b64328") ID3D12Device2;
interface DECLSPEC_UUID("81dadc15-2bad-4392-93c5-101345c4aa98") ID3D12Device3;
interface DECLSPEC_UUID("A1533D18-0AC1-4084-85B9-89A96116806B") ID3D12ProtectedSession;
interface DECLSPEC_UUID("6CD696F4-F289-40CC-8091-5A6C0A099C3D") ID3D12ProtectedResourceSession;
interface DECLSPEC_UUID("e865df17-a9ee-46f9-a463-3098315aa2e5") ID3D12Device4;
interface DECLSPEC_UUID("e667af9f-cd56-4f46-83ce-032e595d70a8") ID3D12LifetimeOwner;
interface DECLSPEC_UUID("f1df64b6-57fd-49cd-8807-c0eb88b45c8f") ID3D12SwapChainAssistant;
interface DECLSPEC_UUID("3fd03d36-4eb1-424a-a582-494ecb8ba813") ID3D12LifetimeTracker;
interface DECLSPEC_UUID("47016943-fca8-4594-93ea-af258b55346d") ID3D12StateObject;
interface DECLSPEC_UUID("de5fa827-9bf9-4f26-89ff-d7f56fde3860") ID3D12StateObjectProperties;
interface DECLSPEC_UUID("8b4f173b-2fea-4b80-8f58-4307191ab95d") ID3D12Device5;
interface DECLSPEC_UUID("82BC481C-6B9B-4030-AEDB-7EE3D1DF1E63") ID3D12DeviceRemovedExtendedDataSettings;
interface DECLSPEC_UUID("DBD5AE51-3317-4F0A-ADF9-1D7CEDCAAE0B") ID3D12DeviceRemovedExtendedDataSettings1;
interface DECLSPEC_UUID("98931D33-5AE8-4791-AA3C-1A73A2934E71") ID3D12DeviceRemovedExtendedData;
interface DECLSPEC_UUID("9727A022-CF1D-4DDA-9EBA-EFFA653FC506") ID3D12DeviceRemovedExtendedData1;
interface DECLSPEC_UUID("c70b221b-40e4-4a17-89af-025a0727a6dc") ID3D12Device6;
interface DECLSPEC_UUID("D6F12DD6-76FB-406E-8961-4296EEFC0409") ID3D12ProtectedResourceSession1;
interface DECLSPEC_UUID("5c014b53-68a1-4b9b-8bd1-dd6046b9358b") ID3D12Device7;
#if defined(NTDDI_WIN10_VB)
interface DECLSPEC_UUID("9218E6BB-F944-4F7E-A75C-B1B2C7B701F3") ID3D12Device8;
#else
interface DECLSPEC_UUID("4393134F-CF31-41F7-BC51-F2DE938B4374") ID3D12Device8;
#endif
interface DECLSPEC_UUID("9D5E227A-4430-4161-88B3-3ECA6BB16E19") ID3D12Resource1;
interface DECLSPEC_UUID("BE36EC3B-EA85-4AEB-A45A-E9D76404A495") ID3D12Resource2;
interface DECLSPEC_UUID("572F7389-2168-49E3-9693-D6DF5871BF6D") ID3D12Heap1;
interface DECLSPEC_UUID("6FDA83A7-B84C-4E38-9AC8-C7BD22016B3D") ID3D12GraphicsCommandList3;
interface DECLSPEC_UUID("DBB84C27-36CE-4FC9-B801-F048C46AC570") ID3D12MetaCommand;
interface DECLSPEC_UUID("8754318e-d3a9-4541-98cf-645b50dc4874") ID3D12GraphicsCommandList4;
interface DECLSPEC_UUID("7071e1f0-e84b-4b33-974f-12fa49de65c5") ID3D12Tools;
interface DECLSPEC_UUID("55050859-4024-474c-87f5-6472eaee44ea") ID3D12GraphicsCommandList5;
interface DECLSPEC_UUID("c3827890-e548-4cfa-96cf-5689a9370f80") ID3D12GraphicsCommandList6;

template <class T>
inline ULONG SafeRelease(T*& ptr)
{
    ULONG ref = 0;
    if (ptr)
    {
        ref = ptr->Release();
        ptr = nullptr;
    }
    return ref;
}

inline void ViewportFromScissor(float projectionMatrix[4][4], int fromX, int fromY, int fromWidth, int fromHeight, int toX, int toY, int toWidth, int toHeight)
{
    float invWidth = 1.0f / toWidth;
    float invHeight = 1.0f / toHeight;
    float scaleWidth = fromWidth * invWidth;
    float scaleHeight = fromHeight * invHeight;
    float offsetWidth = scaleWidth + 2.0f * (fromX - (toX + toWidth * 0.5f)) * invWidth;
    float offsetHeight = scaleHeight + 2.0f * (fromY - (toY + toHeight * 0.5f)) * invHeight;
    projectionMatrix[0][0] = projectionMatrix[0][0] * scaleWidth;
    projectionMatrix[1][1] = projectionMatrix[1][1] * scaleHeight;
    projectionMatrix[3][0] = projectionMatrix[3][0] * scaleWidth + offsetWidth;
    projectionMatrix[3][1] = projectionMatrix[3][1] * scaleHeight - offsetHeight;
}

inline void PatchD3DIM(const char* name)
{
    void* d3dim = xxLoadLibrary(name);
    if (d3dim == nullptr)
        return;

    // https://github.com/UCyborg/LegacyD3DResolutionHack
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)d3dim;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((char*)dosHeader + dosHeader->e_lfanew);
    char* codeBase = (char*)d3dim + ntHeader->OptionalHeader.BaseOfCode;
    DWORD codeSize = ntHeader->OptionalHeader.SizeOfCode;

    const BYTE wantedBytes[] = { 0xB8, 0x00, 0x08, 0x00, 0x00, 0x39 };
    for (DWORD i = 0, size = codeSize - sizeof(wantedBytes); i < size; ++i)
    {
        if (memcmp(codeBase + i, wantedBytes, sizeof(wantedBytes)) == 0)
        {
            DWORD oldProtect;
            VirtualProtect(&codeBase[i + 1], 4, PAGE_EXECUTE_READWRITE, &oldProtect);
            codeBase[i + 1] = -1;
            codeBase[i + 2] = -1;
            codeBase[i + 3] = -1;
            codeBase[i + 4] = -1;
            VirtualProtect(&codeBase[i + 1], 4, oldProtect, &oldProtect);
        }
    }

    xxFreeLibrary(d3dim);
}

inline struct ID3D10Blob* CreateD3D10Shader(const char* shader, const char* entry, const char* target)
{
    static HRESULT(WINAPI * D3D10CompileShader)(const char*, size_t, void*, void*, void*, const char*, const char*, int, ID3D10Blob**, ID3D10Blob**) = nullptr;
    if (D3D10CompileShader == nullptr)
    {
        (void*&)D3D10CompileShader = GetProcAddress(LoadLibraryA("d3d10.dll"), "D3D10CompileShader");
    }
    if (D3D10CompileShader)
    {
        ID3D10Blob* blob = nullptr;
        ID3D10Blob* error = nullptr;
        D3D10CompileShader(shader, strlen(shader), nullptr, nullptr, nullptr, entry, target, 0, &blob, &error);
        if (error)
        {
            struct Blob : public IUnknown
            {
                virtual LPCSTR STDMETHODCALLTYPE GetBufferPointer() = 0;
            };
            Blob* blob = (Blob*)error;
            OutputDebugStringA(blob->GetBufferPointer());
            blob->Release();
        }
        return blob;
    }

    return nullptr;
}

//==============================================================================
//  Resource Type
//==============================================================================
inline uint64_t getResourceType(uint64_t resource)
{
    return resource & 7ull;
}
//------------------------------------------------------------------------------
inline uint64_t getResourceData(uint64_t resource)
{
    return resource & ~7ull;
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct D3DFRAMEBUFFER2
{
    IDirectDrawSurface*     backSurface;
    IDirectDrawSurface*     depthSurface;
};
//------------------------------------------------------------------------------
struct D3DFRAMEBUFFER3
{
    IDirectDrawSurface4*    backSurface;
    IDirectDrawSurface4*    depthSurface;
};
//------------------------------------------------------------------------------
struct D3DFRAMEBUFFER7
{
    IDirectDrawSurface7*    backSurface;
    IDirectDrawSurface7*    depthSurface;
};
//------------------------------------------------------------------------------
struct D3DFRAMEBUFFER8
{
    IDirect3DSurface8*      backBuffer;
    IDirect3DSurface8*      depthStencil;
};
//------------------------------------------------------------------------------
struct D3DFRAMEBUFFER9
{
    IDirect3DSurface9*      backBuffer;
    IDirect3DSurface9*      depthStencil;
};
//------------------------------------------------------------------------------
struct D3D10FRAMEBUFFER
{
    ID3D10RenderTargetView* renderTargetView;
    ID3D10DepthStencilView* depthStencilView;
};
//------------------------------------------------------------------------------
struct D3D11FRAMEBUFFER
{
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct D3DSWAPCHAIN2 : public D3DFRAMEBUFFER2
{
    IDirectDrawClipper*     clipper;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//------------------------------------------------------------------------------
struct D3DSWAPCHAIN3 : public D3DFRAMEBUFFER3
{
    IDirectDrawClipper*     clipper;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//------------------------------------------------------------------------------
struct D3DSWAPCHAIN7 : public D3DFRAMEBUFFER7
{
    IDirectDrawClipper*     clipper;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//------------------------------------------------------------------------------
struct D3DSWAPCHAIN8 : public D3DFRAMEBUFFER8
{
    IDirect3DSwapChain8*    swapchain;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//------------------------------------------------------------------------------
struct D3DSWAPCHAIN9 : public D3DFRAMEBUFFER9
{
    IDirect3DSwapChain9*    swapchain;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//------------------------------------------------------------------------------
struct D3D10SWAPCHAIN : public D3D10FRAMEBUFFER
{
    IDXGISwapChain*         dxgiSwapchain;
    ID3D10Texture2D*        depthStencilTexture;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//------------------------------------------------------------------------------
struct D3D11SWAPCHAIN : public D3D11FRAMEBUFFER
{
    IDXGISwapChain*         dxgiSwapchain;
    ID3D11Texture2D*        depthStencilTexture;
    ID3D11DeviceContext*    deviceContext;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//------------------------------------------------------------------------------
struct D3D11ON12SWAPCHAIN : public D3D11FRAMEBUFFER
{
    IDXGISwapChain3*        dxgiSwapchain;
    ID3D11Texture2D*        depthStencilTexture;
    ID3D11DeviceContext*    deviceContext;
    HWND                    hWnd;
    int                     width;
    int                     height;
};
//==============================================================================
//  Render Pass
//==============================================================================
union D3D10RENDERPASS
{
    uint64_t    value;
    struct
    {
        bool    clearColor;
        DWORD   clearDepthStencil;
    };
};
//------------------------------------------------------------------------------
typedef D3D10RENDERPASS D3D11RENDERPASS;
//==============================================================================
//  Vertex Attribute
//==============================================================================
union D3DVERTEXATTRIBUTE2
{
    uint64_t    value;
    struct
    {
        DWORD   fvf;
        int     stride;
    };
};
//------------------------------------------------------------------------------
typedef D3DVERTEXATTRIBUTE2 D3DVERTEXATTRIBUTE3;
typedef D3DVERTEXATTRIBUTE2 D3DVERTEXATTRIBUTE7;
typedef D3DVERTEXATTRIBUTE2 D3DVERTEXATTRIBUTE8;
typedef D3DVERTEXATTRIBUTE2 D3DVERTEXATTRIBUTE9;
//------------------------------------------------------------------------------
struct D3DVERTEXATTRIBUTE8PS
{
    DWORD   declaration[16];
    int     stride;
};
//------------------------------------------------------------------------------
struct D3DVERTEXATTRIBUTE9PS
{
    IDirect3DVertexDeclaration9*    vertexDeclaration;
    int                             stride;
};
//------------------------------------------------------------------------------
struct D3D10VERTEXATTRIBUTE
{
    ID3D10InputLayout*  inputLayout;
    int                 stride;
};
//------------------------------------------------------------------------------
struct D3D11VERTEXATTRIBUTE
{
    ID3D11InputLayout*  inputLayout;
    int                 stride;
};
//==============================================================================
//  Buffer
//==============================================================================
struct D3DVERTEXBUFFER2
{
    void*           buffer;
    UINT            size;
    UINT            count;
    bool            dirty;
    void*           address;
};
//------------------------------------------------------------------------------
struct D3D10BUFFER
{
    ID3D10Buffer*   buffer;
    UINT            size;
    enum D3D10_MAP  map;
    void*           address;
};
//------------------------------------------------------------------------------
struct D3D11BUFFER
{
    ID3D11Buffer*   buffer;
    UINT            size;
    enum D3D11_MAP  map;
    void*           address;
};
//==============================================================================
//  Texture
//==============================================================================
struct D3D10TEXTURE
{
    ID3D10ShaderResourceView*   resourceView;
    ID3D10Texture1D*            texture1D;
    ID3D10Texture2D*            texture2D;
    ID3D10Texture3D*            texture3D;
    unsigned int                mipmap;
};
//------------------------------------------------------------------------------
struct D3D11TEXTURE
{
    ID3D11ShaderResourceView*   resourceView;
    ID3D11Texture1D*            texture1D;
    ID3D11Texture2D*            texture2D;
    ID3D11Texture3D*            texture3D;
    unsigned int                mipmap;
};
//==============================================================================
//  Sampler
//==============================================================================
union D3DSAMPLER2
{
    uint64_t    value;
    struct
    {
        uint8_t addressU;
        uint8_t addressV;
        uint8_t addressW;
        uint8_t magFilter;
        uint8_t minFilter;
        uint8_t mipFilter;
        uint8_t anisotropy;
    };
};
//------------------------------------------------------------------------------
typedef D3DSAMPLER2 D3DSAMPLER3;
typedef D3DSAMPLER2 D3DSAMPLER7;
typedef D3DSAMPLER2 D3DSAMPLER8;
typedef D3DSAMPLER2 D3DSAMPLER9;
//==============================================================================
//  Pipeline
//==============================================================================
union D3DRENDERSTATE3
{
    uint64_t        value;
    struct
    {
        uint64_t    alphaBlending:1;
        uint64_t    alphaTesting:1;
        uint64_t    depthTest:1;
        uint64_t    depthWrite:1;
        uint64_t    cull:1;
        uint64_t    scissor:1;
    };
};
//------------------------------------------------------------------------------
typedef D3DRENDERSTATE3 D3DRENDERSTATE7;
typedef D3DRENDERSTATE3 D3DRENDERSTATE8;
typedef D3DRENDERSTATE3 D3DRENDERSTATE9;
//------------------------------------------------------------------------------
struct D3DPIPELINE3
{
    D3DRENDERSTATE3 renderState;
};
//------------------------------------------------------------------------------
typedef D3DPIPELINE3 D3DPIPELINE7;
//------------------------------------------------------------------------------
struct D3DPIPELINE8
{
    DWORD           vertexShader;
    DWORD           pixelShader;
    D3DRENDERSTATE8 renderState;
};
//------------------------------------------------------------------------------
struct D3DPIPELINE9
{
    IDirect3DVertexDeclaration9*    vertexDeclaration;
    DWORD                           fvf;
    IDirect3DVertexShader9*         vertexShader;
    IDirect3DPixelShader9*          pixelShader;
    D3DRENDERSTATE9                 renderState;
};
//------------------------------------------------------------------------------
struct D3D10PIPELINE
{
    ID3D10VertexShader*         vertexShader;
    ID3D10PixelShader*          pixelShader;
    ID3D10BlendState*           blendState;
    ID3D10DepthStencilState*    depthStencilState;
    ID3D10RasterizerState*      rasterizerState;
    ID3D10InputLayout*          inputLayout;
};
//------------------------------------------------------------------------------
struct D3D11PIPELINE
{
    ID3D11VertexShader*         vertexShader;
    ID3D11PixelShader*          pixelShader;
    ID3D11BlendState*           blendState;
    ID3D11DepthStencilState*    depthStencilState;
    ID3D11RasterizerState*      rasterizerState;
    ID3D11InputLayout*          inputLayout;
};
//==============================================================================
