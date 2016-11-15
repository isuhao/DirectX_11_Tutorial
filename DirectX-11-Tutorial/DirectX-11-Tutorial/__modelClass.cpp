#include "__modelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer  = 0;
	m_Texture	   = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
		return false;

	// The Initialize function now calls a new private function that will load the texture.
	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);

	if (!result)
		return false;

	return true;
}

void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();
	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	
	//First create two temporary arrays to hold the vertex and index data that we will use later to populate the final buffers with.

	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;



	// --- texturing ---
#if 0

	// The vertex array now has a texture component instead of a color component.
	// The texture vector is always U first and V second. For example the first texture coordinate is bottom left of the triangle which corresponds to U 0.0, V 1.0.
	// Use the diagram at the top of this page to figure out what your coordinates need to be.
	// Note that you can change the coordinates to map any part of the texture to any part of the polygon face.
	// In this tutorial I'm just doing a direct mapping for simplicity reasons.

	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-2.0f, -2.0f, 0.0f);  // Bottom left.
	//vertices[0].color    = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	//vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(-2.0f, 2.0f, 0.0f);  // Top middle.
	//vertices[1].color    = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	//vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[2].position = D3DXVECTOR3(2.0f, 2.0f, 0.0f);  // Bottom right.
	//vertices[2].color    = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	//vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[3].position = D3DXVECTOR3(2.0f, -2.0f, 0.0f);  // Bottom right.
	//vertices[2].color    = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	//vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 1;  // Bottom left.
	indices[1] = 2;  // Top middle.
	indices[2] = 3;  // Bottom right.

	indices[3] = 0;
	indices[4] = 1;
	indices[5] = 2;

#endif

	// --- lighting ---
#if 1

	// The only change to the InitializeBuffers function is here in the vertex setup.
	// Each vertex now has normals associated with it for lighting calculations.
	// The normal is a line that is perpendicular to the face of the polygon so that the exact direction the face is pointing can be calculated.
	// For simplicity purposes I set the normal for each vertex along the Z axis by setting each Z component to -1.0f, which makes the normal point towards the viewer.

	// Load the vertex array with data.
/*
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture  = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].texture  = D3DXVECTOR2(0.5f, 0.0f);
	vertices[1].normal   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture  = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].normal   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
*/
	vertices[0].position = D3DXVECTOR3(-2.0f, -2.0f, 0.0f);  // Bottom left.
	vertices[0].texture = D3DXVECTOR2(1.0f, 0.0f);
	vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].position = D3DXVECTOR3(-2.0f, 2.0f, 0.0f);  // Top middle.
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(2.0f, 2.0f, 0.0f);  // Bottom right.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[3].position = D3DXVECTOR3(2.0f, -2.0f, 0.0f);
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// Load the index array with data.
	indices[0] = 1;  // Bottom left.
	indices[1] = 2;  // Top middle.
	indices[2] = 3;  // Bottom right.

	indices[3] = 0;
	indices[4] = 1;
	indices[5] = 2;

#endif



	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

// RenderBuffers is called from the Render function.
// The purpose of this function is to set the vertex buffer and index buffer as active on the input assembler in the GPU.
// Once the GPU has an active vertex buffer it can then use the shader to render that buffer.
// This function also defines how those buffers should be drawn such as triangles, lines, fans, and so forth.
// In this tutorial we set the vertex buffer and index buffer as active on the input assembler and tell the GPU that the buffers should be drawn
// as triangles using the IASetPrimitiveTopology DirectX function.
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	return;
}

// LoadTexture is a new private function that will create the texture object and then initialize it with the input file name provided.
// This function is called during initialization.
bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
		return false;

	return true;
}

// The ReleaseTexture function will release the texture object that was created and loaded during the LoadTexture function
void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture) {
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
