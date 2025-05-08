#include "Core/Renderer/ScreenQuad.h"
#include "Core/Core.h"

ScreenQuad::ScreenQuad() {
	this->m_core = Core::GetInstance();
	this->m_renderer = nullptr;
	this->m_shader = nullptr;
	this->m_sceneMgr = SceneManager::GetInstance();
	this->m_nSqCBuffIndex = -1;
}

void ScreenQuad::Init() {
	this->m_renderer = this->m_core->GetRenderer();

	this->m_vertices = {
		{{-1.f, 1.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f}},
		{{1.f, 1.f, 0.f}, {0.f, 0.f, 0.f}, {1.f, 0.f}},
		{{1.f, -1.f, 0.f}, {0.f, 0.f, 0.f}, {1.f, 1.f}},
		{{-1.f, -1.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 1.f}},
	};

	this->m_indices = {
		0, 1, 3,
		3, 1, 2
	};
	
	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Init(renderer);
	}

}

void ScreenQuad::D3D12Init(D3D12* renderer) {
	renderer->GetDevice(this->m_dev);
	renderer->GetCommandList(this->m_list);

	renderer->CreateBuffer(&this->m_vertices[0], this->m_vertices.size() * sizeof(Vertex), this->m_VBO);
	renderer->CreateBuffer(&this->m_indices[0], this->m_indices.size() * sizeof(UINT), this->m_IBO);

	this->m_VBV.BufferLocation = this->m_VBO->GetGPUVirtualAddress();
	this->m_VBV.SizeInBytes = this->m_vertices.size() * sizeof(Vertex);
	this->m_VBV.StrideInBytes = sizeof(Vertex);

	this->m_IBV.BufferLocation = this->m_IBO->GetGPUVirtualAddress();
	this->m_IBV.Format = DXGI_FORMAT_R32_UINT;
	this->m_IBV.SizeInBytes = this->m_indices.size() * sizeof(UINT);

	this->m_nRTVIndex = renderer->m_rtvHeap->GetDescriptorCount();
	renderer->m_rtvHeap->Allocate(1);

	this->m_rtvDescriptor = renderer->m_rtvHeap->GetDescriptor(this->m_nRTVIndex);
	
	renderer->CreateTexture(renderer->m_nWidth, renderer->m_nHeight, 8, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, this->m_rtvBuff);
	renderer->ResourceBarrier(this->m_rtvBuff.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

	this->m_rtvBuff->SetName(L"ScreenQuad Buffer");

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = { };
	rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	rtvDesc.Texture2D.MipSlice = 1;
	rtvDesc.Texture2D.PlaneSlice = 1;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

	renderer->m_dev->CreateRenderTargetView(this->m_rtvBuff.Get(), &rtvDesc, this->m_rtvDescriptor.cpuHandle);

	this->m_shader = new Shader("LightPass.hlsl", "VertexMain", "PixelMain");

	Descriptor albedoDesc = renderer->m_cbvSrvHeap->GetDescriptor(0);
	Descriptor normalDesc = renderer->m_cbvSrvHeap->GetDescriptor(1);
	Descriptor depthDesc = renderer->m_cbvSrvHeap->GetDescriptor(2);
	Descriptor ORMDesc = renderer->m_cbvSrvHeap->GetDescriptor(3);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	D3D12_SHADER_RESOURCE_VIEW_DESC dsvSrvDesc = { };
	dsvSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	dsvSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	dsvSrvDesc.Texture2D.MipLevels = 1;
	dsvSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	this->m_dev->CreateShaderResourceView(renderer->m_albedoBuff.Get(), &srvDesc, albedoDesc.cpuHandle);
	this->m_dev->CreateShaderResourceView(renderer->m_uvBuff.Get(), &srvDesc, normalDesc.cpuHandle);
	this->m_dev->CreateShaderResourceView(renderer->m_depthBuffer.Get(), &dsvSrvDesc, depthDesc.cpuHandle);
	this->m_dev->CreateShaderResourceView(renderer->m_ORMBuff.Get(), &srvDesc, ORMDesc.cpuHandle);

	CD3DX12_DESCRIPTOR_RANGE albedoRange;
	CD3DX12_DESCRIPTOR_RANGE normalRange;
	CD3DX12_DESCRIPTOR_RANGE depthRange;
	CD3DX12_DESCRIPTOR_RANGE ORMRange;
	CD3DX12_DESCRIPTOR_RANGE cbuffRange;

	albedoRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	normalRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	depthRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	ORMRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	cbuffRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_ROOT_PARAMETER albedoParam;
	CD3DX12_ROOT_PARAMETER normalParam;
	CD3DX12_ROOT_PARAMETER depthParam;
	CD3DX12_ROOT_PARAMETER ORMParam;
	CD3DX12_ROOT_PARAMETER cbuffParam;
	albedoParam.InitAsDescriptorTable(1, &albedoRange, D3D12_SHADER_VISIBILITY_PIXEL);
	normalParam.InitAsDescriptorTable(1, &normalRange, D3D12_SHADER_VISIBILITY_PIXEL);
	depthParam.InitAsDescriptorTable(1, &depthRange, D3D12_SHADER_VISIBILITY_PIXEL);
	ORMParam.InitAsDescriptorTable(1, &ORMRange, D3D12_SHADER_VISIBILITY_PIXEL);
	cbuffParam.InitAsDescriptorTable(1, &cbuffRange, D3D12_SHADER_VISIBILITY_PIXEL);
	
	D3D12_ROOT_PARAMETER rootParams[] = {
		albedoParam,
		normalParam,
		depthParam,
		ORMParam,
		cbuffParam
	};

	D3D12_ROOT_SIGNATURE_DESC rootDesc = { };
	rootDesc.NumParameters = _countof(rootParams);
	rootDesc.pParameters = rootParams;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootDesc.pStaticSamplers = nullptr;
	rootDesc.NumStaticSamplers = 0;

	ComPtr<ID3DBlob> rootBlob, rootErr;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootBlob.GetAddressOf(), rootErr.GetAddressOf()));

	if (rootErr) {
		spdlog::error("ScreenQuad: ", (char*)rootErr->GetBufferPointer());
		return;
	}

	ThrowIfFailed(renderer->m_dev->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(this->m_rootSig.GetAddressOf())));

	void* vShader, *pShader;
	UINT nVertexLength = this->m_shader->GetBuffer(SHADER_BUFFER::VERTEX, vShader);
	UINT nPixelLength = this->m_shader->GetBuffer(SHADER_BUFFER::PIXEL, pShader);

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, NULL}
	};

	D3D12_INPUT_LAYOUT_DESC layout = { };
	layout.pInputElementDescs = elements;
	layout.NumElements = _countof(elements);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC plDesc = { };
	plDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	plDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	plDesc.DepthStencilState.DepthEnable = TRUE;
	plDesc.DepthStencilState.StencilEnable = FALSE;
	plDesc.InputLayout = layout;
	plDesc.VS.pShaderBytecode = vShader;
	plDesc.VS.BytecodeLength = nVertexLength;
	plDesc.PS.pShaderBytecode = pShader;
	plDesc.PS.BytecodeLength = nPixelLength;
	plDesc.pRootSignature = this->m_rootSig.Get();
	plDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	plDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	plDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	plDesc.NumRenderTargets = 1;
	plDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	plDesc.SampleDesc.Count = 8;
	plDesc.SampleMask = UINT32_MAX;

	ThrowIfFailed(this->m_dev->CreateGraphicsPipelineState(&plDesc, IID_PPV_ARGS(this->m_plState.GetAddressOf())));
	this->InitConstantBuffer();
}

void ScreenQuad::Render() {
	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		this->D3D12Render(renderer);
	}
} 

void ScreenQuad::InitConstantBuffer() {
	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		Camera* currentCamera = this->m_sceneMgr->GetCurrentScene()->GetCurrentCamera();

		Transform cameraTransform = this->m_sceneMgr->GetCurrentScene()->GetCurrentCamera()->transform;
		this->m_sqCBuffData.InverseView = XMMatrixTranspose(XMMatrixIdentity());
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(currentCamera->transform.rotation.x)));
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(currentCamera->transform.rotation.y)));
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(currentCamera->transform.rotation.z)));
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixTranslation(
			currentCamera->transform.location.x,
			currentCamera->transform.location.y,
			currentCamera->transform.location.z));

		this->m_sqCBuffData.InverseProjection = XMMatrixTranspose(XMMatrixPerspectiveFovRH(
			XMConvertToRadians(70.f), 
			static_cast<float>(renderer->m_nWidth) / static_cast<float>(renderer->m_nHeight),
			0.01f, 
			3000.f));


		this->m_sqCBuffData.cameraPosition = XMFLOAT3(
			currentCamera->transform.location.x,
			currentCamera->transform.location.y,
			currentCamera->transform.location.z
		);

		UINT nConstantBufferSize = (sizeof(this->m_sqCBuffData) + 255) & ~255;
		renderer->CreateBuffer(&this->m_sqCBuffData, nConstantBufferSize, m_sqCBuffer);

		renderer->m_cbvSrvHeap->Allocate(1);
		this->m_nSqCBuffIndex = renderer->m_cbvSrvHeap->GetLastDescriptorIndex();
		Descriptor sqBuffDesc = renderer->m_cbvSrvHeap->GetDescriptor(this->m_nSqCBuffIndex);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = { };
		cbvDesc.BufferLocation = this->m_sqCBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = nConstantBufferSize;
		this->m_dev->CreateConstantBufferView(&cbvDesc, sqBuffDesc.cpuHandle);
	}
}

void ScreenQuad::UpdateConstantBuffer() {
	if (D3D12* renderer = dynamic_cast<D3D12*>(this->m_renderer)) {
		Camera* currentCamera = this->m_sceneMgr->GetCurrentScene()->GetCurrentCamera();

		Transform cameraTransform = this->m_sceneMgr->GetCurrentScene()->GetCurrentCamera()->transform;
		this->m_sqCBuffData.InverseView = XMMatrixTranspose(XMMatrixIdentity());
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(currentCamera->transform.rotation.x)));
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(currentCamera->transform.rotation.y)));
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(currentCamera->transform.rotation.z)));
		this->m_sqCBuffData.InverseView *= XMMatrixTranspose(XMMatrixTranslation(
			currentCamera->transform.location.x,
			currentCamera->transform.location.y,
			currentCamera->transform.location.z));
		this->m_sqCBuffData.InverseView =  (XMMatrixInverse(nullptr, this->m_sqCBuffData.InverseView));

		this->m_sqCBuffData.InverseProjection = XMMatrixTranspose(XMMatrixPerspectiveFovRH(
			XMConvertToRadians(70.f),
			static_cast<float>(renderer->m_nWidth) / static_cast<float>(renderer->m_nHeight),
			0.01f,
			3000.f));
		this->m_sqCBuffData.InverseProjection = XMMatrixInverse(nullptr, this->m_sqCBuffData.InverseProjection);
		this->m_sqCBuffData.screenSize = XMFLOAT2(renderer->m_nWidth, renderer->m_nHeight);


		this->m_sqCBuffData.cameraPosition = XMFLOAT3(
			currentCamera->transform.location.x,
			currentCamera->transform.location.y,
			currentCamera->transform.location.z
		);

		UINT nConstantBufferSize = (sizeof(this->m_sqCBuffData) + 255) & ~255;

		PVOID pData;
		ThrowIfFailed(this->m_sqCBuffer->Map(0, nullptr, &pData));
		memcpy(pData, &this->m_sqCBuffData, nConstantBufferSize);
		this->m_sqCBuffer->Unmap(0, nullptr);
	}
}

void ScreenQuad::D3D12Render(D3D12* renderer) {
	this->UpdateConstantBuffer();
	Descriptor albedoDesc = renderer->m_cbvSrvHeap->GetDescriptor(0);
	Descriptor normalDesc = renderer->m_cbvSrvHeap->GetDescriptor(1);
	Descriptor depthParam = renderer->m_cbvSrvHeap->GetDescriptor(2);
	Descriptor materialDesc = renderer->m_cbvSrvHeap->GetDescriptor(3);
	Descriptor sqBuffDesc = renderer->m_cbvSrvHeap->GetDescriptor(this->m_nSqCBuffIndex);
	this->m_list->OMSetRenderTargets(1, &this->m_rtvDescriptor.cpuHandle, FALSE, nullptr);
	this->m_list->SetPipelineState(this->m_plState.Get());
	this->m_list->ClearRenderTargetView(this->m_rtvDescriptor.cpuHandle, RGBA{ 0.f, 0.f, 0.f, 1.f }, 0, nullptr);
	this->m_list->SetGraphicsRootSignature(this->m_rootSig.Get());

	this->m_list->IASetVertexBuffers(0, 1, &this->m_VBV);
	this->m_list->IASetIndexBuffer(&this->m_IBV);
	this->m_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	this->m_list->SetGraphicsRootDescriptorTable(0, albedoDesc.gpuHandle);
	this->m_list->SetGraphicsRootDescriptorTable(1, normalDesc.gpuHandle);
	this->m_list->SetGraphicsRootDescriptorTable(2, depthParam.gpuHandle);
	this->m_list->SetGraphicsRootDescriptorTable(3, materialDesc.gpuHandle);
	this->m_list->SetGraphicsRootDescriptorTable(4, sqBuffDesc.gpuHandle);

	this->m_list->DrawIndexedInstanced(this->m_indices.size(), 1, 0, 0, 0);
}

void ScreenQuad::GetResource(ComPtr<ID3D12Resource>& res) {
	ThrowIfFailed(this->m_rtvBuff.CopyTo(res.GetAddressOf()));
}